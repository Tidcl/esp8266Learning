#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/uart.h"

#include "hello_net.c"
#include "echo_server.c"
#include "nmi_pwm.c"
#include "hc_sr04.c"
#include "sound_sensor.c"
#include "light_sensor.c"
#include "ledc_pwm.c"
#include "software_pwm.c"

#define USE_PWM 1
#define USE_NET 1
#define USE_HC_SR04 1
#define USE_LIGHT_SENSOR 1
#define USE_SOUND_SENSOR 1


extern bool g_useLightSensor;
extern bool g_useDistanceSensor;
extern bool g_useSoundSensor;
void net_recv(char* str)    //处理从网络io接收到的数据，这部分代码采用回调，是为了避免业务逻辑干扰网络处理逻辑
{
    if(strcmp(str, "distance") == 0)
    {
        g_useDistanceSensor = !g_useDistanceSensor;
    }else if(strcmp(str, "light") == 0)
    {
        g_useLightSensor = !g_useLightSensor;
    }
    else if(strcmp(str, "sound") == 0)
    {
        g_useSoundSensor = !g_useSoundSensor;
    }
}

void app_main()
{
    //uart_set_baudrate(UART_NUM_0,115200);

#if USE_NET
    set_net_recv_callbackPtr(net_recv);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_init_sta();
    xTaskCreate(echo_server_task, "echo server", 10240, NULL, 5, NULL);
#endif

    printf(R"(Hello send above word with tcp connect(port 8080) to open/close function:
    1.distance
    2.light
    3.sound
    )");

#if USE_SOUND_SENSOR
    initSoundSensor();
    xTaskCreate(soundSensorTask, "voice_sensor", 1024, NULL, 5, NULL);
#endif

#if USE_LIGHT_SENSOR
    initLightSensor();
    xTaskCreate(lightSensorTask, "voice_sensor", 1024, NULL, 5, NULL);
#endif

#if USE_HC_SR04
    initHCSR04();
    xTaskCreate(hcsr04Task, "hcsr04", 1024, NULL, 5, NULL);
#endif

#if USE_PWM
    // initPWM();
    printf("start software_pwm_task\n");
    xTaskCreate(software_pwm_loop, "software_pwm_loop", 10240, NULL, 1, NULL);
    xTaskCreate(software_pwm_test, "software_pwm_test", 10240, NULL, 2, NULL);
#endif
}

