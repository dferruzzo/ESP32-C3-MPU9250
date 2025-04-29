#include "my_i2c.hpp"

I2C ::I2C(int sda_io, int scl_io) : sda_io(sda_io), scl_io(scl_io)
{
    bus_config.sda_io_num = static_cast<gpio_num_t>(sda_io);
    bus_config.scl_io_num = static_cast<gpio_num_t>(scl_io);
    dev_config.scl_speed_hz = freq_hz;
    ESP_LOGI("I2C", "I2C object created with SDA: %d, SCL: %d", sda_io, scl_io);
    init();
    }
I2C ::I2C(int sda_io, int scl_io, int freq_hz) : sda_io(sda_io), scl_io(scl_io), freq_hz(freq_hz) {
    bus_config.sda_io_num = static_cast<gpio_num_t>(sda_io);
    bus_config.scl_io_num = static_cast<gpio_num_t>(scl_io);
    dev_config.scl_speed_hz = freq_hz;
    ESP_LOGI("I2C", "I2C object created with SDA: %d, SCL: %d, Freq: %d", sda_io, scl_io, freq_hz);
    init();
    }
I2C ::~I2C() {
    ESP_LOGI("I2C", "I2C object destroyed");
}
esp_err_t I2C::init() {
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, bus_handle));
    is_init = true;
    //ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));
    ESP_LOGI("I2C", "I2C initialized successfully");
    return ESP_OK;
}
esp_err_t I2C::deinit() {
    if (is_dev_addr_set) {
        ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));
    }
    ESP_ERROR_CHECK(i2c_del_master_bus(bus_handle));
    ESP_LOGI("I2C", "I2C de-initialized successfully");
    return ESP_OK;
}
esp_err_t I2C::read(uint8_t reg_addr, uint8_t *data, size_t len){
    /*
    return i2c_master_transmit_receive(dev_handle, 
                                        &reg_addr, 
                                        1, 
                                        data, 
                                        len, 
                                        I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
                                        */
    if (i2c_master_transmit_receive(*dev_handle, 
                                        &reg_addr, 
                                        1, 
                                        data, 
                                        len, 
                                        I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) == ESP_OK) {
        ESP_LOGI("I2C", "Read %d bytes from register 0x%02X", len, reg_addr);
        ESP_LOGI("I2C", "Data: ");
        for (size_t i = 0; i < len; i++) {
            ESP_LOGI("I2C", "0x%02X ", data[i]);
        }
        ESP_LOGI("I2C", "Data read successfully");

} else {
        ESP_LOGE("I2C", "Failed to read from register 0x%02X", reg_addr);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t I2C::set_device(uint8_t _dev_addr) {
        dev_addr = _dev_addr;
        ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, dev_handle));
        ESP_LOGI("I2C", "I2C device on Addr: 0x%02X added successfully", dev_addr);
        is_dev_addr_set = true;
        return ESP_OK;
}
//esp_err_t I2C::write(uint8_t reg_addr, const uint8_t *data, size_t len) {}
