#ifndef MPU9250_H
#define MPU9250_H

#include <stdint.h>
#include "esp_log.h"
#include "my_i2c.hpp"
#include "MPU9250_Register_Map.h"
#include "MPU9250_Configuration_Map.h"

#define MPU9250_SENSOR_ADDR         0x68        /*!< Address of the MPU9250 sensor */
#define MPU9250_WHO_AM_I_REG_ADDR   0x75        /*!< Register addresses of the "who am I" register */
#define MPU9250_PWR_MGMT_1_REG_ADDR 0x6B        /*!< Register addresses of the power management register */
#define MPU9250_RESET_BIT           7

class MPU9250
{
    private:
        I2C &i2c;
        uint8_t dev_addr = MPU9250_SENSOR_ADDR;
        uint8_t who_am_i = 0;
        uint8_t gyro_fs_sel = 0x00; // Full-scale range selection
        uint8_t *gyro_fs_sel_ptr = &gyro_fs_sel; // Full-scale range selection pointer
        bool is_mpu9250_init = false;

    public:
        //uint8_t who_am_i = 0;

        MPU9250(I2C &i2c, uint8_t dev_addr);
        ~MPU9250();

        esp_err_t init();
        esp_err_t deinit();
        //esp_err_t set_device(uint8_t dev_addr);
        void log_who_am_i();
        esp_err_t read_register(uint8_t reg_addr, uint8_t *data, size_t len);
        esp_err_t write_register(uint8_t reg_addr, const uint8_t *data, size_t len);
        esp_err_t configure_gyroscope(uint8_t fs_sel);
        esp_err_t gyroscope_reset();
        esp_err_t read_gyroscope(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);
        esp_err_t reset();
};

#endif
