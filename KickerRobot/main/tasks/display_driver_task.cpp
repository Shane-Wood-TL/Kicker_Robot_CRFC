#include "../../include/tasks/display_driver_task.h"
void displayDriver(void *pv){
    //display setup
    i2c_device_config_t ssd1306_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x3C,
        .scl_speed_hz = i2cFreq,
        .scl_wait_us = 0, //use the default value (https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/i2c.html)
        .flags = 0
    };

    //add the device to the bus
    i2c_master_dev_handle_t ssd1306_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &ssd1306_config, &ssd1306_handle));
    (void)i2c_master_bus_reset(bus_handle);

    //create the display class
    ssd1306 display(ssd1306_handle);


    std::string main_menu_text[2] = {"","Batt"};
    bool all_true[2] = {true, true};
    bool all_false[2] = {false, false};
    std::string *main_menu_values[2] = {&controller_connected_string,&battery_voltage_string};
    menu<std::string> status_screen(2,all_true,main_menu_text,main_menu_values,&display,all_false);
    display.clear_buffer();
    display.write_buffer_SSD1306();


    for(;;){
        display.clear_buffer();
        char temp_buffer[4];
        if((battery_voltage >= 0) && (battery_voltage < 30)){
            if(xSemaphoreTake(battery_voltage_mutex, portMAX_DELAY)){
                sprintf(temp_buffer,"%.1f",battery_voltage);
                xSemaphoreGive(battery_voltage_mutex);
            }
            battery_voltage_string = temp_buffer;
            if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                status_screen.draw_to_display();
                xSemaphoreGive(main_menu_values_mutex);
            }
            display.write_buffer_SSD1306();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}