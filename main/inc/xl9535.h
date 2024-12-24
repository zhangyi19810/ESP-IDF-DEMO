#ifndef __DRV_XL9535_H__
#define __DRV_XL9535_H__

#include "system.h"



/* 控制寄存器 （CMD）*/
#define XL9535_INPUT_PORT0                   0x00 // Input Port Register 0
#define XL9535_INPUT_PORT1                   0x01 // Input Port Register 1
#define XL9535_OUTPUT_PORT0                  0x02 // Output Port Register 0
#define XL9535_OUTPUT_PORT1                  0x03 // Output Port Register 1
#define XL9535_POLARITY_INVERSION_PORT0      0x04 // Polarity Inversion Register 0
#define XL9535_POLARITY_INVERSION_PORT1      0x05 // Polarity Inversion Register 1
#define XL9535_CONFIG_PORT0                  0x06 // Configuration Port 0
#define XL9535_CONFIG_PORT1                  0x07 // Configuration Port 1

#define ACK_CHECK_EN    (0x1)                        
#define ACK_CHECK_DIS   (0x0)                       
#define ACK_VAL         (0x0)                             
#define NACK_VAL        (0x1) 

// 初始化XL9535拓展IO芯片
esp_err_t xl9535_Init(void);    

// 设置XL9535拓展IO芯片的IO状态
esp_err_t xl9535_Set_Io_Status(snPinName_t pinx, snPinState_t newState);

// 设置XL9535拓展IO芯片的IO方向
esp_err_t xl9535_Set_Io_Direction(snPinName_t pinx, snPinMode_t newMode);

// 设置XL9535拓展IO芯片的IO极性
esp_err_t xl9535_Set_Input_Polarity(snPinName_t pinx, snPinPolarity_t newPolarity);

// 获取XL9535拓展IO芯片的IO状态
snPinState_t xl9535_Get_Io_Status(snPinName_t pinx);

#endif
