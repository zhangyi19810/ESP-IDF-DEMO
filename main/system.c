#include "system.h"
#include "xl9535.h"

void system_init(void)
{
    ESP_LOGI("SYSTEM", "系统初始化开始...");

    // 初始化XL9535
    esp_err_t ret = xl9535_Init();
    if (ret != ESP_OK) {
        ESP_LOGE("SYSTEM", "XL9535初始化失败");
        return;
    }

    // 使能EN脚
    gpio_config_t en_gpio_config = {    
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = 0,
        .pull_up_en = 0,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << EN_GPIO)
    };
    gpio_config(&en_gpio_config);
    gpio_set_level(EN_GPIO, 1);

    ESP_LOGI("SYSTEM", "EN引脚已使能");

    vTaskDelay(pdMS_TO_TICKS(1000));
}