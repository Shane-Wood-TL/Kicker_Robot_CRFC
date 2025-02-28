#include "../../include/tasks/esp_now_task.h"

//task that sends controller data
void send_data_task(void *pvParameter) {
    static esp_now_data_to_send current_transmission;
    while (1) {
        if (xSemaphoreTake(battery_voltage_mutex, pdMS_TO_TICKS(1000))){
            current_transmission.battery_voltage = battery_voltage;
            xSemaphoreGive(battery_voltage_mutex);
        }
        (void)esp_now_send(controller_mac_address, (uint8_t *)&current_transmission, sizeof(esp_now_data_to_send));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}