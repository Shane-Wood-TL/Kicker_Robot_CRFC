/**
 * @file pinout.h
 * @brief This is the file that contains the pinout and other configuration information
 * @author Shane Wood
 * @date 10/2/2025
*/

#ifndef __pinout__
#define __pinout__

#define sdaPin GPIO_NUM_4 ///< pin for sda
#define sclPin GPIO_NUM_5 ///< pin for scl



//i2c setup
#define i2cFreq 400000 ///< i2c bus frequency (400 kHz)
#define i2cBusNumber I2C_NUM_0 ///< i2c port number

//SSD1306 setup
#define SSD1306Address 0x3C ///< SSD1306 (display) I2C address
#define SSD1306VerticalRes 64 ///< vertical resolution of display, 64 pixels tall
#define SSD1306HorizontalRes 132 ///< Horizontal resolution of display, 128 pixels across

#define SSD1306_page_height 8
//see SSD1306 datasheet for more information
#define SSD1306Pages SSD1306VerticalRes/SSD1306_page_height ///< each page is a vertical 8 bits and goes across the entire display


#define font_width 12
#define font_height 16

#define processed_dpad_left_bit 3
#define processed_dpad_down_bit 2
#define processed_dpad_right_bit 1
#define processed_dpad_up_bit 0

#define processed_triangle_bit 4
#define processed_square_bit 5
#define processed_circle_bit 6
#define processed_x_bit 7

#define processed_options_bit 0
#define processed_r2_bit 1
#define processed_l2_bit 2

#define values_6 6
#define values_5 5
#define values_4 4
#define values_3 3
#define values_2 2
#define values_1 1

#define mac_address_length 6
const uint8_t robotMacAddress[mac_address_length] = {0xD8,0x3B,0xDA,0xA1,0x06,0x30};



#endif