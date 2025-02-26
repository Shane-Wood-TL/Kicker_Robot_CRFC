#ifndef __pinout__
#define __pinout__


#include "all_includes.h"

#define sdaPin GPIO_NUM_4
#define sclPin GPIO_NUM_5

#define servoAPin GPIO_NUM_6
#define servoBPin GPIO_NUM_7

#define led_A_pin GPIO_NUM_15
#define led_B_pin GPIO_NUM_16

#define tackleTXPin GPIO_NUM_17
#define tackleRXPin GPIO_NUM_18
#define tackleEligiblePin GPIO_NUM_8
#define tackleHomePin GPIO_NUM_3

#define C0_4Pin GPIO_NUM_11
#define C0_5Pin GPIO_NUM_12
#define C0_6Pin GPIO_NUM_13
#define C0_7Pin GPIO_NUM_14
#define C0_8Pin GPIO_NUM_1
#define C0_9Pin GPIO_NUM_2
#define C0_10Pin GPIO_NUM_42

#define C1_4Pin GPIO_NUM_41
#define C1_5Pin GPIO_NUM_40
#define C1_6Pin GPIO_NUM_39
#define C1_7Pin GPIO_NUM_38
#define C1_8Pin GPIO_NUM_37
#define C1_9Pin GPIO_NUM_36
#define C1_10Pin GPIO_NUM_35

#define canTXPin GPIO_NUM_48
#define canRXPin GPIO_NUM_47


//i2c setup
#define i2cFreq 400000   // Frequency of I2C (400 kHz)
#define i2cBusNumber I2C_NUM_0    // I2C port number


//SSD1306 setup
#define SSD1306Address 0x3C           // SSD1306 I2C address
#define SSD1306VerticalRes 32           //32 pixels tall
#define SSD1306HorizontalRes 128    //128 pixels across
#define SSD1306Pages SSD1306VerticalRes/8 //each page is a vertical 8 bits



#define servo_latched_angle 270
#define servo_released_angle 0
#define servo_detached_angle 135
#define servo_frequency 50
#define servo_max_pwm 2000
#define servo_min_pwm 1000


const uint8_t controller_mac_address[6] = {0xF0,0x9E,0x9E,0x12,0xA8,0xA4};


#define oDrive0ID 2
#define oDrive1ID 3

#define number_of_leds 16
#define led_color_blue 0,0,255

#endif