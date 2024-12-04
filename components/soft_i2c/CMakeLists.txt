file(GLOB_RECURSE SOURCES   ./*.c)
idf_component_register(SRCS "soft_i2c.c"
                        INCLUDE_DIRS
                                    .
                        REQUIRES
                        driver
                        freertos)