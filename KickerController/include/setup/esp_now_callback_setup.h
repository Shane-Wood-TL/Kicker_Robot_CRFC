/**
 * @file esp_now_callback_setup.h
 * @brief Contains the callback functions for esp-now
 * @author Shane Wood
 * @date 10/2/2025
 */

#ifndef __esp_now_callback_setup__
#define __esp_now_callback_setup__

#include "../all_includes.h"
#include "../pinout.h"

extern SemaphoreHandle_t update_main_display_mutex;
extern bool update_main_display;

extern SemaphoreHandle_t main_menu_values_mutex;
extern std::string contoller_connected;
extern std::string robot_connected;
extern std::string battery_voltage_string;
extern std::string status;

/**
 * @brief Callback function for when a message is received
 * 
 * @param recv_info : const esp_now_recv_info_t* Information about the received message
 * @param data : const uint8_t* The data received
 * @param len : int The length of the data received
 */
void on_receive(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);

/**
 * @brief Callback function for when a message is sent
 * 
 * @param mac_addr : const uint8_t* The mac address of the device the message was sent to
 * @param status : esp_now_send_status_t The status of the message send
 */
void on_send(const uint8_t *mac_addr, esp_now_send_status_t status);

#endif