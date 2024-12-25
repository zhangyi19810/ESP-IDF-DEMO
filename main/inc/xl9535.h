#ifndef __XL9535_H__
#define __XL9535_H__

#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"

#define ACK_CHECK_EN 0x1                        
#define ACK_CHECK_DIS 0x0                       
#define ACK_VAL 0x0                             
#define NACK_VAL 0x1        

/* XL9535 I2C配置 */
#define XL9535_I2C_MASTER_SCL_IO          GPIO_NUM_7 /*!< XL9535 SCL引脚 */
#define XL9535_I2C_MASTER_SDA_IO          GPIO_NUM_6 /*!< XL9535 SDA引脚 */
#define XL9535_I2C_MASTER_FREQ_HZ         400000      /*!< I2C频率: 400kHz */
#define XL9535_I2C_SLAVE_ADDR             0x20        /*!< XL9535从机地址 */

#define XL9535_I2C_MASTER_NUM             LP_I2C_NUM_0   /*!< 使用I2C_0 */

#define XL9535_INT_PIN                    GPIO_NUM_5  /*!< XL9535中断引脚 */

/* 控制寄存器 （CMD）*/
#define XL9535_INPUT_PORT0                     0x00 
#define XL9535_INPUT_PORT1                     0x01 
#define XL9535_OUTPUT_PORT0                    0x02
#define XL9535_OUTPUT_PORT1                    0x03
#define XL9535_POLARITY_INVERSION_PORT0        0x04 
#define XL9535_POLARITY_INVERSION_PORT1        0x05 
#define XL9535_CONFIG_PORT0                    0x06 
#define XL9535_CONFIG_PORT1                    0x07 

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

/* 外部函数引用 */
// 读取寄存器
esp_err_t Xl9535_Read_Reg(uint8_t u8I2cSlaveAddr, uint8_t u8Cmd, uint8_t *pBuff, uint8_t u8Cnt);
// 初始化
esp_err_t Xl9535_Init(void);
// 设置引脚方向
esp_err_t Xl9535_Set_Io_Direction(snPinName_t pinx, snPinMode_t newMode);
// 设置引脚状态
esp_err_t Xl9535_Set_Io_Status(snPinName_t pinx, snPinState_t newState);
// 设置引脚极性
esp_err_t Xl9535_Set_Input_Polarity(snPinName_t pinx, snPinPolarity_t newPolarity);
// 获取引脚状态
snPinState_t Xl9535_Get_Io_Status(snPinName_t pinx);

// 添加新的函数声明
void Xl9535_Set_Io_Status_Multiple(snPinName_t pins[], int num_pins, snPinState_t newState);
// 重新初始化
esp_err_t xl9535_reinit(void);

#endif
