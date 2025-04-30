#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "my_i2c.cpp"
//#include "pl_i2c.h"
//#include "mpu9250.hpp"

#define MPU9250_ADDRESS 0x68
#define MPU9250_WHO_AM_I 0x75

extern "C" void app_main(void) {

    I2C i2c(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    
    i2c.set_device(MPU9250_ADDRESS);
    
    uint8_t who_am_i = 0;
    i2c.read(MPU9250_WHO_AM_I, &who_am_i, 1);
    ESP_LOGI("MAIN", "WHO_AM_I: 0x%02X", who_am_i);

}
