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

uint8_t gyro_fs_sel = 0x00;                             // Full-scale range selection
uint8_t *gyro_fs_sel_ptr = &gyro_fs_sel;                // Full-scale range selection pointer

float gyro_bias[3] = {0.0f,0.0f, 0.0f};  // Allocate as single contiguous block

bool gyro_cal_flag = false;    // Gyroscope calibration flag

bool gyro_calibration_in_progress = false;

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

    if (mpu9250_write_register(dev_handle, MPU9250_GYRO_CONFIG, 0x00) == ESP_OK) {
        ESP_LOGI(TAG, "Gyroscope configuration reset to default value: 0x%02X", 0x00);
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
esp_err_t mpu9250_configure_gyroscope(i2c_master_dev_handle_t dev_handle, uint8_t fs_sel, uint8_t enable_filter, uint8_t gyro_dlpf_cfg) {

    if (fs_sel > 3) {
		ESP_LOGE(TAG, "Invalid FS_SEL value: %d. Must be between 0 and 3.", fs_sel);
		return ESP_FAIL;
    }

    if (enable_filter > 2) {
		ESP_LOGE(TAG, "Invalid ENABLE FILTER value: %d. Must be between 0 and 2.", enable_filter);
		return ESP_FAIL;
    }
    // Reset the gyroscope settings

    mpu9250_gyroscope_reset(dev_handle); // Reset gyroscope settings

    *gyro_fs_sel_ptr = fs_sel; // Set the gyroscope full-scale range based on FS_SEL

    // Set the gyroscope configuration, combine the full-scale range selection
    // with the FCHOICE_B bits (0x00 for DLPF enabled)

	int8_t gyro_conf =
		(gyro_fs_sel << 3) | enable_filter;

	esp_err_t ret1, ret2;

	ret1 = mpu9250_write_register(dev_handle, MPU9250_GYRO_CONFIG, gyro_conf);  // configura escala do giroscópio
    ret2 = mpu9250_write_register(dev_handle, MPU9250_CONFIG, gyro_dlpf_cfg);   // configura DLPF

    if (ret1 == ESP_OK && ret2 == ESP_OK) {
        ESP_LOGI(TAG, "Gyroscope configured with config = 0x%02X", gyro_conf);
        ESP_LOGI(TAG, "DLPF configured with config = 0x%02X", gyro_dlpf_cfg);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to configure gyroscope or DLPF");
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

    // Suppress spell-check warnings for specific words
    if (!gyro_cal_flag && !gyro_calibration_in_progress) {
        ESP_LOGW(TAG, "Gyroscope calibration not completed. Please calibrate the gyroscope first."); // "Gyroscope", "calibration", and "calibrate" are valid terms
        //return ESP_FAIL;
    }
    
    uint8_t raw_data[6];
    if (mpu9250_read_register(dev_handle, MPU9250_GYRO_XOUT_H, raw_data, sizeof(raw_data)) == ESP_OK) {

        float scale = gyro_full_scale_range[gyro_fs_sel] / 32768.0f;
        //ESP_LOGI(TAG, "Gyroscope scale: %.2f", scale);

        if (gyro_calibration_in_progress){

            gyro_data[0] = (float)(((int16_t)((raw_data[0] << 8) | raw_data[1])) * scale);
            gyro_data[1] = (float)(((int16_t)((raw_data[2] << 8) | raw_data[3])) * scale);
            gyro_data[2] = (float)(((int16_t)((raw_data[4] << 8) | raw_data[5])) * scale);        

        } else {

            gyro_data[0] = -gyro_bias[0] + (float)(((int16_t)((raw_data[0] << 8) | raw_data[1])) * scale);
            gyro_data[1] = -gyro_bias[1] + (float)(((int16_t)((raw_data[2] << 8) | raw_data[3])) * scale);
            gyro_data[2] = -gyro_bias[2] + (float)(((int16_t)((raw_data[4] << 8) | raw_data[5])) * scale);
        
        }
        
		//ESP_LOGI(TAG, "Gyroscope data (degrees/sec): X = %.2f, Y = %.2f, Z = %.2f", gyro_data[0], gyro_data[1], gyro_data[2]);
        
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to read gyroscope data");
        return ESP_FAIL;
    }
}

esp_err_t mpu9250_calibrate_gyroscope(i2c_master_dev_handle_t dev_handle) {

    gyro_calibration_in_progress = true;

    int num_samples = 1000;
    float gyro_x_sum = 0, gyro_y_sum = 0, gyro_z_sum = 0;
    float gyro_data[3];

    ESP_LOGI(TAG, "Starting gyroscope calibration...");

    for (int i = 0; i < num_samples; i++) {
        //ESP_LOGI("CALIBRATION","Reading gyroscope data...");
        if (mpu9250_read_gyroscope(dev_handle, gyro_data) == ESP_OK) {
        gyro_x_sum += gyro_data[0];
        gyro_y_sum += gyro_data[1];
        gyro_z_sum += gyro_data[2];
        }
        vTaskDelay(pdMS_TO_TICKS(2));  // Aguarde 2ms entre as leituras
    }

    gyro_bias[0] = gyro_x_sum / num_samples;
    gyro_bias[1] = gyro_y_sum / num_samples;
    gyro_bias[2] = gyro_z_sum / num_samples;

    ESP_LOGI(TAG, "Gyroscope calibration complete:");
    //ESP_LOGI(TAG, "Bias X: %.2f, Y: %.2f, Z: %.2f", *gyro_bias_ptr, *gyro_bias_ptr + 1, *gyro_bias_ptr + 2);
    ESP_LOGI(TAG, "Bias X: %.2f, Y: %.2f, Z: %.2f", gyro_bias[0], gyro_bias[1], gyro_bias[2]);

    gyro_cal_flag = true;

    gyro_calibration_in_progress = false;

    return ESP_OK;
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

esp_err_t mpu9250_configure_accelerometer(i2c_master_dev_handle_t dev_handle, uint8_t acc_fs_sel, uint8_t acc_dlpf_cfg) {
    if (acc_fs_sel > 3) {
        ESP_LOGE(TAG, "Invalid FS_SEL value: %d. Must be between 0 and 3.",
                acc_fs_sel);
        return ESP_FAIL;
    }

    if (acc_dlpf_cfg > 15) {
        ESP_LOGE(TAG, "Invalid DLPF_CFG value: %d. Must be between 0 and 15.",
                acc_dlpf_cfg);
        return ESP_FAIL;
    }

    mpu9250_accelerometer_reset(dev_handle); // Reset gyroscope settings

    *acc_fs_sel_ptr = acc_fs_sel; // Set the accelerometer full-scale range based on FS_SEL

    esp_err_t ret1, ret2;

	ret1 = mpu9250_write_register(dev_handle, MPU9250_ACCEL_CONFIG,
								(acc_fs_sel << 3));
    ret2 = mpu9250_write_register(dev_handle, MPU9250_ACCEL_CONFIG_2,
                                acc_dlpf_cfg); // configura DLPF

	if ((ret1!= ESP_OK) && (ret2 != ESP_OK)) {
        ESP_LOGE(TAG, "Failed to configure accelerometer or DLPF");
        return ESP_FAIL;
    } else if (ret1 == ESP_OK && ret2 == ESP_OK) {
                ESP_LOGI(TAG, "Accelerometer configured with config = 0x%02X", (acc_fs_sel << 3));
                ESP_LOGI(TAG, "Accelerometer DLPF configured with", acc_dlpf_cfg);
                return ESP_OK;
	} else {
        ESP_LOGE(TAG, "Failed to configure accelerometer or DLPF");
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

esp_err_t mpu9250_read_magnetometer(i2c_master_dev_handle_t dev_handle_mpu9250, i2c_master_dev_handle_t dev_handle_magnetometer, float *mag_data) {

    my_i2c_write(dev_handle_mpu9250, MPU9250_INT_PIN_CFG, 0x02);    // Write to set PASS_THROUGH mode - Para ativar o Magnetómetro
    my_i2c_write(dev_handle_magnetometer, MPU9250_MAG_CNTL, 0x01);  // Set magnetometer to single measurement mode

    uint8_t data_ready = 0x00; // Buffer to store data ready status
    do{
        my_i2c_read(dev_handle_magnetometer, MPU9250_MAG_DATA_RDY, &data_ready, 1); // Read data ready status
        } while((data_ready & 0x01) == 0); // Wait for data ready bit to be set
    
    // Read magnetometer data
    uint8_t *mag_data_int = (uint8_t *)malloc(6 * sizeof(uint8_t));  // Allocate as single contiguous block
    if (mag_data_int == NULL) {
        ESP_LOGE("app_main", "Failed to allocate memory for magnetometer data");
        return ESP_FAIL;
    }
    if(my_i2c_read(dev_handle_magnetometer, MPU9250_MAG_HXL, (uint8_t *)mag_data_int, 6) != ESP_OK){
        ESP_LOGE(TAG, "Failed to read magnetometer data");
        free(mag_data_int); // Free allocated memory
        return ESP_FAIL;
    }
    mag_data[0] = (float)(((int16_t)((mag_data_int[1] << 8) | mag_data_int[0])) * 0.1465f);
    mag_data[1] = (float)(((int16_t)((mag_data_int[3] << 8) | mag_data_int[2])) * 0.1465f);
    mag_data[2] = (float)(((int16_t)((mag_data_int[5] << 8) | mag_data_int[4])) * 0.1465f);

    my_i2c_write(dev_handle_mpu9250, MPU9250_INT_PIN_CFG, 0x00);    // Turn-off PASS_THROUGH mode - Para ativar o Magnetómetro
    free(mag_data_int); // Free allocated memory

    return ESP_OK;
};
