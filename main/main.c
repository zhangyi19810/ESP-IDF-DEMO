#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "src/camera.h"
#include "src/network.h"

static const char *TAG = "main";

// 帧回调函数
static void frame_callback(uint8_t *data, size_t len)
{
    static uint32_t frame_count = 0;
    frame_count++;
    ESP_LOGI(TAG, "收到第 %lu 帧图像，大小: %d bytes", frame_count, len);
    
    // 通过UDP发送图像数据
    esp_err_t ret = udp_send_data(data, len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "图像发送失败");
    }
}

void app_main(void)
{
    // 初始化WiFi
    ESP_ERROR_CHECK(wifi_init());
    
    // 等待WiFi连接
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    // 初始化UDP
    ESP_ERROR_CHECK(network_udp_init());
    
    // 初始化摄像头硬件
    ESP_ERROR_CHECK(camera_hw_init());
    
    // 配置摄像头
    camera_config_t config = {
        .width = 320,
        .height = 240,
        .fps = 15,
        .frame_cb = frame_callback
    };

    // 初始化摄像头
    ESP_ERROR_CHECK(camera_init(&config));

    // 启动摄像头
    ESP_ERROR_CHECK(camera_start());

    // 主循环
    while (1) {
        ESP_LOGI(TAG, "摄像头状态: %d", camera_get_state());
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
