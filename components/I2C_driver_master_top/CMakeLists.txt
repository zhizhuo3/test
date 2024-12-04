file(GLOB_RECURSE SOURCES   ./*.c)
idf_component_register(SRCS "I2C_driver_master_top.c"
                        INCLUDE_DIRS
                                    .
                        REQUIRES
                        driver
                        freertos)
