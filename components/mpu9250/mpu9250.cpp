#include "mpu9250.hpp"

MPU9250::MPU9250(I2C &i2c, uint8_t dev_addr) : i2c(i2c), dev_addr(dev_addr) {
    if (!i2c.is_init) {
        ESP_LOGE("MPU9250", "I2C bus not initialized. Please initialize the I2C bus before using the MPU9250.");
        return;
    }
	init();
}
MPU9250::~MPU9250() {
    ESP_LOGI("MPU9250", "MPU9250 object destroyed");
}
esp_err_t MPU9250::init() {
    if (is_mpu9250_init) {
        ESP_LOGW("MPU9250", "MPU9250 is already initialized.");
        return ESP_OK;
    }

    ESP_LOGI("MPU9250", "Initializing MPU9250...");
    if (this->i2c.set_device(dev_addr) == ESP_OK) {
        is_mpu9250_init = true; // Set the MPU9250 as initialized
        ESP_LOGI("MPU9250", "MPU9250 initialized successfully.");
        return ESP_OK;
    } else {
        ESP_LOGE("MPU9250", "Failed to set device address for MPU9250.");
        return ESP_FAIL;
    }
}
esp_err_t MPU9250::deinit() {
    ESP_LOGI("MPU9250", "MPU9250 de-initialized successfully");
    return ESP_OK;
}
void MPU9250::log_who_am_i() {
    if (i2c.read(MPU9250_WHO_AM_I_REG_ADDR, &who_am_i, 1) == ESP_OK) {
        ESP_LOGI("MPU9250", "WHO_AM_I register: 0x%02X", who_am_i);
    } else {
        ESP_LOGE("MPU9250", "Failed to read WHO_AM_I register");
    }
}

