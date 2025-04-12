/**
 * @file esp_now_callback_setup.cpp
 * @brief source file that sets up esp-now callbacks
 * @author Shane Wood
 * @date 1/15/2025
 */
#include "../../include/setup/esp_now_callback_setup.h"




// function that is called, when data is received
void on_receive(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{

    esp_now_data_to_receive received_data;
    if (len == sizeof(received_data))
    {
        //macro to measure latency
        #ifdef TIME_BETWEEN_MESSAGES
            static bool toggle = false;
            (void)gpio_set_level(C0_4_pin,toggle);
            toggle = !toggle;
        #endif


        (void)memcpy(&received_data, data, sizeof(received_data));

        //update the current motor speeds
        if (xSemaphoreTake(motor_speeds, portMAX_DELAY))
        {
            driving_speed = received_data.driving_speed;
            turning_speed = received_data.turning_speed;
            xSemaphoreGive(motor_speeds);
        }

        //update the ramped settings
        if (xSemaphoreTake(ramped_mutex, portMAX_DELAY))
        {
            if ((velocity_ramp_limit != received_data.velocity_ramp_limit) | (velocity_gain != received_data.velocity_gain) | (velocity_integrator_gain != received_data.velocity_integrator_gain))
            {
                velocity_ramp_limit = received_data.velocity_ramp_limit;
                velocity_gain = received_data.velocity_gain;
                velocity_integrator_gain = received_data.velocity_integrator_gain;
                xSemaphoreGive(ramped_mutex);
                if (xSemaphoreTake(ramped_values_updated, portMAX_DELAY))
                {
                    ramped_values_updated_bool = true;
                    xSemaphoreGive(ramped_values_updated);
                }
            }
            else
            {
                xSemaphoreGive(ramped_mutex);
            }
        }

        //update the servo status
        if (xSemaphoreTake(servo_status_mutex, portMAX_DELAY))
        {
            servo_status = (received_data.servo_and_motor_state) & lower_four_bits;
            xSemaphoreGive(servo_status_mutex);
        }

        //update the motor status
        if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY))
        {
            motor_status = (received_data.servo_and_motor_state >> values_4) & lower_four_bits;
            xSemaphoreGive(motor_status_mutex);
        }

        //update the speed multiplers
        if (xSemaphoreTake(motor_speeds_settings_mutex, portMAX_DELAY))
        {
            driving_speed_mult = (received_data.motor_speed_setting >> values_4) & lower_four_bits;
            turning_speed_mult = received_data.motor_speed_setting & lower_four_bits;
            xSemaphoreGive(motor_speeds_settings_mutex);
        }
        if (xSemaphoreTake(network_channel_mutex, portMAX_DELAY))
        {
            current_network_channel = received_data.esp_now_channel;
            xSemaphoreGive(network_channel_mutex);
        }
    }
    else
    {
        return;
    }
}



//call back function for when data is sent
void on_send(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if(mac_addr == nullptr){
        return;
    }
    static bool last_transmit_state=false;

    if(status == ESP_NOW_SEND_SUCCESS){
            if(last_transmit_state == false){
                last_transmit_state = true;
                if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                    controller_connected_string = "Connected";
                    xSemaphoreGive(main_menu_values_mutex);
                    if(xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)){
                        update_main_display = true;
                        xSemaphoreGive(update_main_display_mutex);
                    }
                }
                if(xSemaphoreTake(controller_connected_mutex, portMAX_DELAY)){
                    controller_connected = CONNECTED;
                    xSemaphoreGive(controller_connected_mutex);
                }
            }
        }else{
            if(last_transmit_state == true){
                last_transmit_state = false;
                if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                    controller_connected_string = "DisCon";
                    xSemaphoreGive(main_menu_values_mutex);
                    if(xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)){
                        update_main_display = true;
                        xSemaphoreGive(update_main_display_mutex);
                    }
                }
                if(xSemaphoreTake(controller_connected_mutex, portMAX_DELAY)){
                    controller_connected = DISCONNECTED;
                    xSemaphoreGive(controller_connected_mutex);
                }
            }
        }
    return;
}
