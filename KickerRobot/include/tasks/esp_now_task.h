#ifndef esp_now_task
#define esp_now_task
#include "../all_includes.h"
#include "../pinout.h"

//display battery value + mutex
extern SemaphoreHandle_t battery_voltage_mutex;
extern float battery_voltage;

#define send_data_task_wait_time 1000 ///< The amount of time to wait between sending data (in ms)

/**
 * @brief The task to transmit the battery voltage over esp-now
 */
void send_data_task(void *pvParameter);
#endif