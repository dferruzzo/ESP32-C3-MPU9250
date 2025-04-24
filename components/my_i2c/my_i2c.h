#ifndef MY_I2C_H
#define MY_I2C_H

#include "driver/i2c_master.h"
#include "esp_err.h"

#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL       /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_0                   /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          CONFIG_I2C_MASTER_FREQUENCY /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t my_i2c_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle, int sda_io, int scl_io, int freq_hz, uint8_t dev_addr);
esp_err_t my_i2c_deinit(i2c_master_bus_handle_t bus_handle, i2c_master_dev_handle_t dev_handle);

#ifdef __cplusplus
}
#endif

#endif // MY_I2C_H