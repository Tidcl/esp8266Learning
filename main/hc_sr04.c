#include <esp_timer.h>
#include <driver/gpio.h>
//休眠
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <sys/socket.h>

#define TRIG_PIN GPIO_NUM_4  // D2
#define ECHO_PIN GPIO_NUM_5  // D1

void initHCSR04()
{
    // 初始化TRIG引脚为输出
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    // 初始化ECHO引脚为输入
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
}

double getHCSR04Value()
{
    // 发送10微秒的高电平信号
    gpio_set_level(TRIG_PIN, 1);
    ets_delay_us(10);
    gpio_set_level(TRIG_PIN, 0);


    // 等待ECHO引脚变高
    while (gpio_get_level(ECHO_PIN) == 0);
    // 记录开始时间
    int64_t start_time = esp_timer_get_time();
    // 等待ECHO引脚变低
    while (gpio_get_level(ECHO_PIN) == 1);
    // 记录结束时间
    int64_t end_time = esp_timer_get_time();
    // 计算距离
    double duration = (end_time - start_time) / 1000000.0;  // 转换为秒
    double distance = (duration * 343) / 2;  // 声速34300cm/s，/2因为信号往返
    return distance;
}

bool g_useDistanceSensor = false;
extern int g_sock;
void hcsr04Task()
{
    char strbuffer[32];
    while(1)
    {
        if(g_useDistanceSensor == false)
        {
            vTaskDelay(pdMS_TO_TICKS(200));  //50ms
            continue;
        }

        double m = getHCSR04Value();
        // memset(strbuffer, 0, 32);
        sprintf(strbuffer, "distance: %.2f m\n", m);
        if(g_sock != 0) send(g_sock, strbuffer, 32, 0);
        else printf("Distance: %.2f m\n", m);
        vTaskDelay(pdMS_TO_TICKS(200));  //50ms
    }
}