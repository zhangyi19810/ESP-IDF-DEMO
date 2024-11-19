/**
*文件说明：该文件为系统板子配置、头文件应用及相关参数定义头文件
*
*
*/
#ifndef _MY_SYSTEM_H_
#define _MY_SYSTEM_H_

/*******************************************头文件****************************************************/
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/i2c.h"
#include "driver/rtc_io.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
// #include "esp_bt.h"
// #include "esp_blufi_api.h"
// #include "esp_blufi.h"
#include "esp_crc.h"
#include "esp_random.h"
// #include "esp_bt_main.h"
// #include "esp_bt_device.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_rom_sys.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_chip_info.h"
#include "esp_netif.h"
#include "esp_efuse.h"
#include "esp_task_wdt.h"
#include "esp_sntp.h"

#include "spi_flash_mmap.h"

#include "mbedtls/aes.h"
#include "mbedtls/dhm.h"
#include "mbedtls/md5.h"

#include "time.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "cJSON.h"
#include "sys/unistd.h"

#include "mqtt_client.h"

#include "nvs.h"
#include "nvs_flash.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stdarg.h"
#include <stddef.h>
#include <sys/time.h>
#include <inttypes.h>
/*******************************************头文件****************************************************/


/***************************************板载相关宏定义*************************************************/
/**
 * @brief 其他定义
 */
#define SNTP_SYNC_FLAG                      (1)                     /*!< SNTP同步标志位 */
#define TEST_PRINT                          (0)                     /*!< 测试日志打印标志位 */
#define FUNC_ON                             (1)                     /*!< 功能开启 */
#define FUNC_OFF                            (0)                     /*!< 功能关闭 */
#define TIME_POWEROFF                       (300)                   /*!< 进入低功耗时长计数 */
#define INTERVA_HEARTBEAT                   (60*1000)               /*!< MQTT上传 */
#define LOW_POWER                           (1600)                  /*!< 低电量值 - 单位：mV */
#define DATA_CNTS                           (240)                   /*!< 每条存储数据的个数 */

/**
 * @brief 功能时长计数定义
 */
#define FEED_TIME                           (180)                   /*!< 哺光训练时长计数 */
#define TRAIN_TIME                          (285)                   /*!< 彩光模式训练时长计数 */

/**
 * @brief 哺光LEDC相关定义
 */
#define FEEDL_PWM_TIMER                     LEDC_TIMER_0            /*!< LEDC定时器0 */
#define FEEDL_PWM_MODE                      LEDC_LOW_SPEED_MODE     /*!< LEDC低速模式 */
#define FEEDL_PWM_DUTY_RES                  LEDC_TIMER_8_BIT        /*!< LEDC分辨率 */
#define FEEDL_PWM_FREQUENCY                 (5000)                  /*!< LEDC频率 */

#define FEEDR_PWM_TIMER                     LEDC_TIMER_0            /*!< LEDC定时器0 */
#define FEEDR_PWM_MODE                      LEDC_LOW_SPEED_MODE     /*!< LEDC低速模式 */
#define FEEDR_PWM_DUTY_RES                  LEDC_TIMER_8_BIT        /*!< LEDC分辨率 */
#define FEEDR_PWM_FREQUENCY                 (5000)                  /*!< LEDC频率 */

#define FEEDL_PWM_OUTPUT_IO                 GPIO_NUM_11             /*!< 哺光左眼IO端口 */
#define FEEDR_PWM_OUTPUT_IO                 GPIO_NUM_47             /*!< 哺光右眼IO端口 */

#define FEEDL_PWM_CHANNEL                   LEDC_CHANNEL_0          /*!< 哺光左眼通道 */
#define FEEDR_PWM_CHANNEL                   LEDC_CHANNEL_1          /*!< 哺光右眼通道 */

/**
 * @brief 哺光模式等级及其对应占空比定义
 */
#define FEED_LEVEL1                         (1)                     /*!< 哺光模式等级 - 1级 */
#define FEED_LEVEL2                         (2)                     /*!< 哺光模式等级 - 2级 */
#define FEED_LEVEL3                         (3)                     /*!< 哺光模式等级 - 3级 */

/**
 * @brief DC EN IO定义
 */
#define DC_3V3_EN                          GPIO_NUM_38             /*!< 红光EN引脚IO端口 */

