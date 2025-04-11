/**
 * @file main.cpp
 * @brief This is the file that contains the main function and all other tasks
 * for the robot
 * @author Shane Wood
 * @details This is the code for the kicker robot, it was part of the 2024-2025
 * senior design project.
 *
 * @date 22/12/2024
 * @note Dates are in the format dd/mm/yyyy
 *
 * @note This folder, along with the code for the kicker itself both have
 * version control, intended to be used with github desktop. A .gitignore file
 * is included in the kicker folder to prevent the inclusion of the build folder
 * in the repository.
 *
 * @note This project has the following subfolders:
 * - drivers: contains the code that is interfacing with the hardware
 * - setup: contains the code that is setting up the hardware, this code either
 * just runs once or is a callback function for another task
 * - supporting: This is anything that is not directly interfacing with the
 * hardware, but is not a task itself
 * - tasks: contains the code for the tasks that are running on the
 * microcontroller
 */

#include "../include/all_includes.h"
#include "../include/pinout.h"
#include "../include/setup/esp_now_setup.h"
#include "../include/setup/i2c_setup.h"
#include "../include/setup/led_strip_setup.h"
#include "../include/tasks/display_driver_task.h"
#include "../include/tasks/esp_now_task.h"
#include "../include/tasks/kicker_drive_train_task.h"
#include "../include/tasks/servo_driver_task.h"

#define display_driver_stack_size 10000 ///< stack size for the display driver
#define display_diver_priority 1 ///< display driver priority

#define kicker_drive_train_stack_size 4096 ///< stack size for the kicker drive train task
#define kicker_drive_train_priority configMAX_PRIORITIES - 1 ///< kicker drive train task priority

#define servo_driver_stack_size 4096 ///< stack size for the servo task
#define servo_driver_priority 2 ///< servo task priority

#define send_data_stack_size 4096 ///< stack size for the send data task
#define send_data_priority 5 ///< send data task priority

#define main_task_wait_time 1000 ///< delay time for the main task

// display battery value + mutex
SemaphoreHandle_t battery_voltage_mutex; ///< mutex protecting battery_voltage
float battery_voltage = 0; ///< battery_voltage : float, guarded by battery_voltage_mutex

// controller connected status value + mutex
SemaphoreHandle_t controller_connected_mutex; ///< mutex protecting controller_connected
uint8_t controller_connected = DISCONNECTED; ///< controller_connected : uint8_t, guarded by controller_connected_mutex

// motor status + mutex
SemaphoreHandle_t motor_status_mutex; ///< mutex protecting motor_status
uint8_t motor_status = IDLE; ///< motor_status : uint8_t , guarded by motor_status_mutex

// servo status + mutex
SemaphoreHandle_t servo_status_mutex; ///< mutex protecting servo_status
uint8_t servo_status = LATCHED; ///< servo_status : uint8_t guared by servo_status_mutex

// ramped values + values changed + mutexes
SemaphoreHandle_t ramped_values_updated; ///< mutex protecting ramped_values_updated_bool
bool ramped_values_updated_bool = true; ///< ramped_values_updated_bool : bool, guarded by ramped_values_updated

SemaphoreHandle_t ramped_mutex; ///< mutex protecting the ramped values
float velocity_ramp_limit = 0.5f; ///< velocity_ramp_limit : float guarded by ramped_mutex
float velocity_gain = 0.0f; ///< velocity_gain : float guarded by ramped_mutex
float velocity_integrator_gain = 0.0f; ///< velocity_integrator_gain : float guarded by ramped_mutex

// motor speed settings + mutex
SemaphoreHandle_t motor_speeds_settings_mutex; ///< mutex protecting the motor speed multipliers
uint8_t turning_speed_mult = 0;  ///< turning_speed_mult : uint8_t guarded by motor_speeds_settings_mutex
uint8_t driving_speed_mult = 0; ///< driving_speed_mult : uint8_t guarded by motor_speeds_settings_mutex

// need to update display + mutex
SemaphoreHandle_t update_main_display_mutex; ///< mutex protecting update_main_display
bool update_main_display = false; ///< update_main_display : bool, guarded by update_main_display_mutex

// strings for the display + mutex
SemaphoreHandle_t main_menu_values_mutex; ///< mutex protecting the main menu values
std::string controller_connected_string = "DisCon"; ///< controller_connected_string : string, guarded by main_menu_values_mutex
std::string battery_voltage_string = "0"; ///< battery_voltage_string : string, guarded by main_menu_values_mutex

// current motor speeds + mutex (0-255, centered at 127)
SemaphoreHandle_t motor_speeds; ///< mutex protecting the motor speeds (values written to motors)
uint8_t driving_speed = 127; ///< driving_speed : uint8_t, guarded by motor_speeds
uint8_t turning_speed = 127; ///< turning_speed : uint8_t, guarded by motor_speeds

SemaphoreHandle_t network_channel_mutex = NULL;
uint8_t current_network_channel = 13;



SemaphoreHandle_t right_drive_velocity_estimate_mutex = NULL;
float right_drive_velocity_estimate=0;

SemaphoreHandle_t left_drive_velocity_estimate_mutex= NULL;
float left_drive_velocity_estimate=0;


// i2c bus tags
i2c_master_bus_config_t i2c_mst_config; ///< i2c_mst_config : i2c_master_bus_config_t i2c bus config
i2c_master_bus_handle_t bus_handle; ///< bus_handle : i2c_master_bus_handle_t i2c bus handle

/**
 * @brief the main function for the program, creates all semaphores, tasks, and runs setup functions
 */

extern "C" void app_main(void) {
    #ifdef TIME_BETWEEN_MESSAGES
        gpio_set_direction(C0_4_pin, GPIO_MODE_OUTPUT);
    #endif
    battery_voltage_mutex = xSemaphoreCreateMutex();
    controller_connected_mutex = xSemaphoreCreateMutex();
    motor_status_mutex = xSemaphoreCreateMutex();
    servo_status_mutex = xSemaphoreCreateMutex();
    ramped_values_updated = xSemaphoreCreateMutex();
    motor_speeds_settings_mutex = xSemaphoreCreateMutex();
    update_main_display_mutex = xSemaphoreCreateMutex();
    main_menu_values_mutex = xSemaphoreCreateMutex();
    motor_speeds = xSemaphoreCreateMutex();
    ramped_mutex = xSemaphoreCreateMutex();
    network_channel_mutex = xSemaphoreCreateMutex();

    right_drive_velocity_estimate_mutex = xSemaphoreCreateMutex();
    left_drive_velocity_estimate_mutex = xSemaphoreCreateMutex();

    esp_now_setup();
    i2c_setup();
    led_setup();

    (void)xTaskCreate(display_driver_task, "display_driver_task", display_driver_stack_size, NULL, display_diver_priority, NULL);
    (void)xTaskCreatePinnedToCore(kicker_drive_train_task, "kicker_drive_train_task", kicker_drive_train_stack_size, NULL, kicker_drive_train_priority, NULL, 0);
    (void)xTaskCreate(servo_driver_task, "servo_driver_task", servo_driver_stack_size, NULL, servo_driver_priority, NULL);
    (void)xTaskCreate(send_data_task, "send_data_task", send_data_stack_size, NULL, send_data_priority, NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(main_task_wait_time));
    }
}
