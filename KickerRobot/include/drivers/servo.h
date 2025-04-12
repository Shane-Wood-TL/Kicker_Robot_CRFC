/**
 * @file servo.h
 * @brief This is the header for the servo motor class
 * @author Shane Wood
 * @date 14/12/2024
*/
#ifndef __servo__
#define __servo__

#define max_servo_angle 270 ///< the maximum angle of the servo
#define servo_period_time 20000 ///< period of a servo in us

#include "../all_includes.h"
/**
 * @class servo
 * @brief This is a class for controlling a pwm based servo motor
 */
class servo{
  private:
    ledc_channel_t ledc_timer; ///< the timer channel for the servo to use
    int16_t pwm_low_limit; ///< the upper limit of the servos pulse time
    int16_t pwm_high_limit; ///< the lower limit of the servos pulse time
    bool direction; ///< the direction that the servo should move
    int8_t offset; ///< the angle offset if desired horn position cannot be achieved
  public:

  /**
   * @brief The constructor of the servo class
   * 
   * @param ledc_timer : ledc_channel_t the timer channel for the servo to use
   * @param pwm_low_limit : uint16_t the lower on time limit of the servos 
   * @param pwm_high_limit : uint16_t the upper on time limit of the servos
   * @param direction : bool allows for the servo to be reversed
   * @param offset : int8_t angle to offset the servo
   * 
   */
  servo(ledc_channel_t ledc_timer, uint16_t pwm_low_limit, uint16_t pwm_high_limit, bool direction, int8_t offset);

  /**
   * @brief set the angle of the servo motor
   * 
   * @param new_degree : uint16_t the angle to set the servo motor
   */
  void setDegree(uint16_t new_degree);
};


/*
```plantuml
@startuml
skinparam classAttributeIconSize 0
hide circle

class servo {
    - ledc_timer : ledc_channel_t
    - pwm_low_limit : int16_t
    - pwm_high_limit : int16_t
    - direction : bool
    - offset : int8_t
    + servo(ledc_timer : ledc_channel_t, pwm_low_limit : uint16_t, pwm_high_limit : uint16_t, direction : bool, offset : int8_t)
    + setDegree(new_degree : uint16_t) : void
}

@enduml
```
*/
#endif