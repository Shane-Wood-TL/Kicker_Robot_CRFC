#ifndef __pinout__
#define __pinout__


#include "all_includes.h"

#define sda_pin GPIO_NUM_4 //i2c SDA pin
#define scl_pin GPIO_NUM_5 //i2c scl pin

#define servo_A_pin GPIO_NUM_6 ///< GPIO pin for the servo A
#define servo_B_pin GPIO_NUM_7 ///< GPIO pin for the servo B

#define led_A_pin GPIO_NUM_15
#define led_B_pin GPIO_NUM_16

#define tackle_TX_pin GPIO_NUM_17
#define tackle_RX_pin GPIO_NUM_18
#define tackle_eligible_pin GPIO_NUM_8
#define tackle_home_pin GPIO_NUM_3

#define C0_4_pin GPIO_NUM_11
#define C0_5_pin GPIO_NUM_12
#define C0_6_pin GPIO_NUM_13
#define C0_7_pin GPIO_NUM_14
#define C0_8_pin GPIO_NUM_1
#define C0_9_pin GPIO_NUM_2
#define C0_10_pin GPIO_NUM_42

#define C1_4_pin GPIO_NUM_41
#define C1_5_pin GPIO_NUM_40
#define C1_6_pin GPIO_NUM_39
#define C1_7_pin GPIO_NUM_38
#define C1_8_pin GPIO_NUM_37
#define C1_9_pin GPIO_NUM_36
#define C1_10_pin GPIO_NUM_35

#define can_TX_pin GPIO_NUM_48
#define can_RX_pin GPIO_NUM_47


//i2c setup
#define i2c_frequency 400000   // Frequency of I2C (400 kHz)
#define i2c_bus_number I2C_NUM_0    // I2C port number


//SSD1306 setup
#define SSD1306_address 0x3C ///< SSD1306 (display) I2C address
#define SSD1306_vertical_resolution 32 ///< vertical resolution of display, 64 pixels tall
#define SSD1306_horizontal_resolution 128 ///< Horizontal resolution of display, 128 pixels across

#define SSD1306_page_height 8
//see SSD1306 datasheet for more information
#define SSD1306_page_count (SSD1306_vertical_resolution/SSD1306_page_height) ///< each page is a vertical 8 bits and goes across the entire display


#define font_width 12
#define font_height 16



#define servo_latched_angle 270
#define servo_released_angle 0
#define servo_detached_angle 135
#define servo_frequency 50
#define servo_max_pwm 2000
#define servo_min_pwm 1000


#define mac_address_length 6
const uint8_t controller_mac_address[mac_address_length] = {0xF0,0x9E,0x9E,0x12,0xA8,0xA4};


#define oDrive_0_ID 2
#define oDrive_1_ID 3

#define number_of_leds 16
#define led_color_blue 0,0,255

#define values_8 8
#define values_7 7
#define values_6 6
#define values_5 5
#define values_4 4
#define values_3 3
#define values_2 2
#define values_1 1
#define values_0 0

#define lower_four_bits 0x0F
#endif