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
#define SSD1306HorizontalRes 128 ///< Horizontal resolution of display, 128 pixels across

//see SSD1306 datasheet for more information
#define SSD1306Pages SSD1306VerticalRes/8 ///< each page is a vertical 8 bits and goes across the entire display


const uint8_t robotMacAddress[6] = {0xD8,0x3B,0xDA,0xA1,0x06,0x30};

#endif