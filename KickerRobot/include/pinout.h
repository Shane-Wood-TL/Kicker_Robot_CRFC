/**
 * @file pinout.h
 * @brief This is the file that contains the pinout and other configuration information
 * @author Shane Wood
 * @date 10/2/2025
*/

#ifndef __pinout__
#define __pinout__


#include "all_includes.h"

#define sda_pin GPIO_NUM_4 //i2c SDA pin
#define scl_pin GPIO_NUM_5 //i2c scl pin

#define servo_A_pin GPIO_NUM_6 ///< GPIO pin for the servo A
#define servo_B_pin GPIO_NUM_7 ///< GPIO pin for the servo B

#define led_A_pin GPIO_NUM_15 ///<qio pin for the led A
#define led_B_pin GPIO_NUM_16 ///<qio pin for the led B

#define tackle_TX_pin GPIO_NUM_17 ///< GPIO pin for the tackle TX
#define tackle_RX_pin GPIO_NUM_18 ///< GPIO pin for the tackle RX
#define tackle_eligible_pin GPIO_NUM_8 ///< GPIO pin for the tackle eligible
#define tackle_home_pin GPIO_NUM_3 ///< GPIO pin for the tackle home

#define C0_4_pin GPIO_NUM_11 ///< GPIO pin for the C0_4
#define C0_5_pin GPIO_NUM_12 ///< GPIO pin for the C0_5
#define C0_6_pin GPIO_NUM_13 ///< GPIO pin for the C0_6
#define C0_7_pin GPIO_NUM_14 ///< GPIO pin for the C0_7
#define C0_8_pin GPIO_NUM_1 ///< GPIO pin for the C0_8
#define C0_9_pin GPIO_NUM_2 ///< GPIO pin for the C0_9
#define C0_10_pin GPIO_NUM_42 ///< GPIO pin for the C0_10

#define C1_4_pin GPIO_NUM_41 ///< GPIO pin for the C1_4
#define C1_5_pin GPIO_NUM_40 ///< GPIO pin for the C1_5
#define C1_6_pin GPIO_NUM_39 ///< GPIO pin for the C1_6
#define C1_7_pin GPIO_NUM_38 ///< GPIO pin for the C1_7
#define C1_8_pin GPIO_NUM_37 ///< GPIO pin for the C1_8
#define C1_9_pin GPIO_NUM_36 ///< GPIO pin for the C1_9
#define C1_10_pin GPIO_NUM_35 ///< GPIO pin for the C1_10

#define can_TX_pin GPIO_NUM_48 ///< GPIO pin for can tx
#define can_RX_pin GPIO_NUM_47 ///< GPIO pin for can rx


//i2c setup
#define i2c_frequency 400000   ///< Frequency of I2C, 400 khz
#define i2c_bus_number I2C_NUM_0    ///< I2C port number


//SSD1306 setup
#define SSD1306_address 0x3C ///< SSD1306 (display) I2C address
#define SSD1306_vertical_resolution 32 ///< vertical resolution of display, 64 pixels tall
#define SSD1306_horizontal_resolution 128 ///< Horizontal resolution of display, 128 pixels across

#define SSD1306_page_height 8 ///< see SSD1306 datasheet for more information
#define SSD1306_page_count (SSD1306_vertical_resolution/SSD1306_page_height) ///< each page is a vertical 8 bits and goes across the entire display


#define font_width 12 ///< height of the font in pixels
#define font_height 16 ///< width of the font in pixels



#define servo_latched_angle 0 ///< servo latched angle
#define servo_released_angle 140 ///< servo released angle
#define servo_detached_angle 135 ///< servo detached angle
#define servo_frequency 50 ///< servo frequency
#define servo_max_pwm 2500 ///< max on time for servo
#define servo_min_pwm 1000 ///< min on time for servo


#define mac_address_length 6 ///< how long the esp32's mac address is
const uint8_t controller_mac_address[mac_address_length] = {0xF0,0x9E,0x9E,0x12,0xA8,0xA4}; ///< mac address for the kicker mainboard


#define oDrive_0_ID 2 ///< id of odrive 0
#define oDrive_1_ID 3 ///< id of odrive 1

#define number_of_leds 16 ///< number of leds in a strip
#define led_color_blue 0,0,255 ///< team color as r,g,b

#define values_8 8 ///< value for 8
#define values_7 7 ///< value for 7
#define values_6 6 ///< value for 6
#define values_5 5 ///< value for 5
#define values_4 4 ///< value for 4
#define values_3 3 ///< value for 3
#define values_2 2 ///< value for 2
#define values_1 1 ///< value for 1
#define values_0 0 ///< value for 0

#define lower_four_bits 0x0F ///< mask to keep the lower 4 bits
#endif