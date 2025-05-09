/**
 * @file servo.cpp
 * @brief This is the source file for the servo motor class
 * @author Shane Wood
 * @date 14/12/2024
*/

#include "../../include/drivers/servo.h"


servo::servo(ledc_channel_t ledc_timer, uint16_t pwm_low_limit, uint16_t pwm_high_limit, bool direction, int8_t offset){
    this->ledc_timer = ledc_timer;

    this->pwm_low_limit = pwm_low_limit;
    this->pwm_high_limit = pwm_high_limit;
    
    this->direction = direction;
    this->offset = offset;
}
  


void servo::setDegree(uint16_t target_angle){
    if(target_angle > max_servo_angle){
        target_angle = max_servo_angle;
    }
    uint16_t pulse_width = 0;
    if(direction){
        pulse_width = pwm_low_limit + (target_angle * (pwm_high_limit-pwm_low_limit)/(float)max_servo_angle) + offset;
    }else{
        pulse_width = pwm_high_limit - (target_angle * (pwm_high_limit-pwm_low_limit)/(float)max_servo_angle) + offset;
    }
    //printf("%d\n", pulse_width);
    uint32_t duty = (pulse_width * (1 << LEDC_TIMER_12_BIT)) / servo_period_time; //20,000 us is the period
    (void)ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_timer, duty);
    (void)ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_timer);
}