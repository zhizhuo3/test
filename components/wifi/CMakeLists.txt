file(GLOB_RECURSE SOURCES   ./*.c)
idf_component_register(SRCS "wifi.c"
                        INCLUDE_DIRS
                                    .
                        REQUIRES
                        driver
                        freertos
                        esp_wifi
                        nvs_flash
                        lwip
                        )