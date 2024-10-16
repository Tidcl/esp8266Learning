#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#define PORT 8080

static const char *TAG_SERVER = "echo_server";
int g_sock = 0;

typedef void(*net_recv_callbackPtr)(char* str);
static net_recv_callbackPtr net_recv_fun = 0;
void set_net_recv_callbackPtr(net_recv_callbackPtr ptr)
{
    net_recv_fun = ptr;
}

// TCP Echo 服务器任务
void echo_server_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TAG_SERVER, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG_SERVER, "Socket unable to bind: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
        return;
    }

    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG_SERVER, "Error occurred during listen: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        ESP_LOGI(TAG_SERVER, "Socket listening");

        struct sockaddr_in source_addr;  // 用于 IPv4 客户端
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        g_sock = sock;
        if (sock < 0) {
            ESP_LOGE(TAG_SERVER, "Unable to accept connection: errno %d", errno);
            break;
        }

        inet_ntoa_r(source_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
        ESP_LOGI(TAG_SERVER, "Socket accepted ip address: %s", addr_str);

        while (1) {
            memset(rx_buffer, 0, sizeof(rx_buffer));
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // gpio_set_level(GPIO_NUM_15, 1);
            if (len < 0) {
                ESP_LOGE(TAG_SERVER, "recv failed: errno %d", errno);
                break;
            } else if (len == 0) {
                ESP_LOGI(TAG_SERVER, "Connection closed");
                break;
            } else {
                if(net_recv_fun)
                {
                    net_recv_fun(rx_buffer);                    
                }
            }
        }
        
        if (sock != -1) {
            ESP_LOGI(TAG_SERVER, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
        g_sock = 0;
    }

    vTaskDelete(NULL);
}