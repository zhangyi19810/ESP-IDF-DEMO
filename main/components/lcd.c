/**
 * @file lcd.c
 * @brief 软件SPI驱动LCD屏幕
 */
#include "lcd.h"

static const char *TAG = "LCD";

/**
 * @brief 短延时函数，用于SPI时序的微小延时
 * 
 * 替代原代码中的_nop_()函数，这里使用简单的空循环
 */
static inline void SPI_Delay(void)
{
    for(volatile int i = 0; i < 5; i++);
}

/**
 * @brief 初始化LCD的GPIO引脚
 */
void LCD_GPIO_Init(void)
{
    ESP_LOGI(TAG, "初始化LCD GPIO引脚");
    
    // 配置GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LCD_SPI_CS_PIN) | (1ULL << LCD_SPI_CLK_PIN) | (1ULL << LCD_SPI_DI_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    
    // 初始状态
    gpio_set_level(LCD_SPI_CS_PIN, 1);  // CS高电平
    gpio_set_level(LCD_SPI_CLK_PIN, 1); // CLK高电平
    gpio_set_level(LCD_SPI_DI_PIN, 1);  // DI高电平
}

/**
 * @brief SPI发送一个字节数据
 * 
 * @param data 要发送的数据
 */
void SPI_SendData(uint8_t data)
{
    for (uint8_t n = 0; n < 8; n++)
    {
        // 先发送高位，再发送低位
        if (data & 0x80)
        {
            gpio_set_level(LCD_SPI_DI_PIN, 1);
        }
        else
        {
            gpio_set_level(LCD_SPI_DI_PIN, 0);
        }
        data <<= 1;
        
        // 时钟脉冲
        gpio_set_level(LCD_SPI_CLK_PIN, 0);
        SPI_Delay();
        gpio_set_level(LCD_SPI_CLK_PIN, 1);
        SPI_Delay();
    }
}

/**
 * @brief 发送命令到LCD（3线9位SPI发送命令）
 * 
 * @param cmd 命令
 */
void SPI_WriteComm(uint8_t cmd)
{
    gpio_set_level(LCD_SPI_CS_PIN, 0);  // CS拉低，开始传输
    
    // 发送命令标志：第9位为0
    gpio_set_level(LCD_SPI_DI_PIN, 0);
    gpio_set_level(LCD_SPI_CLK_PIN, 0);
    SPI_Delay();
    gpio_set_level(LCD_SPI_CLK_PIN, 1);
    SPI_Delay();
    
    // 发送8位命令
    SPI_SendData(cmd);
    
    gpio_set_level(LCD_SPI_CS_PIN, 1);  // CS拉高，结束传输
}

/**
 * @brief 发送数据到LCD（3线9位SPI发送数据）
 * 
 * @param data 数据
 */
void SPI_WriteData(uint8_t data)
{
    gpio_set_level(LCD_SPI_CS_PIN, 0);  // CS拉低，开始传输
    
    // 发送数据标志：第9位为1
    gpio_set_level(LCD_SPI_DI_PIN, 1);
    gpio_set_level(LCD_SPI_CLK_PIN, 0);
    SPI_Delay();
    gpio_set_level(LCD_SPI_CLK_PIN, 1);
    SPI_Delay();
    
    // 发送8位数据
    SPI_SendData(data);
    
    gpio_set_level(LCD_SPI_CS_PIN, 1);  // CS拉高，结束传输
}

/**
 * @brief 初始化LCD
 */
