#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

// 定义按键IO
#define KEY1_IO     18
#define KEY2_IO     8
// 定义长按时间阈值（单位：毫秒）
#define LONG_PRESS_TIME 1000
// 定义组合按键时间阈值
#define COMBO_PRESS_TIME 5000
// 定义组合按键允许的最大时间差（毫秒）
#define COMBO_WINDOW_TIME 300
// 定义组合按键结束后的忽略时间（毫秒）
#define COMBO_END_IGNORE_TIME 500

static const char *TAG = "KEY";
static QueueHandle_t gpio_evt_queue = NULL;

// 按键状态结构体
typedef struct {
    uint32_t gpio_num;
    uint32_t press_time;
    bool is_pressed;
} key_state_t;

static key_state_t key1_state = {KEY1_IO, 0, false};
static key_state_t key2_state = {KEY2_IO, 0, false};
static uint32_t combo_start_time = 0;    // 组合按键开始时间
static bool combo_triggered = false;      // 组合按键触发标志
static bool in_combo_mode = false;
// 添加变量来记录组合按键结束时间
static uint32_t combo_end_time = 0;

// GPIO中断服务函数
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

// 添加新的任务来检查组合按键
static void combo_check_task(void* arg)
{
    while(1) {
        if(key1_state.is_pressed && key2_state.is_pressed) {
            // 计算两个按键的按下时间差
            uint32_t time_diff;
            if(key1_state.press_time > key2_state.press_time) {
                time_diff = (key1_state.press_time - key2_state.press_time) * portTICK_PERIOD_MS;
            } else {
                time_diff = (key2_state.press_time - key1_state.press_time) * portTICK_PERIOD_MS;
            }
            
            // 如果按键时间差在允许范围内
            if(time_diff <= COMBO_WINDOW_TIME) {
                if(combo_start_time == 0) {
                    combo_start_time = xTaskGetTickCount();
                    ESP_LOGI(TAG, "组合按键开始！");
                    combo_triggered = false;
                    in_combo_mode = true;  // 设置组合按键模式
                } else if(!combo_triggered) {
                    uint32_t duration = (xTaskGetTickCount() - combo_start_time) * portTICK_PERIOD_MS;
                    if(duration >= COMBO_PRESS_TIME) {
                        ESP_LOGI(TAG, "组合按键持续 5 秒！");
                        combo_triggered = true;
                    }
                }
            }
        } else if(in_combo_mode) {
            ESP_LOGI(TAG, "组合按键结束");
            combo_start_time = 0;
            combo_triggered = false;
            in_combo_mode = false;
            combo_end_time = xTaskGetTickCount();  // 记录组合按键结束时间
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// 按键处理任务
static void key_event_task(void* arg)
{
    uint32_t io_num;
    while(1) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            // 延时消抖
            vTaskDelay(pdMS_TO_TICKS(20));
            
            int level = gpio_get_level(io_num);
            
            if(io_num == KEY1_IO) {
                if(level == 0 && !key1_state.is_pressed) {
                    key1_state.is_pressed = true;
                    key1_state.press_time = xTaskGetTickCount();
                    if(!in_combo_mode) {
                        ESP_LOGI(TAG, "KEY1 按下");
                    }
                } else if(level == 1) {
                    if(key1_state.is_pressed && !in_combo_mode) {
                        uint32_t current_time = xTaskGetTickCount();
                        if(current_time - combo_end_time > pdMS_TO_TICKS(COMBO_END_IGNORE_TIME)) {  // 检查是否在忽略时间内
                            uint32_t duration = (current_time - key1_state.press_time) * portTICK_PERIOD_MS;
                            if(duration >= LONG_PRESS_TIME) {
                                ESP_LOGI(TAG, "KEY1 长按释放");
                                // 在这里执行长按释放的操作，如开灯
                            } else {
                                ESP_LOGI(TAG, "KEY1 短按释放");
                                // 在这里执行短按释放的操作
                            }
                        }
                    }
                    key1_state.is_pressed = false;
                }
            } else if(io_num == KEY2_IO) {
                if(level == 0) {
                    key2_state.is_pressed = true;
                    key2_state.press_time = xTaskGetTickCount();
                    if(!in_combo_mode) {
                        ESP_LOGI(TAG, "KEY2 按下");
                    }
                } else {
                    if(!in_combo_mode) {
                        uint32_t current_time = xTaskGetTickCount();
                        if(current_time - combo_end_time > pdMS_TO_TICKS(COMBO_END_IGNORE_TIME)) {  // 检查是否在忽略时间内
                            ESP_LOGI(TAG, "KEY2 释放");
                            // 在这里执行KEY2释放的操作
                        }
                    }
                    key2_state.is_pressed = false;
                }
            }
        }
    }
}

void app_main(void)
{
    // 创建事件队列
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    
    // GPIO中断配置
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL<<KEY1_IO) | (1ULL<<KEY2_IO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE    // 双边沿触发
    };
    gpio_config(&io_conf);

    // 安装GPIO中断服务
    gpio_install_isr_service(0);
    
    // 添加中断处理函数
    gpio_isr_handler_add(KEY1_IO, gpio_isr_handler, (void*) KEY1_IO);
    gpio_isr_handler_add(KEY2_IO, gpio_isr_handler, (void*) KEY2_IO);

    // 创建按键处理任务
    xTaskCreate(key_event_task, "key_event_task", 4096, NULL, 10, NULL);
    // 创建组合按键检测任务
    xTaskCreate(combo_check_task, "combo_check_task", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG, "按键检测程序启动!");
}