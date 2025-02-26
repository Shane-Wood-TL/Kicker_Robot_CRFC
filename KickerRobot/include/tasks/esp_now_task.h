#ifndef esp_now_task
#define esp_now_task
#include "../all_includes.h"
#include "../pinout.h"

//display battery value + mutex
extern SemaphoreHandle_t battery_voltage_mutex;
extern float battery_voltage;

void send_data_task(void *pvParameter);
#endif