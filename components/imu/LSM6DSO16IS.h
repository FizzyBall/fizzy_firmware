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

void MPU_init(void);