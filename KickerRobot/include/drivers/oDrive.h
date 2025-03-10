#ifndef __oDrive__
#define __oDrive__
#include "../all_includes.h"
#include "../pinout.h"

class oDrive{
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


        uint8_t node_ID; ///< node id as set in the Odrive configurator


        /**
         * @brief the function that is called to update the display's battery voltage
         * 
         * @param rx_message : twai_message_t the message that has been received containing the id of Get_Bus_Voltage_Current
         */
        void update_battery_voltage(twai_message_t rx_message);


        /**
         * @brief the function that handles when a message needs to be received from the Odrive
         * 
         * @param message_ID : the id of the message that is going to be recived
         */
        void receive_message(uint8_t message_ID);


        /**
         * @brief the function that is called when something has gone wrong, and no message is received when Get_Bus_Voltage_Current is called    
         */
        void clear_battery_voltage();
    public:
        /**
         * @brief The constructor for the Odrive class
         * 
         * @param node_IDV : uint8_t the value of the node id to have commands sent to
         */
        oDrive(uint8_t node_IDV);

        /**
         * @brief send a command to the Odrive
         * 
         * @param message_ID : const uint8_t the ID of the message to be sent
         * @param message_contents : const uint8_t* the data to be sent with the message
         * @param message_length : uint8_t the length of the data
         * @param data_returned : const bool if data is returned (determines if receive_message is called or not)
         */
        void send_message(const uint8_t message_ID, const uint8_t* message_contents, uint8_t message_length, const bool data_returned);
        
};

#endif