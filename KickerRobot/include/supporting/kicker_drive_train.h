#ifndef __kicker_drive_train__
#define __kicker_drive_train__

#include "../all_includes.h"
#include "../pinout.h"
#include "../drivers/oDrive.h"

#define battery_voltage_refresh_time 1000000 //in us
#define eight_bytes 8
#define one_byte 1
#define zero_bytes 0
#define odrive_driver_wait_time 10
#define input_velocity_deadzone 0.5
#define motor_speed_multiplier_value 5.0
#define odrive_motor_torque 0.5
#define full_message_start_index 0
#define full_message_middle_index 4
#define full_message_size eight_bytes
#define vel_ramp_rate_id 377
#define odrive_write_rx 0x01

#define eight_bit_minimum 0.0
#define eight_bit_maximum 255.0


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
extern uint8_t left_motor_speed_mult; //values set in settings
extern uint8_t right_motor_speed_mult;

//current motor speeds + mutex (0-255, centered at 127)
extern SemaphoreHandle_t motor_speeds;
extern uint8_t left_motor_speed;
extern uint8_t right_motor_speed;


class kicker_drive_train{
    private:
        const uint8_t Get_Version = 0x00; ///< Odrive command id for get version
        const uint8_t Heartbeat = 0x01; ///< Odrive command id for hearbeat
        const uint8_t Estop = 0x02; ///< Odrive command id for estop
        const uint8_t Get_Error = 0x03; ///< Odrive command id for get error
        const uint8_t RxSdo = 0x04; ///< Odrive command id for receiving any value from the odrive
        const uint8_t TxSdo = 0x05; ///< Odrive command id for transmitting any value to the odrive
        const uint8_t Address = 0x06; ///< Odrive command id for setting node id or sending serial number of Odrive
        const uint8_t Set_Axis_State = 0x07; ///< Odrive command id for changing axis state
        const uint8_t Get_Encoder_Estimates = 0x09; ///< Odrive command id for getting encoder estimates
        const uint8_t Set_Controller_Mode = 0x0b; ///< Odrive command id for setting controller mode
        const uint8_t Set_Input_Pos = 0x0c; ///< Odrive command id for setting input position
        const uint8_t Set_Input_Vel = 0x0d; ///< Odrive command id for setting input velocity
        const uint8_t Set_Input_Torque = 0x0e; ///< Odrive command id for setting input torque
        const uint8_t Set_Limits = 0x0f; ///< Odrive command id for setting velocity and current limits 
        const uint8_t Set_Traj_Vel_Limit = 0x11; ///< Odrive command id for setting trajectory velocity limits
        const uint8_t Set_Traj_Accel_Limits = 0x12; ///< Odrive command id for setting trajectory acceleration limits
        const uint8_t Set_Traj_Inertia = 0x13; ///< Odrive command id for settings trajactory interia limits
        const uint8_t Get_Iq = 0x14; ///< Odrive command id for getting the magnitude of current on each phase (IQ) both setpoint and measured
        const uint8_t Get_Temperature = 0x15; ///< Odrive command id for getting FET and motor temperature
        const uint8_t Reboot = 0x16; ///< Odrive command id for rebooting + saving configuration
        const uint8_t Get_Bus_Voltage_Current = 0x17; ///< Odrive command id for getting the voltage and current of the Odrive
        const uint8_t Clear_Errors = 0x18; ///< Odrive command id for clearing errors
        const uint8_t Set_Absolute_Position = 0x19; ///< Odrive command id for setting an absolute position
        const uint8_t Set_Pos_Gain = 0x1a; ///< Odrive command id for setting position gain
        const uint8_t Set_Vel_Gains = 0x1b; ///< Odrive command id for setting velocity gains
        const uint8_t Get_Torques = 0x1c; ///< Odrive command id for getting torque target and estimate
        const uint8_t Get_Powers = 0x1d; ///< Odrive command id for getting electrical and mechanical power
        const uint8_t Enter_DFU_Mode = 0x1f; ///< Odrive command id for entering DFU mode

        /// options for Set_Axis_State
        const uint8_t CLOSED_LOOP_CONTROL = 0x8; ///< Data to set Odrive to closed loop control
        const uint8_t NO_DATA = 0x0; ///< empty byte if no data is needed to be sent
        const uint8_t IDLE_STATE = 0x1; ///< data to set the motor to the idle state 
        const uint8_t FULL_CALIBRATION_SEQUENCE = 0x3; ///< data to run the calibration sequence

        oDrive *left_drive; ///< pointer to an instance of the odrive class for the left motor
        oDrive *right_drive; ///< pointer to an instance of the odrive class for the right motor

        int64_t last_time; ///< last time that the battery voltage was updated
        int32_t battery_update_time; ///< how often the battery voltage is updated

        uint8_t last_motor_state; ///< the last motor state

        // last value read from the controller joystick
        uint8_t last_left_motor_speed; ///< the last left joystick reading
        uint8_t last_right_motor_speed; ///< the last right joystick reading

        uint8_t last_left_motor_speed_mult; ///< the last left motor speed multiplier
        uint8_t last_right_motor_speed_mult; ///< the last right motor speed multiplier

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
        void drive_motors();

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