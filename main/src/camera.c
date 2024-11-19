#include "camera.h"
#include "esp_log.h"

static const char *TAG = "camera";
static camera_state_t camera_state = CAMERA_IDLE;
static camera_config_t camera_config;

#define DEMO_UVC_XFER_BUFFER_SIZE (55 * 1024)
static uint8_t *xfer_buffer_a = NULL;
static uint8_t *xfer_buffer_b = NULL;
static uint8_t *frame_buffer = NULL;

// USB状态回调
static void stream_state_changed_cb(usb_stream_state_t event, void *arg)
{
    switch (event) {
        case STREAM_CONNECTED:
            ESP_LOGI(TAG, "USB设备已连接");
            break;
        case STREAM_DISCONNECTED:
            ESP_LOGI(TAG, "USB设备已断开");
            camera_state = CAMERA_IDLE;
            break;
        default:
            ESP_LOGW(TAG, "未知的USB事件");
            break;
    }
}

// UVC帧回调
static void uvc_frame_cb(uvc_frame_t *frame, void *ptr)
{
    static uint32_t frame_count = 0;
    
    if (frame->frame_format == UVC_FRAME_FORMAT_MJPEG) {
        frame_count++;
        ESP_LOGI(TAG, "收到第 %lu 帧图像: %dx%d, 大小: %d bytes, format: %d", 
                 frame_count, 
                 (int)frame->width, 
                 (int)frame->height, 
                 (int)frame->data_bytes, 
                 (int)frame->frame_format);
        
        if (camera_config.frame_cb && frame->data && frame->data_bytes > 0) {
            camera_config.frame_cb(frame->data, frame->data_bytes);
        }
    } else {
        ESP_LOGW(TAG, "收到非MJPEG格式帧: format=%d, size=%d", 
                 (int)frame->frame_format, 
                 (int)frame->data_bytes);
    }
}

// 分配缓冲区
static esp_err_t allocate_buffers(void)
{
    if (xfer_buffer_a) heap_caps_free(xfer_buffer_a);
    if (xfer_buffer_b) heap_caps_free(xfer_buffer_b);
    if (frame_buffer) heap_caps_free(frame_buffer);

    xfer_buffer_a = heap_caps_malloc(DEMO_UVC_XFER_BUFFER_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    xfer_buffer_b = heap_caps_malloc(DEMO_UVC_XFER_BUFFER_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    frame_buffer = heap_caps_malloc(DEMO_UVC_XFER_BUFFER_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (!xfer_buffer_a || !xfer_buffer_b || !frame_buffer) {
        ESP_LOGE(TAG, "缓冲区分配失败");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "缓冲区分配成功，剩余PSRAM: %d bytes", 
             heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    return ESP_OK;
}

esp_err_t camera_hw_init(void)
{
    // 初始化IO扩展芯片
    Xl9535_Init();
    Xl9535_Set_Io_Direction(USB_EN_PIN, IO_OUTPUT);
    Xl9535_Set_Io_Direction(USB_OE_PIN, IO_OUTPUT);
    Xl9535_Set_Io_Direction(DC_EN_PIN, IO_OUTPUT);
    
    // 先确保所有设备断电
    Xl9535_Set_Io_Status(USB_EN_PIN, IO_LOW);
    Xl9535_Set_Io_Status(USB_OE_PIN, IO_LOW);
    Xl9535_Set_Io_Status(DC_EN_PIN, IO_LOW);
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // 打开DC电源
    Xl9535_Set_Io_Status(DC_EN_PIN, IO_HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // 打开USB电源
    Xl9535_Set_Io_Status(USB_EN_PIN, IO_HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    ESP_LOGI(TAG, "摄像头硬件初始化完成");
    return ESP_OK;
}

esp_err_t camera_init(camera_config_t *config)
{
    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }

    // 保存配置
    memcpy(&camera_config, config, sizeof(camera_config_t));

    // 分配缓冲区
    esp_err_t ret = allocate_buffers();
    if (ret != ESP_OK) {
        return ret;
    }

    // 注册USB状态回调
    ESP_ERROR_CHECK(usb_streaming_state_register(&stream_state_changed_cb, NULL));

    // 配置UVC
    uvc_config_t uvc_config = {
        .frame_width = FRAME_RESOLUTION_ANY,
        .frame_height = FRAME_RESOLUTION_ANY,
        .frame_interval = FPS2INTERVAL(15),
        .xfer_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
        .xfer_buffer_a = xfer_buffer_a,
        .xfer_buffer_b = xfer_buffer_b,
        .frame_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
        .frame_buffer = frame_buffer,
        .frame_cb = &uvc_frame_cb,
        .frame_cb_arg = NULL,
        .flags = 0,
    };

    ret = uvc_streaming_config(&uvc_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "UVC配置失败");
        return ret;
    }

    // 启动USB流
    ret = usb_streaming_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "启动USB流失败");
        return ret;
    }

    // 等待设备连接
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 获取支持的分辨率列表
    size_t frame_size = 0;
    size_t frame_index = 0;
    if (uvc_frame_size_list_get(NULL, &frame_size, &frame_index) == ESP_OK) {
        ESP_LOGI(TAG, "摄像头支持的分辨率数量: %d, 当前索引: %d", 
                 (int)frame_size, (int)frame_index);
        if (frame_size > 0) {
            uvc_frame_size_t *frame_list = (uvc_frame_size_t *)malloc(frame_size * sizeof(uvc_frame_size_t));
            if (frame_list) {
                uvc_frame_size_list_get(frame_list, NULL, NULL);
                for (size_t i = 0; i < frame_size; i++) {
                    ESP_LOGI(TAG, "支持的分辨率[%d]: %dx%d", 
                            (int)i, 
                            (int)frame_list[i].width, 
                            (int)frame_list[i].height);
                }
                free(frame_list);
            }
        }
    }

    camera_state = CAMERA_IDLE;
    ESP_LOGI(TAG, "摄像头初始化完成");
    return ESP_OK;
}

esp_err_t camera_start(void)
{
    // 检查设备状态
    size_t frame_size = 0;
    size_t frame_index = 0;
    if (uvc_frame_size_list_get(NULL, &frame_size, &frame_index) == ESP_OK) {
        ESP_LOGI(TAG, "USB设备已就绪，支持的分辨率数量: %d", (int)frame_size);
    } else {
        ESP_LOGW(TAG, "USB设备可能未就绪，等待设备...");
        // 等待设备就绪
        for (int i = 0; i < 10; i++) {
            vTaskDelay(pdMS_TO_TICKS(500));
            if (uvc_frame_size_list_get(NULL, &frame_size, &frame_index) == ESP_OK) {
                ESP_LOGI(TAG, "USB设备已就绪");
                break;
            }
            ESP_LOGI(TAG, "等待USB设备就绪... (%d/10)", i+1);
        }
    }

    camera_state = CAMERA_RUNNING;
    ESP_LOGI(TAG, "摄像头已启动");
    return ESP_OK;
}

esp_err_t camera_stop(void)
{
    usb_streaming_stop();
    camera_state = CAMERA_IDLE;
    ESP_LOGI(TAG, "摄像头已停止");
    return ESP_OK;
}

camera_state_t camera_get_state(void)
{
    return camera_state;
} 