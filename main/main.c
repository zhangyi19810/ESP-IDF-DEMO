#include "system.h"
#include "camera.h"

void app_main(void)
{
    system_init();
    
    // 初始化摄像头1
    camera_init();
    
    // 切换到摄像头2
    vTaskDelay(pdMS_TO_TICKS(1000));  // 等待1秒
    camera_switch(CAM_SELECT_2);
    
    // 切回摄像头1
    vTaskDelay(pdMS_TO_TICKS(1000));
    camera_switch(CAM_SELECT_1);
}