/**
 * @brief 按键IO定义
 */
#define KEY_1_INPUT_IO                      GPIO_NUM_18             /*!< 按键1 IO端口 */
#define KEY_2_INPUT_IO                      GPIO_NUM_8              /*!< 按键2 IO端口 */
#define KEY_ONOFF_INPUT_IO                  GPIO_NUM_18             /*!< 唤醒按键 IO端口 */
#define GPIO_INPUT_PIN_SEL                  ((1ULL<<KEY_1_INPUT_IO) | (1ULL<<KEY_2_INPUT_IO))   /*!< GPIO输入引脚掩码 */

#define ESP_INTR_FLAG_DEFAULT               (0)                     /*!< 中断标志位 */

/**
 * @brief LEDC相关配置以及IO定义
 */
#define LED_STATUS_RED_OUTPUT_IO            GPIO_NUM_3              /*!< 状态指示灯IO端口 - 红色 */          
#define LED_STATUS_GREEN_OUTPUT_IO          GPIO_NUM_9              /*!< 状态指示灯IO端口 - 绿色 */          
#define LED_STATUS_BLUE_OUTPUT_IO           GPIO_NUM_10             /*!< 状态指示灯IO端口 - 蓝色 */          
#define LED_STATUS_RED_CHANNEL              LEDC_CHANNEL_5          /*!< 状态指示灯通道 */
#define LED_STATUS_GREEN_CHANNEL            LEDC_CHANNEL_6          /*!< 状态指示灯通道 */
#define LED_STATUS_BLUE_CHANNEL             LEDC_CHANNEL_7          /*!< 状态指示灯通道 */

/**
 * @brief LEDC相关配置以及IO定义
 */
#define LED_FUNC_BLUE_OUTPUT_IO             GPIO_NUM_21             /*!< 功能指示灯IO端口 - 蓝色 */    
#define LED_FUNC_RED_OUTPUT_IO              GPIO_NUM_2              /*!< 功能指示灯IO端口 - 红色 */
#define LED_FUNC_GREEN_OUTPUT_IO            GPIO_NUM_1              /*!< 功能指示灯IO端口 - 绿色 */
#define LED_FUNC_RED_CHANNEL                LEDC_CHANNEL_2          /*!< 功能指示灯通道 */
#define LED_FUNC_GREEN_CHANNEL              LEDC_CHANNEL_3          /*!< 功能指示灯通道 */
#define LED_FUNC_ORG_CHANNEL                LEDC_CHANNEL_4          /*!< 功能指示灯通道 */
#define LED_FUNC_LEDC_TIMER                 LEDC_TIMER_0            /*!< LEDC定时器 */
#define LED_FUNC_LEDC_MODE                  LEDC_LOW_SPEED_MODE     /*!< LEDC低速度模式 */
#define LED_FUNC_LEDC_FREQ_HZ               (5000)                  /*!< LEDC频率 */
#define LED_FUNC_LEDC_RESOLUTION            LEDC_TIMER_8_BIT        /*!< LEDC分辨率 */

/**
 * @brief DS1302 IO定义
 */
#define DS1302_CLK_PIN                      GPIO_NUM_39             /*!< DS1302 CLK引脚端口号 */
#define DS1302_IO_PIN                       GPIO_NUM_40             /*!< DS1302 IO引脚端口号*/
#define DS1302_CE_PIN                       GPIO_NUM_42             /*!< DS1302 CE引脚端口号*/

/**
 * @brief 语音芯片 IO定义
 */
#define IO_CLK                              GPIO_NUM_41             /*!< 语音芯片 - CLK IO端口号 */
#define IO_DATA                             GPIO_NUM_17             /*!< 语音芯片 - DATA IO端口号 */
#define IO_BUSY                             GPIO_NUM_48             /*!< 语音芯片 - BUSY IO端口号 */
#define IO_FREE                             GPIO_NUM_9              /*!< 语音芯片 - FREE IO端口号 */

/**
 * @brief XL9535拓展IO IIC协议相关配置
 */
