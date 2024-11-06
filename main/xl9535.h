#ifndef __XL9535_H__
#define __XL9535_H__

#include "system.h"

#define ACK_CHECK_EN 0x1                        
#define ACK_CHECK_DIS 0x0                       
#define ACK_VAL 0x0                             
#define NACK_VAL 0x1                            

/* 控制寄存器 （CMD）*/
#define XL9535_INPUT_PORT0                     0x00 
#define XL9535_INPUT_PORT1                     0x01 
#define XL9535_OUTPUT_PORT0                    0x02
#define XL9535_OUTPUT_PORT1                    0x03
#define XL9535_POLARITY_INVERSION_PORT0        0x04 
#define XL9535_POLARITY_INVERSION_PORT1        0x05 
#define XL9535_CONFIG_PORT0                    0x06 
#define XL9535_CONFIG_PORT1                    0x07 

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
