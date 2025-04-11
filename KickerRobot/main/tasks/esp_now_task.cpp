/**
 * @file esp_now_task.cpp
 * @brief source file for the task that sends the battery voltage to the controller
 * @author Shane Wood
 * @date 15/02/2025
 */

#include "../../include/tasks/esp_now_task.h"


//task that sends controller data
void send_data_task(void *pvParameter) {
    static esp_now_data_to_send current_transmission;
    uint8_t current_wifi_channel = 0;
    for(;;){
        if (xSemaphoreTake(battery_voltage_mutex, pdMS_TO_TICKS(1000))){
            current_transmission.battery_voltage = battery_voltage;
            xSemaphoreGive(battery_voltage_mutex);
        }
        (void)esp_now_send(controller_mac_address, (uint8_t *)&current_transmission, sizeof(esp_now_data_to_send));


        if(xSemaphoreTake(controller_connected_mutex, portMAX_DELAY)){
            if(controller_connected == CONNECTED){
                xSemaphoreGive(controller_connected_mutex);
                if(xSemaphoreTake(network_channel_mutex, portMAX_DELAY)){
                    if(current_wifi_channel != current_network_channel){
                        current_wifi_channel = current_network_channel;
                        xSemaphoreGive(network_channel_mutex);
                        esp_wifi_set_channel(current_wifi_channel, WIFI_SECOND_CHAN_NONE);
                        esp_now_del_peer(controller_mac_address);
                        esp_now_peer_info_t peer_info = {};
                        memcpy(peer_info.peer_addr, controller_mac_address, mac_address_length);
                        peer_info.channel = current_wifi_channel;
                        peer_info.encrypt = false;
                        ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
                    }else{
                        xSemaphoreGive(network_channel_mutex);
                    }
                    vTaskDelay(pdMS_TO_TICKS(send_data_task_wait_time));
                }
            }else{
                xSemaphoreGive(controller_connected_mutex);
                current_wifi_channel++;
                if(current_wifi_channel > 13){
                    current_wifi_channel = 0;
                }
                esp_wifi_set_channel(current_wifi_channel, WIFI_SECOND_CHAN_NONE);
                esp_now_del_peer(controller_mac_address);
                esp_now_peer_info_t peer_info = {};
                memcpy(peer_info.peer_addr, controller_mac_address, mac_address_length);
                peer_info.channel = current_wifi_channel;
                peer_info.encrypt = false;
                ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            
        }
    }
}