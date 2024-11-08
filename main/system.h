#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/i2c.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

// NVS相关头文件
#include "nvs.h"
#include "nvs_flash.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stdarg.h"
#include <stddef.h>

/**********************************板载相关宏定义*************************************************/
/**
 * @brief 按键并行数据IO
 */
#define TOUCH_BIN0                                  PIN_P03                 /*!< 触摸按键芯片BIN0数据口 */
#define TOUCH_BIN1                                  PIN_P04                 /*!< 触摸按键芯片BIN1数据口 */
#define TOUCH_BIN2                                  PIN_P05                 /*!< 触摸按键芯片BIN2数据口 */
#define TOUCH_BIN3                                  GPIO_NUM_21             /*!< 触摸按键芯片BIN3校验口 */

/** 
 * @brief LEDC定时器相关定义
 */ 
#define LEDC_LIGHT_TIMER                            LEDC_TIMER_0            /*!< LEDC定时器 */
#define LEDC_LIGHT_TIMER_40HZ                       LEDC_TIMER_1            /*!< LEDC40hz定时器 */
#define LEDC_LIGHT_FREQ                             (10000)                 /*!< LEDC频率 */
#define LEDC_LIGHT_FREQ_40HZ                        (40)                    /*!< LEDC40hz频率 */
#define LEDC_LIGHT_RESOLUTION                       LEDC_TIMER_8_BIT        /*!< LEDC分辨率 */
#define LEDC_LIGHT_RESOLUTION_40HZ                  LEDC_TIMER_13_BIT
#define LEDC_LIGHT_MODE                             LEDC_LOW_SPEED_MODE     /*!< LEDC模式 */

#define BUZZER_TIMER                                LEDC_LIGHT_TIMER_40HZ
#define BUZZER_MODE                                 LEDC_LOW_SPEED_MODE
 
/** 
 * @brief 灯板LEDC IO 
 */ 
#define ROUND_LIGHT_PWM_IO_NUM                      GPIO_NUM_42             /*!< 氛围灯板PWM IO引脚 */
#define TOP_LIGHT_PWM_IO_NUM                        GPIO_NUM_40             /*!< 上发光灯板PWM IO引脚 */
#define LOWER_LIGHT_PWM_IO_NUM                      GPIO_NUM_39             /*!< 下发光灯板PWM IO引脚 */
#define LIGHT_THERAPY_RED_IO_NUM                    GPIO_NUM_11             /*!< 光疗灯板PWM R IO引脚 */
#define LIGHT_THERAPY_GREEN_IO_NUM                  GPIO_NUM_1              /*!< 光疗灯板PWM G IO引脚 */
#define LIGHT_THERAPY_BLUE_IO_NUM                   GPIO_NUM_2              /*!< 光疗灯板PWM B IO引脚 */
#define LIGHT_THERAPY_WHITE_IO_NUM                  GPIO_NUM_10             /*!< 光疗灯板PWM W IO引脚 */
#define BUZZER_IO_NUM                               GPIO_NUM_47             /*!< 蜂鸣器PWM IO引脚 */

/**
 * @brief 灯板LEDC通道
 */
#define LOWER_LIGHT_PWM_CHANNEL                     LEDC_CHANNEL_0          /*!< 下发光灯板PWM通道 */
#define TOP_LIGHT_PWM_CHANNEL                       LEDC_CHANNEL_1          /*!< 上发光灯板PWM通道 */
#define ROUND_LIGHT_PWM_CHANNEL                     LEDC_CHANNEL_2          /*!< 氛围灯板PWM通道 */
#define LIGHT_THERAPY_RED_CHANNEL                   LEDC_CHANNEL_3          /*!< 光疗灯板PWM R 通道 */
#define LIGHT_THERAPY_GREEN_CHANNEL                 LEDC_CHANNEL_4          /*!< 光疗灯板PWM G 通道 */
#define LIGHT_THERAPY_BLUE_CHANNEL                  LEDC_CHANNEL_5          /*!< 光疗灯板PWM B 通道 */
#define LIGHT_THERAPY_WHITE_CHANNEL                 LEDC_CHANNEL_6          /*!< 光疗灯板PWM W 通道 */
#define BUZZER_PWM_CHANNEL                          LEDC_CHANNEL_7          /*!< 蜂鸣器PWM通道 */
 
/**
 * @brief XL9535拓展IO IIC协议相关配置
 */
#define XL9535_I2C_MASTER_SCL_IO                    GPIO_NUM_14             /*!< 拓展IO芯片IIC协议时钟线IO */
#define XL9535_I2C_MASTER_SDA_IO                    GPIO_NUM_13             /*!< 拓展IO芯片IIC协议数据线IO */
#define XL9535_I2C_MASTER_FREQ_HZ                   (400000)                /*!< 拓展IO芯片IIC协议频率 */
#define XL9535_I2C_MASTER_TX_BUF_DISABLE            (0)                 
#define XL9535_I2C_MASTER_RX_BUF_DISABLE            (0)
#define XL9535_I2C_SLAVE_ADDR                       (0x20)                  /*!< 拓展IO芯片IIC协议地址 */
#define XL9535_I2C_MASTER_NUM                       I2C_NUM_1               /*!< 拓展IO芯片IIC协议端口号 */

