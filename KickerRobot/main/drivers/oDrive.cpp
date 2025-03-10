#include "../../include/drivers/oDrive.h"

extern SemaphoreHandle_t battery_voltage_mutex;
extern float battery_voltage;
extern QueueHandle_t displayBatteryQueue;
extern std::string battery_voltage_string;
extern SemaphoreHandle_t main_menu_values_mutex;

#define bytes_in_float 4
#define max_message_size 8
#define node_id_offset 5
#define lower_five_bits 0x1F

oDrive::oDrive(uint8_t node_IDV)
{
    node_ID = node_IDV;
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
    uint32_t canID = (node_ID << node_id_offset) | (message_ID & lower_five_bits);

    // setup the message to transmit
    twai_message_t txMessage;
    txMessage.identifier = canID;
    memcpy(txMessage.data, message_contents, message_length); // copy the message data
    txMessage.extd = 0;                                       // not a extended frame
    txMessage.rtr = 0;  // no remote transmission request
    txMessage.data_length_code = message_length;              // set the number of bytes in the message

    twai_message_t rxMessage;
    if(data_returned){
        while(twai_receive(&rxMessage,0)== ESP_OK){
        
        }
    }
    esp_err_t result = twai_transmit(&txMessage, pdMS_TO_TICKS(200));

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
    twai_message_t rxMessage;
    esp_err_t result = twai_receive(&rxMessage, pdMS_TO_TICKS(500));
    if (result == ESP_OK)
    {
        // majority of these are not implemented as they are not needed, only the battery voltage is needed
        if (rxMessage.identifier == ((node_ID << node_id_offset) | Get_Version))
        {
        }
        else if (rxMessage.identifier == ((node_ID << node_id_offset) | Get_Error))
        {
        }
        else if (rxMessage.identifier == ((node_ID << node_id_offset) | Get_Encoder_Estimates))
        {
        }
        else if (rxMessage.identifier == ((node_ID << node_id_offset) | Get_Iq))
        {
        }
        else if (rxMessage.identifier == ((node_ID << node_id_offset) | Get_Temperature))
        {
        }
        else if (rxMessage.identifier == ((node_ID << node_id_offset) | Get_Bus_Voltage_Current))
        {
            update_battery_voltage(rxMessage);   
        }
        else if (rxMessage.identifier == ((node_ID << node_id_offset) | Get_Torques))
        {
        }
        else if (rxMessage.identifier == ((node_ID << node_id_offset) | Get_Powers))
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



void oDrive::update_battery_voltage(twai_message_t rxMessage)
{
    union
    {
        float a;
        uint8_t bytes[bytes_in_float];
    } tempUnion;

    // convert the 4 bytes to a float
    tempUnion.bytes[values_0] = rxMessage.data[values_0];
    tempUnion.bytes[values_1] = rxMessage.data[values_1];
    tempUnion.bytes[values_2] = rxMessage.data[values_2];
    tempUnion.bytes[values_3] = rxMessage.data[values_3];
    // update the value to be displayed

    if (xSemaphoreTake(battery_voltage_mutex, portMAX_DELAY))
    {
        battery_voltage = tempUnion.a;
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