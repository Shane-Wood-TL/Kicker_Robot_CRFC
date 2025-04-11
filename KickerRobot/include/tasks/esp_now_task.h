/**
 * @file esp_now_task.h
 * @brief Contains the externs and defines for the esp_now task
 * @author Shane Wood
 */

#ifndef esp_now_task
#define esp_now_task
#include "../all_includes.h"
#include "../pinout.h"

//display battery value + mutex
extern SemaphoreHandle_t battery_voltage_mutex;
extern float battery_voltage;

extern SemaphoreHandle_t controller_connected_mutex;
extern uint8_t controller_connected;

extern SemaphoreHandle_t network_channel_mutex;
extern uint8_t current_network_channel;

#define send_data_task_wait_time 250 ///< The amount of time to wait between sending data (in ms)

/**
 * @brief The task to transmit the battery voltage over esp-now
 */
void send_data_task(void *pvParameter);
#endif