#include "driver/gpio.h"

//休眠
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <sys/socket.h>

#define LIGHT_SENSOR_PIN GPIO_NUM_16  // D0gpio16可以   D4gpio02可以  D3gpio0可以   D5gpio14没用   D6gpio12没用   D7gpio13没用   D8gpio15没用

void initLightSensor()
{
    // 初始化声音传感器引脚为输入
    // gpio_pad_select_gpio(SOUND_SENSOR_PIN);
    gpio_set_direction(LIGHT_SENSOR_PIN, GPIO_MODE_INPUT);
    // gpio_set_pull_mode(GPIO_NUM_14, GPIO_PULLUP_ONLY); // 启用上拉电阻
}

double getLightSensorValue()
{
    return gpio_get_level(LIGHT_SENSOR_PIN);
}

bool g_useLightSensor = false;
extern int g_sock;
void lightSensorTask()
{
    while(1)
    {
        if(g_useLightSensor == false)
        {
            vTaskDelay(pdMS_TO_TICKS(200)); 
            continue;
        }

        int sound_level = gpio_get_level(LIGHT_SENSOR_PIN);
        if (sound_level == 1) {
            // printf("dark\n");
            if(g_sock != 0) send(g_sock, "dark\n", 6, 0);
            else printf("dark\n");
        } else {
            // printf("light\n");
            if(g_sock != 0) send(g_sock, "light\n", 7, 0);
            else printf("light\n");
        }
        vTaskDelay(pdMS_TO_TICKS(200)); 
    }
}