#ifndef __display_driver_task__
#define __display_driver_task__
#include "../all_includes.h"
#include "../pinout.h"
#include "../drivers/ssd1306.h"
#include "../supporting/menus.h"


extern i2c_master_bus_handle_t bus_handle;

//strings for the display + mutex
extern SemaphoreHandle_t main_menu_values_mutex;
extern std::string controller_connected_string;
extern SemaphoreHandle_t battery_voltage_mutex;
extern std::string battery_voltage_string;
extern float battery_voltage;

#define float_display_length 4 ///< the number of points in a float XX.XX
#define minimum_battery_voltage 0 ///< the minimum battery voltage to display
#define maximum_battery_voltage 30 ///< the maximum battery voltage to display
#define display_driver_wait_time 1000 ///< The amount of time to wait between display updates (in ms)

/**
 * @brief The display driver task
 * @details This task is responsible for updating the display with the current values, in this case 
 */
void display_driver_task(void *pv);
#endif