#ifndef MPU9250_H
#define MPU9250_H

#include <stdint.h>
#include "driver/i2c_master.h"
#include "MPU9250_Register_Map.h"
#include "my_i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "MPU9250_Configuration_Map.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MPU9250_SENSOR_ADDR         0x68        /*!< Address of the MPU9250 sensor */
#define MPU9250_MAGNETOMETER_ADDR  0x0C        /*!< Address of the magnetometer (AK8963) */
#define MPU9250_WHO_AM_I_REG_ADDR   0x75        /*!< Register addresses of the "who am I" register */
#define MPU9250_PWR_MGMT_1_REG_ADDR 0x6B        /*!< Register addresses of the power management register */
#define MPU9250_RESET_BIT           7

#ifdef __cplusplus
extern "C" {
#endif

/* MPU9250 */

esp_err_t mpu9250_read_register(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len);
esp_err_t mpu9250_write_register(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data);
void mpu9250_log_who_am_i(i2c_master_dev_handle_t dev_handle);

/* Gyroscope */

//esp_err_t mpu9250_configure_gyroscope(i2c_master_dev_handle_t dev_handle, uint8_t fs_sel);
esp_err_t mpu9250_configure_gyroscope(i2c_master_dev_handle_t dev_handle, uint8_t fs_sel, uint8_t gyro_dlpf_cfg);
esp_err_t mpu9250_gyroscope_reset(i2c_master_dev_handle_t dev_handle);
esp_err_t mpu9250_read_gyroscope(i2c_master_dev_handle_t dev_handle, float *gyro_data);

/* Accelerometer */

esp_err_t mpu9250_read_accelerometer(i2c_master_dev_handle_t dev_handle, float *accel_data);
esp_err_t mpu9250_configure_accelerometer(i2c_master_dev_handle_t dev_handle, uint8_t fs_sel);
esp_err_t mpu9250_accelerometer_reset(i2c_master_dev_handle_t dev_handle);

/* Temperatura */

esp_err_t mpu9250_read_temperature(i2c_master_dev_handle_t dev_handle, float *temp_data);

/* Magnetometer */

esp_err_t
mpu9250_read_magnetometer(i2c_master_dev_handle_t dev_handle_mpu9250,
						  i2c_master_dev_handle_t dev_handle_magnetometer,
						  float *mag_data);

                          
#ifdef __cplusplus
}
#endif

#endif // MPU9250_H