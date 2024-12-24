#include "camera.h"
#include "system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"

#include "usb_stream.h"

#include "xl9535.h"

static const char *TAG = "UVC_CAMERA";
static EventGroupHandle_t s_evt_handle;
static camera_select_t current_camera = CAM_SELECT_1;

#define DEMO_UVC_XFER_BUFFER_SIZE (55 * 1024)

#define ENABLE_UVC_FRAME_RESOLUTION_ANY   1        /* 从相机获取的任何分辨率 */

#if (ENABLE_UVC_FRAME_RESOLUTION_ANY)
    #define DEMO_UVC_FRAME_WIDTH        FRAME_RESOLUTION_ANY
    #define DEMO_UVC_FRAME_HEIGHT       FRAME_RESOLUTION_ANY
#else
    #define DEMO_UVC_FRAME_WIDTH        320
    #define DEMO_UVC_FRAME_HEIGHT       240
#endif

static void stream_state_changed_cb(usb_stream_state_t event, void *arg);

static void camera_frame_cb(uvc_frame_t *frame, void *ptr)
{
    ESP_LOGI(TAG, "UVC回调! 帧格式 = %d, 序号 = %"PRIu32", 宽度 = %"PRIu32", 高度 = %"PRIu32", 长度 = %u",
             frame->frame_format, frame->sequence, frame->width, frame->height, frame->data_bytes);
             
    switch (frame->frame_format) {
    case UVC_FRAME_FORMAT_MJPEG:
        // 处理MJPEG格式的帧
        break;
    default:
        ESP_LOGW(TAG, "不支持的帧格式");
        break;
    }
}

// 摄像头切换函数
esp_err_t camera_switch(camera_select_t camera)
{
    esp_err_t ret = ESP_OK;
    
    // 1. 先关闭USB
    ret = xl9535_Set_Io_Status(USB_EN_GPIO, IO_LOW);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "USB_EN禁用失败");
        return ret;
    }
    
    // 2. 设置OE为高,禁用MUX
    ret = xl9535_Set_Io_Status(OE_GPIO, IO_HIGH);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "设置OE为高电平失败");
        return ret;
    }

    // 3. 设置SEL选择摄像头
    ret = xl9535_Set_Io_Status(USB_EN_GPIO, camera == CAM_SELECT_1 ? IO_LOW : IO_HIGH);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "设置SEL选择失败");
        return ret;
    }

    // 4. 设置OE为低,使能MUX
    ret = xl9535_Set_Io_Status(OE_GPIO, IO_LOW);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "设置OE为低电平失败");
        return ret;
    }

    // 5. 使能USB
    ret = xl9535_Set_Io_Status(USB_EN_GPIO, IO_HIGH);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "USB_EN使能失败");
        return ret;
    }

    current_camera = camera;
    
    // 6. 重新配置UVC
    camera_init();

    return ret;
}

