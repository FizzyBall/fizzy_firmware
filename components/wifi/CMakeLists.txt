cmake_minimum_required(VERSION 3.16)

idf_component_register(
    SRCS            "wifi_setup.c"
    INCLUDE_DIRS    "."
    REQUIRES        "esp_wifi"
                    "esp_event"
                    "nvs_flash"
)