// 亮度档位占空比定义
#define BRIGHTNESS_DUTY_10                          26                      /*!< 10%亮度 */
#define BRIGHTNESS_DUTY_40                          102                     /*!< 40%亮度 */
#define BRIGHTNESS_DUTY_60                          153                     /*!< 60%亮度 */
#define BRIGHTNESS_DUTY_80                          204                     /*!< 80%亮度 */
#define BRIGHTNESS_DUTY_100                         255                     /*!< 100%亮度 */

#define BRIGHTNESS_DUTY_40HZ_10                     (26 * 32)               /*!< 40HZ 10%亮度 */
#define BRIGHTNESS_DUTY_40HZ_40                     (102 * 32)              /*!< 40HZ 40%亮度 */
#define BRIGHTNESS_DUTY_40HZ_60                     (153 * 32)              /*!< 40HZ 60%亮度 */
#define BRIGHTNESS_DUTY_40HZ_80                     (204 * 32)              /*!< 40HZ 80%亮度 */
#define BRIGHTNESS_DUTY_40HZ_100                    (255 * 32)              /*!< 40HZ 100%亮度 */

#define LIGHT_THERAPY_MODE_NUM                      (6)                     /*!< 光疗灯模式数量 */

#define PIR_SENSOR_IO_NUM                           GPIO_NUM_18             /*!< 人体检测传感器 */

#define EVN_LIGHT_SENSOR_CH                         ADC_CHANNEL_0   

#define VOICE_CLK_IO_NUM                            GPIO_NUM_41
#define VOICE_SDA_IO_NUM                            GPIO_NUM_17
#define VOICE_BUSY_IO_NUM                           GPIO_NUM_48

/**********************************板载相关宏定义*************************************************/

/**********************************系统相关枚举定义***********************************************/
/**
 * @brief 触摸按键按键号定义
 */
typedef enum{
    KEY_NUM_0 = 0,
    KEY_NUM_1,
    KEY_NUM_2,
    KEY_NUM_3,
    KEY_NUM_4,
    KEY_NUM_5,
    KEY_NUM_6,
    KEY_NUM_7,
}keyNumber_t;

/**
 * @brief 拓展IO芯片IO口定义
 */
typedef enum __pinname {
    PIN_P00 = 0,
    PIN_P01,
    PIN_P02,
    PIN_P03,
    PIN_P04,
    PIN_P05,
    PIN_P06,
    PIN_P07,
    PIN_P10,
    PIN_P11,
    PIN_P12,
    PIN_P13,
    PIN_P14,
    PIN_P15,
    PIN_P16,
    PIN_P17
} snPinName_t;

/**
 * @brief 拓展IO芯片IO高低电平定义
 */
typedef enum __pinstate {
    IO_LOW = 0,
    IO_HIGH = 1,
    IO_UNKNOW,
} snPinState_t;

/**
 * @brief 拓展IO芯片IO输出模式定义
 */
typedef enum __pinmode {
    IO_OUTPUT = 0,
    IO_INPUT = 1
} snPinMode_t;

/**
 * @brief 拓展IO芯片IO中断定义
 */
typedef enum __pinpolarity {
    IO_NON_INVERTED = 0,
    IO_INVERTED = 1
} snPinPolarity_t;

/**
 * @brief 按键标志为定义
 */
typedef enum {
    KEY_PRESSED = 0,  
    KEY_RELEASED  = 1   
} keyState_t;

/**
 * @brief 亮度档位
 */
typedef enum {
    BRIGHTNESS_LEVEL_10 = 0,   // 10%亮度
    BRIGHTNESS_LEVEL_40,       // 40%亮度
    BRIGHTNESS_LEVEL_60,       // 60%亮度
    BRIGHTNESS_LEVEL_80,       // 80%亮度
    BRIGHTNESS_LEVEL_100,      // 100%亮度
    BRIGHTNESS_LEVEL_MAX       // MAX
} brightnessLevel_t;

// 灯光状态机
typedef enum {
    STATE_LIGHT_OFF,
    STATE_LIGHT_ON
} LightState_t;

/**********************************系统相关枚举定义***********************************************/

/**********************************系统相关结构体定义*********************************************/
typedef struct 
{
    uint8_t touch_key_id;
    bool flag;
}keyData_t;

// 灯光控制结构体
typedef struct {
    bool state;
    LightState_t light_state; // 状态机当前状态
    brightnessLevel_t brightness;
    uint8_t pwm_channel_1;
    uint8_t pwm_channel_2;
    uint8_t indicator_pin;
} LightControl_t;

/**********************************系统相关结构体定义*********************************************/

#define BLUFI_EXAMPLE_TAG "BLUFI_DEMO"
#define BLUFI_INFO(fmt, ...)   ESP_LOGI(BLUFI_EXAMPLE_TAG, fmt, ##__VA_ARGS__)
#define BLUFI_ERROR(fmt, ...)  ESP_LOGE(BLUFI_EXAMPLE_TAG, fmt, ##__VA_ARGS__)

// 系统初始化
void systemInit(void);

#endif // _SYSTEM_H_