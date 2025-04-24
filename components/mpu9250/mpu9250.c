#include "mpu9250.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MPU9250";

esp_err_t mpu9250_read_register(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len) {
    return i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

esp_err_t mpu9250_write_register(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data) {
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void mpu9250_log_who_am_i(i2c_master_dev_handle_t dev_handle) {
    uint8_t who_am_i = 0;
    if (mpu9250_read_register(dev_handle, MPU9250_WHO_AM_I, &who_am_i, 1) == ESP_OK) {
        ESP_LOGI(TAG, "WHO_AM_I register: 0x%02X", who_am_i);
    } else {
        ESP_LOGE(TAG, "Failed to read WHO_AM_I register");
    }
}

void mpu9250_reset(i2c_master_dev_handle_t dev_handle) {
    uint8_t data = 1 << MPU9250_RESET_BIT;
    if (mpu9250_write_register(dev_handle, MPU9250_PWR_MGMT_1, data) == ESP_OK) {
        ESP_LOGI(TAG, "MPU9250 reset command sent");
    } else {
        ESP_LOGE(TAG, "Failed to send reset command to MPU9250");
    }
}

/**
 * @brief Configures the gyroscope settings of the MPU9250 sensor.
 *
 * This function writes the specified gyroscope configuration value to the 
 * MPU9250_GYRO_CONFIG register of the MPU9250 sensor using the provided 
 * I2C master device handle.
 *
 * @param[in] dev_handle The I2C master device handle used to communicate with the MPU9250 sensor.
 * @param[in] gyro_config The gyroscope configuration value to be written to the MPU9250_GYRO_CONFIG register.
 *                        This value determines the gyroscope's full-scale range and other settings.
 *
 * @return
 *      - ESP_OK: If the gyroscope configuration was successfully written.
 *      - ESP_FAIL: If the gyroscope configuration write operation failed.
 *
 * @note Ensure that the I2C master device handle is properly initialized and 
 *       the MPU9250 sensor is connected before calling this function.
 * @note The `gyro_config` parameter must be set according to the MPU9250 datasheet.
 */
esp_err_t mpu9250_configure_gyroscope(i2c_master_dev_handle_t dev_handle, uint8_t gyro_config) {
    if (mpu9250_write_register(dev_handle, MPU9250_GYRO_CONFIG, gyro_config) == ESP_OK) {
        ESP_LOGI(TAG, "Gyroscope configured with value: 0x%02X", gyro_config);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to configure gyroscope");
        return ESP_FAIL;
    }
}
