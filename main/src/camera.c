#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"

#include "system.h"
#include "camera.h"
#include "udp.h"

#include "usb_stream.h"

#include "xl9535.h"

static const char *TAG = "UVC_CAMERA";
static EventGroupHandle_t s_evt_handle;
static camera_select_t current_camera = CAM_SELECT_2;
static uint32_t last_capture_time = 0;  // 上次拍照时间
#define CAPTURE_INTERVAL_MS 10000       // 拍照间隔(10秒)
static bool camera_ready = false;       // 摄像头就绪标志
extern TaskHandle_t notify_task_handle; // 声明外部变量

#define DEMO_UVC_XFER_BUFFER_SIZE (55 * 1024)

#define ENABLE_UVC_FRAME_RESOLUTION_ANY   1        /* 从相机获取的任何分辨率 */

#if (ENABLE_UVC_FRAME_RESOLUTION_ANY)
    #define DEMO_UVC_FRAME_WIDTH        FRAME_RESOLUTION_ANY
    #define DEMO_UVC_FRAME_HEIGHT       FRAME_RESOLUTION_ANY
#else
    #define DEMO_UVC_FRAME_WIDTH        320
    #define DEMO_UVC_FRAME_HEIGHT       240
#endif

// 函数前向声明
static void stream_state_changed_cb(usb_stream_state_t event, void *arg);
static void camera_switch_task(void* pvParameters);

static void camera_frame_cb(uvc_frame_t *frame, void *ptr)
{
    ESP_LOGI(TAG, "UVC回调! 帧格式 = %d, 序号 = %"PRIu32", 宽度 = %"PRIu32", 高度 = %"PRIu32", 长度 = %u",
             frame->frame_format, frame->sequence, frame->width, frame->height, frame->data_bytes);
             
    switch (frame->frame_format) {
    case UVC_FRAME_FORMAT_MJPEG:
        {
            uint32_t current_time = pdTICKS_TO_MS(xTaskGetTickCount());
            // 检查是否达到拍照间隔
            if (current_time - last_capture_time >= CAPTURE_INTERVAL_MS && camera_ready) {
                // 通过UDP发送JPEG图像数据
                notify_task_handle = xTaskGetCurrentTaskHandle();  // 保存当前任务句柄
                if (udp_send_frame(frame->data, frame->data_bytes) != ESP_OK) {
                    ESP_LOGE(TAG, "UDP发送失败");
                } else {
                    last_capture_time = current_time;
                    ESP_LOGI(TAG, "摄像头%d开始发送", current_camera);
                    
                    // 等待发送完成
                    if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000)) == 0) {
                        ESP_LOGE(TAG, "等待发送完成超时");
                        return;
                    }
                    
                    ESP_LOGI(TAG, "摄像头%d发送完成，开始切换", current_camera);
                    
                    // 切换到另一个摄像头
                    camera_ready = false;  // 禁用回调
                    camera_select_t next_camera = (current_camera == CAM_SELECT_1) ? CAM_SELECT_2 : CAM_SELECT_1;
                    xTaskCreate(camera_switch_task, "cam_switch", 4096, (void*)next_camera, 5, NULL);
                }
                notify_task_handle = NULL;  // 清除任务句柄
            }
        }
        break;
    default:
        ESP_LOGW(TAG, "不支持帧格式");
        break;
    }
}

// 摄像头切换任务
static void camera_switch_task(void* pvParameters)
{
    camera_select_t next_camera = (camera_select_t)pvParameters;
    ESP_LOGI(TAG, "开始切换到摄像头%d", next_camera);
    
    if (camera_switch(next_camera) == ESP_OK) {
        ESP_LOGI(TAG, "摄像头%d切换成功", next_camera);
    } else {
        ESP_LOGE(TAG, "摄像头%d切换失败", next_camera);
    }
    
    vTaskDelete(NULL);
}

// 摄像头切换函数
esp_err_t camera_switch(camera_select_t camera)
{
    esp_err_t ret = ESP_OK;

    // 先停止USB流
    ESP_ERROR_CHECK(usb_streaming_stop());
    vTaskDelay(pdMS_TO_TICKS(100));  // 等待停止完成

    // 1. 先禁用OE
    ret = xl9535_Set_Io_Status(OE_GPIO, IO_HIGH);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "OE禁用失败");
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(200));
    
    // 2. 切换摄像头
    ret = xl9535_Set_Io_Status(SEL_GPIO, camera == CAM_SELECT_1 ? IO_LOW : IO_HIGH);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "设置SEL选择失败");
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(200));
    
    current_camera = camera;
    
    // 释放事件组
    if (s_evt_handle != NULL) {
        vEventGroupDelete(s_evt_handle);
        s_evt_handle = NULL;
    }

    // 重新配置UVC
    camera_init();

    return ret;
}

