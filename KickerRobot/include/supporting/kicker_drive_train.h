/**
 * @file kicker_drive_train.h
 * @brief Header for the kicker drive_train class
 * @author Shane Wood
 */
#ifndef __kicker_drive_train__
#define __kicker_drive_train__

#include "../all_includes.h"
#include "../pinout.h"
#include "../drivers/oDrive.h"
#include "../supporting/odrive_commands.h"

#define battery_voltage_refresh_time 1000000 ///< how often the battery voltage is refreshed in us
#define eight_bytes 8 ///< 8 bytes in a message
#define one_byte 1 ///< 1 byte in a message
#define zero_bytes 0 ///< 0 bytes in a command message
#define input_velocity_deadzone 0.5 ///< deadzone for the input velocity
#define motor_drive_speed_multiplier_value 5.0 ///< value to multiply the joystick value by to get the motor speed
#define motor_turn_speed_multiplier_value 1.25 ///< value to multiply the joystick value by to get the motor speed
#define odrive_motor_torque 0.5 ///< torque value for the odrive motors
#define full_message_start_index 0 ///< start index of the full message
#define full_message_middle_index 4 ///< middle index of the full message (useful for 2 floats)
#define full_message_size eight_bytes ///< max message size
#define vel_ramp_rate_id 377 ///< id for the velocity ramp rate
#define odrive_write_rx 0x01 ///< odrive write rx id

#define eight_bit_minimum 0.0 ///< minimum value for an 8 bit number
#define eight_bit_maximum 255.0 ///< maximum value for an 8 bit number


//controller connected status value + mutex
extern SemaphoreHandle_t controller_connected_mutex;
extern uint8_t controller_connected;

//motor status + mutex
extern SemaphoreHandle_t motor_status_mutex;
extern uint8_t motor_status;

//ramped values + values changed + mutexes
extern SemaphoreHandle_t ramped_values_updated;
extern bool ramped_values_updated_bool;
extern SemaphoreHandle_t ramped_mutex;
extern float velocity_ramp_limit;
extern float velocity_gain;  
extern float velocity_integrator_gain;

//motor speed settings + mutex
extern SemaphoreHandle_t motor_speeds_settings_mutex;
extern uint8_t turning_speed_mult; //values set in settings
extern uint8_t driving_speed_mult;

//current motor speeds + mutex (0-255, centered at 127)
extern SemaphoreHandle_t motor_speeds;
extern uint8_t driving_speed;
extern uint8_t turning_speed;

extern SemaphoreHandle_t right_drive_velocity_estimate_mutex;
extern float right_drive_velocity_estimate;

extern SemaphoreHandle_t left_drive_velocity_estimate_mutex;
extern float left_drive_velocity_estimate;

/**
 * @class kicker_drive_train
 * @brief This class is for controlling the kicker drive train (2 odrives)
 * @details Controls two odrives in a tank drive configuration
 */
class kicker_drive_train{
    private:
        oDrive *left_drive; ///< pointer to an instance of the odrive class for the left motor
        oDrive *right_drive; ///< pointer to an instance of the odrive class for the right motor

        int64_t last_time; ///< last time that the battery voltage was updated
        int32_t battery_update_time; ///< how often the battery voltage is updated

        uint8_t last_motor_state; ///< the last motor state

        // last value read from the controller joystick
        uint8_t last_driving_speed; ///< the last left joystick reading
        uint8_t last_turning_speed; ///< the last right joystick reading

        uint8_t last_driving_speed_mult; ///< the last left motor speed multiplier
        uint8_t last_turning_speed_mult; ///< the last right motor speed multiplier

        /**
         * @brief update the battery voltage if the correct amount of time has passed
         */
        void periodic_voltage_reading();

        /**
         * @brief update the ramped velocity parameters if they have been changed
         */
        void ramped_settings_updater();

        /**
         * @brief drive the motors based on set parameters and last joystick values
         */
        void drive_motors(bool boosted);

        /**
         * @brief run the calibration sequence with periodic messages over can bus to prevent the odrives from entering watchdog timeout
         */
        void calibrating();

        /**
         * @brief clear the errors on the odrives
         */
        void clear_errors();

        /**
         * @brief set the motors to their idle state
         */
        void disable_motors();

        /**
         * @brief set the motors to their active state
         */
        void enable_motors();

        /**
         * @brief change the status of the motors
         */
        void motor_status_updater();

        /**
         * @brief modify the speed multipliers
         */
        void motor_speed_multiplier_updater();

        /**
         * @brief update the speeds of the motors based on the joysticks
         */
        void motor_speeds_updater();

        /**
         * @brief send estop to the odrives
         */
        void estop();

        void break_motors();

        /// options for Set_Axis_State
        uint8_t CLOSED_LOOP_CONTROL = 0x8;  ///< Data to set Odrive to closed loop control
        uint8_t NO_DATA = 0x0;              ///< empty byte if no data is needed to be sent
        uint8_t IDLE_STATE = 0x1;           ///< data to set the motor to the idle state
        uint8_t FULL_CALIBRATION_SEQUENCE = 0x3;  ///< data to run the calibration sequence

    public:
        /**
         * @brief constructor for the kicker_drive_train class
         * 
         * @param left_drive : oDrive* a pointer to the left odrive class instance
         * @param right_drive : oDrive* a pointer to the right odrive class instance
         * @param battery_voltage_refresh : uint32_t the amount of time to wait to update the battery voltage (in micro seconds)
         */
        kicker_drive_train(oDrive *left_drive, oDrive *right_drive, uint32_t battery_voltage_refresh);

        /**
         * @brief The main loop for the drive train, updates values, checks battery, and drives motors
         */
        void update();

};

#endif