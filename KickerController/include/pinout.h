/**
 * @file pinout.h
 * @brief This is the file that contains the pinout and other configuration information
 * @author Shane Wood
 * @date 10/2/2025
*/

#ifndef __pinout__
#define __pinout__

#define sda_pin GPIO_NUM_4 ///< pin for sda
#define scl_pin GPIO_NUM_5 ///< pin for scl



//i2c setup
#define i2c_frequency 400000   // Frequency of I2C (400 kHz)
#define i2c_bus_number I2C_NUM_0    // I2C port number

//SSD1306 setup
#define SSD1306_address 0x3C ///< SSD1306 (display) I2C address
#define SSD1306_vertical_resolution 64 ///< vertical resolution of display, 64 pixels tall
#define SSD1306_horizontal_resolution 128 ///< Horizontal resolution of display, 128 pixels across

#define SSD1306_page_height 8 ///< each "page" is 8 pixels high, see SSD1306 datasheet for more information
#define SSD1306_pages (SSD1306_vertical_resolution/SSD1306_page_height) ///< each page is a vertical 8 bits and goes across the entire display



#define mac_address_length 6 ///< length of a mac address
const uint8_t robotMacAddress[mac_address_length] = {0xD8,0x3B,0xDA,0xA1,0x06,0x30}; ///< mac address of the robot


#define minimum_battery_reading 0.0f ///< minimum battery reading
#define maximum_battery_reading 30.0f ///< maximum battery reading


#endif