
#ifndef __display_task__
#define __display_task__

#include "../all_includes.h"
#include "../drivers/ssd1306.h"
#include "../supporting/changeable_values.h"
#include "../supporting/menus.h"
#include "../supporting/menu_handler.h"
#include "../pinout.h"

// i2c bus config
extern i2c_master_bus_config_t i2c_mst_config;
extern i2c_master_bus_handle_t bus_handle;

#define maximum_left_motor_speed_modifier 15
#define maximum_right_motor_speed_modifier 15

#define minimum_left_motor_speed_modifier 0
#define minimum_right_motor_speed_modifier 0

#define left_motor_speed_modifier_step_size 1
#define right_motor_speed_modifier_step_size 1

#define boot_screen_wait_time 1000


#define minimum_velocity_ramp_limit 0.0f
#define maximum_velocity_ramp_limit 50.0f
#define velocity_ramp_limit_step_size 0.5f

#define minimum_velocity_gain 0.0f
#define maximum_velocity_gain 0.7f
#define velocity_gain_step_size 0.01f

#define minimum_velocity_integrator_gain 0.0f
#define maximum_velocity_integrator_gain 1.0f
#define velocity_integrator_gain_step_size 0.01f

#define display_task_wait_time 10

/**
 * @brief This is the task that handles driving the display with the menus and values
 */
void display_task(void *pv); 
#endif