#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// RTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_log.h"

//Motor
#include "motor.h"

// ADC 
#include "v_bat.h"

//UDP
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#define UDP_PORT    4711

//OAT update
#include "wifi_setup.h"
#include "http_ota.h"


static const char *TAG = "fizzy";
static const char *git_info = GIT_STAT;


//UDP dataframes

char addr_str[128];

struct {
    uint8_t command;
    float   motor_speed;
} udp_dataframe_in;

struct __attribute__((packed, aligned(1))) {
    int64_t timestamp;
    float   motor_speed;
    float   v_bat;
} udp_dataframe_out;


// --- MAIN ---

void app_main(void) {

    v_bat_init();
    motor_init();
    //MPU_spi_init();
    //MPU_imu_init();
    WIFI_init();
    OTA_init();

    // create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: erron %d", errno);
    }

    // set timeout
    //struct timeval timeout;
    //timeout.tv_sec = 10;
    //timeout.tv_usec = 0;
    //setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

    // bind socket
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(UDP_PORT);
    int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    }
    ESP_LOGI(TAG, "Socket bound, port %d", UDP_PORT);

    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
    socklen_t socklen = sizeof(source_addr);

    

    while(true) {
        // Get data
        int len = recvfrom(sock, (char *) &udp_dataframe_in, sizeof(udp_dataframe_in), 0, (struct sockaddr *)&source_addr, &socklen);
        // check data length
        if (len < 0) {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            break;
        }
        // Process data
        else {
            // Get the sender's ip address as string
            if (source_addr.ss_family == PF_INET) {
                inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
            }
            else if (source_addr.ss_family == PF_INET6) {
                inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
            }
            switch (udp_dataframe_in.command) {
                case 0: // Stop all operations
                    motor_set_speed(0.0f);
                    break;
                case 1: // Set speed
                    motor_set_speed(udp_dataframe_in.motor_speed);
                    udp_dataframe_out.motor_speed = motor_get_speed();
                    udp_dataframe_out.timestamp = esp_timer_get_time();
                    udp_dataframe_out.v_bat = v_bat_read();
                    int err = sendto(sock, &udp_dataframe_out, sizeof(udp_dataframe_out), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                    if (err < 0) {
                        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                        break;
                    }
                default:
                    //tbd
                    break;
            }
        }
    }
}