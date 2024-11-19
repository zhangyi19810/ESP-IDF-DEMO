#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include "esp_err.h"
#include "usb_stream.h"
#include "xl9535.h"

// 摄像头引脚定义
#define USB_EN_PIN  PIN_P00    // USB电源使能引脚
#define USB_OE_PIN  PIN_P01    // USB摄像头切换选择引脚
#define DC_EN_PIN   PIN_P02    // DC电源使能引脚

// 摄像头状态枚举
typedef enum {
    CAMERA_IDLE,
    CAMERA_RUNNING,
    CAMERA_ERROR
} camera_state_t;

// 摄像头配置结构体
typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t fps;
    void (*frame_cb)(uint8_t *data, size_t len);  // 帧回调函数
} camera_config_t;

// 初始化摄像头硬件
esp_err_t camera_hw_init(void);

// 初始化摄像头
esp_err_t camera_init(camera_config_t *config);

// 启动摄像头
esp_err_t camera_start(void);

// 停止摄像头
esp_err_t camera_stop(void);

// 获取摄像头状态
camera_state_t camera_get_state(void); 

#endif // CAMERA_H