
#include "../../include/drivers/servo.h"

servo::servo(ledc_channel_t ledCTimerV, uint16_t pwmHighLimitV, uint16_t pwmLowLimitV, bool directionV,int8_t offsetV){
    ledCTimer = ledCTimerV;
    Hlimit = pwmHighLimitV;
    Llimit = pwmLowLimitV;
    direction = directionV;
    offset = offsetV;
}
  
void servo::setDegree(uint16_t nDegree){
    if(nDegree > 270){
        nDegree = 270;
    }

    uint16_t pulseWidth = 0;
    if(direction){
        pulseWidth = Llimit + (nDegree * (Hlimit-Llimit)/270) + offset;
    }else{
        pulseWidth = Hlimit - (nDegree * (Hlimit-Llimit)/270) + offset;
    }

    uint32_t duty = (pulseWidth * (1 << LEDC_TIMER_12_BIT)) / 20000; //20,000 us is the period
    ledc_set_duty(LEDC_LOW_SPEED_MODE, ledCTimer, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, ledCTimer);
}