void LCD_Init(void)
{
    ESP_LOGI(TAG, "初始化LCD");
    
    // 初始化GPIO
    LCD_GPIO_Init();
    
    //------display control setting------------//
    SPI_WriteComm(0xFE);
    SPI_WriteComm(0xEF);

    SPI_WriteComm(0xEB);
    SPI_WriteData(0x14);

    SPI_WriteComm(0x84);
    SPI_WriteData(0x61);

    SPI_WriteComm(0x85);
    SPI_WriteData(0xFF);

    SPI_WriteComm(0x86);
    SPI_WriteData(0xFF);

    SPI_WriteComm(0x87);
    SPI_WriteData(0xFF);

    SPI_WriteComm(0x8E);
    SPI_WriteData(0xFF);

    SPI_WriteComm(0x8F);
    SPI_WriteData(0xFF);

    SPI_WriteComm(0x88);
    SPI_WriteData(0x0A);

    SPI_WriteComm(0x89);
    SPI_WriteData(0x23);

    SPI_WriteComm(0x8A);
    SPI_WriteData(0x40);

    SPI_WriteComm(0x8B);
    SPI_WriteData(0x80);

    SPI_WriteComm(0x8C);
    SPI_WriteData(0x01);

    SPI_WriteComm(0x8D);
    SPI_WriteData(0x01);

    SPI_WriteComm(0xB6);
    SPI_WriteData(0x00);
    SPI_WriteData(0x00);

    SPI_WriteComm(0xb0);
    SPI_WriteData(0x40);

    SPI_WriteComm(0xF6);
    SPI_WriteData(0xc6);

    SPI_WriteComm(0xb5);
    SPI_WriteData(0x08);
    SPI_WriteData(0x09);
    SPI_WriteData(0x14);

    SPI_WriteComm(0x36);
    SPI_WriteData(0x40);

    SPI_WriteComm(0x3A);
    SPI_WriteData(0x66);

    SPI_WriteComm(0x90);
    SPI_WriteData(0x08);
    SPI_WriteData(0x08);
    SPI_WriteData(0x08);
    SPI_WriteData(0x08);

    SPI_WriteComm(0xBD);
    SPI_WriteData(0x06);

    SPI_WriteComm(0xBC);
    SPI_WriteData(0x00);

    SPI_WriteComm(0xFF);
    SPI_WriteData(0x60);
    SPI_WriteData(0x01);
    SPI_WriteData(0x04);

    SPI_WriteComm(0xC3);
    SPI_WriteData(0x1d);
    
    SPI_WriteComm(0xC4);
    SPI_WriteData(0x1d);

    SPI_WriteComm(0xC9);
    SPI_WriteData(0x25);

    SPI_WriteComm(0xBE);
    SPI_WriteData(0x11);

    SPI_WriteComm(0xE1);
    SPI_WriteData(0x10);
    SPI_WriteData(0x0E);

    SPI_WriteComm(0xDF);
    SPI_WriteData(0x21);
    SPI_WriteData(0x0c);
    SPI_WriteData(0x02);

    SPI_WriteComm(0xF0);
    SPI_WriteData(0x45);
    SPI_WriteData(0x09);
    SPI_WriteData(0x08);
    SPI_WriteData(0x08);
    SPI_WriteData(0x26);
    SPI_WriteData(0x2A);

    SPI_WriteComm(0xF1);
    SPI_WriteData(0x43);
    SPI_WriteData(0x70);
    SPI_WriteData(0x72);
    SPI_WriteData(0x36);
    SPI_WriteData(0x37);
    SPI_WriteData(0x6F);

    SPI_WriteComm(0xF2);
    SPI_WriteData(0x45);
    SPI_WriteData(0x09);
    SPI_WriteData(0x08);
    SPI_WriteData(0x08);
    SPI_WriteData(0x26);
    SPI_WriteData(0x2A);

    SPI_WriteComm(0xF3);
    SPI_WriteData(0x43);
    SPI_WriteData(0x70);
    SPI_WriteData(0x72);
    SPI_WriteData(0x36);
    SPI_WriteData(0x37);
    SPI_WriteData(0x6F);

    SPI_WriteComm(0xED);
    SPI_WriteData(0x1B);
    SPI_WriteData(0x0B);

    SPI_WriteComm(0xAE);
    SPI_WriteData(0x77);

    SPI_WriteComm(0xCD);
    SPI_WriteData(0x63);

    SPI_WriteComm(0x70);
    SPI_WriteData(0x07);
    SPI_WriteData(0x07);
    SPI_WriteData(0x04);
    SPI_WriteData(0x0E);
    SPI_WriteData(0x0F);
    SPI_WriteData(0x09);
    SPI_WriteData(0x07);
    SPI_WriteData(0x08);
    SPI_WriteData(0x03);

    SPI_WriteComm(0xE8);
    SPI_WriteData(0x34);

    SPI_WriteComm(0x60);
    SPI_WriteData(0x38);
    SPI_WriteData(0x0B);
    SPI_WriteData(0x6D);
    SPI_WriteData(0x6D);
    SPI_WriteData(0x39);
    SPI_WriteData(0xF0);
    SPI_WriteData(0x6D);
    SPI_WriteData(0x6D);

    SPI_WriteComm(0x61);
    SPI_WriteData(0x38);
    SPI_WriteData(0xF4);
    SPI_WriteData(0x6D);
    SPI_WriteData(0x6D);
    SPI_WriteData(0x38);
    SPI_WriteData(0xF7);
    SPI_WriteData(0x6D);
    SPI_WriteData(0x6D);

    SPI_WriteComm(0x62);
    SPI_WriteData(0x38);
    SPI_WriteData(0x0D);
    SPI_WriteData(0x71);
    SPI_WriteData(0xED);
    SPI_WriteData(0x70);
    SPI_WriteData(0x70);
    SPI_WriteData(0x38);
    SPI_WriteData(0x0F);
    SPI_WriteData(0x71);
    SPI_WriteData(0xEF);
    SPI_WriteData(0x70);
    SPI_WriteData(0x70);

    SPI_WriteComm(0x63);
    SPI_WriteData(0x38);
    SPI_WriteData(0x11);
    SPI_WriteData(0x71);
    SPI_WriteData(0xF1);
    SPI_WriteData(0x70);
    SPI_WriteData(0x70);
    SPI_WriteData(0x38);
    SPI_WriteData(0x13);
    SPI_WriteData(0x71);
    SPI_WriteData(0xF3);
    SPI_WriteData(0x70);
    SPI_WriteData(0x70);

    SPI_WriteComm(0x64);
    SPI_WriteData(0x28);
    SPI_WriteData(0x29);
    SPI_WriteData(0xF1);
    SPI_WriteData(0x01);
    SPI_WriteData(0xF1);
    SPI_WriteData(0x00);
    SPI_WriteData(0x07);

    SPI_WriteComm(0x66);
    SPI_WriteData(0x3C);
    SPI_WriteData(0x00);
    SPI_WriteData(0xCD);
    SPI_WriteData(0x67);
    SPI_WriteData(0x45);
    SPI_WriteData(0x45);
    SPI_WriteData(0x10);
    SPI_WriteData(0x00);
    SPI_WriteData(0x00);
    SPI_WriteData(0x00);

    SPI_WriteComm(0x67);
    SPI_WriteData(0x00);
    SPI_WriteData(0x3C);
    SPI_WriteData(0x00);
    SPI_WriteData(0x00);
    SPI_WriteData(0x00);
    SPI_WriteData(0x01);
    SPI_WriteData(0x54);
    SPI_WriteData(0x10);
    SPI_WriteData(0x32);
    SPI_WriteData(0x98);

    SPI_WriteComm(0x74);
    SPI_WriteData(0x10);
    SPI_WriteData(0x85);
    SPI_WriteData(0x80);
    SPI_WriteData(0x00);
    SPI_WriteData(0x00);
    SPI_WriteData(0x4E);
    SPI_WriteData(0x00);

    SPI_WriteComm(0x98);
    SPI_WriteData(0x3e);
    SPI_WriteData(0x07);

    SPI_WriteComm(0x21);
    
    SPI_WriteComm(0x11);
    LCD_DELAY(120);  // 延时120ms
    
    SPI_WriteComm(0x29);
    SPI_WriteComm(0x2c);

    ESP_LOGI(TAG, "LCD初始化完成");
}

