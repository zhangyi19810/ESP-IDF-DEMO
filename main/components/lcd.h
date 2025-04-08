/**
 * @file lcd.h
 * @brief 软件SPI驱动LCD屏幕
 */
#ifndef _LCD_H_
#define _LCD_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// 定义LCD引脚 (根据您的硬件连接进行修改)
#define LCD_SPI_CS_PIN     GPIO_NUM_42   // CS芯片选择引脚
#define LCD_SPI_CLK_PIN    GPIO_NUM_41  // CLK时钟引脚
#define LCD_SPI_DI_PIN     GPIO_NUM_40  // DI/SDA数据引脚

// LCD屏幕尺寸
#define LCD_WIDTH  240
#define LCD_HEIGHT 240

// 延时函数
#define LCD_DELAY(ms) vTaskDelay(ms / portTICK_PERIOD_MS)

// 16位颜色定义 (RGB565)
#define COLOR_BLACK        0x0000  // 黑色    0,   0,   0
#define COLOR_NAVY         0x000F  // 深蓝色  0,   0, 127
#define COLOR_DARKGREEN    0x03E0  // 深绿色  0, 127,   0
#define COLOR_DARKCYAN     0x03EF  // 深青色  0, 127, 127
#define COLOR_MAROON       0x7800  // 深红色127,   0,   0
#define COLOR_PURPLE       0x780F  // 紫色  127,   0, 127
#define COLOR_OLIVE        0x7BE0  // 橄榄色127, 127,   0
#define COLOR_LIGHTGREY    0xC618  // 灰色  192, 192, 192
#define COLOR_DARKGREY     0x7BEF  // 深灰色127, 127, 127
#define COLOR_BLUE         0x001F  // 蓝色    0,   0, 255
#define COLOR_GREEN        0x07E0  // 绿色    0, 255,   0
#define COLOR_CYAN         0x07FF  // 青色    0, 255, 255
#define COLOR_RED          0xF800  // 红色  255,   0,   0
#define COLOR_MAGENTA      0xF81F  // 品红  255,   0, 255
#define COLOR_YELLOW       0xFFE0  // 黄色  255, 255,   0
#define COLOR_WHITE        0xFFFF  // 白色  255, 255, 255
#define COLOR_ORANGE       0xFD20  // 橙色  255, 165,   0
#define COLOR_GREENYELLOW  0xAFE5  // 黄绿色173, 255,  41

// 函数声明
/**
 * @brief 初始化LCD的GPIO引脚
 */
void LCD_GPIO_Init(void);

/**
 * @brief SPI发送一个字节数据
 * 
 * @param data 要发送的数据
 */
void SPI_SendData(uint8_t data);

/**
 * @brief 发送命令到LCD
 * 
 * @param cmd 命令
 */
void SPI_WriteComm(uint8_t cmd);

/**
 * @brief 发送数据到LCD
 * 
 * @param data 数据
 */
void SPI_WriteData(uint8_t data);

/**
 * @brief 初始化LCD
 */
void LCD_Init(void);

/**
 * @brief 设置LCD显示窗口
 * 
 * @param x_start 起始X坐标
 * @param y_start 起始Y坐标
 * @param x_end 结束X坐标
 * @param y_end 结束Y坐标
 */
void LCD_SetWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

/**
 * @brief 在LCD指定位置画点
 * 
 * @param x X坐标
 * @param y Y坐标
 * @param color 16位RGB565颜色值
 */
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief 填充指定区域为某种颜色
 * 
 * @param x_start 起始X坐标
 * @param y_start 起始Y坐标
 * @param x_end 结束X坐标
 * @param y_end 结束Y坐标
 * @param color 16位RGB565颜色值
 */
void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);

/**
 * @brief 清屏，将整个屏幕填充为指定颜色
 * 
 * @param color 16位RGB565颜色值
 */
void LCD_Clear(uint16_t color);

/**
 * @brief 颜色测试，绘制彩色测试图案
 */
void LCD_ColorTest(void);

#endif /* _LCD_H_ */