void camera_init()
{
    esp_err_t ret = ESP_FAIL;

    ESP_LOGI(TAG, "开始初始化摄像头...");

    // 配置XL9535 IO方向
    xl9535_Set_Io_Direction(OE_GPIO, IO_OUTPUT); 
    xl9535_Set_Io_Direction(USB_EN_GPIO, IO_OUTPUT); // SEL pin

    // 1. 先确保所有控制信号为初始状态
    xl9535_Set_Io_Status(OE_GPIO, IO_HIGH);
    xl9535_Set_Io_Status(USB_EN_GPIO, IO_LOW);
    vTaskDelay(pdMS_TO_TICKS(100));  // 等待信号稳定
    
    // 2. 设置摄像头选择信号
    xl9535_Set_Io_Status(USB_EN_GPIO, current_camera == CAM_SELECT_1 ? IO_LOW : IO_HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));  // 等待选择信号稳定
    
    // 3. 使能MUX
    xl9535_Set_Io_Status(OE_GPIO, IO_LOW);
    vTaskDelay(pdMS_TO_TICKS(100));  // 等待MUX稳定
    
    // 4. 使能USB
    xl9535_Set_Io_Status(USB_EN_GPIO, IO_HIGH);
    ESP_LOGI(TAG, "USB供电已使能");
    vTaskDelay(pdMS_TO_TICKS(500));  // 等待USB设备上电完成

    s_evt_handle = xEventGroupCreate();
    if (s_evt_handle == NULL) {
        ESP_LOGE(TAG, "事件组创建失败");
        return;
    }

    ESP_LOGI(TAG, "开始分配UVC缓冲区...");
    /* 创建缓冲区*/
    // malloc双缓冲区用于USB负载， xfer_buffer_size >= frame_buffer_size
    uint8_t *xfer_buffer_a = (uint8_t *)malloc(DEMO_UVC_XFER_BUFFER_SIZE); 
    assert(xfer_buffer_a != NULL);
    uint8_t *xfer_buffer_b = (uint8_t *)malloc(DEMO_UVC_XFER_BUFFER_SIZE);
    assert(xfer_buffer_b != NULL);
    // malloc帧缓冲区用于jpeg图像帧
    uint8_t *frame_buffer = (uint8_t *)malloc(DEMO_UVC_XFER_BUFFER_SIZE); 
    assert(frame_buffer != NULL);

    ESP_LOGI(TAG, "配置UVC参数...");
    /* UVC配置 */
    uvc_config_t uvc_config = {
        .frame_width = DEMO_UVC_FRAME_WIDTH,
        .frame_height = DEMO_UVC_FRAME_HEIGHT,
        .frame_interval = FPS2INTERVAL(5),  // 降低帧率，提高稳定性
        .xfer_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
        .xfer_buffer_a = xfer_buffer_a,
        .xfer_buffer_b = xfer_buffer_b,
        .frame_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
        .frame_buffer = frame_buffer,
        .frame_cb = &camera_frame_cb,
        .frame_cb_arg = NULL,
    };

    ESP_LOGI(TAG, "注册USB流回调...");
    /* 注册状态改变回调 */
    ESP_ERROR_CHECK(usb_streaming_state_register(&stream_state_changed_cb, NULL));

    ESP_LOGI(TAG, "配置USB流...");
    /* 配置并启动UVC流 */
    ESP_ERROR_CHECK(uvc_streaming_config(&uvc_config));
    ESP_LOGI(TAG, "启动USB流...");
    ESP_ERROR_CHECK(usb_streaming_start());
    ESP_LOGI(TAG, "等待USB设备连接...");
    ESP_ERROR_CHECK(usb_streaming_connect_wait(portMAX_DELAY));
}

/* 处理设备连接状态变化 */
static void stream_state_changed_cb(usb_stream_state_t event, void *arg)
{
    switch (event) {
    case STREAM_CONNECTED: {
        /* 获取摄像头支持的分辨率列表 */
        size_t frame_size = 0;
        size_t frame_index = 0;
        ESP_ERROR_CHECK(uvc_frame_size_list_get(NULL, &frame_size, &frame_index));
        if (frame_size) {
            ESP_LOGI(TAG, "摄像头已连接, 支持的分辨率数量: %u", frame_size);
            uvc_frame_size_t *frame_list = (uvc_frame_size_t *)malloc(frame_size * sizeof(uvc_frame_size_t));
            ESP_ERROR_CHECK(uvc_frame_size_list_get(frame_list, NULL, NULL));
            for (size_t i = 0; i < frame_size; i++) {
                ESP_LOGI(TAG, "分辨率 %u: %ux%u", i, frame_list[i].width, frame_list[i].height);
            }
            free(frame_list);
        }
        break;
    }
    case STREAM_DISCONNECTED:
        ESP_LOGI(TAG, "摄像头已断开");
        break;
    default:
        ESP_LOGE(TAG, "未知事件");
        break;
    }
}