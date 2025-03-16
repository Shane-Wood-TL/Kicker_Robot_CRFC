/**
 * @file kicker_drive_train_task.h
 * @brief Contains the externs and defines for the kicker drive train task
 * @author Shane Wood
 */
#ifndef __odrive_driver_task__
#define __odrive_driver_task__
#include "../all_includes.h"
#include "../pinout.h"
#include "../drivers/oDrive.h"
#include "../supporting/kicker_drive_train.h"

//controller connected status value + mutex
extern SemaphoreHandle_t controller_connected_mutex;
extern uint8_t controller_connected;

//motor status + mutex
extern SemaphoreHandle_t motor_status_mutex;
extern uint8_t motor_status;

//ramped values + values changed + mutexes
extern SemaphoreHandle_t ramped_values_updated;
extern bool ramped_values_updated_bool;
extern SemaphoreHandle_t ramped_mutex;
extern float velocity_ramp_limit;
extern float velocity_gain;
extern float velocity_integrator_gain;

//motor speed settings + mutex
extern SemaphoreHandle_t motor_speeds_settings_mutex;
extern uint8_t left_motor_speed_mult; //values set in settings
extern uint8_t right_motor_speed_mult;

//current motor speeds + mutex (0-255, centered at 127)
extern SemaphoreHandle_t motor_speeds;
extern uint8_t left_motor_speed;
extern uint8_t right_motor_speed;

#define odrive_driver_wait_time 10 ///< The amount of time to wait between odrive updates (in ms)

/**
 * @brief The task to drive the odrive motors, set speeds, modify ramped velocity parameters, and update the battery voltage
 */
void kicker_drive_train_task(void *pv);
#endif