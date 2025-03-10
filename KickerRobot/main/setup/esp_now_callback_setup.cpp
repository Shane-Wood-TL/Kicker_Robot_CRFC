#include "../../include/setup/esp_now_callback_setup.h"




// function that is called, when data is received
void on_receive(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{

    esp_now_data_to_receive received_data;
    if (len == sizeof(received_data))
    {
        #ifdef TIME_BETWEEN_MESSAGES
            static bool toggle = false;
            gpio_set_level(C0_4Pin,toggle);
            toggle = !toggle;
        #endif
        memcpy(&received_data, data, sizeof(received_data));
        if (xSemaphoreTake(motor_speeds, portMAX_DELAY))
        {
            left_motor_speed = received_data.left_motor_speed;
            right_motor_speed = received_data.right_motor_speed;
            xSemaphoreGive(motor_speeds);
        }

        if (xSemaphoreTake(ramped_mutex, portMAX_DELAY))
        {
            if ((velocity_ramp_limit != received_data.kp) | (velocity_gain != received_data.ki) | (velocity_integrator_gain != received_data.kd))
            {
                velocity_ramp_limit = received_data.kp;
                velocity_gain = received_data.ki;
                velocity_integrator_gain = received_data.kd;
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

        if (xSemaphoreTake(servo_status_mutex, portMAX_DELAY))
        {
            servo_status = (received_data.servo_and_motor_state) & 0x0F;
            xSemaphoreGive(servo_status_mutex);
        }
        if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY))
        {
            motor_status = (received_data.servo_and_motor_state >> 4) & 0x0F;
            xSemaphoreGive(motor_status_mutex);
        }
        if (xSemaphoreTake(motor_speeds_settings_mutex, portMAX_DELAY))
        {
            left_motor_speed_mult = (received_data.motor_speed_setting >> 4) & 0x0F;
            right_motor_speed_mult = received_data.motor_speed_setting & 0x0F;
            xSemaphoreGive(motor_speeds_settings_mutex);
        }
    }
    else
    {
        return;
    }
}

//call back function for when data is sent
void on_send(const uint8_t *mac_addr, esp_now_send_status_t status) {
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
