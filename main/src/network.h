#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include "esp_err.h"

// UDP配置
#define UDP_PORT 3333
#define SERVER_IP "192.168.2.26"  // 替换为您电脑的IP地址

// 初始化WiFi
esp_err_t wifi_init(void);

// 初始化UDP
esp_err_t network_udp_init(void);

// 通过UDP发送数据
esp_err_t udp_send_data(const uint8_t* data, size_t len);

#endif // NETWORK_H 