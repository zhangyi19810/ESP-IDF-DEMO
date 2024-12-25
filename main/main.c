#include "system.h"
#include "camera.h"
#include "wifi.h"

void app_main(void)
{
    system_init();
    
    // 初始化WiFi
    ESP_LOGI("MAIN", "开始WiFi初始化...");
    ESP_ERROR_CHECK(wifi_init_sta());
      
    // 初始化摄像头1
    camera_init();

    gpio_reset_pin(GPIO_NUM_12);
    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT);
    

    while (1)
    {
        gpio_set_level(GPIO_NUM_12, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(GPIO_NUM_12, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
      
}
