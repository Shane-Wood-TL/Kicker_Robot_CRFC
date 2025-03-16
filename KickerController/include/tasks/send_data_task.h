/**
 * @file esp_now_task.h
 * @brief Contains the externs and defines for the esp_now task
 * @author Shane Wood
 */

 
#ifndef __send_data_task__
#define __send_data_task__

#include "../all_includes.h"
#include "../pinout.h"



extern SemaphoreHandle_t motor_status_mutex;
extern uint8_t motor_status;

extern SemaphoreHandle_t ramped_velocity_mutex;
extern float velocity_ramp_limit;
extern float velocity_gain;
extern float velocity_integrator_gain;

extern SemaphoreHandle_t motor_speeds_mutex;
extern uint8_t left_motor_speed; // values set in settings
extern uint8_t right_motor_speed;

extern SemaphoreHandle_t servo_status_mutex;
extern uint8_t servo_status;

extern std::atomic<uint8_t> controller_byte_2;
extern std::atomic<uint8_t> controller_byte_4;

extern std::atomic<bool> ps4_controller_connected;


#define send_data_task_wait_time 10 ///< time to wait until another packet of data is to be sent to the kicker mainboard

/**
 * @brief This is the task that sends data to the kicker mainboard
 */
void send_data_task(void *pvParameter);
#endif