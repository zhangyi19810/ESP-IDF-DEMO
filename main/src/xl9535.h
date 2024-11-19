//
// Created by ubuntu on 24-8-14.
//

#ifndef BGY001_101_XL9535_H
#define BGY001_101_XL9535_H

#include "My_system.h"

#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DIS 0x0
#define ACK_VAL 0x0
#define NACK_VAL 0x1

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

/* 控制寄存器 （CMD）*/
#define XL9535_INPUT_PORT0 0x00 // Input Port Register 0
#define XL9535_INPUT_PORT1 0x01 // Input Port Register 1
#define XL9535_OUTPUT_PORT0 0x02 // Output Port Register 0
#define XL9535_OUTPUT_PORT1 0x03 // Output Port Register 1
#define XL9535_POLARITY_INVERSION_PORT0 0x04 // Polarity Inversion Register 0
#define XL9535_POLARITY_INVERSION_PORT1 0x05 // Polarity Inversion Register 1
#define XL9535_CONFIG_PORT0 0x06 // Configuration Port 0
#define XL9535_CONFIG_PORT1 0x07 // Configuration Port 1

/* 外部函数引用 */
esp_err_t Xl9535_Init();
esp_err_t Xl9535_Set_Io_Direction(snPinName_t pinx, snPinMode_t newMode);
esp_err_t Xl9535_Set_Io_Status(snPinName_t pinx, snPinState_t newState);
esp_err_t Xl9535_Set_Input_Polarity(snPinName_t pinx, snPinPolarity_t newPolarity);
snPinState_t Xl9535_Get_Io_Status(snPinName_t pinx);
esp_err_t Xl9535_Read_Reg(uint8_t u8I2cSlaveAddr, uint8_t u8Cmd, uint8_t *pBuff, uint8_t u8Cnt);

// 添加新的函数声明
void Xl9535_Set_Io_Status_Multiple(snPinName_t pins[], int num_pins, snPinState_t newState);

#endif //BGY001_101_XL9535_H
