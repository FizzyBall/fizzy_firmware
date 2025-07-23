#pragma once
#include <driver/spi_master.h>

// SPI GPIO definitions
#define MPU_MOSI_IO         23
#define MPU_MISO_IO         19
#define MPU_SCLK_IO         18
#define MPU_CS_IO           5
#define MPU_INT_IO          25
#define MPU_SPI_HOST        SPI3_HOST           // VSPI
#define MPU_CLK             SPI_MASTER_FREQ_10M 
#define SPI_REC_BUF_SIZE    17

// public functions
void MPU_init(void);
uint8_t *MPU_read_RAW();

// semaphore to sync interrupt driven mpu readout
extern SemaphoreHandle_t    sem_mpu_read;

// LSM6DSO16IS register map
#define FUNC_CFG_ACCESS             0x01    // R/W default = 0b00000000
#define PIN_CTRL                    0x02    // R/W default = 0b00111111
#define DRDY_PULSED_REG             0x0B    // R/W default = 0b00000000
#define INT1_CTRL                   0x0D    // R/W default = 0b00000000
#define INT2_CTRL                   0x0E    // R/W default = 0b00000000
#define WHO_AM_I_IMU                0x0F    // R   default = 0b00100010
#define CTRL1_XL                    0x10    // R/W default = 0b00000000
#define CTRL2_G                     0x11    // R/W default = 0b00000000
#define CTRL3_C                     0x12    // R/W default = 0b00000100
#define CTRL4_C                     0x13    // R/W default = 0b00000000
#define CTRL5_C                     0x14    // R/W default = 0b00000000
#define CTRL6_C                     0x15    // R/W default = 0b00000000
#define CTRL7_G                     0x16    // R/W default = 0b00000000
#define CTRL9_C                     0x18    // R/W default = 0b11100000
#define CTRL10_C                    0x19    // R/W default = 0b00000000
#define ISPU_INT_STATUS0_MAINPAGE   0x1A    // R   output
#define ISPU_INT_STATUS1_MAINPAGE   0x1B    // R   output
#define ISPU_INT_STATUS2_MAINPAGE   0x1C    // R   output
#define ISPU_INT_STATUS3_MAINPAGE   0x1D    // R   output
#define STATUS_REG_IMU              0x1E    // R   output
#define OUT_TEMP_L                  0x20    // R   output
#define OUT_TEMP_H                  0x21    // R   output
#define OUTX_L_G                    0x22    // R   output
#define OUTX_H_G                    0x23    // R   output
#define OUTY_L_G                    0x24    // R   output
#define OUTY_H_G                    0x25    // R   output
#define OUTZ_L_G                    0x26    // R   output
#define OUTZ_H_G                    0x27    // R   output
#define OUTX_L_A                    0x28    // R   output
#define OUTX_H_A                    0x29    // R   output
#define OUTY_L_A                    0x2A    // R   output
#define OUTY_H_A                    0x2B    // R   output
#define OUTZ_L_A                    0x2C    // R   output
#define OUTZ_H_A                    0x2D    // R   output
#define STATUS_MASTER_MAINPAGE      0x39    // R   output
#define TIMESTAMP0                  0x40    // R   output
#define TIMESTAMP1                  0x41    // R   output
#define TIMESTAMP2                  0x42    // R   output
#define TIMESTAMP3                  0x43    // R   output
#define MD1_CFG                     0x5E    // R/W default = 00000000
#define MD2_CFG                     0x5F    // R/W default = 00000000
#define INTERNAL_FREQ_FINE          0x63    // R   output
#define ISPU_DUMMY_CFG_1_L          0x73    // R/W default = 00000000
#define ISPU_DUMMY_CFG_1_H          0x74    // R/W default = 00000000
#define ISPU_DUMMY_CFG_2_L          0x75    // R/W default = 00000000
#define ISPU_DUMMY_CFG_2_H          0x76    // R/W default = 00000000
#define ISPU_DUMMY_CFG_3_L          0x77    // R/W default = 00000000
#define ISPU_DUMMY_CFG_3_H          0x78    // R/W default = 00000000
#define ISPU_DUMMY_CFG_4_L          0x79    // R/W default = 00000000
#define ISPU_DUMMY_CFG_4_H          0x7A    // R/W default = 00000000

// LSM6DSO16IS sensor hub register
#define ENSOR_HUB_1                 0x02
#define ENSOR_HUB_2                 0x03
#define ENSOR_HUB_3                 0x04
#define ENSOR_HUB_4                 0x05
#define ENSOR_HUB_5                 0x06
#define ENSOR_HUB_6                 0x07
#define ENSOR_HUB_7                 0x08
#define ENSOR_HUB_8                 0x09
#define ENSOR_HUB_9                 0x0A
#define ENSOR_HUB_10                0x0B
#define ENSOR_HUB_11                0x0C
#define ENSOR_HUB_12                0x0D
#define ENSOR_HUB_13                0x0E
#define ENSOR_HUB_14                0x0F
#define ENSOR_HUB_15                0x10
#define ENSOR_HUB_16                0x11
#define ENSOR_HUB_17                0x12
#define ENSOR_HUB_18                0x13
#define ASTER_CONFIG                0x14
#define LV0_ADD                     0x15
#define LV0_SUBADD                  0x16
#define LV0_CONFIG                  0x17
#define LV1_ADD                     0x18
#define LV1_SUBADD                  0x19
#define LV1_CONFIG                  0x1A
#define LV2_ADD                     0x1B
#define LV2_SUBADD                  0x1C
#define LV2_CONFIG                  0x1D
#define LV3_ADD                     0x1E
#define LV3_SUBADD                  0x1F
#define LV3_CONFIG                  0x20
#define ATAWRITE_SLV0               0x21
#define TATUS_MASTER                0x22

// LIS2MDL register map
#define OFFSET_X_REG_L              0x45    // R/W default = 00000000
#define OFFSET_X_REG_H              0x46    // R/W default = 00000000
#define OFFSET_Y_REG_L              0x47    // R/W default = 00000000
#define OFFSET_Y_REG_H              0x48    // R/W default = 00000000
#define OFFSET_Z_REG_L              0x49    // R/W default = 00000000
#define OFFSET_Z_REG_H              0x4A    // R/W default = 00000000
#define WHO_AM_I_MAG                0x4F    // R   output
#define CFG_REG_A                   0x60    // R/W default = 00000011
#define CFG_REG_B                   0x61    // R/W default = 00000000
#define CFG_REG_C                   0x62    // R/W default = 00000000
#define INT_CRTL_REG                0x63    // R/W default = 11100000
#define INT_SOURCE_REG              0x64    // R   output
#define INT_THS_L_REG               0x65    // R/W default = 00000000
#define INT_THS_H_REG               0x66    // R/W default = 00000000
#define STATUS_REG_MAG              0x67    // R   output
#define OUTX_L_REG                  0x68    // R   output
#define OUTX_H_REG                  0x69    // R   output
#define OUTY_L_REG                  0x6A    // R   output
#define OUTY_H_REG                  0x6B    // R   output
#define OUTZ_L_REG                  0x6C    // R   output
#define OUTZ_H_REG                  0x6D    // R   output
#define TEMP_OUT_L_REG              0x6E    // R   output
#define TEMP_OUT_H_REG              0x6F    // R   output