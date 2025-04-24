#include "my_i2c.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "MY_I2C";

esp_err_t my_i2c_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle, int sda_io, int scl_io, int freq_hz, uint8_t dev_addr) {
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = sda_io,
        .scl_io_num = scl_io,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, bus_handle));

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = freq_hz,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_config, dev_handle));

    ESP_LOGI(TAG, "I2C initialized successfully");
    return ESP_OK;
}

esp_err_t my_i2c_deinit(i2c_master_bus_handle_t bus_handle, i2c_master_dev_handle_t dev_handle) {
    ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));
    ESP_ERROR_CHECK(i2c_del_master_bus(bus_handle));
    ESP_LOGI(TAG, "I2C de-initialized successfully");
    return ESP_OK;
}