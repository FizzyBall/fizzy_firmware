#include <stdint.h>

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>

#include "LSM6DSO16IS.h"

// 16-bit 2's complement to dec
#define TWO2DEC(val)    (0x8000 & (val) ? (int16_t)(0x7FFF & (val))-0x8000 : (int16_t)(val))

static const char *TAG = "MPU";

static spi_device_handle_t spi_handle;

// local functions
static void cs_active(spi_transaction_t *trans);
static void cs_inactive(spi_transaction_t *trans);
static void spi_write(uint8_t reg_addr, uint8_t *data, uint8_t len_data);
static void spi_write_reg(uint8_t reg_addr, uint8_t data);
static void spi_read(uint8_t reg_addr, uint8_t *data, uint8_t len_data);

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
    
    //setup data-ready interrupt on INT1 (not needed for now)
    spi_write_reg(INT1_CTRL, 0b00000000);
    // power up ACC (6667Hz, +/-8g)
    spi_write_reg(CTRL1_XL, 0b10101100);
    // power up GYRO (6667Hz, +/-250dps)
    spi_write_reg(CTRL2_G, 0b10100000);
}


static uint8_t raw_data[12];
static uint8_t imu_status;

uint8_t *MPU_read_ACC_GYRO() {
    // read old data to trigger new measurement
    spi_read(OUTX_L_G, raw_data, 12);
    // wait a ms
    //vTaskDelay(1 / portTICK_PERIOD_MS);
    // wait until we have data ready (IMU and GYRO)
    do {
        spi_read(STATUS_REG_IMU, &imu_status, 1);
    } while((imu_status & 0b11) < 0b11);
    // read actual data
    spi_read(OUTX_L_G, raw_data, 12);
    return raw_data;
}

