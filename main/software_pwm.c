#include <stdio.h>
#include <driver/gpio.h>
//休眠
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "esp_task_wdt.h"


int g_soft_ware_duty = 0;
int g_soft_ware_gpio = -1;
int g_software_pwm_freq = 100;

//设置软件PWM输出的GPIO针脚
void software_pwm_setOutGPIO(int number)
{
    gpio_config_t gpio_config_structure;
    gpio_config_structure.pin_bit_mask = (1ULL << number); /* 选择GPIO2 */
    gpio_config_structure.mode = GPIO_MODE_OUTPUT; /* 输出模式 */
    gpio_config_structure.pull_up_en = 0; /* 不上拉 */
    gpio_config_structure.pull_down_en = 0; /* 不下拉 */
    gpio_config_structure.intr_type = GPIO_INTR_DISABLE; /* 禁止中断 */
    gpio_config(&gpio_config_structure);
    g_soft_ware_gpio = number;
}

//设置GPIO的值
void software_pwm_setDuty(int value)
{
    g_soft_ware_duty = value;
}

//pwm输出循环
void software_pwm_loop()
{
    float freqTime = 1000*1000/1000;             //1000hz
    float resolution = 255;                     //duty分辨率
    float resoTime = freqTime/resolution;       //得到一个duty分辨率的耗时us

    int hightime = 0;
    int lowTime = 0;
    while(1)
    {
        esp_task_wdt_reset();   //重置看门口
        if(g_soft_ware_gpio == -1)  //如果没有GPIO则直接空转
        {
            ets_delay_us(1*1000);
            continue;
        }
        //针对一个pwm周期进行pwm输出
        hightime = g_soft_ware_duty * resoTime;
        lowTime = (resolution - g_soft_ware_duty) * resoTime; 
        gpio_set_level(g_soft_ware_gpio, 1);    //设置高电平
        ets_delay_us(hightime);
        
        gpio_set_level(g_soft_ware_gpio, 0);    //设置低电平
        ets_delay_us(lowTime);
    }
}

//软件PWM测试
void software_pwm_test()
{
    software_pwm_setOutGPIO(12);
    software_pwm_setDuty(0);
    int freqTime = 5000;//5000ms一个周期，从暗到亮再到暗
    int switchCount = 500;   //一个周期内变化次数0--250--0 500次
    int onceSwitchTime = freqTime/switchCount; //6ms

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

        software_pwm_setDuty(duty);
        vTaskDelay(pdMS_TO_TICKS(onceSwitchTime)); 
    }
}