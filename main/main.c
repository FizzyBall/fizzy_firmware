#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// RTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_timer.h>
#include <esp_log.h>

//Motor
#include <motor.h>

// ADC 
#include <v_bat.h>

//MPU
#include <LSM6DSO16IS.h>

//UDP
#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#define UDP_PORT    4711

//OAT update
#include <wifi_setup.h>
#include <http_ota.h>


static const char *TAG = "fizzy";
static const char *git_info = GIT_STAT;

static void MPU_downlink_task(void * param);

//UDP dataframes

static char addr_str[128];

static struct {
    uint8_t command;
    float   motor_speed;
} udp_dataframe_in;

static struct __attribute__((packed, aligned(1))) {
    int64_t timestamp;
    float   motor_speed;
    float   v_bat;
    uint8_t imu_raw_data[SPI_REC_BUF_SIZE];
} udp_dataframe_out;

typedef struct {
    int socket;
    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
} socket_info_t;

static socket_info_t socket_info;
static volatile bool DOWNLINK_ACTIVE = false;

// data downlink task
static void MPU_downlink_task(void * param) {
    int err;
    DOWNLINK_ACTIVE = true;
    while(DOWNLINK_ACTIVE) {
        if(xSemaphoreTake(sem_mpu_read, portMAX_DELAY) == pdTRUE) {
            udp_dataframe_out.timestamp = esp_timer_get_time();
            uint8_t *imu_data = MPU_read_RAW();
            memcpy(udp_dataframe_out.imu_raw_data, imu_data, SPI_REC_BUF_SIZE);
            udp_dataframe_out.motor_speed = motor_get_speed();
            udp_dataframe_out.v_bat = 0;//v_bat_read();
            err = sendto(socket_info.socket,
                            &udp_dataframe_out,
                            sizeof(udp_dataframe_out),
                            0,
                            (struct sockaddr *)&socket_info.source_addr,
                            sizeof(socket_info.source_addr)
                        );
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

// --- MAIN ---

void app_main(void) {
    int err;
    v_bat_init();
    motor_init();
    MPU_init();
    WIFI_init();
    OTA_init();

    // create socket
    socket_info.socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_info.socket < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
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
    err = bind(socket_info.socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    }
    ESP_LOGI(TAG, "Socket bound, port %d", UDP_PORT);

    socklen_t socklen = sizeof(socket_info.source_addr);

    while(true) {
        // Get data
        int len = recvfrom(socket_info.socket,
                            (char *) &udp_dataframe_in,
                            sizeof(udp_dataframe_in),
                            0,
                            (struct sockaddr *)&socket_info.source_addr,
                            &socklen
                        );
        // check data length
        if (len < 0) {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            break;
        }
        // Process data
        else {
            // Get the sender's ip address as string
            if (socket_info.source_addr.ss_family == PF_INET) {
                inet_ntoa_r(((struct sockaddr_in *)&socket_info.source_addr)->sin_addr,
                                addr_str,
                                sizeof(addr_str) - 1
                            );
            }
            else if (socket_info.source_addr.ss_family == PF_INET6) {
                inet6_ntoa_r(((struct sockaddr_in6 *)&socket_info.source_addr)->sin6_addr,
                                addr_str,
                                sizeof(addr_str) - 1
                            );
            }
            switch (udp_dataframe_in.command) {
                case 0: // Stop all operations
                    motor_set_speed(0.0f);
                    DOWNLINK_ACTIVE = false;
                    break;
                case 1: // Set speed
                    motor_set_speed(udp_dataframe_in.motor_speed);
                    if(!DOWNLINK_ACTIVE) { 
                        udp_dataframe_out.motor_speed = motor_get_speed();
                        udp_dataframe_out.timestamp = esp_timer_get_time();
                        udp_dataframe_out.v_bat = v_bat_read();
                        err = sendto(socket_info.socket,
                                        &udp_dataframe_out,
                                        sizeof(udp_dataframe_out),
                                        0,
                                        (struct sockaddr *)&socket_info.source_addr,
                                        sizeof(socket_info.source_addr)
                                    );
                        if (err < 0) {
                            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                            break;
                        }
                    }
                    break;
                case 2: // start data downlink
                    if(!DOWNLINK_ACTIVE) {
                        xTaskCreate(MPU_downlink_task,
                                    "MPU_downlink_task",
                                    1024,
                                    NULL,
                                    2,
                                    NULL
                                );  
                    }
                    break;
                case 66: //get raw IMU data
                    if(!DOWNLINK_ACTIVE) {
                        uint8_t *imu_data = MPU_read_RAW();
                        memcpy(udp_dataframe_out.imu_raw_data, imu_data, SPI_REC_BUF_SIZE);
                        udp_dataframe_out.motor_speed = motor_get_speed();
                        udp_dataframe_out.timestamp = esp_timer_get_time();
                        udp_dataframe_out.v_bat = v_bat_read();
                        err = sendto(socket_info.socket,
                                        &udp_dataframe_out,
                                        sizeof(udp_dataframe_out),
                                        0,
                                        (struct sockaddr *)&socket_info.source_addr,
                                        sizeof(socket_info.source_addr)
                                    );
                        if (err < 0) {
                            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                            break;
                        }
                    }
                    break;
                case 0xff: // request firmware version
                    if(!DOWNLINK_ACTIVE) {
                        err = sendto(socket_info.socket,
                                        git_info,
                                        strlen(git_info),
                                        0,
                                        (struct sockaddr *)&socket_info.source_addr,
                                        sizeof(socket_info.source_addr)
                                    );
                        if (err < 0) {
                            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                        }
                    }
                    break;
                default:
                    //tbd
                    break;
            }
        }
    }
}