/**
 * @file oDrive.h
 * @brief This is the source file for the oDrive class
 * @author Shane Wood
*/

#include "../../include/drivers/oDrive.h"




oDrive::oDrive(uint8_t node_ID)
{
    this->node_ID = node_ID;
}

void oDrive::send_message(const uint8_t message_ID, const uint8_t *message_contents, uint8_t message_length, const bool data_returned)
{
    // max data size is 8 bytes
    if (message_length > max_message_size)
    {
        return;
    }
    if (message_contents == nullptr)
    {
        message_length = 0;
    }

    // setup can node + message ID with some bit manipulation to ensure the messageID is the least sigificant node_id_offset bits
    uint32_t can_ID = (node_ID << node_id_offset) | (message_ID & lower_five_bits);

    // setup the message to transmit
    twai_message_t tx_message;
    tx_message.identifier = can_ID;
    (void)memcpy(tx_message.data, message_contents, message_length); // copy the message data
    tx_message.extd = 0;                                       // not a extended frame
    tx_message.rtr = 0;  // no remote transmission request
    tx_message.data_length_code = message_length;              // set the number of bytes in the message

    twai_message_t rx_message;
    if(data_returned){
        while(twai_receive(&rx_message,0)== ESP_OK){
        
        }
    }
    esp_err_t result = twai_transmit(&tx_message, pdMS_TO_TICKS(200));

    // if the data was sent successfully, and a response is expected
    if (data_returned && (result == ESP_OK))
    {
        receive_message(message_ID);
    }
    else if (data_returned && (result != ESP_OK))
    {
        // if the transmit failed, and data is expected, set the battery voltage to 0
        // if the transmit failed the, then the receive will not be called
        if (message_ID == Get_Bus_Voltage_Current)
        {
            clear_battery_voltage();
        }
    }
}



// not used in the code, but was used for testing to detect the heartbeat
// messages that are sending data to the microcontroller have a true for sendMessage()
void oDrive::receive_message(uint8_t message_ID)
{
    twai_message_t rx_message;
    esp_err_t result = twai_receive(&rx_message, pdMS_TO_TICKS(500));
    if (result == ESP_OK)
    {
        // majority of these are not implemented as they are not needed, only the battery voltage is needed
        if (rx_message.identifier == ((node_ID << node_id_offset) | Get_Version))
        {
        }
        else if (rx_message.identifier == ((node_ID << node_id_offset) | Get_Error))
        {
        }
        else if (rx_message.identifier == ((node_ID << node_id_offset) | Get_Encoder_Estimates))
        {
            update_current_motor_velocity(rx_message);
        }
        else if (rx_message.identifier == ((node_ID << node_id_offset) | Get_Iq))
        {
        }
        else if (rx_message.identifier == ((node_ID << node_id_offset) | Get_Temperature))
        {
        }
        else if (rx_message.identifier == ((node_ID << node_id_offset) | Get_Bus_Voltage_Current))
        {
            update_battery_voltage(rx_message);   
        }
        else if (rx_message.identifier == ((node_ID << node_id_offset) | Get_Torques))
        {
        }
        else if (rx_message.identifier == ((node_ID << node_id_offset) | Get_Powers))
        {
        }
    }
    else
    {
        // if the data was not received, set the battery voltage to 0
        if (message_ID == Get_Bus_Voltage_Current)
        {
            clear_battery_voltage();
        }
    }
}



void oDrive::update_battery_voltage(twai_message_t rx_message)
{
    union
    {
        float a;
        uint8_t bytes[bytes_in_float];
    } temp_union;

    // convert the 4 bytes to a float
    temp_union.bytes[values_0] = rx_message.data[values_0];
    temp_union.bytes[values_1] = rx_message.data[values_1];
    temp_union.bytes[values_2] = rx_message.data[values_2];
    temp_union.bytes[values_3] = rx_message.data[values_3];
    // update the value to be displayed

    if (xSemaphoreTake(battery_voltage_mutex, portMAX_DELAY))
    {
        battery_voltage = temp_union.a;
        xSemaphoreGive(battery_voltage_mutex);
        if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY))
        {
            battery_voltage_string = battery_voltage;
            xSemaphoreGive(main_menu_values_mutex);
        }
    }
}

void oDrive::clear_battery_voltage()
{
    if (xSemaphoreTake(battery_voltage_mutex, portMAX_DELAY))
    {
        battery_voltage = 0;
        xSemaphoreGive(battery_voltage_mutex);
        if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY))
        {
            battery_voltage_string = battery_voltage;
            xSemaphoreGive(main_menu_values_mutex);
        }
    }
}

void oDrive::update_current_motor_velocity(twai_message_t rx_message){
    union
    {
        float a;
        uint8_t bytes[bytes_in_float];
    } temp_union;

    // convert the 4 bytes to a float
    temp_union.bytes[values_0] = rx_message.data[values_4];
    temp_union.bytes[values_1] = rx_message.data[values_5];
    temp_union.bytes[values_2] = rx_message.data[values_6];
    temp_union.bytes[values_3] = rx_message.data[values_7];

    if(node_ID == oDrive_0_ID){
        if (xSemaphoreTake(left_drive_velocity_estimate_mutex, portMAX_DELAY))
        {
            left_drive_velocity_estimate = temp_union.a;
            xSemaphoreGive(left_drive_velocity_estimate_mutex);
        }
    }else if(node_ID == oDrive_1_ID){
        if (xSemaphoreTake(right_drive_velocity_estimate_mutex, portMAX_DELAY))
        {
            right_drive_velocity_estimate = temp_union.a;
            xSemaphoreGive(right_drive_velocity_estimate_mutex);
        }
    }
}