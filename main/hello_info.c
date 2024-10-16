#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/uart.h"

void print_chip_info() {
    // 定义日志标签
    const char *TAG = "CHIP_INFO";
    
    // 定义结构体用于存储芯片信息
    esp_chip_info_t chip_info;
    
    // 获取芯片信息
    esp_chip_info(&chip_info);
    
    // 打印芯片信息
    ESP_LOGI(TAG, "This is ESP8266 chip with %d CPU cores, WiFi%s%s",
             chip_info.cores,
             (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
             (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    
    ESP_LOGI(TAG, "Silicon revision: %d", chip_info.revision);
    ESP_LOGI(TAG, "Flash: %dMB %s", spi_flash_get_chip_size() / (1024 * 1024),
             (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void printRuntimeStatus()
{
    printf("run time ram\n");
      // 获取当前可用的堆内存
    size_t free_heap_size = esp_get_free_heap_size();
    printf("Current free heap size: %d bytes\n", free_heap_size);

    // 获取系统启动以来的最小剩余堆内存
    size_t min_free_heap_size = esp_get_minimum_free_heap_size();
    printf("Minimum free heap size since boot: %d bytes\n", min_free_heap_size);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

