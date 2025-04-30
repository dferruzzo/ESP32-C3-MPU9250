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

void show_vector(int16_t *data) {
    for (int i = 0; i < 3; i++) {
        ESP_LOGI("test_func", "Data[%d]: %.2f", i, data[i]);  // Fixed pointer arithmetic
    }
}

void app_main() {

    int16_t *gyro_data = (int16_t *)malloc(3 * sizeof(int16_t));  // Allocate as single contiguous block
    if (gyro_data == NULL) {
        ESP_LOGE("app_main", "Failed to allocate memory for data");
        return;
    } else {
        ESP_LOGI("app_main", "Memory allocated successfully");
    }

	i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;

    // Initialize I2C using my_i2c component
    ESP_ERROR_CHECK(my_i2c_init(&bus_handle, &dev_handle, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ, MPU9250_SENSOR_ADDR));
    
    // Use the MPU9250 component
    mpu9250_log_who_am_i(dev_handle);

    // Configure gyroscope settings
	mpu9250_configure_gyroscope(dev_handle, MPU9250_GYRO_FS_SEL_1000);

    while(1){
        // Read gyroscope data
        //mpu9250_read_gyroscope(dev_handle, &gyro_x, &gyro_y, &gyro_z);
        mpu9250_read_gyroscope(dev_handle, gyro_data);
        show_vector(gyro_data);

        
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 1 second
    }

	// Deinitialize I2C using my_i2c component
    ESP_ERROR_CHECK(my_i2c_deinit(bus_handle, dev_handle));

    //free(gyro_data);
}