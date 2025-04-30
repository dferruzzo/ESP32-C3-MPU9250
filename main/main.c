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

    /* Verifica se a memoria foi alocada com sucesso */

    if ((gyr_data == NULL)||(acc_data == NULL)|| (mag_data == NULL)) {
        ESP_LOGE("app_main", "Failed to allocate memory for data");
        return;
    } else {
        ESP_LOGI("app_main", "Memory allocated successfully");
    }

    /* Iniciando o i2c */

	i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;

    // Initialize I2C com o endereço do MPU9250
    ESP_ERROR_CHECK(my_i2c_init(&bus_handle, &dev_handle, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ, MPU9250_SENSOR_ADDR));
    
    /* Testa o MPU9250 */
    mpu9250_log_who_am_i(dev_handle);

    // Configure gyroscope settings
	mpu9250_configure_gyroscope(dev_handle, MPU9250_GYRO_FS_SEL_1000);

    /* Loop */

    while(1){
        // Read gyroscope data
        
        mpu9250_read_gyroscope(dev_handle, gyr_data);
		ESP_LOGI("main ", "Gyroscope data (degrees/sec):");
		show_vector(gyr_data);

        mpu9250_read_accelerometer(dev_handle, acc_data);
        ESP_LOGI("main ", "Accelerometer data (g):");
        show_vector(acc_data);

		vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 1 second
    }

	// Deinitialize I2C using my_i2c component
    ESP_ERROR_CHECK(my_i2c_deinit(bus_handle, dev_handle));

    /* Limpando a memoria */

    free(gyr_data);
    free(acc_data);
    free(mag_data);

    ESP_LOGI("app_main", "Memory freed successfully");
}