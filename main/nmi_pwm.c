#include "driver/pwm.h"
//休眠
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PWM_PERIOD    1000  // PWM 周期，单位微秒，表示 1 kHz 的频率
#define PWM_DUTY      500   // 初始占空比，50%

void nmi_pwm(void)
{
    // 定义输出 PWM 的 GPIO 引脚
    uint32_t pin_num[1] = {2};      // GPIO2
    uint32_t duties[1] = {PWM_DUTY}; // 初始占空比 50%
    int16_t phase[1] = {0};          // 相位偏移为 0

    // 初始化 PWM 配置，包含周期、占空比、通道数量、引脚配置
    pwm_init(PWM_PERIOD, duties, 1, pin_num);

    // 设置相位（如果需要）
    pwm_set_phases(phase);

    int freqTime = 5000;//5000ms一个周期，从暗到亮再到暗
    int switchCount = 500;   //一个周期内变化次数0--250--0 500次
    int onceSwitchTime = freqTime / switchCount;

    int duty = 0;
    bool isAdd = true;
    while(1)
    {
        if(isAdd)
        {
            duty++;
        }else
        {
            duty--;
        }

        if(isAdd && duty >= 250)
        {
            isAdd = false;    
        }else if(isAdd == false && duty <= 10)
        {
            isAdd = true;
        }

        pwm_set_duty(0, duty);
        pwm_start();
        vTaskDelay(pdMS_TO_TICKS(onceSwitchTime));
    }
}
