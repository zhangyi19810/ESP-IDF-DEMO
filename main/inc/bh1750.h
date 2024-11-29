#ifndef __BH1750_H__
#define __BH1750_H__

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

// I2C配置相关宏定义
#define I2C_MASTER_SCL_IO GPIO_NUM_8           /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO GPIO_NUM_3           /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM 1                        /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 50000               /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0             /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0             /*!< I2C master doesn't need buffer */

// I2C读写位定义
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */

// BH1750相关宏定义
#define BH1750_SLAVE_ADDR_L   0x23  // 低地址
#define BH1750_SLAVE_ADDR_H   0x5C  // 高地址
#define BH1750_PWR_DOWN     0x00    // 关闭模块
#define BH1750_PWR_ON       0x01    // 打开模块等待测量指令
#define BH1750_RST          0x07    // 重置数据寄存器值在PowerOn模式下有效
#define BH1750_CON_H        0x10    // 连续高分辨率模式，1lx，120ms
#define BH1750_CON_H2       0x11    // 连续高分辨率模式，0.5lx，120ms
#define BH1750_CON_L        0x13    // 连续低分辨率模式，4lx，16ms
#define BH1750_ONE_H        0x20    // 一次高分辨率模式，1lx，120ms
#define BH1750_ONE_H2       0x21    // 一次高分辨率模式，0.5lx，120ms
#define BH1750_ONE_L        0x23    // 一次低分辨率模式，4lx，16ms

// 函数声明
int I2C_Init(void);
int I2C_WriteData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen);
int I2C_ReadData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen);
void BH1750_Init_L(void);
void BH1750_Init_H(void);
float BH1750_ReadLightIntensity_L(void);
float BH1750_ReadLightIntensity_H(void);
void i2c_scanner(void);
void i2c_reinit(void);

#endif /* __BH1750_H__ */ 