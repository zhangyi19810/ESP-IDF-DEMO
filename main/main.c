#include <stdio.h>
#include "system.h"

// 初始化LED定时器，特别为RGBW灯预留一个定时器给红色通道的40Hz模式
void drv_ledc_tim_init(void)
{
    // 常规灯光的定时器配置 (如10kHz)
    ledc_timer_config_t ledc_tim_conf = {
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_LIGHT_RESOLUTION,
        .freq_hz = 50000,  // 10kHz频率
        .speed_mode = LEDC_LIGHT_MODE,
        .timer_num = LEDC_LIGHT_TIMER
    };
    ledc_timer_config(&ledc_tim_conf);
#if 1
    // 为红色通道的40Hz特殊模式配置独立定时器
    ledc_timer_config_t ledc_red_40hz_tim_conf = {
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 40,  // 40Hz频率
        .speed_mode = LEDC_LIGHT_MODE,
        .timer_num = LEDC_TIMER_1  // 独立定时器
    };
    ledc_timer_config(&ledc_red_40hz_tim_conf);
#endif
}

// 初始化LED通道，包括上下灯、环绕灯和RGBW灯光，红色通道有两种模式（10kHz和40Hz）
void drv_ledc_channel_init(void)
{
    // gpio_set_level(TOP_LIGHT_PWM_IO_NUM, 0);  // 确保初始为低电平
    // 初始化上下灯环绕灯通道
 
#if 1
    ledc_channel_config_t ledc_lower_conf = {
        .channel = LOWER_LIGHT_PWM_CHANNEL,
        .duty = 0,  // 初始设置为最小值（全低电平），经过反转后变为高电平
        .flags.output_invert = 0,  // 启用输出反转
        .gpio_num = LOWER_LIGHT_PWM_IO_NUM,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LIGHT_MODE,
        .timer_sel = LEDC_LIGHT_TIMER
    };
    ledc_channel_config(&ledc_lower_conf);
#endif
#if 1
    ledc_channel_config_t ledc_top_conf = {
        .channel = TOP_LIGHT_PWM_CHANNEL,
        .duty = 0,  // 初始设置为最小值，反转后为高电平
        .flags.output_invert = 0,  // 启用输出反转
        .gpio_num = TOP_LIGHT_PWM_IO_NUM,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LIGHT_MODE,
        .timer_sel = LEDC_LIGHT_TIMER  // 10kHz定时器
    };
    ledc_channel_config(&ledc_top_conf);
  
    ledc_set_duty(LEDC_LIGHT_MODE, TOP_LIGHT_PWM_CHANNEL, 0);
    ledc_update_duty(LEDC_LIGHT_MODE, TOP_LIGHT_PWM_CHANNEL);
#endif 
    ledc_channel_config_t ledc_round_conf = {
        .channel = ROUND_LIGHT_PWM_CHANNEL,
        .duty = 0,  // 初始设置为最小值，反转后为高电平
        .flags.output_invert = 0,  // 启用输出反转
        .gpio_num = ROUND_LIGHT_PWM_IO_NUM,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LIGHT_MODE,
        .timer_sel = LEDC_LIGHT_TIMER  // 10kHz定时器
    };
    ledc_channel_config(&ledc_round_conf);

    // 初始化RGBW灯通道（红色通道初始化两个模式）
    ledc_channel_config_t ledc_r_conf_10khz = {
        .channel = LIGHT_THERAPY_RED_CHANNEL,
        .duty = 0,  // 初始设置为最小值，反转后为高电平
        .flags.output_invert = 0,  // 启用输出反转
        .gpio_num = LIGHT_THERAPY_RED_IO_NUM,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LIGHT_MODE,
        .timer_sel = LEDC_LIGHT_TIMER  // 红色通道使用10kHz定时器
    };
    ledc_channel_config(&ledc_r_conf_10khz);

    // ledc_channel_config_t ledc_r_conf_40hz = {
    //     .channel = LIGHT_THERAPY_RED_CHANNEL,
    //     .duty = 0,  // 初始设置为最小值，反转后为高电平
    //     .flags.output_invert = 0,  // 启用输出反转
    //     .gpio_num = LIGHT_THERAPY_RED_IO_NUM,
    //     .hpoint = 0,
    //     .intr_type = LEDC_INTR_DISABLE,
    //     .speed_mode = LEDC_LIGHT_MODE,
    //     .timer_sel = LEDC_TIMER_1  // 红色通道使用40Hz定时器
    // };
    // ledc_channel_config(&ledc_r_conf_40hz);

    ledc_channel_config_t ledc_g_conf = {
        .channel = LIGHT_THERAPY_GREEN_CHANNEL,
        .duty = 0,  // 初始设置为最小值，反转后为高电平
        .flags.output_invert = 0,  // 启用输出反转
        .gpio_num = LIGHT_THERAPY_GREEN_IO_NUM,
        .hpoint = 255,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LIGHT_MODE,
        .timer_sel = LEDC_LIGHT_TIMER  // 10kHz定时器
    };
    ledc_channel_config(&ledc_g_conf);

    ledc_channel_config_t ledc_b_conf = {
        .channel = LIGHT_THERAPY_BLUE_CHANNEL,
        .duty = 0,  // 初始设置为最小值，反转后为高电平
        .flags.output_invert = 0,  // 启用输出反转
        .gpio_num = LIGHT_THERAPY_BLUE_IO_NUM,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LIGHT_MODE,
        .timer_sel = LEDC_LIGHT_TIMER  // 10kHz定时器
    };
    ledc_channel_config(&ledc_b_conf);

    ledc_channel_config_t ledc_w_conf = {
        .channel = LIGHT_THERAPY_WHITE_CHANNEL,
        .duty = 0,  // 初始设置为最小值，反转后为高电平
        .flags.output_invert = 0,  // 启用输出反转
        .gpio_num = LIGHT_THERAPY_WHITE_IO_NUM,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LIGHT_MODE,
        .timer_sel = LEDC_LIGHT_TIMER  // 10kHz定时器
    };
    ledc_channel_config(&ledc_w_conf);
}

