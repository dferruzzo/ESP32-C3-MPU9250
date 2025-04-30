#include "utils.h"

const char *axis[] = {
    [0] = "x",
    [1] = "y",
    [2] = "z"
};

void show_vector(float *data) {
    for (int i = 0; i < 3; i++) {
        //ESP_LOGI("utils", "Data[%d]: %.2f", i, data[i]);  // Fixed pointer arithmetic
        ESP_LOGI("utils", "axis[%d] = %s, value = %.2f", i, axis[i], data[i]);
    }
}