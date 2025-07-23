#include <stdint.h>

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "LSM6DSO16IS.h"
#include "sensor_fusion_9x.h"


static const char *TAG = "MPU";


// local functions
static void cs_active(spi_transaction_t *trans);
static void cs_inactive(spi_transaction_t *trans);
static void spi_write(uint8_t reg_addr, uint8_t *data, uint8_t len_data);
static void spi_write_reg(uint8_t reg_addr, uint8_t data);
static void IRAM_ATTR imu_interrupt_handler(void *args);


static spi_device_handle_t  spi_handle;
extern SemaphoreHandle_t    sem_mpu_read = NULL;
DMA_ATTR static uint8_t     spi_rec_buf[SPI_REC_BUF_SIZE];


static void cs_active(spi_transaction_t *trans) {
    gpio_set_level(MPU_CS_IO, 0);
}

static void cs_inactive(spi_transaction_t *trans) {
    gpio_set_level(MPU_CS_IO, 1);
}

static void spi_write(uint8_t reg_addr, uint8_t *data, uint8_t len_data) {
    spi_transaction_t spi_trans = {
        .addr = reg_addr,
        .length = 8 * len_data,
        .tx_buffer = data,
    };
    ESP_ERROR_CHECK(spi_device_transmit(spi_handle, &spi_trans));
}

static void spi_write_reg(uint8_t reg_addr, uint8_t data) {
    spi_transaction_t spi_trans = {
        .addr = reg_addr,
        .length = 8,
        .tx_buffer = &data,
    };
    ESP_ERROR_CHECK(spi_device_transmit(spi_handle, &spi_trans));
}

static void spi_read(uint8_t reg_addr, uint8_t *data, uint8_t len_data){
    spi_transaction_t spi_trans = {
        .addr = reg_addr | 0x80,
        .length = 8 * len_data,
        .rx_buffer = data,
    };
    ESP_ERROR_CHECK(spi_device_transmit(spi_handle, &spi_trans));
}


void MPU_init(void) {

    spi_bus_config_t buscfg = {
        .mosi_io_num = MPU_MOSI_IO,
        .miso_io_num = MPU_MISO_IO,
        .sclk_io_num = MPU_SCLK_IO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = SPI_REC_BUF_SIZE,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(MPU_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = MPU_CLK,
        .mode = 3,
        .spics_io_num = -1,
        .queue_size = 1,
        .address_bits = 8,
        .pre_cb = cs_active,
        .post_cb = cs_inactive, 
    };
    ESP_ERROR_CHECK(spi_bus_add_device(MPU_SPI_HOST, &devcfg, &spi_handle));

    gpio_set_direction(MPU_CS_IO, GPIO_MODE_OUTPUT);
    gpio_set_level(MPU_CS_IO, 1);
    
    // upload ISPU config
    uint8_t dummy;
    for(int i=0; i<MEMS_CONF_ARRAY_LEN(ispu_conf_conf_0); i++) {
        switch(ispu_conf_conf_0[i].type) {
            case MEMS_CONF_OP_TYPE_WRITE:
                spi_write_reg(ispu_conf_conf_0[i].address, ispu_conf_conf_0[i].data);
                break;
            case MEMS_CONF_OP_TYPE_DELAY:
                vTaskDelay(ispu_conf_conf_0[i].data/portTICK_PERIOD_MS);
                break;
            case MEMS_CONF_OP_TYPE_READ:
                spi_read(ispu_conf_conf_0[i].address, &dummy, 1);
                break;
            case MEMS_CONF_OP_TYPE_POLL_SET:
                do {
                    spi_read(ispu_conf_conf_0[i].address, &dummy, 1);
                } while(!(dummy&ispu_conf_conf_0[i].data));
                break;
            case MEMS_CONF_OP_TYPE_POLL_RESET:
                do {
                    spi_read(ispu_conf_conf_0[i].address, &dummy, 1);
                } while(dummy&ispu_conf_conf_0[i].data);
                break;
        }
    }

    // switch to ISPU register mode
    spi_write_reg(FUNC_CFG_ACCESS, 0x80);

    // Interrupt handling
    sem_mpu_read = xSemaphoreCreateBinary();
    gpio_set_direction(MPU_INT_IO, GPIO_MODE_INPUT);
    gpio_pullup_dis(MPU_INT_IO);
    gpio_pulldown_dis(MPU_INT_IO);
    gpio_set_intr_type(MPU_INT_IO, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(MPU_INT_IO, imu_interrupt_handler, (void *)MPU_INT_IO);
}

static void IRAM_ATTR imu_interrupt_handler(void *args) {
    if((int)args == MPU_INT_IO) {
        BaseType_t task_woken = pdFALSE;
        xSemaphoreGiveFromISR(sem_mpu_read, &task_woken);
        if(task_woken) {
            portYIELD_FROM_ISR();
        }
    }
}

uint8_t *MPU_read_RAW() {
    spi_read(0x10, spi_rec_buf, SPI_REC_BUF_SIZE);
    return spi_rec_buf;
}