#define XL9535_I2C_MASTER_SCL_IO            GPIO_NUM_14             /*!< 拓展IO芯片IIC协议时钟线IO */
#define XL9535_I2C_MASTER_SDA_IO            GPIO_NUM_13             /*!< 拓展IO芯片IIC协议数据线IO */
#define XL9535_I2C_MASTER_FREQ_HZ           (400000)                /*!< 拓展IO芯片IIC协议频率 */
#define XL9535_I2C_MASTER_TX_BUF_DISABLE    (0)                 
#define XL9535_I2C_MASTER_RX_BUF_DISABLE    (0)
#define XL9535_I2C_SLAVE_ADDR               (0x20)                  /*!< 拓展IO芯片IIC协议地址 */
#define XL9535_I2C_MASTER_NUM               I2C_NUM_1               /*!< 拓展IO芯片IIC协议端口号 */
#define XL9535_INT_PIN                      GPIO_NUM_12             /*!< 拓展IO芯片中断引脚 */

#define MODE_COUNT                          6                       // 总共5种模式加关闭功能，一共六种
#define MAX_TOTAL_TIME_MIN 30   // 最大允许的总时间（分钟）
#define MIN_TOTAL_TIME_MIN 5    // 最小允许的总时间（分钟）


/**
 * @brief 设备可设置参数数量
 */
#define num_device_parameters 6
/***************************************板载相关宏定义*************************************************/

/*****************************************相关枚举定义*************************************************/
/**
 * @brief 按键相关枚举
 */
typedef enum {
    KEY_FUNC_OFF        = 0x00,         // 按键功能关闭
    KEY_FUNC_BUGUANG_1  = 0x01,         // 哺光一档
    KEY_FUNC_BUGUANG_2  = 0x02,         // 哺光二档
    KEY_FUNC_BUGUANG_3  = 0x03,         // 哺光三档
    KEY_FUNC_RUOSHI_ON  = 0x04,         // 弱视开启
    KEY_FUNC_OTA_ING    = 0x05,         // OTA升级
}KEY_FUNC_E;

/**
 * @brief OTA相关枚举
 */
typedef enum {
    OTA_STATUS_OFF      = 0x00,         // OTA关闭
    OTA_STATUS_ING      = 0x01,         // OTA进行中
    OTA_STATUS_SUCCESS  = 0x02,         // OTA升级成功
    OTA_STATUS_FAIL     = 0x03,         // OTA升级失败
}OTA_STATUS;

/**
 * @brief WiFi相关枚举
 */
typedef enum {
    WIFI_STATE_IDLE,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTED,
} wifi_state_t;

/**
 * @brief 电量相关枚举
 */
typedef enum {
   POWER_STATE_LOW      = 0x00,
   POWER_STATE_NORMAL   = 0x01,
} power_state_t;



/**
 * @brief 语音相关枚举
 */
typedef enum {
    WELCOME = 0x00,
    OTA_START,
    OTA_SUCCESS,
    OTA_FAIL, 
    FEED_1, 
    FEED_2, 
    FEED_3, 
    COLOR_LIGHT, 
    MUSIC,
    COUNT_DOWN_1,
    COUNT_DOWN_2,
    COUNT_DOWN_3,
    COUNT_DOWN_4,
    COUNT_DOWN_5,
    REMIND,
    NO_NETWORK,
    POWER_WARNING,
    FEEDTRAIN_END,
    COLORTRAIN_END,
    STOP
} SCENE;
/*****************************************相关枚举定义*************************************************/

// 系统任务优先级
#define PRIORITY_KEY_INTERRUPT_TASK     12  // 按键中断任务
#define PRIORITY_USB_CONNECT_TASK       4   // 摄像头重连任务
#define PRIORITY_VOICE_TASK             3   // 语音任务
#define PRIORITY_DEEPSLEEP_TASK         2   // 深度睡眠任务
#define PRIORITY_POWER_CHECK_TASK       5   // 电量检测任务
#define PRIORITY_OTA_TASK               6   // OTA升级任务
#define PRIORITY_MQTT_SEND_TASK         8   // MQTT心跳发送任务
#define PRIORITY_CAMERA_TASK            11  // 摄像头任务
#define PRIORITY_RUOSHI_TASK            9   // 彩光功能任务
#define PRIORITY_FEED_TASK              9   // 哺光任务
#define PRIORITY_STATUS_LED_TASK        7   // 指示灯任务

// 系统初始化函数
void SystemInit(void);

#endif // _MY_SYSTEM_H_