#ifndef CAMERA_H
#define CAMERA_H

#include "esp_err.h"

/**
 * @brief 摄像头选择枚举
 */
typedef enum {
    CAM_SELECT_1 = 0,  // 改名避免冲突
    CAM_SELECT_2 = 1
} camera_select_t;

/**
 * @brief 初始化UVC摄像头
 */
void camera_init(void);

/**
 * @brief 切换摄像头
 * 
 * @param camera 要切换到的摄像头
 * @return esp_err_t 
 */
esp_err_t camera_switch(camera_select_t camera);

#endif