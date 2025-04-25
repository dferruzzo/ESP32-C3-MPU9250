#include "mpu9250.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MPU9250_Configuration_Map.h"

static const char *TAG = "MPU9250";

uint16_t gyro_full_scale_range[] = {
    [0] = MPU9250_GYRO_FS_SEL_250_VALUE,
    [1] = MPU9250_GYRO_FS_SEL_500_VALUE,
    [2] = MPU9250_GYRO_FS_SEL_1000_VALUE,
    [3] = MPU9250_GYRO_FS_SEL_2000_VALUE
};

uint8_t gyro_fs_sel = 0x00; // Full-scale range selection

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} int16_vector3_t;

int16_vector3_t *gyro_data_raw_ptr = NULL;
int16_vector3_t *gyro_data_scaled_ptr = NULL;

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
esp_err_t mpu9250_configure_gyroscope(i2c_master_dev_handle_t dev_handle, uint8_t fs_sel) {
    if (fs_sel > 3) {
		gyro_fs_sel = fs_sel;
		ESP_LOGE(TAG, "Invalid FS_SEL value: %d. Must be between 0 and 3.",
				fs_sel);
		return ESP_FAIL;
    }

    mpu9250_gyroscope_reset(dev_handle); // Reset gyroscope settings

    uint8_t gyro_config = (fs_sel << 3); // Set the gyroscope full-scale range based on FS_SEL

    if (mpu9250_write_register(dev_handle, MPU9250_GYRO_CONFIG, gyro_config) == ESP_OK) {
        ESP_LOGI(TAG, "Gyroscope configured with FS_SEL = %d (value: 0x%02X)", fs_sel, gyro_config);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to configure gyroscope");
        return ESP_FAIL;
    }
}

/**
 * @brief Resets the gyroscope configuration of the MPU9250 sensor.
 *
 * This function resets the gyroscope configuration by writing the default value
 * to the MPU9250_GYRO_CONFIG register using the provided I2C master device handle.
 *
 * @param[in] dev_handle The I2C master device handle used to communicate with the MPU9250 sensor.
 *
 * @return
 *      - ESP_OK: If the gyroscope configuration was successfully reset.
 *      - ESP_FAIL: If the gyroscope configuration reset operation failed.
 *
 * @note Ensure that the I2C master device handle is properly initialized and 
 *       the MPU9250 sensor is connected before calling this function.
 */
esp_err_t mpu9250_gyroscope_reset(i2c_master_dev_handle_t dev_handle) {
    uint8_t default_gyro_config = 0x00; // Default value for gyroscope configuration register
    if (mpu9250_write_register(dev_handle, MPU9250_GYRO_CONFIG, default_gyro_config) == ESP_OK) {
        ESP_LOGI(TAG, "Gyroscope configuration reset to default value: 0x%02X", default_gyro_config);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to reset gyroscope configuration");
        return ESP_FAIL;
    }
}

/**
 * @brief Reads gyroscope data from the MPU9250 sensor.
 *
 * This function reads the raw gyroscope data (X, Y, Z axes) from the MPU9250 sensor
 * using the provided I2C master device handle. The data is stored in the provided
 * output parameters.
 *
 * @param[in] dev_handle The I2C master device handle used to communicate with the MPU9250 sensor.
 * @param[out] gyro_x Pointer to store the raw gyroscope X-axis data.
 * @param[out] gyro_y Pointer to store the raw gyroscope Y-axis data.
 * @param[out] gyro_z Pointer to store the raw gyroscope Z-axis data.
 *
 * @return
 *      - ESP_OK: If the gyroscope data was successfully read.
 *      - ESP_FAIL: If the gyroscope data read operation failed.
 *
 * @note Ensure that the I2C master device handle is properly initialized and 
 *       the MPU9250 sensor is connected before calling this function.
 */
esp_err_t mpu9250_read_gyroscope(i2c_master_dev_handle_t dev_handle, int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z) {
    uint8_t raw_data[6];
    if (mpu9250_read_register(dev_handle, MPU9250_GYRO_XOUT_H, raw_data, sizeof(raw_data)) == ESP_OK) {
        *gyro_x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
        *gyro_y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
        *gyro_z = (int16_t)((raw_data[4] << 8) | raw_data[5]);

        float scale = gyro_full_scale_range[gyro_fs_sel] / 32768.0f;
		ESP_LOGI(TAG, "Gyroscope data (degrees/sec): X = %.2f, Y = %.2f, Z = %.2f",
				 *gyro_x * scale, *gyro_y * scale, *gyro_z * scale);
		return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to read gyroscope data");
        return ESP_FAIL;
    }
}