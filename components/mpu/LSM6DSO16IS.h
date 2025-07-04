#pragma once
#include <driver/spi_master.h>

// SPI GPIO definitions
#define MPU_MOSI_IO     23
#define MPU_MISO_IO     19
#define MPU_SCLK_IO     18
#define MPU_CS_IO       5
#define MPU_INT_IO      25
#define MPU_SPI_HOST    SPI3_HOST           // VSPI
#define MPU_CLK         SPI_MASTER_FREQ_10M 

// public functions
void MPU_init(void);

// LSM6DSO16IS register map
#define FUNC_CFG_ACCESS             0x01    // R/W default = 0b00000000
#define PIN_CTRL                    0x02    // R/W default = 0b00111111
#define DRDY_PULSED_REG             0x0B    // R/W default = 0b00000000
#define INT1_CTRL                   0x0D    // R/W default = 0b00000000
#define INT2_CTRL                   0x0E    // R/W default = 0b00000000
#define WHO_AM_I                    0x0F    // R   default = 0b00100010
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
#define STATUS_REG                  0x1E    // R   output
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