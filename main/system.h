#ifndef _SYS_TEM_H_
#define _SYS_TEM_H_

/* 标准库包含 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stdarg.h"
#include <stddef.h>

/* ESP32驱动库包含 */
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c_master.h"

/* FreeRTOS包含 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"

/* LEDC PWM配置 */
#define LEDC_TIMER              LEDC_TIMER_0        /*!< 使用定时器0 */
#define LEDC_MODE               LEDC_LOW_SPEED_MODE /*!< 低速模式 */
#define LEDC_FREQUENCY          5000                /*!< PWM频率: 5kHz */
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT   /*!< PWM分辨率: 13位 */

/* 电机控制通道定义 */
#define LEDC_CHANNEL_INA1       LEDC_CHANNEL_0      /*!< 电机A控制信号1通道 */
#define LEDC_CHANNEL_INA2       LEDC_CHANNEL_1      /*!< 电机A控制信号2通道 */
#define LEDC_CHANNEL_INB1       LEDC_CHANNEL_2      /*!< 电机B控制信号1通道 */
#define LEDC_CHANNEL_INB2       LEDC_CHANNEL_3      /*!< 电机B控制信号2通道 */
#define LEDC_CHANNEL_INC1       LEDC_CHANNEL_4      /*!< 电机C控制信号1通道 */
#define LEDC_CHANNEL_INC2       LEDC_CHANNEL_5      /*!< 电机C控制信号2通道 */

/* 电机控制引脚定义 */
#define LEDC_OUTPUT_IO_INA1     GPIO_NUM_7          /*!< 电机A控制信号1引脚 */
#define LEDC_OUTPUT_IO_INA2     GPIO_NUM_9          /*!< 电机A控制信号2引脚 */
#define LEDC_OUTPUT_IO_INB1     GPIO_NUM_47         /*!< 电机B控制信号1引脚 */
#define LEDC_OUTPUT_IO_INB2     GPIO_NUM_48         /*!< 电机B控制信号2引脚 */
#define LEDC_OUTPUT_IO_INC1     GPIO_NUM_45         /*!< 电机C控制信号1引脚 */
#define LEDC_OUTPUT_IO_INC2     GPIO_NUM_38         /*!< 电机C控制信号2引脚 */

/* XL9535 I2C配置 */
#define XL9535_I2C_MASTER_SCL_IO          GPIO_NUM_14 /*!< XL9535 SCL引脚 */
#define XL9535_I2C_MASTER_SDA_IO          GPIO_NUM_13 /*!< XL9535 SDA引脚 */
#define XL9535_I2C_MASTER_FREQ_HZ         400000      /*!< I2C频率: 400kHz */
#define XL9535_I2C_SLAVE_ADDR             0x20        /*!< XL9535从机地址 */
#define XL9535_I2C_MASTER_NUM             I2C_NUM_0   /*!< 使用I2C_1 */
#define XL9535_INT_PIN                    GPIO_NUM_12  /*!< XL9535中断引脚 */

/* RGB LED引脚定义 */
#define RGB_R1                         GPIO_NUM_2    /*!< RGB LED红色引脚 */
#define RGB_G1                         GPIO_NUM_1    /*!< RGB LED绿色引脚 */
#define RGB_B1                         GPIO_NUM_21   /*!< RGB LED蓝色引脚 */

#define ESP_INTR_FLAG_DEFAULT          0            /*!< 默认中断标志 */

/**
 * @brief 触摸按键编号枚举
 */
typedef enum {
    KEY_NUM_0 = 0,
    KEY_NUM_1,
    KEY_NUM_2,
    KEY_NUM_3,
    KEY_NUM_4,
    KEY_NUM_5,
    KEY_NUM_6,
    KEY_NUM_7,
} keyNumber_t;

/**
 * @brief XL9535 IO引脚枚举
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
 * @brief XL9535 IO状态枚举
 */
typedef enum __pinstate {
    IO_LOW = 0,     /*!< 低电平 */
    IO_HIGH = 1,    /*!< 高电平 */
    IO_UNKNOW,      /*!< 未知状态 */
} snPinState_t;

/**
 * @brief XL9535 IO模式枚举
 */
typedef enum __pinmode {
    IO_OUTPUT = 0,  /*!< 输出模式 */
    IO_INPUT = 1    /*!< 输入模式 */
} snPinMode_t;

/**
 * @brief XL9535 IO中断极性枚举
 */
typedef enum __pinpolarity {
    IO_NON_INVERTED = 0,  /*!< 不反转 */
    IO_INVERTED = 1       /*!< 反转 */
} snPinPolarity_t;

/**
 * @brief 电机状态枚举
 */
typedef enum {
    MOTOR_STOP = 0,       /*!< 停止 */
    MOTOR_FORWARD,        /*!< 正转 */
    MOTOR_BACKWARD        /*!< 反转 */
} motor_state_t;

/* 函数声明 */
// 系统初始化
void SystemInit(void);
// 切换状态指示灯
void Sys_running_status_TogglePin(snPinName_t pin);
// 测量光照强度任务
//void Sys_measure_light_task(void *arg);

#endif // _SYS_TEM_H_