/**
 * @brief 设置LCD显示窗口
 * 
 * @param x_start 起始X坐标
 * @param y_start 起始Y坐标
 * @param x_end 结束X坐标
 * @param y_end 结束Y坐标
 */
void LCD_SetWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    // 列地址设置
    SPI_WriteComm(0x2A);
    SPI_WriteData(x_start >> 8);     // 起始列地址高8位
    SPI_WriteData(x_start & 0xFF);   // 起始列地址低8位
    SPI_WriteData(x_end >> 8);       // 结束列地址高8位
    SPI_WriteData(x_end & 0xFF);     // 结束列地址低8位
    
    // 行地址设置
    SPI_WriteComm(0x2B);
    SPI_WriteData(y_start >> 8);     // 起始行地址高8位
    SPI_WriteData(y_start & 0xFF);   // 起始行地址低8位
    SPI_WriteData(y_end >> 8);       // 结束行地址高8位
    SPI_WriteData(y_end & 0xFF);     // 结束行地址低8位
    
    // 开始写入GRAM
    SPI_WriteComm(0x2C);
}

/**
 * @brief 在LCD指定位置画点
 * 
 * @param x X坐标
 * @param y Y坐标
 * @param color 16位RGB565颜色值
 */
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    // 设置窗口为一个点
    LCD_SetWindow(x, y, x, y);
    
    // 发送颜色数据
    SPI_WriteData(color >> 8);    // 颜色高8位
    SPI_WriteData(color & 0xFF);  // 颜色低8位
}