// 切换红色通道的模式（10kHz模式和40Hz模式）
void drv_switch_red_mode(bool is_40hz)
{
    if (is_40hz)
    {
        // 切换到40Hz模式
        ledc_bind_channel_timer(LEDC_LIGHT_MODE, LIGHT_THERAPY_RED_CHANNEL, LEDC_TIMER_1);
    }
    else
    {
        // 切换到10kHz模式
        ledc_bind_channel_timer(LEDC_LIGHT_MODE, LIGHT_THERAPY_RED_CHANNEL, LEDC_LIGHT_TIMER);
    }
}

// 设置LED的占空比
void drv_set_duty(uint8_t channel, uint16_t duty)
{
    ledc_set_duty(LEDC_LIGHT_MODE, channel, duty);
    ledc_update_duty(LEDC_LIGHT_MODE, channel);
}


void app_main(void)
{
    // 为蜂鸣器配置一个单独的定时器（例如2kHz）
    ledc_timer_config_t buzzer_tim_conf = {
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_13_BIT,  // 8位分辨率
        .freq_hz = 50,  // 2kHz频率，适合蜂鸣器发声
        .speed_mode = LEDC_LOW_SPEED_MODE,  // 使用高频模式
        .timer_num = LEDC_TIMER_2  // 独立定时器
    };
    ledc_timer_config(&buzzer_tim_conf);

    ledc_channel_config_t buzzer_conf = {
        .channel = BUZZER_PWM_CHANNEL,
        .duty = 0,
        .flags.output_invert = 0,
        .gpio_num = BUZZER_IO_NUM,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,  // 对应之前设置的模式
        .timer_sel = LEDC_TIMER_2  // 使用专门为蜂鸣器配置的定时器
    };
    ledc_channel_config(&buzzer_conf);

    ledc_stop(BUZZER_MODE, BUZZER_PWM_CHANNEL, 0);  

    // gpio_config_t config = {
    //     .pin_bit_mask = (1ULL << LIGHT_THERAPY_GREEN_IO_NUM) | (1ULL << LIGHT_THERAPY_RED_IO_NUM) | (1ULL << LIGHT_THERAPY_BLUE_IO_NUM) | (1ULL << LIGHT_THERAPY_WHITE_IO_NUM),
    //     .mode = GPIO_MODE_OUTPUT,
    //     .pull_down_en = 0,
    //     .pull_up_en = 1,
    //     .intr_type = GPIO_INTR_DISABLE
    // };
    // gpio_config(&config);
    // gpio_set_level(LIGHT_THERAPY_GREEN_IO_NUM, 1);
    // gpio_set_level(LIGHT_THERAPY_RED_IO_NUM, 1);
    // gpio_set_level(LIGHT_THERAPY_BLUE_IO_NUM, 1);
    // gpio_set_level(LIGHT_THERAPY_WHITE_IO_NUM, 0);

    drv_ledc_tim_init();
    drv_ledc_channel_init();

    

    while(1)
    {
        drv_set_duty(LIGHT_THERAPY_RED_CHANNEL, 200);
        printf("green duty 200\n");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        drv_set_duty(LIGHT_THERAPY_RED_CHANNEL, 150);
        printf("green duty 150\n");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        drv_set_duty(LIGHT_THERAPY_RED_CHANNEL, 100);
        printf("green duty 100\n");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        drv_set_duty(LIGHT_THERAPY_RED_CHANNEL, 50);
        printf("green duty 50\n");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        drv_set_duty(LIGHT_THERAPY_RED_CHANNEL, 0);
        printf("green duty 0\n");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        drv_set_duty(LIGHT_THERAPY_RED_CHANNEL, 255);
        printf("green duty 255\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
