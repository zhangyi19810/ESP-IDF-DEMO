#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "i2c_test";

#define I2C_MASTER_SCL_IO           17      // SCL 引脚
#define I2C_MASTER_SDA_IO           18      // SDA 引脚

// #define I2C_MASTER_SCL_IO           7      // SCL 引脚
// #define I2C_MASTER_SDA_IO           6      // SDA 引脚

#define I2C_MASTER_NUM              I2C_NUM_0    // I2C 端口号
#define I2C_MASTER_FREQ_HZ          400000       // I2C 频率: 400kHz
#define I2C_MASTER_TIMEOUT_MS       1000

#define DEVICE_ADDR                 0x23    // 设备地址

// I2C 初始化
static esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C 参数配置失败");
        return err;
    }

    err = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C 驱动安装失败");
        return err;
    }

    return ESP_OK;
}

// 测试设备通信
static void test_device(void)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DEVICE_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "成功与设备 0x%02x 通信!", DEVICE_ADDR);
    } else {
        ESP_LOGE(TAG, "与设备 0x%02x 通信失败: %s", DEVICE_ADDR, esp_err_to_name(ret));
    }
}

void app_main(void)
{
    // 初始化 I2C
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C 初始化成功");

    // 延时等待设备就绪
    vTaskDelay(pdMS_TO_TICKS(100));

    while (1) {
        // 每秒测试一次设备通信
        test_device();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}