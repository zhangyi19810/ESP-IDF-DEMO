#include <stdio.h>
#include "esp_log.h"
#include "driver/ledc.h"
#include "bh1750.h"
#include "xl9535.h"

static const char *TAG = "main";

#define BUZZER_IO_NUM           (GPIO_NUM_47)
#define BUZZER_PWM_CHANNEL      (0)
#define BUZZER_MODE             (LEDC_LOW_SPEED_MODE)
#define BUZZER_TIMER            (LEDC_TIMER_0)
#define DC_EN                   (GPIO_NUM_1)

SemaphoreHandle_t print_mux = NULL;

static void i2c_test_task(void *arg)
{
    int cnt = 0;
    float lux_l, lux_h;
    
    BH1750_Init_L();
    BH1750_Init_H();
    vTaskDelay(180 / portTICK_PERIOD_MS);

    while (1) {
        ESP_LOGI(TAG, "test cnt: %d", cnt++);
        
        lux_l = BH1750_ReadLightIntensity_L();
        lux_h = BH1750_ReadLightIntensity_H();
        
        xSemaphoreTake(print_mux, portMAX_DELAY);
        printf("sensor L val: %.02f [Lux]\n", lux_l);
        printf("sensor H val: %.02f [Lux]\n", lux_h);
        xSemaphoreGive(print_mux);
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vSemaphoreDelete(print_mux);
    vTaskDelete(NULL);
}

void app_main(void)
{
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = false,
        .pull_up_en = true,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << DC_EN),
    };
    gpio_config(&gpio_conf);
    gpio_set_level(DC_EN, 1);

    print_mux = xSemaphoreCreateMutex();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    esp_err_t ret = I2C_Init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C initialization failed: %d", ret);
        return;
    }
    ESP_LOGI(TAG, "I2C initialized successfully");
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    i2c_scanner();
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    xTaskCreate(i2c_test_task, "i2c_test_task", 1024 * 4, NULL, 10, NULL);
}
