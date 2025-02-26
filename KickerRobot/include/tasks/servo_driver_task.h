#ifndef __servo_driver_task__
#define __servo_driver_task__

#include "../all_includes.h"
#include "../pinout.h"
#include "../drivers/servo.h"

//servo status + mutex
extern SemaphoreHandle_t servo_status_mutex;
extern uint8_t servo_status;


void servoDriver(void *pv);
#endif