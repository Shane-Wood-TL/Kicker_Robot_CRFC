/**
 * @file display_task.h
 * @brief This is the file that contains the display task
 * @author Shane Wood
 */
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

#define maximum_left_motor_speed_modifier 15  ///< max multipler for the left motor
#define maximum_right_motor_speed_modifier 15 ///< max multipler for the right motor

#define minimum_left_motor_speed_modifier 0 ///< min multipler for the left motor
#define minimum_right_motor_speed_modifier 0 ///< min multipler for the right motor

#define left_motor_speed_modifier_step_size 1 ///< step size for the left motor multipler
#define right_motor_speed_modifier_step_size 1 ///< step size for the right motor multipler

#define boot_screen_wait_time 1000 ///< time to wait for the boot screen to be displayed


#define minimum_velocity_ramp_limit 0.0f ///< min value for the velocity ramp limit
#define maximum_velocity_ramp_limit 50.0f ///< max value for the velocity ramp limit
#define velocity_ramp_limit_step_size 0.5f ///< step size for the velocity ramp limit

#define minimum_velocity_gain 0.0f ///< min value for the velocity gain
#define maximum_velocity_gain 0.7f ///< max value for the velocity gain
#define velocity_gain_step_size 0.01f ///< step size for the velocity gain

#define minimum_velocity_integrator_gain 0.0f ///< min value for the velocity integrator gain
#define maximum_velocity_integrator_gain 1.0f ///< max value for the velocity integrator gain
#define velocity_integrator_gain_step_size 0.01f ///< step size for the velocity integrator gain

#define display_task_wait_time 25 ///< time to wait in the display task

/**
 * @brief This is the task that handles driving the display with the menus and values
 */
void display_task(void *pv); 
#endif