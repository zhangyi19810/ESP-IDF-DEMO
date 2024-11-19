#include "network.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

static const char *TAG = "network";
static int sock = -1;
static struct sockaddr_in dest_addr;

#define WIFI_SSID "HTB"        // 替换为您的WiFi名称
#define WIFI_PASS "htb66666666"   // 替换为您的WiFi密码

// WiFi事件处理
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data)
{
    static int retry_num = 0;
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_START) {
            ESP_LOGI(TAG, "WiFi STA 启动，尝试连接到AP...");
            esp_wifi_connect();
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            if (retry_num < 10) {
                ESP_LOGW(TAG, "WiFi连接失败，正在重试... (%d/10)", retry_num + 1);
                esp_wifi_connect();
                retry_num++;
            } else {
                ESP_LOGE(TAG, "WiFi连接失败，重试次数已达上限");
            }
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "WiFi连接成功! IP地址: " IPSTR, IP2STR(&event->ip_info.ip));
        retry_num = 0;
    }
}

esp_err_t wifi_init(void)
{
    ESP_LOGI(TAG, "正在初始化WiFi...");
    
    // 初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_LOGI(TAG, "正在连接到WiFi: %s", WIFI_SSID);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    return ESP_OK;
}

esp_err_t network_udp_init(void)
{
    ESP_LOGI(TAG, "正在初始化UDP...");
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "创建UDP socket失败");
        return ESP_FAIL;
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(UDP_PORT);
    inet_aton(SERVER_IP, &dest_addr.sin_addr);
    
    ESP_LOGI(TAG, "UDP初始化完成，目标IP: %s, 端口: %d", SERVER_IP, UDP_PORT);
    return ESP_OK;
}

esp_err_t udp_send_data(const uint8_t* data, size_t len)
{
    if (sock < 0) {
        ESP_LOGE(TAG, "UDP socket未初始化");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "开始发送数据，总大小: %d bytes", len);

    // 发送数据大小
    sendto(sock, &len, sizeof(len), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    // 分包发送数据
    const int packet_size = 1400;
    int sent = 0;
    while (sent < len) {
        int to_send = (len - sent) > packet_size ? packet_size : (len - sent);
        int ret = sendto(sock, data + sent, to_send, 0, 
                        (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (ret < 0) {
            ESP_LOGE(TAG, "发送失败");
            return ESP_FAIL;
        }
        sent += to_send;
        vTaskDelay(pdMS_TO_TICKS(1));  // 避免发送太快
    }

    ESP_LOGI(TAG, "数据发送完成");
    return ESP_OK;
} 