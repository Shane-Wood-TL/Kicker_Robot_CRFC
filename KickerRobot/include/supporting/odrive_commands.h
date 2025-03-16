/**
 * @files Contains the command id for various oDrive commands
 * @author Shane Wood
 */
#ifndef __odrive_commands__
#define __odrive_commands__

#define Get_Version 0x00  ///< Odrive command id for get version
#define Heartbeat 0x01    ///< Odrive command id for hearbeat
#define Estop 0x02        ///< Odrive command id for estop
#define Get_Error 0x03    ///< Odrive command id for get error
#define RxSdo 0x04  ///< Odrive command id for receiving any value from the odrive
#define TxSdo 0x05  ///< Odrive command id for transmitting any value to the odrive
#define Address 0x06  ///< Odrive command id for setting node id or sending serial number of Odrive
#define Set_Axis_State 0x07  ///< Odrive command id for changing axis state
#define Get_Encoder_Estimates 0x09  ///< Odrive command id for getting encoder estimates
#define Set_Controller_Mode 0x0b  ///< Odrive command id for setting controller mode
#define Set_Input_Pos 0x0c  ///< Odrive command id for setting input position
#define Set_Input_Vel 0x0d  ///< Odrive command id for setting input velocity
#define Set_Input_Torque 0x0e  ///< Odrive command id for setting input torque
#define Set_Limits 0x0f  ///< Odrive command id for setting velocity and current limits
#define Set_Traj_Vel_Limit 0x11  ///< Odrive command id for setting trajectory velocity limits
#define Set_Traj_Accel_Limits 0x12  ///< Odrive command id for setting trajectory acceleration limits
#define Set_Traj_Inertia 0x13  ///< Odrive command id for settings trajactory interia limits
#define Get_Iq 0x14  ///< Odrive command id for getting the magnitude of current on each phase (IQ) both setpoint and measure
#define Get_Temperature 0x15  ///< Odrive command id for getting FET and motor temperature
#define Reboot 0x16  ///< Odrive command id for rebooting + saving configuration
#define Get_Bus_Voltage_Current 0x17  ///< Odrive command id for getting the voltage and current of the Odrive
#define Clear_Errors 0x18  ///< Odrive command id for clearing errors
#define Set_Absolute_Position 0x19  ///< Odrive command id for setting an absolute position
#define Set_Pos_Gain 0x1a  ///< Odrive command id for setting position gain
#define Set_Vel_Gains 0x1b  ///< Odrive command id for setting velocity gains
#define Get_Torques 0x1c  ///< Odrive command id for getting torque target and estimate
#define Get_Powers 0x1d  ///< Odrive command id for getting electrical and mechanical power
#define Enter_DFU_Mode 0x1f  ///< Odrive command id for entering DFU mode

#endif