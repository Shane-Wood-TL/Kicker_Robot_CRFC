#ifndef __send_data_task__
#define __send_data_task__

#include "../all_includes.h"
#include "../pinout.h"

extern SemaphoreHandle_t main_menu_values_mutex;
extern std::string contoller_connected;


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

#define send_data_task_wait_time 10

extern std::atomic<uint8_t> controller_byte_2;
extern std::atomic<uint8_t> controller_byte_4;

/**
 * @brief This is the task that sends data to the kicker mainboard
 */
void send_data_task(void *pvParameter);
#endif