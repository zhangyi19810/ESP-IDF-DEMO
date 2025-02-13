#include <stdio.h>
#include "esp_system.h"
#include "esp_mac.h"

void app_main() {
    uint8_t mac[6];

    // 获取默认的 MAC 地址（从 eFuse 读取）
    esp_efuse_mac_get_default(mac);

    // 打印 MAC 地址
    printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
