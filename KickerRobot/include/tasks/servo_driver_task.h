#ifndef __servo_driver_task__
#define __servo_driver_task__

#include "../all_includes.h"
#include "../pinout.h"
#include "../drivers/servo.h"

#define servo_driver_wait_time 100 ///< The amount of time to wait between servo updates (in ms)

//servo status + mutex
extern SemaphoreHandle_t servo_status_mutex;
extern uint8_t servo_status;


/**
 * @brief The task to drive the servo motor
 */
void servo_driver_task(void *pv);
#endif