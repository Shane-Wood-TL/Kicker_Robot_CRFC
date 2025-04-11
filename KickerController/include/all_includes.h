/**
 * @file all_includes.h
 * @brief Contains the system wide includes for esp-idf and c++, along with structs and enums
 * @author Shane Wood
 */

#ifndef __all_includes__
#define __all_includes__
//standard C includes
#include <stdio.h>
#include <string>
#include <stdbool.h>
#include <string.h>
#include <atomic>


//rtos includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

//esp includes
#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "errno.h"
#include "esp_system.h"
#include "esp_timer.h"


//esp now includes
#include "esp_wifi.h"
#include "espnow.h"
#include "espnow_ctrl.h"
#include "espnow_utils.h"
#include "esp_crc.h"
#include "esp_netif.h"
#include "nvs_flash.h"

//usb includes
#include "usb/usb_host.h"
#include "usb/hid_host.h"

//drives includes
#include "driver/gpio.h"
#include "driver/i2c_master.h"


/**
 * @struct display_controller_data
 * @brief Contains the data of inputs to drive the menu handler
 */
typedef struct {
    /**
     * @brief Button states
     * 
     * bit[7] = x button  
     * bit[6] = circle button  
     * bit[5] = square  
     * bit[4] = triangle  
     * bit[3] = left dpad  
     * bit[2] = down dpad  
     * bit[1] = uint8_t turn_motor_speed dpad  
     * bit[0] = up dpad  
     */
    uint8_t buttons;

    /**
     * @brief Options and trigger states
     * 
     * bit[7] = unused  
     * bit[6] = unused  
     * bit[5] = unused  
     * bit[4] = unused  
     * bit[3] = unused  
     * bit[2] = l2 pressed  
     * bit[1] = r2 pressed  
     * bit[0] = options button pressed  
     */
    uint8_t options_triggers;
} display_controller_data;

/**
 * @enum motor_status_list
 * @brief Contains the different types of states for the Odrive motor controllers
 *
 * @details motor_status_list has the following options: ENABLED, DISABLED, ERRORLESS, CALIBRATING, IDLE
 * ENABLED = motor working as normal
 * DISABLED = motor set to idle (not by choice, this state happens when something wrong has occured, such as controller disconnect)
 * ERRORLESS = clears motor errors
 * CALIBRATING = motor is running it's calibration sequence
 * IDLE = motor is not moving / has no power going through it
 * 
 */
enum motor_status_list{ENABLED, DISABLED, ERRORLESS,CALIBRATING, IDLE, BOOSTED};

/**
 * @enum servo_status_list
 * @brief Contains the different types of states for the servo motors
 *
 * @details servo_status_list has the following options: LATCHED, RELEASED, DETACHED
 * LATCHED = servo is in position to keep the kicking leg from moving
 * RELEASED = servo is in position to release the kicking leg
 * DETACHED = state servo is in on boot, will update to the expected state when controller connects
 */
enum servo_status_list{LATCHED, RELEASED, DETACHED};


/**
 * @struct esp_now_data_to_send
 * @brief Contains the data that is sent to the robot from the controller
 */
typedef struct {
    /**
     * @brief driving_speed : uint8_t
     * Data from the left joystick on the ps4 controller
     */
    uint8_t driving_speed;

    /**
     * @brief turning_speed : uint8_t
     * Data from the uint8_t turn_motor_speed joystick on the ps4 controller
     */
    uint8_t turning_speed;

    /**
     * @brief velocity_ramp_limit : float
     * Max value at which the velocity will increase by, as set in the controller menu
     */
    float velocity_ramp_limit;

    /**
     * @brief velocity_gain : float
     * Acts as a proportional gain for the ramped velocity mode, as set in the controller menu
     */
    float velocity_gain;

    /**
     * @brief velocity_integrator_gain : float
     * Acts as a integral gain for the ramped velocity mode, as set in the controller menu
     */
    float velocity_integrator_gain;

    /**
     * @brief servo_and_motor_state : uint8_t
     * Contains the data for the servo and Odrive motor controllers current state
     * The servo status is the lower 4 bits
     * The motor status is the upper 4 bits
     */
    uint8_t servo_and_motor_state;

    /**
     * @brief motor_speed_setting : uint8_t
     * Contains the data for the speed settings for the left and uint8_t turn_motor_speed motor
     * The turning speed is the lower 4 bits
     * The dirving speed is the upper 4 bits
     */
    uint8_t motor_speed_setting;

    uint8_t esp_now_channel;
} esp_now_data_to_send;


/**
 * @struct esp_now_data_to_receive
 * @brief Contains the data that the controller is expecting from the robot
 */
typedef struct {
    /**
     * @brief battery_voltage : float
     * The battery voltage as a float
     */
    float battery_voltage; 
} esp_now_data_to_receive;



#define font_width 12 ///< width of the font in pixels
#define font_height 16 ///< height of the font in pixels

#define processed_dpad_left_bit 3 ///< bit for dpad left
#define processed_dpad_down_bit 2 ///< bit for dpad down
#define processed_dpad_right_bit 1 ///< bit for dpad uint8_t turn_motor_speed
#define processed_dpad_up_bit 0 ///< bit for dpad up

#define processed_triangle_bit 4 ///< bit for triangle
#define processed_square_bit 5 ///< bit for square
#define processed_circle_bit 6 ///< bit for circle
#define processed_x_bit 7 ///< bit for x

#define processed_options_bit 0 ///< bit for options
#define processed_r2_bit 1 ///< bit for r2
#define processed_l2_bit 2 ///< bit for l2

#define values_6 6 ///< basic define to prevent magic numbers for 6
#define values_5 5 ///< basic define to prevent magic numbers for 5
#define values_4 4 ///< basic define to prevent magic numbers for 4
#define values_3 3 ///< basic define to prevent magic numbers for 3
#define values_2 2 ///< basic define to prevent magic numbers for 2
#define values_1 1 ///< basic define to prevent magic numbers for 1

#define left_shift_4 << 4 ///< left shift by 4 bits

#define bytes_in_float 4 ///< number of bytes in a float for an esp32s3
#define lower_four_bits 0x0F ///< mask for the lower four bits
#endif


