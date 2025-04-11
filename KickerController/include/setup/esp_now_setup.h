/**
 * @file esp_now_setup.h
 * @brief Contains the code that sets up the ESP-NOW communication
 * @author Shane Wood
 * @date 10/2/2025
 */

#ifndef __esp_now_setup__
#define __esp_now_setup__
#include "../all_includes.h"
#include "../pinout.h"
#include "esp_now_callback_setup.h"


extern SemaphoreHandle_t network_channel_mutex;
extern uint8_t current_network_channel;


/**
 * @brief Sets up the ESP-NOW communication, including setting up the callback functions
 */
void esp_now_setup();
#endif