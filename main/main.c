#include "system.h"
#include "xl9535.h"

#define VOICE_CLK_IO_NUM                            GPIO_NUM_41             /*!< 语音时钟线IO */
#define VOICE_SDA_IO_NUM                            GPIO_NUM_17             /*!< 语音时数据IO */
#define VOICE_BUSY_IO_NUM                           GPIO_NUM_48             /*!< 语音时中断IO */

void voice_init(void)
{
    gpio_config_t voice_io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << VOICE_CLK_IO_NUM) | (1ULL << VOICE_SDA_IO_NUM),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    gpio_config(&voice_io_conf);

    gpio_set_level(VOICE_CLK_IO_NUM, 1);
    gpio_set_level(VOICE_SDA_IO_NUM, 1);

    Xl9535_Init();
    Xl9535_Set_Io_Direction(PIN_P00, IO_OUTPUT);
    Xl9535_Set_Io_Status(PIN_P00, IO_HIGH);
}

void voice_send_data(uint8_t data)
{
    uint8_t b_data;
    b_data = data & 0x01; // 获取待发送数据的最低位
    gpio_set_level(VOICE_CLK_IO_NUM, 1);
    gpio_set_level(VOICE_SDA_IO_NUM, 1);

    gpio_set_level(VOICE_CLK_IO_NUM, 0);
    esp_rom_delay_us(5000);

    for(int i=0; i<8; i++)
    {
        gpio_set_level(VOICE_CLK_IO_NUM,0); // 时钟线拉低
        gpio_set_level(VOICE_SDA_IO_NUM, b_data);
        esp_rom_delay_us(1000);

        gpio_set_level(VOICE_CLK_IO_NUM,1);
        esp_rom_delay_us(1000);

        data = data >> 1; // data右移1位，相当于删除原本最低位的数据
        b_data = data & 0x01; // 取处理后data的最后一位
    }

    // 数据发送完成之后恢复高电平
    gpio_set_level(VOICE_CLK_IO_NUM,1);
    gpio_set_level(VOICE_SDA_IO_NUM,1);
}

void app_main(void)
{
    voice_init();

    voice_send_data(0x08);
    
}
