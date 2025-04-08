/**
 * @file main.c
 * @brief ESP-IDF LCD SPI示例程序
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "components/lcd.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "LCD软件SPI驱动示例");
    
    // 初始化LCD
    LCD_Init();
    
    ESP_LOGI(TAG, "LCD初始化完成，开始颜色测试...");
    
    // 进行LCD颜色测试
    LCD_ColorTest();
    
    ESP_LOGI(TAG, "颜色测试完成，请观察屏幕显示效果");
    
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
