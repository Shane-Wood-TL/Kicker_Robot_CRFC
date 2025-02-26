#ifndef __oDrive__
#define __oDrive__
#include "../all_includes.h"

class oDrive{
    private:
        const uint8_t Get_Version = 0x00;
        const uint8_t Heartbeat = 0x01;
        const uint8_t Estop = 0x02;
        const uint8_t Get_Error = 0x03;
        const uint8_t RxSdo = 0x04;
        const uint8_t TxSdo = 0x05;
        const uint8_t Address = 0x06;
        const uint8_t Set_Axis_State = 0x07;
        const uint8_t Get_Encoder_Estimates = 0x09;
        const uint8_t Set_Controller_Mode = 0x0b;
        const uint8_t Set_Input_Pos = 0x0c;
        const uint8_t Set_Input_Vel = 0x0d;
        const uint8_t Set_Input_Torque = 0x0e;
        const uint8_t Set_Limits = 0x0f;
        const uint8_t Set_Traj_Vel_Limit = 0x11;
        const uint8_t Set_Traj_Accel_Limits = 0x12;
        const uint8_t Set_Traj_Inertia = 0x13;
        const uint8_t Get_Iq = 0x14;
        const uint8_t Get_Temperature = 0x15;
        const uint8_t Reboot = 0x16;
        const uint8_t Get_Bus_Voltage_Current = 0x17;
        const uint8_t Clear_Errors = 0x18;
        const uint8_t Set_Absolute_Position = 0x19;
        const uint8_t Set_Pos_Gain = 0x1a;
        const uint8_t Set_Vel_Gains = 0x1b;
        const uint8_t Get_Torques = 0x1c;
        const uint8_t Get_Powers = 0x1d;
        const uint8_t Enter_DFU_Mode = 0x1f;
        uint8_t node_ID;
    public:
        oDrive(uint8_t node_IDV);
        void send_message(const uint8_t messageID, const uint8_t* messageContents, uint8_t messageLength, const bool dataReturned);
        void receive_message();
};

#endif