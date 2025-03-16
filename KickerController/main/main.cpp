/**
 * @file main.cpp
 * @brief This is the file that contains the main function and all other tasks for the robot controller
 * @author Shane Wood
 * @details This is the code for the kicker controller, it was part of the 2024-2025 senior design project. The
 * controller communicates with a ps4 controller over usb, and transmits data to the robot over esp-now. The
 * controller allows for various parameters to be tuned using the menu interfaces on the oled display, with all
 * input coming from the ps4 controller. The controller also displays the battery voltage of the robot on the oled
 * display and the connection status to the robot and the controller.
 *
 * @date 22/12/2024
 * @note Dates are in the format dd/mm/yyyy
 *
 * @note This folder, along with the code for the kicker itself both have version control, intended to be used
 * with github desktop. A .gitignore file is included in the kicker folder to prevent the inclusion of the build
 * folder in the repository.
 *
 * @note This project has the following subfolders:
 * - drivers: contains the code that is interfacing with the
 * hardware
 * - setup: contains the code that is setting up the
 * hardware, this code either just runs once or is a
 * callback function for another task
 * - supporting: This is anything that is not directly
 * interfacing with the hardware, but is not a task itself
 * - tasks: contains the code for the tasks that are running
 * on the microcontroller
 */

#include "../include/all_includes.h"


#include "../include/setup/i2c_setup.h"
#include "../include/setup/esp_now_setup.h"
#include "../include/setup/esp_now_callback_setup.h"


#include "../include/drivers/ps4.h"
#include "../include/drivers/ssd1306.h"

#include "../include/supporting/changeable_values.h"
#include "../include/supporting/edge_dector.h"
#include "../include/supporting/menu_handler.h"
#include "../include/supporting/menus.h"

#include "../include/tasks/display_task.h"
#include "../include/tasks/ps4_data_processor_task.h"
#include "../include/tasks/send_data_task.h"
#include "../include/tasks/ps4_task.h"







#define send_data_task_stack_size 8092 ///< stack size for the send data task
#define ps4_task_stack_size 8092 ///< stack size for the ps4 task
#define display_task_stack_size 16384 ///< stack size for the display task
#define ps4_data_processor_stack_sze 8092 ///< stack size for the ps4 data processor task

#define send_data_task_priority 8 ///< priority for the send data task
#define ps4_task_priority 10 ///< priority for the ps4 task
#define display_task_priority 5 ///< priority for the display task
#define ps4_data_processor_priority 11 ///< priority for the ps4 data processor task

#define size_of_other_controller_data_queue 10 ///< size of the queue for the other controller data

#define main_task_wait_time 1000 ///< time to wait in the main task



SemaphoreHandle_t update_main_display_mutex = NULL; ///< mutex to determine if the main display needs to be updated or not
bool update_main_display = true; ///< flag to determine if the main display needs to be updated

SemaphoreHandle_t main_menu_values_mutex=NULL; ///< mutex protecting the main menu values
std::string contoller_connected = "DisCon"; ///< string to display if the controller is connected or not
std::string robot_connected = "DisCon"; ///< string to display if the robot is connected or not
std::string battery_voltage_string = "0"; ///< string to display the battery voltage
std::string status = ""; ///< string to display the status of the robot

std::atomic<bool> ps4_controller_connected = false; ///< atomic variable to determine if the ps4 controller is connected or not

SemaphoreHandle_t motor_status_mutex=NULL; ///< mutex protecting the motor status
uint8_t motor_status = IDLE; ///< status of the motors

SemaphoreHandle_t ramped_velocity_mutex=NULL; ///< mutex protecting the ramped velocity values
float velocity_ramp_limit = 10.0f; ///< 
float velocity_gain = 0.07f; ///< 
float velocity_integrator_gain = 0.05f;

SemaphoreHandle_t motor_speeds_mutex=NULL; ///< mutex protecting the motor speeds
uint8_t left_motor_speed = 1; ///< speed multiplier of the left motor
uint8_t right_motor_speed = 1; ///< speed multiplier of the right motor

SemaphoreHandle_t servo_status_mutex=NULL; ///< mutex protecting the servo status
uint8_t servo_status = LATCHED; ///< status of the servo

// i2c bus config
i2c_master_bus_config_t i2c_mst_config; ///< i2c bus config
i2c_master_bus_handle_t bus_handle=NULL; ///< i2c bus handle

// queue handles
QueueHandle_t other_controller_data_queue = NULL; ///< queue used by the menu handler from the processed controller data

SemaphoreHandle_t new_controller_data = NULL; ///< mutex to determine if there is new controller data for the ps4 data processor

std::atomic<uint8_t> controller_byte_2 = 0; ///< atomic variable for the controller byte 2 (left stick y)
std::atomic<uint8_t> controller_byte_4 = 0; ///< atomic variable for the controller byte 4 (right stick y)
std::atomic<uint8_t> controller_byte_5 = 0; ///< atomic variable for the controller byte 5 (buttons + dpad)
std::atomic<uint8_t> controller_byte_6 = 0; ///< atomic variable for the controller byte 6 (l1, l2, r1, r2, options, share, l3, r3)


/**
 * @brief the main function for the program, creates all semaphores, tasks, and runs setup functions
 */
extern "C" void app_main(void) {
  // create queues
  other_controller_data_queue = xQueueCreate(size_of_other_controller_data_queue, sizeof(display_controller_data));

  // create semaphores/mutuexes (espidf does not have
  // mutexes directly)
  update_main_display_mutex = xSemaphoreCreateMutex();
  main_menu_values_mutex = xSemaphoreCreateMutex();
  ramped_velocity_mutex = xSemaphoreCreateMutex();
  motor_speeds_mutex = xSemaphoreCreateMutex();
  servo_status_mutex = xSemaphoreCreateMutex();
  motor_status_mutex = xSemaphoreCreateMutex();

  new_controller_data = xSemaphoreCreateBinary();

  i2c_setup();
  esp_now_setup();

  
  vTaskDelay(pdMS_TO_TICKS(main_task_wait_time));
  
  // create tasks
  xTaskCreate(send_data_task, "SendDataTask", send_data_task_stack_size, NULL, send_data_task_priority, NULL);
  xTaskCreate(ps4_task, "ps4_task", ps4_task_stack_size, NULL, ps4_task_priority, NULL);
  xTaskCreate(display_task, "display_task", display_task_stack_size, NULL, display_task_priority, NULL);
  xTaskCreate(ps4_data_processor, "ps4_data_processor", ps4_data_processor_stack_sze, NULL, ps4_data_processor_priority, NULL);

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(main_task_wait_time));
  }
}

