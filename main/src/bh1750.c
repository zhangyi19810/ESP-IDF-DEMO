#include "bh1750.h"

static const char *TAG = "i2c-example";

int I2C_Init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, 
                            I2C_MASTER_RX_BUF_DISABLE, 
                            I2C_MASTER_TX_BUF_DISABLE, 0);
}

int I2C_WriteData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    ESP_LOGI(TAG, "Writing to slave addr: 0x%02X", slaveAddr);
    
    i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, (slaveAddr << 1) | WRITE_BIT, ACK_CHECK_EN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error sending slave address: %d", ret);
        i2c_cmd_link_delete(cmd);
        return ret;
    }
    
    if(regAddr != 0) {
        ret = i2c_master_write_byte(cmd, regAddr, ACK_CHECK_EN);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error sending register address: %d", ret);
            i2c_cmd_link_delete(cmd);
            return ret;
        }
    }
    
    ret = i2c_master_write(cmd, pData, dataLen, ACK_CHECK_EN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error writing data: %d", ret);
        i2c_cmd_link_delete(cmd);
        return ret;
    }
    
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error executing I2C transaction: %d", ret);
    }
    
    i2c_cmd_link_delete(cmd);
    return ret;
}

int I2C_ReadData(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaveAddr << 1) | READ_BIT, ACK_CHECK_EN);
    if(regAddr != 0) {
        i2c_master_write_byte(cmd, regAddr, ACK_CHECK_EN);
    }
    if (dataLen > 1) {
        i2c_master_read(cmd, pData, dataLen - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, &pData[dataLen - 1], NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void BH1750_Init_L(void)
{
    esp_err_t ret;
    uint8_t data;
    int retry = 3;
    
    while (retry--) {
        data = BH1750_PWR_ON;
        ret = I2C_WriteData(BH1750_SLAVE_ADDR_L, 0, &data, 1);
        if (ret == ESP_OK) {
            break;
        }
        ESP_LOGE(TAG, "BH1750(L) power on failed: %d, retrying...", ret);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BH1750(L) power on failed after retries");
        return;
    }
    
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
    data = BH1750_CON_H;
    ret = I2C_WriteData(BH1750_SLAVE_ADDR_L, 0, &data, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BH1750(L) mode set failed: %d", ret);
        return;
    }
}

void BH1750_Init_H(void)
{
    esp_err_t ret;
    uint8_t data;
    int retry = 3;
    
    while (retry--) {
        data = BH1750_PWR_ON;
        ret = I2C_WriteData(BH1750_SLAVE_ADDR_H, 0, &data, 1);
        if (ret == ESP_OK) {
            break;
        }
        ESP_LOGE(TAG, "BH1750(H) power on failed: %d, retrying...", ret);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BH1750(H) power on failed after retries");
        return;
    }
    
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
    data = BH1750_CON_H;
    ret = I2C_WriteData(BH1750_SLAVE_ADDR_H, 0, &data, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BH1750(H) mode set failed: %d", ret);
        return;
    }
}

float BH1750_ReadLightIntensity_L(void)
{
    float lux = 0.0;
    uint8_t sensorData[2] = {0};
    esp_err_t ret = I2C_ReadData(BH1750_SLAVE_ADDR_L, 0, sensorData, 2);
    if (ret == ESP_OK) {
        lux = (sensorData[0] << 8 | sensorData[1]) / 1.2;
    } else {
        ESP_LOGE(TAG, "Error reading from BH1750(L): %d", ret);
    }
    return lux;
}

float BH1750_ReadLightIntensity_H(void)
{
    float lux = 0.0;
    uint8_t sensorData[2] = {0};
    esp_err_t ret = I2C_ReadData(BH1750_SLAVE_ADDR_H, 0, sensorData, 2);
    if (ret == ESP_OK) {
        lux = (sensorData[0] << 8 | sensorData[1]) / 1.2;
    } else {
        ESP_LOGE(TAG, "Error reading from BH1750(H): %d", ret);
    }
    return lux;
}

void i2c_scanner(void)
{
    ESP_LOGI(TAG, ">> I2C scanning ...");
    uint8_t count = 0;
    
    for (uint8_t i = 1; i < 127; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | WRITE_BIT, ACK_CHECK_EN);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Found I2C device at address 0x%02X", i);
            count++;
        }
    }
    
    ESP_LOGI(TAG, "Found %d I2C devices", count);
}

void i2c_reinit(void)
{
    i2c_driver_delete(I2C_MASTER_NUM);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    I2C_Init();
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

// ... (其他函数实现，从原文件复制过来) 