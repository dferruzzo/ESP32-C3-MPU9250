idf_component_register(SRCS "mpu9250.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver my_i2c)