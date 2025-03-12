/**
 * @file servo_driver_task.cpp
 * @brief source file for the task to control the servos
 * @author Shane Wood
 * @date 15/02/2025
 */
#include "../../include/tasks/servo_driver_task.h"



void servo_driver_task(void *pv){
    ledc_channel_config_t servo_A_channel = {
        .gpio_num = servo_A_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .flags ={
            .output_invert = 0
        }
    };

    ledc_timer_config_t servo_A_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num  = LEDC_TIMER_0,
        .freq_hz    = servo_frequency,
        .clk_cfg = LEDC_USE_APB_CLK,
        .deconfigure = false              
    };

    ledc_channel_config_t servo_B_channel = {
        .gpio_num = servo_B_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_1,
        .duty = 0,
        .hpoint = 0,
        .flags ={
            .output_invert = 0
        }
    };

    ledc_timer_config_t servo_B_timer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_12_BIT,
    .timer_num  = LEDC_TIMER_1,
    .freq_hz    = servo_frequency,
    .clk_cfg = LEDC_USE_APB_CLK,
    .deconfigure = false             
    };

    ledc_timer_config(&servo_A_timer);
    ledc_channel_config(&servo_A_channel);

    ledc_timer_config(&servo_B_timer);
    ledc_channel_config(&servo_B_channel);

    servo servo_motor_A(LEDC_CHANNEL_0,(uint16_t)servo_min_pwm,(uint16_t)servo_max_pwm,(bool)true,(int8_t)0);
    servo servo_motor_B(LEDC_CHANNEL_1,(uint16_t)servo_min_pwm,(uint16_t)servo_max_pwm,(bool)false,(int8_t)0);

    for(;;){
        if(xSemaphoreTake(servo_status_mutex, portMAX_DELAY)){
            switch (servo_status)
            {
            case (LATCHED):{
                servo_motor_A.setDegree(servo_latched_angle);
                servo_motor_B.setDegree(servo_latched_angle);
                break;
            }
            case (RELEASED):{
                servo_motor_A.setDegree(servo_released_angle);
                servo_motor_B.setDegree(servo_released_angle);
                break;
            }
            case (DETACHED):{
                servo_motor_A.setDegree(servo_detached_angle);
                servo_motor_B.setDegree(servo_detached_angle);
                break;
            }
            }
        xSemaphoreGive(servo_status_mutex);
        }
    vTaskDelay(pdMS_TO_TICKS(servo_driver_wait_time));
    }
}