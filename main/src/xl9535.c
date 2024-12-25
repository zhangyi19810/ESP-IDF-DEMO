#include "xl9535.h"
#include "esp_log.h"
#include "driver/i2c.h"

static const char *TAG = "Xl9535";

esp_err_t Xl9535_Init(void)
{
    int i2c_master_port = XL9535_I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = XL9535_I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = XL9535_I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = XL9535_I2C_MASTER_FREQ_HZ,
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2c_param_config failed: %s", esp_err_to_name(err));
        return err;
    }
    err = i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2c_driver_install failed: %s", esp_err_to_name(err));
    }
    uint8_t current_state;
    err = Xl9535_Read_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_INPUT_PORT0, &current_state, 2);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Xl9535_Read_Reg failed: %s", esp_err_to_name(err));
    }
    return err;
}

esp_err_t Xl9535_Write_Reg(uint8_t u8I2cSlaveAddr, uint8_t u8Cmd, uint8_t u8Value)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (u8I2cSlaveAddr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, u8Cmd, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, u8Value, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(XL9535_I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C Write Reg Error: %s, Addr: 0x%02X, Cmd: 0x%02X, Value: 0x%02X", esp_err_to_name(ret), u8I2cSlaveAddr, u8Cmd, u8Value);
    }
    return ret;
}

esp_err_t Xl9535_Read_Reg(uint8_t u8I2cSlaveAddr, uint8_t u8Cmd, uint8_t *pBuff, uint8_t u8Cnt)
{
    esp_err_t ret;

    i2c_cmd_handle_t wr_cmd = i2c_cmd_link_create();
    i2c_master_start(wr_cmd);
    i2c_master_write_byte(wr_cmd, (u8I2cSlaveAddr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(wr_cmd, u8Cmd, ACK_CHECK_EN);
    i2c_master_stop(wr_cmd);
    ret = i2c_master_cmd_begin(XL9535_I2C_MASTER_NUM, wr_cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(wr_cmd);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C Write Cmd Error: %s, Addr: 0x%02X, Cmd: 0x%02X", esp_err_to_name(ret), u8I2cSlaveAddr, u8Cmd);
        return ret;
    }

    i2c_cmd_handle_t rd_cmd = i2c_cmd_link_create();
    i2c_master_start(rd_cmd);
    i2c_master_write_byte(rd_cmd, (u8I2cSlaveAddr << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read(rd_cmd, pBuff, u8Cnt, I2C_MASTER_LAST_NACK);
    i2c_master_stop(rd_cmd);
    ret = i2c_master_cmd_begin(XL9535_I2C_MASTER_NUM, rd_cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(rd_cmd);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C Read Error: %s, Addr: 0x%02X, Cmd: 0x%02X", esp_err_to_name(ret), u8I2cSlaveAddr, u8Cmd);
    }

    return ret;
}

esp_err_t Xl9535_Set_Io_Direction(snPinName_t pinx, snPinMode_t newMode)
{
    esp_err_t ret;
    uint8_t current_mode[2];

    ret = Xl9535_Read_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_CONFIG_PORT0, current_mode, 2);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read config registers: %s", esp_err_to_name(ret));
        return ret;
    }

    if (newMode == IO_OUTPUT)
    {
        if (pinx <= PIN_P07)
        {
            current_mode[0] &= ~(1 << pinx);
        }
        else
        {
            current_mode[1] &= ~(1 << (pinx - 8));
        }
    }
    else
    {
        if (pinx <= PIN_P07)
        {
            current_mode[0] |= (1 << pinx);
        }
        else
        {
            current_mode[1] |= (1 << (pinx - 8));
        }
    }

    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_CONFIG_PORT0, current_mode[0]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write config register 0: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_CONFIG_PORT1, current_mode[1]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write config register 1: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t Xl9535_Set_Io_Status(snPinName_t pinx, snPinState_t newState)
{
    esp_err_t ret;
    uint8_t current_state[2];

    ret = Xl9535_Read_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_OUTPUT_PORT0, current_state, 2);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read output registers: %s", esp_err_to_name(ret));
        return ret;
    }

    if (pinx <= PIN_P07)
    {
        if (newState == IO_LOW)
        {
            current_state[0] &= ~(1 << pinx);
        }
        else
        {
            current_state[0] |= (1 << pinx);
        }
    }
    else
    {
        if (newState == IO_LOW)
        {
            current_state[1] &= ~(1 << (pinx - 8));
        }
        else
        {
            current_state[1] |= (1 << (pinx - 8));
        }
    }

    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_OUTPUT_PORT0, current_state[0]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write output register 0: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_OUTPUT_PORT1, current_state[1]);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write output register 1: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t Xl9535_Set_Input_Polarity(snPinName_t pinx, snPinPolarity_t newPolarity)
{
    esp_err_t ret;
    uint8_t current_state[2];
    ret = Xl9535_Read_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_POLARITY_INVERSION_PORT0, current_state, 2);
    if (ret != ESP_OK)
    {
        return ret;
    }

    if (pinx <= PIN_P07)
    {
        if (newPolarity == IO_NON_INVERTED)
        {
            current_state[0] &= ~(1 << pinx);
        }
        else
        {
            current_state[0] |= (1 << pinx);
        }
    }
    else
    {
        if (newPolarity == IO_NON_INVERTED)
        {
            current_state[1] &= ~(1 << (pinx - 8));
        }
        else
        {
            current_state[1] |= (1 << (pinx - 8));
        }
    }

    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_POLARITY_INVERSION_PORT0, current_state[0]);
    if (ret != ESP_OK)
    {
        return ret;
    }
    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_POLARITY_INVERSION_PORT1, current_state[1]);
    return ret;
}

