/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* i2c - Simple Example

   Simple I2C example that shows how to initialize I2C
   as well as reading and writing from and to registers for a sensor connected over I2C.

   The sensor used in this example is a MPU9250 inertial measurement unit.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "mpu9250.h"
#include "my_i2c.h"

    void app_main() {
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;

    // Initialize I2C using my_i2c component
    ESP_ERROR_CHECK(my_i2c_init(&bus_handle, &dev_handle, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ, MPU9250_SENSOR_ADDR));
    
    // Use the MPU9250 component
    mpu9250_log_who_am_i(dev_handle);

    // Deinitialize I2C using my_i2c component
    ESP_ERROR_CHECK(my_i2c_deinit(bus_handle, dev_handle));

}
