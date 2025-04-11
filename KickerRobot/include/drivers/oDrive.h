/**
 * @file oDrive.h
 * @brief This is the header for the oDrive class
 * @author Shane Wood
*/

#ifndef __oDrive__
#define __oDrive__
#include "../all_includes.h"
#include "../supporting/odrive_commands.h"
#include "../pinout.h"

extern SemaphoreHandle_t battery_voltage_mutex;
extern float battery_voltage;
extern QueueHandle_t displayBatteryQueue;
extern std::string battery_voltage_string;
extern SemaphoreHandle_t main_menu_values_mutex;

extern SemaphoreHandle_t right_drive_velocity_estimate_mutex;
extern float right_drive_velocity_estimate;

extern SemaphoreHandle_t left_drive_velocity_estimate_mutex;
extern float left_drive_velocity_estimate;


#define bytes_in_float 4 ///< the number of bytes in a float
#define max_message_size 8 ///< the maximum size of a message
#define node_id_offset 5 ///< the offset of the node id in the message
#define lower_five_bits 0x1F ///< the lower five bits of a byte

/**
 * @class oDrive
 * @brief This is the class to control an odrive motor controller
 */
class oDrive {
   private:
    uint8_t node_ID;  ///< node id as set in the Odrive configurator

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


    void update_current_motor_velocity(twai_message_t rx_message);
};

#endif