/**
 * @brief 填充指定区域为某种颜色
 * 
 * @param x_start 起始X坐标
 * @param y_start 起始Y坐标
 * @param x_end 结束X坐标
 * @param y_end 结束Y坐标
 * @param color 16位RGB565颜色值
 */
void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t i, j;
    uint32_t pixel_count = (x_end - x_start + 1) * (y_end - y_start + 1);
    
    // 设置窗口
    LCD_SetWindow(x_start, y_start, x_end, y_end);
    
    // 填充颜色
    for (i = 0; i < pixel_count; i++) {
        SPI_WriteData(color >> 8);    // 颜色高8位
        SPI_WriteData(color & 0xFF);  // 颜色低8位
    }
}

/**
 * @brief 清屏，将整个屏幕填充为指定颜色
 * 
 * @param color 16位RGB565颜色值
 */
void LCD_Clear(uint16_t color)
{
    LCD_Fill(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, color);
}

/**
 * @brief 颜色测试，绘制彩色测试图案
 */
void LCD_ColorTest(void)
{
    ESP_LOGI(TAG, "开始颜色测试");
    
    uint16_t colors[] = {
        COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, 
        COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE, COLOR_BLACK
    };
    uint8_t num_colors = sizeof(colors) / sizeof(colors[0]);
    
    // 清屏，黑底
    LCD_Clear(COLOR_BLACK);
    LCD_DELAY(500);
    
    // 1. 全屏颜色测试
    for (uint8_t i = 0; i < num_colors; i++) {
        LCD_Clear(colors[i]);
        LCD_DELAY(500);  // 每种颜色显示500ms
    }
    
    // 2. 彩色条纹测试
    uint16_t block_height = LCD_HEIGHT / num_colors;
    for (uint8_t i = 0; i < num_colors; i++) {
        LCD_Fill(0, i * block_height, LCD_WIDTH - 1, (i + 1) * block_height - 1, colors[i]);
    }
    LCD_DELAY(1000);  // 显示1秒
    
    // 3. 方格测试
    uint16_t block_width = LCD_WIDTH / 4;
    block_height = LCD_HEIGHT / 2;
    uint8_t color_idx = 0;
    
    for (uint16_t y = 0; y < 2; y++) {
        for (uint16_t x = 0; x < 4; x++) {
            LCD_Fill(x * block_width, y * block_height, 
                    (x + 1) * block_width - 1, (y + 1) * block_height - 1, 
                    colors[color_idx % num_colors]);
            color_idx++;
        }
    }
    LCD_DELAY(1000);  // 显示1秒
    
    // 4. 渐变色测试 (红色渐变)
    for (uint16_t i = 0; i < 32; i++) {
        uint16_t red_intensity = i * 8;  // 0-255
        uint16_t color = (red_intensity << 8) & 0xF800;  // 转为RGB565格式
        uint16_t block_width = LCD_WIDTH / 32;
        LCD_Fill(i * block_width, 0, (i + 1) * block_width - 1, LCD_HEIGHT / 3 - 1, color);
    }
    
    // 绿色渐变
    for (uint16_t i = 0; i < 64; i++) {
        uint16_t green_intensity = i * 4;  // 0-255
        uint16_t color = (green_intensity << 3) & 0x07E0;  // 转为RGB565格式
        uint16_t block_width = LCD_WIDTH / 64;
        LCD_Fill(i * block_width, LCD_HEIGHT / 3, (i + 1) * block_width - 1, 2 * LCD_HEIGHT / 3 - 1, color);
    }
    
    // 蓝色渐变
    for (uint16_t i = 0; i < 32; i++) {
        uint16_t blue_intensity = i * 8;  // 0-255
        uint16_t color = (blue_intensity >> 3) & 0x001F;  // 转为RGB565格式
        uint16_t block_width = LCD_WIDTH / 32;
        LCD_Fill(i * block_width, 2 * LCD_HEIGHT / 3, (i + 1) * block_width - 1, LCD_HEIGHT - 1, color);
    }
    
    ESP_LOGI(TAG, "颜色测试完成");
}
