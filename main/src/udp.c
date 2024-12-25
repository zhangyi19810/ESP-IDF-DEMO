#include "udp.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include <fcntl.h>
#include "freertos/queue.h"

static const char *TAG = "UDP";

#define PACKET_SIZE (96*3*8)  // 每个包的大小
#define MAX_PACKETS 13        // 最大包数
#define PACKET_INTERVAL_MS 5   // 减少包间隔到5ms
#define QUEUE_SIZE 5          // 增加队列大小

static inline size_t min_size(size_t a, size_t b) {
    return (a < b) ? a : b;
}

typedef struct {
    uint8_t *data;
    size_t len;
} frame_buffer_t;

static QueueHandle_t send_queue = NULL;
static uint8_t *frame_buffer = NULL;
static TaskHandle_t udp_task_handle = NULL;
TaskHandle_t notify_task_handle = NULL;  // 用于通知的任务句柄，改为全局变量
static struct sockaddr_in dest_addr;

static void udp_send_task(void *arg)
{
    frame_buffer_t frame;
    uint8_t *packet_buffer = malloc(PACKET_SIZE + sizeof(packet_header_t));
    if (!packet_buffer) {
        ESP_LOGE(TAG, "无法分配包缓冲区");
        return;
    }

    while (1) {
        if (xQueueReceive(send_queue, &frame, portMAX_DELAY) == pdTRUE) {
            int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
            if (sock < 0) {
                ESP_LOGE(TAG, "无法创建socket: errno %d", errno);
                continue;
            }

            packet_header_t header = {
                .total_packets = MAX_PACKETS,
                .payload_length = 0  // 由于uint8_t的限制，这里设为0
            };

            // 发送图像数据包
            for (int i = 0; i < MAX_PACKETS - 1; i++) {
                header.seq_num = i + 1;
                
                memcpy(packet_buffer, &header, sizeof(packet_header_t));
                memcpy(packet_buffer + sizeof(packet_header_t), 
                       frame.data + i * PACKET_SIZE, 
                       PACKET_SIZE);

                int err = sendto(sock, packet_buffer, PACKET_SIZE + sizeof(packet_header_t), 
                               0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                if (err < 0) {
                    ESP_LOGE(TAG, "发送错误: errno %d", errno);
                    break;
                }
                
                ESP_LOGI(TAG, "包 %d/%d 已发送", header.seq_num, header.total_packets);
                vTaskDelay(pdMS_TO_TICKS(PACKET_INTERVAL_MS));  // 减少发送间隔
            }

            // 发送最后一个包（预留给结果）
            uint8_t result_packet[4] = {MAX_PACKETS, MAX_PACKETS, 0, 0};
            sendto(sock, result_packet, sizeof(result_packet), 
                   0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

            close(sock);

            // 通知发送完成
            if (notify_task_handle != NULL) {
                xTaskNotifyGive(notify_task_handle);
            }
        }
    }

    free(packet_buffer);
}

esp_err_t udp_camera_init(void)
{
    // 创建发送队列
    send_queue = xQueueCreate(QUEUE_SIZE, sizeof(frame_buffer_t));
    if (send_queue == NULL) {
        ESP_LOGE(TAG, "创建队列失败");
        return ESP_FAIL;
    }

    // 分配帧缓冲区
    frame_buffer = heap_caps_malloc(PACKET_SIZE * MAX_PACKETS, MALLOC_CAP_DMA);
    if (frame_buffer == NULL) {
        ESP_LOGE(TAG, "分配帧缓冲区失败");
        return ESP_FAIL;
    }

    // 配置目标地址
    dest_addr.sin_addr.s_addr = inet_addr(UDP_HOST_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(UDP_PORT);

    // 创建发送任务
    xTaskCreate(udp_send_task, "udp_send", 4096, NULL, 5, &udp_task_handle);

    return ESP_OK;
}

esp_err_t udp_send_frame(const uint8_t *data, size_t len)
{
    if (!frame_buffer) {
        ESP_LOGE(TAG, "未初始化");
        return ESP_ERR_INVALID_STATE;
    }

    // 复制数据到缓冲区
    memcpy(frame_buffer, data, min_size(len, PACKET_SIZE * (MAX_PACKETS - 1)));

    // 发送到队列
    frame_buffer_t frame = {
        .data = frame_buffer,
        .len = len
    };

    if (xQueueSend(send_queue, &frame, 0) != pdTRUE) {
        ESP_LOGW(TAG, "队列已满，丢弃此帧");
    }

    return ESP_OK;
}

void udp_camera_deinit(void)
{
    if (udp_task_handle != NULL) {
        vTaskDelete(udp_task_handle);
    }

    if (send_queue != NULL) {
        vQueueDelete(send_queue);
    }

    if (frame_buffer != NULL) {
        free(frame_buffer);
    }
} 