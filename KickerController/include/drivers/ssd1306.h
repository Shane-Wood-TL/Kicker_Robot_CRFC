#ifndef __ssd1306__
#define __ssd1306__

#include "../supporting/font.h"
#include "../all_includes.h"




class ssd1306{
private:
    font font_instance; ///< instance of the font class
    uint8_t buffer[128 * 64 / 8] = {0}; ///< SSD1306 display buffer

    /**
     * @brief Sends a command to the SSD1306
     * 
     * @param command : uint8_t The command to send
     * 
     * @return esp_err_t ESP_OK if successful, ESP_FAIL otherwise
     */
    esp_err_t send_command_SSD1306(uint8_t command);

    /**
     * @brief Sends data to the SSD1306
     * 
     * @param data : uint8_t* The data to send
     * @param len : size_t The length of the data
     * 
     * @return esp_err_t ESP_OK if successful, ESP_FAIL otherwise
     */
    esp_err_t send_data_SSD1306(uint8_t *data, size_t len);

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

        /**
         * @brief Draws a the set page with columnStart and columnStop pixels
         * 
         * @param page : const uint8_t The page to write to
         * @param column_start : const uint8_t The start column pixel
         * @param column_stop : const uint8_t The stop column pixel
         */
        void write_section(const uint8_t page,const uint8_t column_start, const uint8_t column_stop);
};
#endif