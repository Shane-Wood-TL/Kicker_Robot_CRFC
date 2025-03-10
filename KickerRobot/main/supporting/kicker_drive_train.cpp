#include "../../include/supporting/kicker_drive_train.h"

kicker_drive_train::kicker_drive_train(oDrive *left_drive, oDrive *right_drive, uint32_t battery_voltage_refresh){
    this->left_drive = left_drive;
    this->right_drive = right_drive;
    last_time = esp_timer_get_time()-battery_voltage_refresh; //ensures value is read initially
    battery_update_time = battery_voltage_refresh;
    last_motor_state = DISABLED;

    last_left_motor_speed_mult = 0;
    last_right_motor_speed_mult = 0;

    last_left_motor_speed = 0;
    last_right_motor_speed = 0;

}

void kicker_drive_train::update(){
    periodic_voltage_reading();
    motor_status_updater();
    motor_speed_multiplier_updater();
    ramped_settings_updater();
    motor_speeds_updater();

    if (xSemaphoreTake(controller_connected_mutex, portMAX_DELAY))
    {
        if (controller_connected == CONNECTED)
        {
            xSemaphoreGive(controller_connected_mutex);
            drive_motors();
        }
        else
        {
            xSemaphoreGive(controller_connected_mutex);
            estop();
        }
    }

}

void kicker_drive_train::periodic_voltage_reading(){
    if ((esp_timer_get_time() - last_time) > battery_update_time)
    {
        last_time = esp_timer_get_time();
        left_drive->send_message(Get_Bus_Voltage_Current, nullptr, 0, true);
    }
}

void kicker_drive_train::motor_status_updater()
{
    if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY))
    {
        if (motor_status != last_motor_state)
        {
            uint8_t current_motor_state = motor_status;
            xSemaphoreGive(motor_status_mutex);
            switch (current_motor_state)
            {
                case (ENABLED):
                {
                    if (last_motor_state != ENABLED)
                    {
                        enable_motors(); 
                    }
                    break;
                }
                case (DISABLED):
                {
                    if (last_motor_state != DISABLED)
                    {
                        disable_motors();
                    }
                    break;
                }
                case (ERRORLESS):
                {
                    if (last_motor_state != ERRORLESS)
                    {
                        clear_errors();
                    }
                    break;
                }
                case (CALIBRATING):
                {
                    if (last_motor_state != CALIBRATING)
                    {
                        calibrating();
                        disable_motors();
                    }
                    break;
                }
            }
            last_motor_state = current_motor_state;
        }
        else
        {
            xSemaphoreGive(motor_status_mutex);
        }
    }
}

