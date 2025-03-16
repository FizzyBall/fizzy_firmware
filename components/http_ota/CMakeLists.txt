cmake_minimum_required(VERSION 3.16)

idf_component_register(
    SRCS            "http_ota.c"
    INCLUDE_DIRS    "."
    EMBED_TXTFILES  "web/index.html" 
    REQUIRES        "app_update"
                    "esp_http_server"
)