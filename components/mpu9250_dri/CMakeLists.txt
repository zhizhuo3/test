file(GLOB_RECURSE SOURCES   ./*.c)
idf_component_register(SRCS "mpu9250_dri.c"
                        INCLUDE_DIRS
                                    .
                        REQUIRES
                        driver
                        freertos)