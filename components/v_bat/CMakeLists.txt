cmake_minimum_required(VERSION 3.16)

idf_component_register(
    SRCS            "v_bat.c"
    INCLUDE_DIRS    "."
    REQUIRES        "esp_adc"
)