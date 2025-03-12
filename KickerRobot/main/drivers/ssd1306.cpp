#include "../../include/drivers/ssd1306.h"
#include "../../include/pinout.h"

void ssd1306::send_command_SSD1306(uint8_t command)
{
    uint8_t data[SSD1306_COMMAND_SIZE] = {SSD1306_COMMAND_PREFIX, command}; // 0x00 is for command
    (void)i2c_master_transmit(ssd1306_handle, data, SSD1306_COMMAND_SIZE,SSD1306_I2C_TIMEOUT);
}

void ssd1306::send_data_SSD1306(uint8_t *data, size_t len)
{
    uint8_t data_to_send[len+1] = {0};
    data_to_send[0] = SSD1306_DATA_PREFIX;  // 0x40 is for data
    memcpy(&(data_to_send[1]), data, len);
    (void)i2c_master_transmit(ssd1306_handle, &data_to_send[0], (1+len), SSD1306_I2C_TIMEOUT);
}


void ssd1306::init_SSD1306()
{
    send_command_SSD1306(SSD1306_DISPLAY_OFF);
    send_command_SSD1306(SSD1306_SET_MUX_RATIO);
    send_command_SSD1306(SSD1306_MUX_128x32); // Change to SSD1306_MUX_128x64 for 128x64 displays
    send_command_SSD1306(SSD1306_SET_DISPLAY_OFFSET);
    send_command_SSD1306(SSD1306_DISPLAY_OFFSET);
    send_command_SSD1306(SSD1306_SET_START_LINE);
    send_command_SSD1306(SSD1306_SET_MEMORY_MODE);
    send_command_SSD1306(SSD1306_MEMORY_HORIZONTAL);
    send_command_SSD1306(SSD1306_SET_SEGMENT_REMAP);
    send_command_SSD1306(SSD1306_SET_COM_SCAN_DIR);
    send_command_SSD1306(SSD1306_SET_COM_PINS);
    send_command_SSD1306(SSD1306_COM_PINS_128x32); // Change to SSD1306_COM_PINS_128x64 for 128x64 displays
    send_command_SSD1306(SSD1306_SET_CONTRAST);
    send_command_SSD1306(SSD1306_CONTRAST_DEFAULT);
    send_command_SSD1306(SSD1306_ENTIRE_DISPLAY_ON);
    send_command_SSD1306(SSD1306_SET_NORMAL_DISPLAY);
    send_command_SSD1306(SSD1306_SET_OSC_FREQ);
    send_command_SSD1306(SSD1306_OSC_FREQ_DEFAULT);
    send_command_SSD1306(SSD1306_ENABLE_CHARGE_PUMP);
    send_command_SSD1306(SSD1306_CHARGE_PUMP_ON);
    send_command_SSD1306(SSD1306_DISPLAY_ON);
}

void ssd1306::clear_SSD1306()
{
    uint8_t emptyBuffer[SSD1306_horizontal_resolution] = {0}; // each page is 128 x 8
    // display is 128 x 64, each page is 8 pixels high
    for (uint8_t i = 0; i < (SSD1306_page_count); i++)
    {
        send_command_SSD1306(SSD1306_PAGE_OFFSET + i); // 0B0 - 0B7 are the 8 vertical pages

        // sets the column address to 0
        send_command_SSD1306(SSD1306_LOWER_COLUMN_START);
        send_command_SSD1306(SSD1306_UPPER_COLUMN_START);

        // write the empty buffer to the display
        send_data_SSD1306(emptyBuffer, sizeof(emptyBuffer));
    }
}

void ssd1306::draw_pixel_SSD1306(uint8_t x, uint8_t y)
{
    if ((x >= SSD1306_horizontal_resolution) || (y >= SSD1306_vertical_resolution)){
        return; // Out of bounds
    }
    uint8_t pixel_in_page = (1 << (y % (SSD1306_page_height))); //limit the pixel from 0 to 7 with a 1 in the position 0000_0001, 0000_0010, etc
    //buffer is indexed as such
	// x straight across 0-127
	//each one of these is 8 pixels bits tall, or 1 page
	//repeat for pages 0-8
	//leading to a total size of 128*8 = 1024 page "strips" each containing the data for 8 pixels
	uint16_t buffer_index = uint16_t(x + ((y / (SSD1306_page_height)) * SSD1306_horizontal_resolution));
    buffer[buffer_index] |= pixel_in_page;  //write to the buffer based on page and column
}

void ssd1306::write_buffer_SSD1306()
{
    for (uint8_t i = 0; i < SSD1306_page_count; i++)
    {                                 // Send each page
        send_command_SSD1306(SSD1306_PAGE_OFFSET + i); // Set page start address
        send_command_SSD1306(SSD1306_LOWER_COLUMN_START);     // Set lower column address
        send_command_SSD1306(SSD1306_UPPER_COLUMN_START);     // Set higher column address

        send_data_SSD1306(&buffer[SSD1306_horizontal_resolution * i], SSD1306_horizontal_resolution);
    }
}

void ssd1306::write_letter_SSD1306(uint8_t letter, uint8_t x_pos, uint8_t y_pos)
{
    const uint16_t *matrix = font_instance.get_letter(letter);//get the pixel data for one font char

    if (matrix == nullptr){
        return;
    }

    for (uint8_t i = 0; i < font_height; i++)
    { // 16 tall
        for (uint8_t j = 0; j < font_width; j++)
        { // 12 wide
            if ((matrix[i]) & (1 << (font_width - j)))
            {
                draw_pixel_SSD1306(x_pos + j, y_pos + i); //draw the pixel to buffer if on
            }
        }
    }
}

void ssd1306::write_string_SSD1306(std::string word, uint8_t x_pos, uint8_t y_pos)
{
    for (uint8_t i = 0; i < word.length(); i++)
    {
        write_letter_SSD1306(word[i], x_pos + i * font_width, y_pos); //each char is 12 wide, go through each letter
        //with proper spacing
    }
}

ssd1306::ssd1306(i2c_master_dev_handle_t ssd1306_handle)
{
    this->ssd1306_handle = ssd1306_handle;
    init_SSD1306(); // set up display
    clear_SSD1306(); //clear display

    // Send the empty buffer to the display
    clear_buffer();
}

// empties buffer so that new data can be put into it
void ssd1306::clear_buffer()
{
    //reset the buffer to all 0's
    (void)memset(buffer, 0, SSD1306_BUFFER_SIZE_128x32);
    return;
}