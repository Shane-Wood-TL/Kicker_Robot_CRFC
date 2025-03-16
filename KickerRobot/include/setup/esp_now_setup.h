/**
 * @file esp_now_setup.h
 * @brief Contains the code that sets up the ESP-NOW communication
 * @author Shane Wood
 */
#ifndef __esp_now_setup__
#define __esp_now_setup__
#include "../all_includes.h"
#include "../pinout.h"
#include "esp_now_callback_setup.h"

/**
 * @brief Sets up the ESP-NOW communication, including setting up the callback functions
 */
void esp_now_setup();
#endif