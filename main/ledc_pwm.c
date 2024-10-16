#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#define LEDC_CHANNEL LEDC_CHANNEL_0    // 选择LEDC通道
#define LEDC_TIMER LEDC_TIMER_0         // 选择LEDC定时器
#define LEDC_OUTPUT_GPIO GPIO_NUM_2     // 输出到GPIO2引脚
#define LEDC_FREQUENCY 1000              // 设置频率为5kHz
#define LEDC_DUTY 128                    // 设置占空比（0-255，128为50%）

void test_ledc_pwm() {
    // 配置LEDC定时器
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT, // 8位分辨率
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY,
        // .clk_cfg = LEDC_AUTO_CLK
    };
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK) {
        printf("Failed to configure LEDC timer: %d\n", ret);
    }

    // 配置LEDC通道
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .gpio_num = LEDC_OUTPUT_GPIO,
        .duty = LEDC_DUTY, // 初始占空比
        .hpoint = 0
    };
    ret = ledc_channel_config(&ledc_channel);
    if (ret != ESP_OK) {
        printf("Failed to configure LEDC channel: %d\n", ret);
    }
    // 启动LEDC
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, LEDC_DUTY);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延迟1秒
    }
}
