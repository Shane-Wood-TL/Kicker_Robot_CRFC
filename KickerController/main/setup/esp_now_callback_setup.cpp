/**
 * @file esp_now_callback_setup.cpp
 * @brief source file that sets up esp-now callbacks
 * @author Shane Wood
 * @date 1/15/2025
 */
#include "../../include/setup/esp_now_callback_setup.h"



// call back function for when data is sent
void on_send(const uint8_t *mac_addr, esp_now_send_status_t status) {
  static bool last_transmit_state = false;
  if (status == ESP_NOW_SEND_SUCCESS) {
    if (last_transmit_state == false) { //was not connected, now connected
      last_transmit_state = true;
      if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)) {
        robot_connected = connected_string;
        xSemaphoreGive(main_menu_values_mutex);
        if (xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)) {
          update_main_display = true;
          xSemaphoreGive(update_main_display_mutex);
        }
      }
    }
  } else {
    if (last_transmit_state == true) { //was connected, now not connected
      last_transmit_state = false;
      if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)) {
        robot_connected = disconnected_string;
        xSemaphoreGive(main_menu_values_mutex);
        battery_voltage_string = no_battery_voltage_string;
        if (xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)) {
          update_main_display = true;
          xSemaphoreGive(update_main_display_mutex);
        }
      }
    }
  }
}



// function that is called, when data is received
void on_receive(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len) {
  static float old_battery_reading = minimum_battery_reading;
  esp_now_data_to_receive received_data;
  memcpy(&received_data, data, sizeof(esp_now_data_to_receive));
  // update the battery voltage
  if ((received_data.battery_voltage >= minimum_battery_reading) && (received_data.battery_voltage < maximum_battery_reading)) {
    char temp_buffer[bytes_in_float] = "";
    std::string value_as_string = "";
    sprintf(temp_buffer, one_decimal_place, received_data.battery_voltage);
    value_as_string = temp_buffer;
    if (old_battery_reading != received_data.battery_voltage) {
      if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)) {
        battery_voltage_string = value_as_string;
        xSemaphoreGive(main_menu_values_mutex);
        if (xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)) {
          update_main_display = true;
          xSemaphoreGive(update_main_display_mutex);
        }
      }
    }
    old_battery_reading = received_data.battery_voltage;
  }
}