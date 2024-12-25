#ifndef __WIFI_H__
#define __WIFI_H__

#include "system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/event_groups.h"

/* WiFi连接配置 */
#define WIFI_SSID      "HTB"        // WiFi名称
#define WIFI_PASS      "htb66666666"     // WiFi密码
#define WIFI_MAXIMUM_RETRY  5              // 最大重试次数

/* WiFi连接事件位 */
#define WIFI_CONNECTED_BIT BIT0     // WiFi连接成功
#define WIFI_FAIL_BIT    BIT1     // WiFi连接失败

esp_err_t wifi_init_sta(void);     // WiFi初始化函数声明
esp_err_t udp_send_frame(const uint8_t *data, size_t len); // UDP发送函数声明

#endif