snPinState_t Xl9535_Get_Io_Status(snPinName_t pinx)
{
    esp_err_t ret;
    uint8_t current_state[2];
    ret = Xl9535_Read_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_INPUT_PORT0, current_state, 2);

    if (ret == ESP_OK)
    {
        if (pinx <= PIN_P07)
        {
            if (current_state[0] & (1 << pinx))
            {
                return IO_HIGH;
            }
            else
            {
                return IO_LOW;
            }
        }
        else
        {
            if (current_state[1] & (1 << (pinx - 8)))
            {
                return IO_HIGH;
            }
            else
            {
                return IO_LOW;
            }
        }
    }
    else
    {
        return IO_UNKNOW;
    }
}

void Xl9535_Set_Io_Status_Multiple(snPinName_t pins[], int num_pins, snPinState_t newState)
{
    for (int i = 0; i < num_pins; i++)
    {
        Xl9535_Set_Io_Direction(pins[i], IO_OUTPUT);
        Xl9535_Set_Io_Status(pins[i], newState);
    }
}

esp_err_t xl9535_reinit(void) 
{
    esp_err_t ret;

    // 初始化 I2C 总线
    ret = Xl9535_Init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C for XL9535: %s", esp_err_to_name(ret));
        return ret;
    }

    // 设置所有引脚方向为输入（可以根据需要选择具体引脚配置）
    uint8_t config = 0xFF; // 设置所有引脚为输入
    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_CONFIG_PORT0, config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write configuration to PORT0: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_CONFIG_PORT1, config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write configuration to PORT1: %s", esp_err_to_name(ret));
        return ret;
    }

    // 如果需要，可以配置极性反转寄存器，确保所有引脚都按正确的逻辑电平工作
    uint8_t polarity = 0x00; // 不反转，所有输入按原始逻辑
    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_POLARITY_INVERSION_PORT0, polarity);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set polarity inversion for PORT0: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = Xl9535_Write_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_POLARITY_INVERSION_PORT1, polarity);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set polarity inversion for PORT1: %s", esp_err_to_name(ret));
        return ret;
    }

    // 清除任何潜在的中断状态
    uint8_t input_state[2];
    ret = Xl9535_Read_Reg(XL9535_I2C_SLAVE_ADDR, XL9535_INPUT_PORT0, input_state, 2);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read input state for clearing interrupts: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "XL9535 reinitialized successfully");
    return ESP_OK;
}
