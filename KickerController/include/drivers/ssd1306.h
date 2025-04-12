/**
 * @file ssd1306.h
 * @brief Contains the code for ssd1306 display, setting it up, and writing text to it
 * @author Shane Wood
 * @date 10/2/2025
 */

#ifndef __ssd1306__
#define __ssd1306__

#include "../all_includes.h"
#include "../pinout.h"
#include "../supporting/font.h"

#define SSD1306_DISPLAY_OFF 0xAE ///< command to turn off the display
#define SSD1306_DISPLAY_ON 0xAF ///< command to turn on the display
#define SSD1306_SET_MUX_RATIO 0xA8 ///< command to set the mux ratio
#define SSD1306_MUX_128x32 0x1F ///< mux value for 128x32 display
#define SSD1306_MUX_128x64 0x3F ///< mux value for 128x64 display
#define SSD1306_SET_DISPLAY_OFFSET 0xD3 ///< command to set the display offset
#define SSD1306_DISPLAY_OFFSET 0x00 ///< display offset value
#define SSD1306_SET_START_LINE 0x40 ///< command to set the start line
#define SSD1306_SET_MEMORY_MODE 0x20 ///< command to set the memory mode
#define SSD1306_MEMORY_HORIZONTAL 0x00 ///< memory mode value for horizontal
#define SSD1306_SET_SEGMENT_REMAP 0xA1 ///< command to set the segment remap
#define SSD1306_SET_COM_SCAN_DIR 0xC8 ///< command to set the com scan direction
#define SSD1306_SET_COM_PINS 0xDA ///< command to set the com pins
#define SSD1306_COM_PINS_128x32 0x02 ///< com pins value for 128x32 display
#define SSD1306_COM_PINS_128x64 0x12 ///< com pins value for 128x64 display
#define SSD1306_SET_CONTRAST 0x81 ///< command to set the contrast
#define SSD1306_CONTRAST_DEFAULT 0x9F ///< default contrast value
#define SSD1306_ENTIRE_DISPLAY_ON 0xA4 ///< command to turn on the entire display
#define SSD1306_SET_NORMAL_DISPLAY 0xA6 ///< command to set the display to normal
#define SSD1306_SET_OSC_FREQ 0xD5 ///< command to set the osc frequency
#define SSD1306_OSC_FREQ_DEFAULT 0x80 ///< default osc frequency value
#define SSD1306_ENABLE_CHARGE_PUMP 0x8D ///< command to enable the charge pump
#define SSD1306_CHARGE_PUMP_ON 0x14 ///< charge pump on value

#define SSD1306_BUFFER_SIZE_128x64 1056 ///< buffer size for 128x64 display (technically this is for a 132 x 64 display, purchased displays were not 128x64 despite claiming to be)
#define SSD1306_BUFFER_SIZE_128x32 512 ///< buffer size for 128x32 display

#define SSD1306_COMMAND_PREFIX 0x00 ///< prefix for a command
#define SSD1306_COMMAND_SIZE 2 ///< size of a command
#define SSD1306_DATA_PREFIX 0x40 ///< prefix for data
#define SSD1306_I2C_TIMEOUT 100 ///< i2c timeout

#define SSD1306_PAGE_OFFSET 0xB0 ///< page offset command

#define SSD1306_LOWER_COLUMN_START 0x00 ///< lower column start command
#define SSD1306_UPPER_COLUMN_START 0x10 ///< upper column start command

/**
 * @class ssd1306
 * @brief This class is for interfacing with the ssd1306 display
 * @details This class is for interfacing with the ssd1306 display, it can write text to the display data to a buffer and the buffer can be written to the display
 * @ref https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
 */
class ssd1306 {
private:
  font font_instance; ///< instance of the font class
  uint8_t buffer[SSD1306_BUFFER_SIZE_128x64] = {0}; ///< SSD1306 display buffer

  /**
    * @brief Sends a command to the SSD1306
    * 
    * @param command : uint8_t The command to send
    */
  void send_command_SSD1306(uint8_t command);

  /**
    * @brief Sends data to the SSD1306
    * 
    * @param data : uint8_t* The data to send
    * @param len : size_t The length of the data
    */
  void send_data_SSD1306(uint8_t *data, size_t len);

  /**
    * @brief Initializes the SSD1306
    */
  void init_SSD1306();

  /**
    * Clears the display display
    */
  void clear_SSD1306();

  /**
    * @brief Draws a pixel on the display buffer
    * 
    * @param x : uint8_t The x coordinate of the pixel
    * @param y : uint8_t The y coordinate of the pixel
    */
  void draw_pixel_SSD1306(uint8_t x, uint8_t y);


  /**
    * @brief Writes a letter to the display buffer
    * 
    * @param letter : uint8_t The letter to write
    * @param x_pos : uint8_t The x position to write the letter
    * @param y_pos : uint8_t The y position to write the letter
    */
  void write_letter_SSD1306(uint8_t letter, uint8_t x_pos, uint8_t y_pos);


  i2c_master_dev_handle_t ssd1306_handle; ///< I2C handle for the SSD1306

public:
  /**
    * @brief Constructor for the SSD1306 class
    * 
    * @param ssd1306_handle : i2c_master_dev_handle_t The handle for the SSD1306
    */
  ssd1306(i2c_master_dev_handle_t ssd1306_handle);

  /**
    * @brief Writes a string to the display
    * 
    * @param word : std::string The string to write
    * @param x_pos : uint8_t The x position to write the string
    * @param y_pos : uint8_t The y position to write the string
    */
  void write_string_SSD1306(std::string word, uint8_t x_pos, uint8_t y_pos);


  /**
    * @brief Clears the display buffer
    */
  void clear_buffer();

  /**
    * @brief Writes the buffer to the display
    */
  void write_buffer_SSD1306();
};

/*

```plantuml
@startuml
skinparam classAttributeIconSize 0
hide circle
class ssd1306 {
    - font_instance : font
    - buffer : uint8_t[SSD1306_BUFFER_SIZE_128x32]
    - ssd1306_handle : i2c_master_dev_handle_t

    - send_command_SSD1306(command : uint8_t) : void
    - send_data_SSD1306(data : uint8_t*, len : size_t) : void
    - init_SSD1306() : void
    - clear_SSD1306() : void
    - draw_pixel_SSD1306(x : uint8_t, y : uint8_t) : void
    - write_letter_SSD1306(letter : uint8_t, x_pos : uint8_t, y_pos : uint8_t) : void

    + ssd1306(ssd1306_handle : i2c_master_dev_handle_t)
    + write_string_SSD1306(word : std::string, x_pos : uint8_t, y_pos : uint8_t) : void
    + clear_buffer() : void
    + write_buffer_SSD1306() : void
}
@enduml
```

*/

#endif