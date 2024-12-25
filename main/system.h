#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "driver/gpio.h"
#include "camera.h"
#include "usb_stream.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "driver/i2c.h"

#define EN_GPIO          GPIO_NUM_1

#define OE_GPIO          PIN_P00     // oe
#define SEL_GPIO         PIN_P01     // sel引脚，用于切换摄像头
#define DC_EN_GPIO       PIN_P07     // dc使能引脚

/* XL9535拓展IO IIC协议相关配置 */
#define XL9535_I2C_MASTER_SCL_IO                    (GPIO_NUM_7)             /*!< 拓展IO芯片IIC协议时钟线IO */
#define XL9535_I2C_MASTER_SDA_IO                    (GPIO_NUM_6)             /*!< 拓展IO芯片IIC协议数据线IO */
#define XL9535_I2C_MASTER_FREQ_HZ                   (400000)                  /*!< 拓展IO芯片IIC协议频率 */
#define XL9535_I2C_MASTER_TX_BUF_DISABLE            (0)                       /*!< 拓展IO芯片IIC协议发送缓存 */
#define XL9535_I2C_MASTER_RX_BUF_DISABLE            (0)                       /*!< 拓展IO芯片IIC协议接收缓存 */
#define XL9535_I2C_SLAVE_ADDR                       (0x20)                    /*!< 拓展IO芯片IIC协议地址 */
#define XL9535_I2C_MASTER_NUM                       (I2C_NUM_1)               /*!< 拓展IO芯片IIC协议端口号 */

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

void system_init(void);

#endif
