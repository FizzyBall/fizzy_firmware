cmake_minimum_required(VERSION 3.16)

idf_component_register(
    SRCS            "LSM6DSO16IS.c"
    INCLUDE_DIRS    "."
    REQUIRES        "esp_driver_spi"
                    "esp_driver_gpio"
)