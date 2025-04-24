/* Escrever aqui a descrição do aquivo

*/
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

class BlinkApp {
private:
    static constexpr const char* TAG = "esp32-c3-mpu9250";
    static constexpr gpio_num_t BLINK_GPIO = static_cast<gpio_num_t>(CONFIG_BLINK_GPIO);
    
    uint8_t ledState = 0;

    void blinkLed() {
        gpio_set_level(BLINK_GPIO, ledState);
    }

    void configureLed() {
        ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
        gpio_reset_pin(BLINK_GPIO);
        gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    }

public:
    void run() {
        configureLed();

        while (true) {
            ESP_LOGI(TAG, "Turning the LED %s!", ledState ? "ON" : "OFF");
            blinkLed();
            ledState = !ledState;
            vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        }
    }
};

extern "C" void app_main() {
    BlinkApp app;
    app.run();
    
    // 1000 ms delay    
    vTaskDelay(portMAX_DELAY);
}
