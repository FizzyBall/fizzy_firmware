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
// ADC 
//#include "driver/adc.h"
//#include "esp_adc_cal.h"

//UDP
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "wifi_setup.h"
#include "http_ota.h"

#define UDP_PORT    4711

static const char *git_info = GIT_STAT;

void app_main(void) {

    //ADC_init();
    //MOTOR_init();
    //MPU_spi_init();
    //MPU_imu_init();
    WIFI_init();
    OTA_init();

    while(true) {
        printf("Git: %s\n", git_info);
        // Wait for one second
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}