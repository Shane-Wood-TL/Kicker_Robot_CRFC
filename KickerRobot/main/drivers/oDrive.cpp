#include "../../include/all_includes.h"
#include "../../include/drivers/oDrive.h"


extern SemaphoreHandle_t battery_voltage_mutex;
extern float battery_voltage;
extern QueueHandle_t displayBatteryQueue; 
extern std::string battery_voltage_string;
extern SemaphoreHandle_t main_menu_values_mutex;


oDrive::oDrive(uint8_t node_IDV){
    node_ID = node_IDV;
}

void oDrive::send_message(const uint8_t message_ID, const uint8_t* message_contents, uint8_t message_length, const bool data_returned) {
    // max data size is 8 bytes
    if (message_length > 8) {
        return;
    }
    if(message_contents == nullptr){
        message_length = 0;
    }
    //setup can node + message ID with some bit manipulation to ensure the messageID is the least sigificant 5 bits
    uint32_t canID = (node_ID << 5) | (message_ID & 0x1F); 

    //setup the message to transmit
    twai_message_t txMessage;
    txMessage.identifier = canID;
    memcpy(txMessage.data, message_contents, message_length);//copy the message data
    txMessage.extd = 0; //not a extended frame
    txMessage.rtr = 0;  //no remote transmission request
    txMessage.data_length_code = message_length; //set the number of bytes in the message

    //send the message
    esp_err_t result = twai_transmit(&txMessage, pdMS_TO_TICKS(200));

    //if the data was sent successfully, and a response is expected
    if(data_returned && (result == ESP_OK)){
        twai_message_t rxMessage;

        result = twai_receive(&rxMessage, pdMS_TO_TICKS(200));
        if(result == ESP_OK){
            //majority of these are not implemented as they are not needed, only the battery voltage is needed
            if(rxMessage.identifier == ((node_ID << 5) | Get_Version)) {}
            else if(rxMessage.identifier == ((node_ID << 5) | Get_Error)) {}
            else if(rxMessage.identifier == ((node_ID << 5) | Get_Encoder_Estimates)) {}
            else if(rxMessage.identifier == ((node_ID << 5) | Get_Iq)) {}
            else if(rxMessage.identifier == ((node_ID << 5) | Get_Temperature)) {}
            else if(rxMessage.identifier == ((node_ID << 5) | Get_Bus_Voltage_Current)) {
                union {
                    float a;
                    uint8_t bytes[4];
                } tempUnion;

                //convert the 4 bytes to a float
                tempUnion.bytes[0] = rxMessage.data[0];
                tempUnion.bytes[1] = rxMessage.data[1];
                tempUnion.bytes[2] = rxMessage.data[2];
                tempUnion.bytes[3] = rxMessage.data[3];

                //update the value to be displayed
                if (xSemaphoreTake(battery_voltage_mutex, portMAX_DELAY)){
                    battery_voltage = tempUnion.a;
                    xSemaphoreGive(battery_voltage_mutex);
                    if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                        battery_voltage_string = battery_voltage;
                        xSemaphoreGive(main_menu_values_mutex);
                    }
                }
            }
            else if(rxMessage.identifier == ((node_ID << 5) | Get_Torques)) {}
            else if(rxMessage.identifier == ((node_ID << 5) | Get_Powers)) {}
        }else{
            //if the data was not received, set the battery voltage to 0
            if(message_ID == Get_Bus_Voltage_Current){
                if (xSemaphoreTake(battery_voltage_mutex, portMAX_DELAY)){
                    battery_voltage = 0;
                    xSemaphoreGive(battery_voltage_mutex);
                    if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                        battery_voltage_string = battery_voltage;
                        xSemaphoreGive(main_menu_values_mutex);
                    }
                }
            }
        }
    }else if (data_returned && (result != ESP_OK)){
        //if the transmit failed, and data is expected, set the battery voltage to 0
        //if the transmit failed the, then the receive will not be called
        if(message_ID == Get_Bus_Voltage_Current){
            if (xSemaphoreTake(battery_voltage_mutex, portMAX_DELAY)){
                battery_voltage = 0;
                xSemaphoreGive(battery_voltage_mutex);
                if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                    battery_voltage_string = battery_voltage;
                    xSemaphoreGive(main_menu_values_mutex);
                }
            }
        }
    }
}



// not used in the code, but was used for testing to detect the heartbeat
//messages that are sending data to the microcontroller have a true for sendMessage()
void oDrive::receive_message(){
    twai_message_t rxMessage;
    esp_err_t result = twai_receive(&rxMessage, pdMS_TO_TICKS(200));
    if(result == ESP_OK){
        if(rxMessage.identifier == ((node_ID << 5) | Heartbeat)){
            ESP_LOGI("ODRIVE", "tag: 0x%03X: %02X %02X %02X %02X %02X %02X",
            (unsigned int)rxMessage.identifier,
            (unsigned int)rxMessage.data[0], 
            (unsigned int)rxMessage.data[1], 
            (unsigned int)rxMessage.data[2], 
            (unsigned int)rxMessage.data[3], 
            (unsigned int)rxMessage.data[4], 
            (unsigned int)rxMessage.data[5]);
        }else {
            ESP_LOGW("ODRIVE", "Unexpected CAN ID: 0x%03X", (unsigned int)rxMessage.identifier);
        }
    }
}
