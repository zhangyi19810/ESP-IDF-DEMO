#ifndef __UDP_H__
#define __UDP_H__

#include "esp_err.h"
#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* UDP配置 */
#define UDP_PORT        3333         // UDP端口
#define UDP_HOST_IP     "192.168.2.181"  // 目标IP地址
#define UDP_BUFFER_SIZE 1460         // UDP缓冲区大小(MTU=1500-20-8-12=1460)

/* 包头结构体 */
typedef struct {
    uint8_t seq_num;          // 包序号
    uint8_t total_packets;    // 总包数
    uint8_t payload_length;   // 数据长度
} __attribute__((packed)) packet_header_t;

/* 全局变量声明 */
extern TaskHandle_t notify_task_handle;

/* UDP函数声明 */
esp_err_t udp_camera_init(void);
esp_err_t udp_send_frame(const uint8_t *data, size_t len);
void udp_camera_deinit(void);

#endif 