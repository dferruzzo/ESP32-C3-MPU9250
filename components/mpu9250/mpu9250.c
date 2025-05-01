#include "mpu9250.h"

static const char *TAG = "MPU9250";

/*********** */
/* Gyroscope */
/*********** */

uint16_t gyro_full_scale_range[] = {
    [0] = MPU9250_GYRO_FS_SEL_250_VALUE,
    [1] = MPU9250_GYRO_FS_SEL_500_VALUE,
    [2] = MPU9250_GYRO_FS_SEL_1000_VALUE,
    [3] = MPU9250_GYRO_FS_SEL_2000_VALUE
};

uint8_t gyro_fs_sel = 0x00; // Full-scale range selection
uint8_t *gyro_fs_sel_ptr = &gyro_fs_sel; // Full-scale range selection pointer

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
		ESP_LOGE(TAG, "Invalid FS_SEL value: %d. Must be between 0 and 3.",
				fs_sel);
		return ESP_FAIL;
    }

    mpu9250_gyroscope_reset(dev_handle); // Reset gyroscope settings

    *gyro_fs_sel_ptr = fs_sel; // Set the gyroscope full-scale range based on FS_SEL

    if (mpu9250_write_register(dev_handle, MPU9250_GYRO_CONFIG, (gyro_fs_sel << 3)) == ESP_OK) {
        ESP_LOGI(TAG, "Gyroscope configured with FS_SEL = %d (value: 0x%02X)", fs_sel, (gyro_fs_sel << 3));
                return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to configure gyroscope");
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
esp_err_t mpu9250_read_gyroscope(i2c_master_dev_handle_t dev_handle, float *gyro_data) {
    uint8_t raw_data[6];
    if (mpu9250_read_register(dev_handle, MPU9250_GYRO_XOUT_H, raw_data, sizeof(raw_data)) == ESP_OK) {

        float scale = gyro_full_scale_range[gyro_fs_sel] / 32768.0f;
        //ESP_LOGI(TAG, "Gyroscope scale: %.2f", scale);

        gyro_data[0] = (float)(((int16_t)((raw_data[0] << 8) | raw_data[1])) * scale);
        gyro_data[1] = (float)(((int16_t)((raw_data[2] << 8) | raw_data[3])) * scale);
        gyro_data[2] = (float)(((int16_t)((raw_data[4] << 8) | raw_data[5])) * scale);
        
		//ESP_LOGI(TAG, "Gyroscope data (degrees/sec): X = %.2f, Y = %.2f, Z = %.2f", gyro_data[0], gyro_data[1], gyro_data[2]);
        
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to read gyroscope data");
        return ESP_FAIL;
    }
}

/*************** */
/* Accelerometer */
/*************** */

uint16_t acc_full_scale_range[] = {
    [0] = MPU9250_ACCEL_FS_SEL_2_VALUE,
    [1] = MPU9250_ACCEL_FS_SEL_4_VALUE,
    [2] = MPU9250_ACCEL_FS_SEL_8_VALUE,
    [3] = MPU9250_ACCEL_FS_SEL_16_VALUE
};

uint8_t acc_fs_sel = 0x00; // Full-scale range selection
uint8_t *acc_fs_sel_ptr = &acc_fs_sel; // Full-scale range selection pointer

esp_err_t mpu9250_read_accelerometer(i2c_master_dev_handle_t dev_handle, float *accel_data){
    uint8_t raw_data[6];
    if (mpu9250_read_register(dev_handle, MPU9250_ACCEL_XOUT_H, raw_data, sizeof(raw_data)) == ESP_OK) {
        //float scale = (float)(acc_full_scale_range[acc_fs_sel] / 32768.0f);

        float scale = (float)((uint16_t)(acc_full_scale_range[acc_fs_sel]) / (float)(INT16_MAX));
        //ESP_LOGI(TAG, "Accelerometer full scale range: %d", acc_full_scale_range[acc_fs_sel]);
        //ESP_LOGI(TAG, "Accelerometer scale: %.6f", scale);

        accel_data[0] = (float)(((int16_t)((raw_data[0] << 8) | raw_data[1])) * scale);
        accel_data[1] = (float)(((int16_t)((raw_data[2] << 8) | raw_data[3])) * scale);
        accel_data[2] = (float)(((int16_t)((raw_data[4] << 8) | raw_data[5])) * scale );
        
        //ESP_LOGI(TAG, "Accelerometer data (g): X = %.2f, Y = %.2f, Z = %.2f", accel_data[0], accel_data[1], accel_data[2]);
        
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to read accelerometer data");
        return ESP_FAIL;
    }
};
esp_err_t mpu9250_configure_accelerometer(i2c_master_dev_handle_t dev_handle, uint8_t acc_fs_sel){
    if (acc_fs_sel > 3) {
        ESP_LOGE(TAG, "Invalid FS_SEL value: %d. Must be between 0 and 3.",
                acc_fs_sel);
        return ESP_FAIL;
    }

    mpu9250_accelerometer_reset(dev_handle); // Reset gyroscope settings

    *acc_fs_sel_ptr = acc_fs_sel; // Set the accelerometer full-scale range based on FS_SEL

    if (mpu9250_write_register(dev_handle, MPU9250_ACCEL_CONFIG, (acc_fs_sel << 3)) == ESP_OK) {
        ESP_LOGI(TAG, "Accelerometer configured with FS_SEL = %d (value: 0x%02X)", acc_fs_sel, (acc_fs_sel << 3));
                return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to configure accelerometer");
        return ESP_FAIL;
    }
};
esp_err_t mpu9250_accelerometer_reset(i2c_master_dev_handle_t dev_handle){
    uint8_t default_accel_config = 0x00; // Default value for accelerometer configuration register
    if (mpu9250_write_register(dev_handle, MPU9250_ACCEL_CONFIG, default_accel_config) == ESP_OK) {
        ESP_LOGI(TAG, "Accelerometer configuration reset to default value: 0x%02X", default_accel_config);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to reset accelerometer configuration");
        return ESP_FAIL;
    }
};

/***************/
/* Temperature */
/***************/

esp_err_t mpu9250_read_temperature(i2c_master_dev_handle_t dev_handle, float *temp_data){
    uint8_t raw_data[2];
    if (mpu9250_read_register(dev_handle, MPU9250_TEMP_OUT_H, raw_data, sizeof(raw_data)) == ESP_OK) {
        int16_t temp_raw = (int16_t)((raw_data[0] << 8) | raw_data[1]);
        *temp_data = (float)(temp_raw / 333.87f + 21.00f);
        //*temp_data = (float)(temp_raw);
        //ESP_LOGI(TAG, "Temperature data: %.2f °C", *temp_data);

        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to read temperature data");
        return ESP_FAIL;
    }
};

/****************/
/* Magnetometer */
/****************/
/*
esp_err_t mpu9250_read_magnetometer(i2c_master_dev_handle_t dev_handle, float *mag_data){
    uint8_t raw_data[6];
    if (mpu9250_read_register(dev_handle, MPU9250_MAG_XOUT_L, raw_data, sizeof(raw_data)) == ESP_OK) {
        mag_data[0] = (float)(((int16_t)((raw_data[0] << 8) | raw_data[1])) * 0.15f);
        mag_data[1] = (float)(((int16_t)((raw_data[2] << 8) | raw_data[3])) * 0.15f);
        mag_data[2] = (float)(((int16_t)((raw_data[4] << 8) | raw_data[5])) * 0.15f);
        
        ESP_LOGI(TAG, "Magnetometer data: X = %.2f, Y = %.2f, Z = %.2f", mag_data[0], mag_data[1], mag_data[2]);
        
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to read magnetometer data");
        return ESP_FAIL;
    }   
};
*/  