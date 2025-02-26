#include "../include/ssd1306.h"
#include "../include/all_includes.h"
#include "../include/pinout.h"

//display inputs
extern QueueHandle_t displayBatteryQueue;
extern bool connected;

esp_err_t ssd1306::send_command_SSD1306(uint8_t command)
{
    uint8_t data[2] = {0x00, command}; // 0x00 is for command
    esp_err_t ret = i2c_master_transmit(ssd1306_handle, data, 2,100);
    //ret = i2c_master_transmit(ssd1306_handle, &command, 1,100);
    return ret;
}

esp_err_t ssd1306::send_data_SSD1306(uint8_t *data, size_t len)
{
    uint8_t data_to_send[len+1] = {0};
    data[0] = 0x40;  // 0x40 is for data
    memcpy(&(data_to_send[1]), data, len);
    esp_err_t ret = i2c_master_transmit(ssd1306_handle, &data[0], 1, 100);
    ret = i2c_master_transmit(ssd1306_handle, data, len, 100);
    return ret; // return if the writing was sucessful
}

void ssd1306::init_SSD1306()
{
    // command list taken from mircocontrollers lab
    send_command_SSD1306(0xAE);
    send_command_SSD1306(0xA8); // Set MUX ratio
    send_command_SSD1306(0x3F); // Set MUX ratio, 0x1F for 128x32 and 0x3F for 128x64
    send_command_SSD1306(0xD3); // Set Display Offset
    send_command_SSD1306(0x00); // Set Display Offset
    send_command_SSD1306(0x40); // Set Display Start Line
    send_command_SSD1306(0x20); // Set Display Mode to Horizontal
    send_command_SSD1306(0x00); // Set Display Mode to Horizontal
    send_command_SSD1306(0xA1); // Set Segment Re-map
    send_command_SSD1306(0xC8); // Set COM Output Scan Direction
    send_command_SSD1306(0xDA); // Set COM Pins hardware configuration
    send_command_SSD1306(0x12); // Set COM Pins hardware configuration, 0x02 for 128x32 and 0x12 for 128x64
    send_command_SSD1306(0x81); // Set Contrast Control
    send_command_SSD1306(0x9F); // Set Contrast Control Value from 0x00 to 0xFF minimum to maximum
    send_command_SSD1306(0xA4); // Disable Entire Display
    send_command_SSD1306(0xA6); // Set Normal Display
    send_command_SSD1306(0xD5); // Set Oscillation Frequency
    send_command_SSD1306(0x80); // Set Oscillation Frequency
    send_command_SSD1306(0x8D); // Enable Charge Pump Regulator
    send_command_SSD1306(0x14); // Enable Charge Pump Regulator
    send_command_SSD1306(0xAF); // Turn Display On
}

void ssd1306::clear_SSD1306()
{
    uint8_t emptyBuffer[SSD1306HorizontalRes] = {0}; // each page is 128 x 8
    // display is 128 x 64, each page is 8 pixels high
    for (uint8_t i = 0; i < SSD1306Pages; i++)
    {
        send_command_SSD1306(0xB0 + i); // 0B0 - 0B7 are the 8 vertical pages

        // sets the column address from 0-127
        send_command_SSD1306(0x00);
        send_command_SSD1306(0x10);

        // write the empty buffer to the display
        send_data_SSD1306(emptyBuffer, sizeof(emptyBuffer));
    }
}

void ssd1306::draw_pixel_SSD1306(uint8_t x, uint8_t y)
{
    if (x >= SSD1306HorizontalRes || y >= SSD1306VerticalRes){
        return; // Out of bounds
    }
    uint8_t pixelInPage = (1 << (y % 8)); //limit the pixel from 0 to 7 with a 1 in the position 0000_0001, 0000_0010, etc
    //buffer is indexed as such
	// x straight across 0-127
	//each one of these is 8 pixels bits tall, or 1 page
	//repeat for pages 0-8
	//leading to a total size of 128*8 = 1024 page "strips" each containing the data for 8 pixels
	uint16_t bufferIndex = uint16_t(x + ((y / 8) * SSD1306HorizontalRes));
    buffer[bufferIndex] |= pixelInPage;  //write to the buffer based on page and column
}

void ssd1306::write_buffer_SSD1306()
{
    for (uint8_t i = 0; i < 8; i++)
    {                                 // Send each page
        send_command_SSD1306(0xB0 + i); // Set page start address
        send_command_SSD1306(0x00);     // Set lower column address
        send_command_SSD1306(0x10);     // Set higher column address

        send_data_SSD1306(&buffer[128 * i], 128);
        //vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void ssd1306::write_letter_SSD1306(uint8_t letter, uint8_t x_pos, uint8_t y_pos)
{
    const uint16_t *matrix = font_instance.get_letter(letter);//get the pixel data for one font char
    for (int i = 0; i < 16; i++)
    { // 16 tall
        for (int j = 0; j < 12; j++)
        { // 12 wide
            if ((matrix[i]) & (1 << (12 - j)))
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
        write_letter_SSD1306(word[i], x_pos + i * 12, y_pos); //each char is 12 wide, go through each letter
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
    for (uint16_t i = 0; i < 1024; i++)
    {
        buffer[i] = 0;
    }
    return;
}


void ssd1306::write_section(const uint8_t page,const uint8_t column_start, const uint8_t column_stop){
	send_command_SSD1306(0x22);
	send_command_SSD1306(page);
	send_command_SSD1306(page);
	send_command_SSD1306(0x21);
	send_command_SSD1306(column_start);
	send_command_SSD1306(column_stop);
	return;
}