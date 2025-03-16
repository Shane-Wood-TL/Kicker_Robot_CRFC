/**
 * @file all_includes.h
 * @brief Contains the system wide includes for esp-idf and c++, along with structs and enums
 * @author Shane Wood
 */
#ifndef __allIncludes__
#define __allIncludes__
#include <string>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/twai.h"
#include "driver/ledc.h"


#include "esp_system.h"
#include "esp_wifi.h"
#include "espnow.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "espnow_ctrl.h"
#include "espnow_utils.h"

#include "esp_timer.h"
#include "led_strip.h"

/**
 * @brief macro to enable the toggling of the gpio pin for debugging time between esp-now messages
 */
#define TIME_BETWEEN_MESSAGES

/**
 * @brief macro to keep the code from progressing if a condition is not met
 */
#define assertS(x) while((x)){}

/**
 * @brief map function to map a value from one range to another
 * 
 * @param a : numerical value to change the range of
 * @param b : the lower bound of the current range
 * @param c : the upper bound of the current range
 * @param w : the lower bound of the new range
 * @param q : the upper bound of the new range
 * 
 * @note No checking is done on the input or output side, if a is outside of [b,c] the results
 * will not be as expected, given that the input is also not an expected value
 */
#define map(a,b,c,w,q) ((w)+(((a) - (b)) * ((q) - (w)))/((c)-(b)))


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
enum motor_status_list{ENABLED, DISABLED, ERRORLESS,CALIBRATING, IDLE};

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
 * @enum controller_status_list
 * @brief Contains the different types of states for the controller
 * CONNECTED = controller is connected
 * DISCONNECTED = controller is disconnected
 */
enum controller_status_list{CONNECTED, DISCONNECTED};


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
} esp_now_data_to_send;


/**
 * @struct esp_now_data_to_receive
 * @brief Contains the data that is sent to the robot from the controller
 */
typedef struct {
    /**
     * @brief left_motor_speed : uint8_t
     * Data from the left joystick on the ps4 controller
     */
    uint8_t left_motor_speed;

    /**
     * @brief right_motor_speed : uint8_t
     * Data from the right joystick on the ps4 controller
     */
    uint8_t right_motor_speed;

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
     * Contains the data for the speed settings for the left and right motor
     * The right speed is the lower 4 bits
     * The left speed is the upper 4 bits
     */
    uint8_t motor_speed_setting;
} esp_now_data_to_receive;

#endif