#include "my_i2c.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "MY_I2C";

esp_err_t my_i2c_init(i2c_master_bus_handle_t *bus_handle, int sda_io, int scl_io) {
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = sda_io,
        .scl_io_num = scl_io,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, bus_handle));

    ESP_LOGI(TAG, "I2C bus initialized successfully");

	return ESP_OK;
}

esp_err_t my_i2c_add_dev(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle, int freq_hz, uint8_t dev_addr) {

    ESP_LOGI(TAG, "Adding I2C device with address 0x%02X", dev_addr);

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = freq_hz,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_config, dev_handle));

    ESP_LOGI(TAG, "Device added successfully");
    return ESP_OK;
}

esp_err_t my_i2c_deinit(i2c_master_bus_handle_t bus_handle) {
    //ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));
    ESP_ERROR_CHECK(i2c_del_master_bus(bus_handle));
    ESP_LOGI(TAG, "I2C de-initialized successfully");
    return ESP_OK;
}

esp_err_t my_i2c_scan(i2c_master_bus_handle_t bus_handle) {
    ESP_LOGI(TAG, "Scanning I2C bus...");
    esp_err_t ret;
    int devices_found = 0;

    for (uint8_t addr = 1; addr < 127; addr++) { // Endereços válidos no barramento I2C
        ret = i2c_master_probe(bus_handle, addr, I2C_MASTER_TIMEOUT_MS);
        if (ret == ESP_ERR_INVALID_ARG) {
            ESP_LOGW(TAG, "Invalid address: 0x%02X", addr);
            continue;
        } else if (ret == ESP_ERR_TIMEOUT) {
            // Timeout, no device found at this address
            continue;
        }
        // If we reach here, the device is found
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Device found at address 0x%02X", addr);
            devices_found++;
		}// else if (ret != ESP_ERR_TIMEOUT) {
		//	ESP_LOGW("I2C_SCAN", "Error at address 0x%02X: %s", addr, esp_err_to_name(ret));
		//}
	}

    if (devices_found == 0) {
        ESP_LOGI("I2C_SCAN", "No I2C devices found");
    } else {
        ESP_LOGI("I2C_SCAN", "Scan complete, %d device(s) found", devices_found);
    }

    return ESP_OK;
}

esp_err_t my_i2c_read(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len){
    esp_err_t ret;
    ret = i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret != ESP_OK) {
        //ESP_LOGI(TAG, "Read %d bytes from register 0x%02X", len, reg_addr);
        ESP_LOGE(TAG, "Failed to read from register 0x%02X: %s", reg_addr, esp_err_to_name(ret));
    } //else {
        
    //}
    return ret;
}

esp_err_t my_i2c_write(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data){
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}


