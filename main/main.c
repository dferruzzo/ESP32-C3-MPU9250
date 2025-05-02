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
#include "utils.h"

void app_main() {

    /* Reservando memoria para os dados */

    float *gyr_data = (float *)malloc(3 * sizeof(float));  // Allocate as single contiguous block
    float *acc_data = (float *)malloc(3 * sizeof(float));  // Allocate as single contiguous block
    float *mag_data = (float *)malloc(3 * sizeof(float));  // Allocate as single contiguous block
    float *temp_data = (float *)malloc(1 * sizeof(float));  // Allocate as single contiguous block

    /* Verifica se a memoria foi alocada com sucesso */

    if ((gyr_data == NULL) || (acc_data == NULL) || (temp_data == NULL) || (mag_data == NULL)) {
        ESP_LOGE("app_main", "Failed to allocate memory for data");
        return;
    } else {
        ESP_LOGI("app_main", "Memory allocated successfully");
    }

    /* Iniciando o i2c */

	i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle_mpu9250;
    i2c_master_dev_handle_t dev_handle_magnetometer;

    // Initialize I2C bus
	ESP_ERROR_CHECK(my_i2c_init(&bus_handle, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO));

    // add MPU9250 device to I2C bus
	ESP_ERROR_CHECK(my_i2c_add_dev(&bus_handle, &dev_handle_mpu9250, I2C_MASTER_FREQ_HZ, MPU9250_SENSOR_ADDR));

    // add magnetometer device to I2C bus
    ESP_ERROR_CHECK(my_i2c_add_dev(&bus_handle, &dev_handle_magnetometer, I2C_MASTER_FREQ_HZ, MPU9250_MAGNETOMETER_ADDR));

	// Scan I2C bus for devices
	ESP_ERROR_CHECK(my_i2c_scan(bus_handle));

	/* Testa o MPU9250 */
    mpu9250_log_who_am_i(dev_handle_mpu9250);

    // Configure gyroscope settings
	mpu9250_configure_gyroscope(dev_handle_mpu9250, MPU9250_GYRO_FS_SEL_1000,  MPU9250_FCHOICE_B_GYRO_FILTER_ENABLED, MPU9250_GYRO_DLPF_CFG_20HZ);
    mpu9250_calibrate_gyroscope(dev_handle_mpu9250);

	// Configure accelerometer settings
	mpu9250_configure_accelerometer(dev_handle_mpu9250, MPU9250_ACCEL_FS_SEL_4, MPU9250_ACCEL_FIL_EN_BW_21Hz);

    while(1){
        // Read gyroscope data
        
        mpu9250_read_gyroscope(dev_handle_mpu9250, gyr_data);
		
        //Read acelerometer data

        mpu9250_read_accelerometer(dev_handle_mpu9250, acc_data);
        
        // Read Magnetometer data

		mpu9250_read_magnetometer(dev_handle_mpu9250, dev_handle_magnetometer, mag_data);
		
        //Read temperature data
        
        mpu9250_read_temperature(dev_handle_mpu9250, temp_data);
        
        // Print data in CSV format. The format is:
        // "GyrX,GyrY, GyrZ, AccX, AccY, AccZ, MagX, MagY, MagZ, Temp"

        printf("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
            gyr_data[0], gyr_data[1], gyr_data[2], 
            acc_data[0], acc_data[1], acc_data[2],
            mag_data[0], mag_data[1], mag_data[2],
            temp_data[0]);

        // Delay for 0.5 second        

        vTaskDelay(pdMS_TO_TICKS(250)); // Delay for 1 second
    }

}