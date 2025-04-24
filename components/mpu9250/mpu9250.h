#ifndef MPU9250_H
#define MPU9250_H

#include <stdint.h>
#include "driver/i2c_master.h"
#include "MPU9250_Register_Map.h"
#include "my_i2c.h"
#include "esp_err.h"

#define MPU9250_SENSOR_ADDR         0x68        /*!< Address of the MPU9250 sensor */
#define MPU9250_WHO_AM_I_REG_ADDR   0x75        /*!< Register addresses of the "who am I" register */
#define MPU9250_PWR_MGMT_1_REG_ADDR 0x6B        /*!< Register addresses of the power management register */
#define MPU9250_RESET_BIT           7

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t mpu9250_read_register(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len);
esp_err_t mpu9250_write_register(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data);
void mpu9250_log_who_am_i(i2c_master_dev_handle_t dev_handle);

#ifdef __cplusplus
}
#endif

#endif // MPU9250_H