void camera_init()
{
    static uint8_t *xfer_buffer_a = NULL;
    static uint8_t *xfer_buffer_b = NULL;
    static uint8_t *frame_buffer = NULL;

    camera_ready = false;  // 初始化时禁用回调
    ESP_LOGI(TAG, "开始初始化摄像头...");

    // 释放之前的缓冲区
    if (xfer_buffer_a) {
        heap_caps_free(xfer_buffer_a);
        xfer_buffer_a = NULL;
    }
    if (xfer_buffer_b) {
        heap_caps_free(xfer_buffer_b);
        xfer_buffer_b = NULL;
    }
    if (frame_buffer) {
        heap_caps_free(frame_buffer);
        frame_buffer = NULL;
    }

    // 创建件组
    if (s_evt_handle == NULL) {
        s_evt_handle = xEventGroupCreate();
        if (s_evt_handle == NULL) {
            ESP_LOGE(TAG, "事件组创建失败");
            return;
        }
    }

    // 配置XL9535 IO方向
    xl9535_Set_Io_Direction(OE_GPIO, IO_OUTPUT); 
    xl9535_Set_Io_Direction(SEL_GPIO, IO_OUTPUT);    // SEL引脚
    xl9535_Set_Io_Direction(DC_EN_GPIO, IO_OUTPUT);  // DC使能引脚

    // 1. 先关闭所有信号
    xl9535_Set_Io_Status(OE_GPIO, IO_HIGH);        // OE高电平，断开连接
    xl9535_Set_Io_Status(SEL_GPIO, IO_HIGH);       // SEL初始为高，准备选择HSD2
    xl9535_Set_Io_Status(DC_EN_GPIO, IO_LOW);      // DC电源关闭
    vTaskDelay(pdMS_TO_TICKS(100));   // 缩短断电等待时间
    
    // 2. 先使能DC电源
    ESP_LOGI(TAG, "使能DC电源...");
    // xl9535_Set_Io_Status(DC_EN_GPIO, IO_HIGH);     // 使能DC电源
    xl9535_Set_Io_Status(DC_EN_GPIO, IO_LOW);     // 使能DC电源
    vTaskDelay(pdMS_TO_TICKS(500));   // 等待电源稳定
    
    // 3. 根据选择的摄像头设置SEL信号
    ESP_LOGI(TAG, "设置SEL信号...");
    if (current_camera == CAM_SELECT_1) {
        xl9535_Set_Io_Status(SEL_GPIO, IO_LOW);    // SEL低电平选择HSD1
    } else {
        xl9535_Set_Io_Status(SEL_GPIO, IO_HIGH);   // SEL高电平选择HSD2
    }
    vTaskDelay(pdMS_TO_TICKS(100));   // 缩短SEL信号等待时间
    
    // 4. 最后通过OE使能选中的摄像头
    ESP_LOGI(TAG, "选择摄像头...");
    xl9535_Set_Io_Status(OE_GPIO, IO_LOW);         // OE低电平能选中的摄像头
    vTaskDelay(pdMS_TO_TICKS(100));   // 缩短OE信号等待时间
    
    // 5. 等待USB设备完全初始化
    vTaskDelay(pdMS_TO_TICKS(500));   // 缩短USB初始化等待时间

    // 预分配所有缓冲区，避免运行时分配
    ESP_LOGI(TAG, "开始分配UVC缓冲区...");
    /* 创建缓冲区*/
    // malloc双缓冲区用于USB负载， xfer_buffer_size >= frame_buffer_size
    xfer_buffer_a = heap_caps_malloc(DEMO_UVC_XFER_BUFFER_SIZE, MALLOC_CAP_DMA); 
    assert(xfer_buffer_a != NULL);
    xfer_buffer_b = heap_caps_malloc(DEMO_UVC_XFER_BUFFER_SIZE, MALLOC_CAP_DMA);
    assert(xfer_buffer_b != NULL);
    // malloc帧缓冲区用于jpeg图像帧
    frame_buffer = heap_caps_malloc(DEMO_UVC_XFER_BUFFER_SIZE, MALLOC_CAP_DMA); 
    assert(frame_buffer != NULL);

    ESP_LOGI(TAG, "配置UVC参数...");
    /* UVC配置 */
    uvc_config_t uvc_config = {
        .frame_width = DEMO_UVC_FRAME_WIDTH,
        .frame_height = DEMO_UVC_FRAME_HEIGHT,
        .frame_interval = FPS2INTERVAL(10),  // 进一步降低帧率，因为我们只需要每10秒一张
        .xfer_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
        .xfer_buffer_a = xfer_buffer_a,
        .xfer_buffer_b = xfer_buffer_b,
        .frame_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
        .frame_buffer = frame_buffer,
        .frame_cb = &camera_frame_cb,
        .frame_cb_arg = NULL,
    };

    ESP_LOGI(TAG, "注册USB流回...");
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
            camera_ready = true;  // 摄像头连接成功后启用回调
        }
        break;
    }
    case STREAM_DISCONNECTED:
        ESP_LOGI(TAG, "摄像头已断开");
        camera_ready = false;  // 摄像头断开时禁用回调
        break;
    default:
        ESP_LOGE(TAG, "未知事件");
        break;
    }
}