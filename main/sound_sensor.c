#include "driver/gpio.h"

//休眠
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <sys/socket.h>

#define SOUND_SENSOR_PIN GPIO_NUM_0  // D0gpio16可以   D4gpio2可以  D3gpio0可以   D5gpio14没用   D6gpio12没用   D7gpio13没用   D8gpio15没用

void initSoundSensor()
{
    // 初始化声音传感器引脚为输入
    gpio_set_direction(SOUND_SENSOR_PIN, GPIO_MODE_INPUT);
}

double getSoundSensorValue()
{
    return gpio_get_level(SOUND_SENSOR_PIN);
}

bool g_useSoundSensor = false;
extern int g_sock;
void soundSensorTask()
{
    g_useSoundSensor = false;
    while(1)
    {
        while(g_useSoundSensor == false)
        {
            vTaskDelay(pdMS_TO_TICKS(200)); 
            continue;
        }

        int sound_level = gpio_get_level(SOUND_SENSOR_PIN);
        if (sound_level == 1) {
            // printf("has voice!\n");
            if(g_sock != 0) send(g_sock, "has Sound!\n", 12, 0);
            else printf("has Sound!\n");
        } else {
            // printf("no voice.\n");
            if(g_sock != 0) send(g_sock, "no Sound.\n", 11, 0);
            else printf("no Sound.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(200)); 
    }
}