#ifndef __esp_now_callback_setup__
#define __esp_now_callback_setup__
#include "../all_includes.h"
#include "../pinout.h"

//controller connected status value + mutex
extern SemaphoreHandle_t controller_connected_mutex;
extern uint8_t controller_connected;

//motor status + mutex
extern SemaphoreHandle_t motor_status_mutex;
extern uint8_t motor_status;

//servo status + mutex
extern SemaphoreHandle_t servo_status_mutex;
extern uint8_t servo_status;

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

//need to update display + mutex
extern SemaphoreHandle_t update_main_display_mutex;
extern bool update_main_display;

//strings for the display + mutex
extern SemaphoreHandle_t main_menu_values_mutex;
extern std::string controller_connected_string; 
extern std::string battery_voltage_string;

//current motor speeds + mutex (0-255, centered at 127)
extern SemaphoreHandle_t motor_speeds;
extern uint8_t left_motor_speed;
extern uint8_t right_motor_speed;


void on_receive(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
void on_send(const uint8_t *mac_addr, esp_now_send_status_t status);

#endif