void kicker_drive_train::calibrating()
{
    left_drive->send_message(Set_Axis_State, &FULL_CALIBRATION_SEQUENCE, one_byte, false);
    right_drive->send_message(Set_Axis_State, &FULL_CALIBRATION_SEQUENCE, one_byte, false);
    static uint32_t delay_ms = 100;
    static uint32_t total_delay_ms = 16000;
    // prevent watchdog timeout
    for (uint32_t elapsed = 0; elapsed < total_delay_ms; elapsed += delay_ms)
    {
        left_drive->send_message(Get_Error, NULL, zero_bytes, true);
        right_drive->send_message(Get_Error, NULL, zero_bytes, true);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

void kicker_drive_train::clear_errors(){
    left_drive->send_message(Clear_Errors, &NO_DATA, one_byte, false);
    right_drive->send_message(Clear_Errors, &NO_DATA, one_byte, false);
}

void kicker_drive_train::disable_motors(){
    left_drive->send_message(Set_Axis_State, &IDLE_STATE, one_byte, false);
    right_drive->send_message(Set_Axis_State, &IDLE_STATE, one_byte, false);
}

void kicker_drive_train::enable_motors(){
    left_drive->send_message(Set_Axis_State, &CLOSED_LOOP_CONTROL, one_byte, false);
    right_drive->send_message(Set_Axis_State, &CLOSED_LOOP_CONTROL, one_byte, false);
}

void kicker_drive_train::motor_speed_multiplier_updater(){
    if (xSemaphoreTake(motor_speeds_settings_mutex, portMAX_DELAY))
    {
        last_right_motor_speed_mult = right_motor_speed_mult;
        last_left_motor_speed_mult = left_motor_speed_mult;
        xSemaphoreGive(motor_speeds_settings_mutex);
    }
}

void kicker_drive_train::motor_speeds_updater(){
    if (xSemaphoreTake(motor_speeds, portMAX_DELAY))
    {
        last_left_motor_speed = left_motor_speed;
        last_right_motor_speed = right_motor_speed;
        xSemaphoreGive(motor_speeds);
    }
}

void kicker_drive_train::ramped_settings_updater()
{
    if (xSemaphoreTake(ramped_values_updated, portMAX_DELAY))
    {
        if (ramped_values_updated_bool == true)
        {
            ramped_values_updated_bool = false;
            xSemaphoreGive(ramped_values_updated);
            if (xSemaphoreTake(ramped_mutex, portMAX_DELAY))
            {
                // https://odrive-cdn.nyc3.digitaloceanspaces.com/releases/firmware/g7yj6u0Fugoi13Icdm_B6EDZTJlWAGS6hzh3qWvzm7E/flat_endpoints.json
                static uint8_t ramped_rate[eight_bytes] = {0};
                static uint16_t vel_ramp_rate_id_var = vel_ramp_rate_id;
                ramped_rate[0] = odrive_write_rx; // write

                memcpy(&ramped_rate[1], &vel_ramp_rate_id_var, sizeof(uint16_t)); // id

                memcpy(&ramped_rate[full_message_middle_index], &velocity_ramp_limit, sizeof(float));
                left_drive->send_message(RxSdo, ramped_rate, eight_bytes, false);
                right_drive->send_message(RxSdo, ramped_rate, eight_bytes, false);

                uint8_t ramped_values[eight_bytes] = {0};
                memcpy(&ramped_values[0], &velocity_gain, sizeof(float));
                memcpy(&ramped_values[full_message_middle_index], &velocity_integrator_gain, sizeof(float));
                left_drive->send_message(Set_Vel_Gains, ramped_values, eight_bytes, false);
                right_drive->send_message(Set_Vel_Gains, ramped_values, eight_bytes, false);
                xSemaphoreGive(ramped_mutex);
            }
        }
        else
        {
            xSemaphoreGive(ramped_values_updated);
        }
    }
    
}

void kicker_drive_train::drive_motors(){
    float current_left_motor_speed = map(last_left_motor_speed, eight_bit_minimum, eight_bit_maximum, (-last_left_motor_speed_mult * motor_speed_multiplier_value), (last_left_motor_speed_mult * motor_speed_multiplier_value));
    float current_right_motor_speed = map(last_right_motor_speed, eight_bit_minimum, eight_bit_maximum, (-last_right_motor_speed_mult * motor_speed_multiplier_value), (last_right_motor_speed_mult * motor_speed_multiplier_value));

    float Input_Torque_FF = odrive_motor_torque;
    uint8_t vel_left_as_int[full_message_size] = {0};
    uint8_t vel_right_as_int[full_message_size] = {0};

    if (abs(current_left_motor_speed) < (input_velocity_deadzone * last_left_motor_speed_mult))
    {
        current_left_motor_speed = 0;
    }
    if (abs(current_right_motor_speed) < (input_velocity_deadzone * last_right_motor_speed_mult))
    {
        current_right_motor_speed = 0;
    }
    memcpy(&vel_left_as_int[full_message_start_index], &current_left_motor_speed, sizeof(float));
    memcpy(&vel_left_as_int[full_message_middle_index], &Input_Torque_FF, sizeof(float));

    memcpy(&vel_right_as_int[full_message_start_index], &current_right_motor_speed, sizeof(float));
    memcpy(&vel_right_as_int[full_message_middle_index], &Input_Torque_FF, sizeof(float));

    left_drive->send_message(Set_Input_Vel, vel_left_as_int, eight_bytes, false);
    right_drive->send_message(Set_Input_Vel, vel_right_as_int, eight_bytes, false);
}

void kicker_drive_train::estop(){
    left_drive->send_message(Estop, &NO_DATA, 1, false);
    right_drive->send_message(Estop, &NO_DATA, 1, false);
    //left_drive->send_message(Estop, nullptr, zero_bytes, false);
    //right_drive->send_message(Estop, nullptr, zero_bytes, false);
}