/**
 * @file display_task.cpp
 * @brief This is the file that contains the display task
 * @author Shane Wood
 */
#include "../../include/tasks/display_task.h"

void display_task(void *pv) {
  if (bus_handle == NULL) {
    ESP_LOGE("I2C", "Bus handle is NULL. Call i2c_setup() first.");
    return;
  }
    // display setup
    i2c_device_config_t ssd1306_config = {.dev_addr_length = I2C_ADDR_BIT_LEN_7,
                                          .device_address = SSD1306_address,
                                          .scl_speed_hz = i2c_frequency,
                                          .scl_wait_us = 0, // use the default value (https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/i2c.html)
                                          .flags = 0};
  
    // add the device to the bus
    i2c_master_dev_handle_t ssd1306_handle = NULL;
    esp_err_t err = i2c_master_bus_add_device(bus_handle, &ssd1306_config, &ssd1306_handle);
    if (err != ESP_OK) {
        ESP_LOGE("I2C", "Failed to add SSD1306 device: %s", esp_err_to_name(err));
        return;
    }

    if (ssd1306_handle == NULL) {
        ESP_LOGE("I2C", "SSD1306 handle is NULL");
        return;
    }

    //ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &ssd1306_config, &ssd1306_handle));
    i2c_master_bus_reset(bus_handle);
    vTaskDelay(pdMS_TO_TICKS(display_task_wait_time));
    // create the display class
    ssd1306 display(ssd1306_handle);
  
    // bools used for determining if a row has a numerical
    // value + a selectable value
    bool all_false[values_4] = {false, false, false, false};
    bool all_true[values_4] = {true, true, true, true};
    bool one_false_two_true[values_3] = {false, true, true};
  
    // the startup menu
    std::string startup_text[values_4] = {"Kicker", "Robot", "2025", "Trine"};
  
    // menu(uint8_t lines, bool *have_values, std::string *words, T **values, ssd1306 *display, bool *selectable_values);
    // values_4, indicates that there is 4 lines of text
    // all_false: the startup menu shows no values, only text constants 
    // startup_text: a pointer to the array of the text to be displayed 
    // nullptr: a pointer to an array of pointers with the values to be display (there are none in this case)
    // &display, a pointer to a ssd1306 class
    // all_false, a pointer to an array of bools, all false so
    // no values are selectable
    menu<uint8_t> startup_menu(values_4, all_false, startup_text, nullptr, &display, all_false);
    display.clear_buffer();
    startup_menu.draw_to_display();
    display.write_buffer_SSD1306();
    vTaskDelay(pdMS_TO_TICKS(boot_screen_wait_time));
  
    std::string main_menu_text[values_4] = {"Bot", "Ps4", "Batt", ""};
  
    std::string *main_menu_values[values_4] = {&robot_connected, &contoller_connected, &battery_voltage_string, &status};
    menu<std::string> status_screen(values_4, all_true, main_menu_text, main_menu_values, &display, all_false);
    display.clear_buffer();
    status_screen.draw_to_display();
    display.write_buffer_SSD1306();
  
    changeable_values<uint8_t> drive_motor_speed_values(&drive_motor_speed, minimum_drive_motor_speed_modifier, maximum_drive_motor_speed_modifier, drive_motor_speed_modifier_step_size);
    changeable_values<uint8_t> turning_motor_speed_values(&turn_motor_speed, minimum_turn_motor_speed_modifier, maximum_turn_motor_speed_modifier, turn_motor_speed_modifier_step_size);
    changeable_values<uint8_t> *motor_speeds[values_3] = {nullptr, &drive_motor_speed_values, &turning_motor_speed_values};
    std::string motor_speed_text[values_3] = {"Speeds", "Drive", "Turn"};
  
    menu<changeable_values<uint8_t>> motor_speed_menu(values_3, one_false_two_true, motor_speed_text, motor_speeds, &display, one_false_two_true);
  
    changeable_values<float> velocity_ramp_limit_value(&velocity_ramp_limit, minimum_velocity_ramp_limit, maximum_velocity_ramp_limit, velocity_ramp_limit_step_size);
    changeable_values<float> velocity_gain_value(&velocity_gain, minimum_velocity_gain, maximum_velocity_gain, velocity_gain_step_size);
    changeable_values<float> velocity_integrator_gain_value(&velocity_integrator_gain, minimum_velocity_integrator_gain, maximum_velocity_integrator_gain, velocity_integrator_gain_step_size);
  
    changeable_values<float> *ramped_velocity_settings[values_4] = {nullptr, &velocity_ramp_limit_value, &velocity_gain_value, &velocity_integrator_gain_value};
  
    std::string ramped_velocity_text[values_4] = {"Ramp Velo", "RL", "VG", "VIG"};
    bool ramped_velocity_varible_settings[values_4] = {false, true, true, true};
    menu<changeable_values<float>> ramped_velocity_menu(values_4, ramped_velocity_varible_settings, ramped_velocity_text, ramped_velocity_settings, &display, ramped_velocity_varible_settings);
  
    std::string servo_latched_text[values_2] = {"Servo", "Latched"};
    menu<uint8_t> servo_latched_menu(values_2, all_false, servo_latched_text, nullptr, &display, all_false);
  
    std::string servo_released_text[values_2] = {"Servo", "Released"};
    menu<uint8_t> servo_released_menu(values_2, all_false, servo_released_text, nullptr, &display, all_false);
  
    std::string motors_calibrating_text[values_2] = {"Motors", "Calib"};
    menu<uint8_t> motors_calibrating_menu(values_2, all_false, motors_calibrating_text, nullptr, &display, all_false);
  
    std::string motors_enable_text[values_2] = {"Motors", "Enable"};
    menu<uint8_t> motors_enable_menu(values_2, all_false, motors_enable_text, nullptr, &display, all_false);
  
    std::string motors_disable_text[values_2] = {"Motors", "Disable"};
    menu<uint8_t> motors_disable_menu(values_2, all_false, motors_disable_text, nullptr, &display, all_false);
  
    std::string errors_clear_text[values_2] = {"Errors", "Clear"};
    menu<uint8_t> errors_clear_menu(values_2, all_false, errors_clear_text, nullptr, &display, all_false);
  
    menu_handler main_menu_handler(&display, &status_screen, &motor_speed_menu, &ramped_velocity_menu, &servo_latched_menu, &servo_released_menu, &motors_calibrating_menu, &motors_enable_menu, &motors_disable_menu, &errors_clear_menu);
  
    for (;;) {
      main_menu_handler.update();
      vTaskDelay(pdMS_TO_TICKS(display_task_wait_time));
    }
  }