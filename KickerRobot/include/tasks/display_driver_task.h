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
extern std::string battery_voltage_string;

void displayDriver(void *pv);
#endif