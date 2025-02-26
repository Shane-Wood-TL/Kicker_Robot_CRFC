#include "../../include/tasks/odrive_driver_task.h"

void oDriveDriver(void *pv)
{
    static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_100KBITS();
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    static const twai_general_config_t g_config = {
        .mode = TWAI_MODE_NORMAL,
        .tx_io = canTXPin,
        .rx_io = canRXPin,
        .clkout_io = TWAI_IO_UNUSED,
        .bus_off_io = TWAI_IO_UNUSED,
        .tx_queue_len = 1,
        .rx_queue_len = 1,
        .alerts_enabled = TWAI_ALERT_NONE,
        .clkout_divider = 0};

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK)
    {
        assert(false);
    }

    while (twai_start() != ESP_OK)
    {
        assert(false);
    }

    oDrive rightDrive(oDrive0ID);
    oDrive leftDrive(oDrive1ID);

    static uint8_t last_motor_state = DISABLED;

    // value from the controller menu settings
    static uint8_t last_left_motor_speed_mult = 0;
    static uint8_t last_right_motor_speed_mult = 0;

    // last value read from the controller joystick
    static uint8_t last_left_motor_speed = 0;
    static uint8_t last_right_motor_speed = 0;

    // value written to the motor
    float current_left_motor_speed = 0;
    float current_right_motor_speed = 0;

    int64_t last_time = esp_timer_get_time();
    int32_t batterry_update_time = esp_timer_get_time() - 5000000;
    for (;;)
    {
        if (esp_timer_get_time() - last_time > batterry_update_time)
        {
            last_time = esp_timer_get_time();
            leftDrive.send_message(Get_Bus_Voltage_Current, NULL, 0, true);
        }

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
                        leftDrive.send_message(Set_Axis_State, &CLOSED_LOOP_CONTROL, 1, true);
                        rightDrive.send_message(Set_Axis_State, &CLOSED_LOOP_CONTROL, 1, true);
                    }
                    break;
                }
                case (DISABLED):
                {
                    if (last_motor_state != DISABLED)
                    {
                        leftDrive.send_message(Set_Axis_State, &IDLE_STATE, 1, true);
                        rightDrive.send_message(Set_Axis_State, &IDLE_STATE, 1, true);
                    }
                    break;
                }
                case (ERRORLESS):
                {
                    if (last_motor_state != ERRORLESS)
                    {
                        leftDrive.send_message(Clear_Errors, &NO_DATA, 1, true);
                        rightDrive.send_message(Clear_Errors, &NO_DATA, 1, true);
                    }
                    break;
                }
                case (CALIBRATING):
                {
                    if (last_motor_state != CALIBRATING)
                    {
                        leftDrive.send_message(Set_Axis_State, &FULL_CALIBRATION_SEQUENCE, 1, true);
                        rightDrive.send_message(Set_Axis_State, &FULL_CALIBRATION_SEQUENCE, 1, true);

                        const uint32_t delay_ms = 100;
                        const uint32_t total_delay_ms = 16000;
                        // prevent watchdog timeout
                        for (uint32_t elapsed = 0; elapsed < total_delay_ms; elapsed += delay_ms)
                        {
                            leftDrive.send_message(Get_Error, NULL, 0, true);
                            rightDrive.send_message(Get_Error, NULL, 0, true);
                            vTaskDelay(pdMS_TO_TICKS(delay_ms));
                        }
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
        if (xSemaphoreTake(motor_speeds_settings_mutex, portMAX_DELAY))
        {
            last_right_motor_speed_mult = right_motor_speed_mult;
            last_left_motor_speed_mult = left_motor_speed_mult;
            xSemaphoreGive(motor_speeds_settings_mutex);
        }
        if (xSemaphoreTake(motor_speeds, portMAX_DELAY))
        {
            last_left_motor_speed = left_motor_speed;
            last_right_motor_speed = right_motor_speed;
            xSemaphoreGive(motor_speeds);
        }

        if (xSemaphoreTake(ramped_values_updated, portMAX_DELAY))
        {
            if (ramped_values_updated_bool == true)
            {
                ramped_values_updated_bool = false;
                xSemaphoreGive(ramped_values_updated);
                if (xSemaphoreTake(ramped_mutex, portMAX_DELAY))
                {
                    // https://odrive-cdn.nyc3.digitaloceanspaces.com/releases/firmware/g7yj6u0Fugoi13Icdm_B6EDZTJlWAGS6hzh3qWvzm7E/flat_endpoints.json
                    uint8_t ramped_rate[8] = {0};
                    uint16_t vel_ramp_rate_id = 377;
                    ramped_rate[0] = 0x01; // write

                    memcpy(&ramped_rate[1], &vel_ramp_rate_id, sizeof(uint16_t)); // id

                    memcpy(&ramped_rate[4], &velocity_ramp_limit, sizeof(float));
                    leftDrive.send_message(RxSdo, ramped_rate, 8, false);
                    rightDrive.send_message(RxSdo, ramped_rate, 8, false);

                    uint8_t ramped_values[8] = {0};
                    // uint8_t ramped_values[8] = {0};
                    memcpy(&ramped_values[0], &velocity_gain, sizeof(float));
                    memcpy(&ramped_values[4], &velocity_integrator_gain, sizeof(float));
                    leftDrive.send_message(Set_Vel_Gains, ramped_values, 8, false);
                    rightDrive.send_message(Set_Vel_Gains, ramped_values, 8, false);
                    xSemaphoreGive(ramped_mutex);
                }
            }
            else
            {
                xSemaphoreGive(ramped_values_updated);
            }
        }

        current_left_motor_speed = map(last_left_motor_speed, 0.0, 255.0, (-last_left_motor_speed_mult * 10.0), (last_left_motor_speed_mult * 10.0));
        current_right_motor_speed = map(last_right_motor_speed, 0.0, 255.0, (-last_right_motor_speed_mult * 10.0), (last_right_motor_speed_mult * 10.0));

        float Input_Torque_FF = 0.5f;
        uint8_t vel_left_as_int[8] = {0};
        uint8_t vel_right_as_int[8] = {0};

        if (abs(current_left_motor_speed) < (0.5 * last_left_motor_speed_mult))
        {
            current_left_motor_speed = 0;
        }
        if (abs(current_right_motor_speed) < (0.5 * last_right_motor_speed_mult))
        {
            current_right_motor_speed = 0;
        }
        memcpy(&vel_left_as_int[0], &current_left_motor_speed, sizeof(float));
        memcpy(&vel_left_as_int[4], &Input_Torque_FF, sizeof(float));

        memcpy(&vel_right_as_int[0], &current_right_motor_speed, sizeof(float));
        memcpy(&vel_right_as_int[4], &Input_Torque_FF, sizeof(float));
        if (xSemaphoreTake(controller_connected_mutex, portMAX_DELAY))
        {
            if (controller_connected == CONNECTED)
            {
                xSemaphoreGive(controller_connected_mutex);
                leftDrive.send_message(Set_Input_Vel, vel_left_as_int, 8, false);
                rightDrive.send_message(Set_Input_Vel, vel_right_as_int, 8, false);
            }
            else
            {
                xSemaphoreGive(controller_connected_mutex);
                leftDrive.send_message(Estop, &NO_DATA, 1, false);
                rightDrive.send_message(Estop, &NO_DATA, 1, false);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}