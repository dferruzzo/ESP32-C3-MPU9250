#ifndef MY_I2C_H
#define MY_I2C_H

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/i2c_master.h" 


#define I2C_MASTER_SDA_IO       GPIO_NUM_6
#define I2C_MASTER_SCL_IO       GPIO_NUM_7

#define I2C_MASTER_TIMEOUT_MS   1000 // Adicionado para definir o tempo limite
#define I2C_MASTER_FREQ_HZ      CONFIG_I2C_MASTER_FREQUENCY /*!< I2C master clock frequency */

class I2C 
{
    
    private:
    int sda_io;
    int scl_io;
    int freq_hz = I2C_MASTER_TIMEOUT_MS;
	uint8_t dev_addr = 0x00;
    bool is_dev_addr_set = false;
	
        
    public:
    i2c_master_bus_handle_t bus_handle;
	i2c_master_dev_handle_t dev_handle;    
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = static_cast<gpio_num_t>(sda_io),
        .scl_io_num = static_cast<gpio_num_t>(scl_io),
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 10,
        .flags={
            .enable_internal_pullup = true,
            .allow_pd = false,
        },
    };
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = static_cast<uint32_t>(freq_hz),
        .scl_wait_us = 0,
        .flags = {
            .disable_ack_check = false,
        },
    };
    I2C(int sda_io, int scl_io);
    I2C(int sda_io, int scl_io, int freq_hz);
    ~I2C();
    
    bool is_init = false;

    esp_err_t init();
    esp_err_t deinit();
	esp_err_t set_device(uint8_t dev_addr);
	esp_err_t read(uint8_t reg_addr, uint8_t *data, size_t len);
	esp_err_t write(uint8_t reg_addr, const uint8_t *data, size_t len);
};

#endif
