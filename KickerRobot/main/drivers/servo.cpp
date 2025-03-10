
#include "../../include/drivers/servo.h"


servo::servo(ledc_channel_t ledc_timer, uint16_t pwm_low_limit, uint16_t pwm_high_limit, bool direction, int8_t offset){
    this->ledc_timer = ledc_timer;

    this->pwm_low_limit = pwm_low_limit;
    this->pwm_high_limit = pwm_high_limit;
    
    this->direction = direction;
    this->offset = offset;
}
  
void servo::setDegree(uint16_t nDegree){
    if(nDegree > max_servo_angle){
        nDegree = max_servo_angle;
    }
    uint16_t pulseWidth = 0;
    if(direction){
        pulseWidth = pwm_low_limit + (nDegree * (pwm_high_limit-pwm_low_limit)/max_servo_angle) + offset;
    }else{
        pulseWidth = pwm_high_limit - (nDegree * (pwm_high_limit-pwm_low_limit)/max_servo_angle) + offset;
    }

    uint32_t duty = (pulseWidth * (1 << LEDC_TIMER_12_BIT)) / servo_period_time; //20,000 us is the period
    (void)ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_timer, duty);
    (void)ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_timer);
}