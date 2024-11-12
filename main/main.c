#include "xl9535.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "driver/gpio.h"    

#define FEEDL_PWM_TIMER                     LEDC_TIMER_0            /*!< LEDC定时器0 */
#define FEEDL_PWM_MODE                      LEDC_LOW_SPEED_MODE     /*!< LEDC低速模式 */
#define FEEDL_PWM_DUTY_RES                  LEDC_TIMER_8_BIT        /*!< LEDC分辨率 */
#define FEEDL_PWM_FREQUENCY                 (5000)                  /*!< LEDC频率 */

#define FEEDR_PWM_TIMER                     LEDC_TIMER_0            /*!< LEDC定时器0 */
#define FEEDR_PWM_MODE                      LEDC_LOW_SPEED_MODE     /*!< LEDC低速模式 */
#define FEEDR_PWM_DUTY_RES                  LEDC_TIMER_8_BIT        /*!< LEDC分辨率 */
#define FEEDR_PWM_FREQUENCY                 (5000)                  /*!< LEDC频率 */

#define FEEDL_PWM_OUTPUT_IO                 GPIO_NUM_11             /*!< 哺光左眼IO端口 */
#define FEEDR_PWM_OUTPUT_IO                 GPIO_NUM_47             /*!< 哺光右眼IO端口 */

#define FEEDL_PWM_CHANNEL                   LEDC_CHANNEL_0          /*!< 哺光左眼通道 */
#define FEEDR_PWM_CHANNEL                   LEDC_CHANNEL_1          /*!< 哺光右眼通道 */

// 呼吸灯任务函数
static void breathing_led_task(void *arg)
{
    ledc_fade_func_install(0);

    while (1)
    {
        // 第一段：0-64，非常慢（10秒）
        ESP_ERROR_CHECK(ledc_set_fade_with_time(FEEDL_PWM_MODE, FEEDL_PWM_CHANNEL, 64, 10000));
        ESP_ERROR_CHECK(ledc_set_fade_with_time(FEEDL_PWM_MODE, FEEDR_PWM_CHANNEL, 64, 10000));
        ESP_ERROR_CHECK(ledc_fade_start(FEEDL_PWM_MODE, FEEDL_PWM_CHANNEL, LEDC_FADE_NO_WAIT));
        ESP_ERROR_CHECK(ledc_fade_start(FEEDL_PWM_MODE, FEEDR_PWM_CHANNEL, LEDC_FADE_WAIT_DONE));
        
        // 第二段：64-128，较慢（3秒）
        ESP_ERROR_CHECK(ledc_set_fade_with_time(FEEDL_PWM_MODE, FEEDL_PWM_CHANNEL, 128, 3000));
        ESP_ERROR_CHECK(ledc_set_fade_with_time(FEEDL_PWM_MODE, FEEDR_PWM_CHANNEL, 128, 3000));
        ESP_ERROR_CHECK(ledc_fade_start(FEEDL_PWM_MODE, FEEDL_PWM_CHANNEL, LEDC_FADE_NO_WAIT));
        ESP_ERROR_CHECK(ledc_fade_start(FEEDL_PWM_MODE, FEEDR_PWM_CHANNEL, LEDC_FADE_WAIT_DONE));

        vTaskDelay(pdMS_TO_TICKS(2000));

        // 64-0，非常慢（10秒）
        ESP_ERROR_CHECK(ledc_set_fade_with_time(FEEDL_PWM_MODE, FEEDL_PWM_CHANNEL, 0, 500));
        ESP_ERROR_CHECK(ledc_set_fade_with_time(FEEDL_PWM_MODE, FEEDR_PWM_CHANNEL, 0, 500));
        ESP_ERROR_CHECK(ledc_fade_start(FEEDL_PWM_MODE, FEEDL_PWM_CHANNEL, LEDC_FADE_NO_WAIT));
        ESP_ERROR_CHECK(ledc_fade_start(FEEDL_PWM_MODE, FEEDR_PWM_CHANNEL, LEDC_FADE_WAIT_DONE));
    }
}

void app_main(void)
{
    // Xl9535_Init();

    // Xl9535_Set_Io_Direction(PIN_P07, IO_OUTPUT);
    // Xl9535_Set_Io_Status(PIN_P07, IO_HIGH);

    // int pin_sequence[] = {PIN_P05, PIN_P04, PIN_P03, PIN_P02, PIN_P01, PIN_P00, PIN_P14, PIN_P12, PIN_P13, PIN_P17, PIN_P16, PIN_P15, PIN_P14, PIN_P00};
    // Xl9535_Set_Io_Status_Multiple(pin_sequence, sizeof(pin_sequence) / sizeof(pin_sequence[0]), IO_HIGH);

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << 38);
    gpio_config(&io_conf);

    gpio_set_level(38, 1);

    // PWM定时器配置
    ledc_timer_config_t feed_ledc_timer = {
        .speed_mode       = FEEDL_PWM_MODE,
        .timer_num        = FEEDL_PWM_TIMER,
        .duty_resolution  = FEEDL_PWM_DUTY_RES,
        .freq_hz          = FEEDL_PWM_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&feed_ledc_timer));

    // 左眼PWM通道配置
    ledc_channel_config_t feed_ledc_channel_l = {
        .speed_mode     = FEEDL_PWM_MODE,
        .channel        = FEEDL_PWM_CHANNEL,
        .timer_sel      = FEEDL_PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = FEEDL_PWM_OUTPUT_IO,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&feed_ledc_channel_l));

    // 右眼PWM通道配置
    ledc_channel_config_t feed_ledc_channel_r = {
        .speed_mode     = FEEDR_PWM_MODE,
        .channel        = FEEDR_PWM_CHANNEL,
        .timer_sel      = FEEDR_PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = FEEDR_PWM_OUTPUT_IO,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&feed_ledc_channel_r));

    // 创建呼吸灯任务
    xTaskCreate(breathing_led_task, "breathing_led", 2048, NULL, 5, NULL);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
