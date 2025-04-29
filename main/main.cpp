#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "my_i2c.hpp"
#include "mpu9250.hpp"

extern "C" void app_main() {
    I2C i2c(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    MPU9250 mpu9250(i2c, MPU9250_SENSOR_ADDR);
    mpu9250.log_who_am_i();
    
    i2c.deinit();
}
