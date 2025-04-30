#include "utils.h"

void show_vector(float *data) {
    for (int i = 0; i < 3; i++) {
        ESP_LOGI("utils", "Data[%d]: %.2f", i, data[i]);  // Fixed pointer arithmetic
    }
}