/**
 * @file main.cpp
 * @brief This is the file that contains the main function and all other tasks for the robot controller
 * @author Shane Wood
 * @details This is the code for the kicker controller, it was part of the 2024-2025 senior design project.
 * The controller communicates with a ps4 controller over usb, and transmits data to the robot over esp-now.
 * The controller allows for various parameters to be tuned using the menu interfaces on the oled display, with all input coming from the ps4 controller.
 * The controller also displays the battery voltage of the robot on the oled display and the connection status to the robot and the controller.
 * 
 * @date 22/12/2024
 * @note Dates are in the format dd/mm/yyyy
 * 
 * @note This folder, along with the code for the kicker itself both have version control, intended to be used with github desktop.
 * A .gitignore file is included in the kicker folder to prevent the inclusion of the build folder in the repository.
 * 
 * @note This project has the following subfolders:
 * - drivers: contains the code that is interfacing with the hardware
 * - setup: contains the code that is setting up the hardware, this code either just runs once or is a callback function for another task
 * - supporting: This is anything that is not directly interfacing with the hardware, but is not a task itself
 * - tasks: contains the code for the tasks that are running on the microcontroller
 */

#include "../include/all_includes.h"

#include "../include/drivers/ps4.h"
#include "../include/drivers/ssd1306.h"

#include "../include/supporting/menus.h"
#include "../include/supporting/edge_dector.h"
#include "../include/supporting/menu_handler.h"
#include "../include/supporting/changeable_values.h"

#include "esp_timer.h"

//rtos entry functions for tasksk
void ps4_task(void *pv);
void display_task(void *pv);
void controller_handler(void *pv);
void ps4_data_processor(void *pv);

//other function declarations
void esp_now_setup();
void i2c_setup();



SemaphoreHandle_t update_main_display_mutex;
bool update_main_display = false;

SemaphoreHandle_t main_menu_values_mutex;
std::string contoller_connected="DisCon"; 
std::string robot_connected="DisCon";
std::string battery_voltage_string="0";
std::string status = "";


SemaphoreHandle_t motor_status_mutex;
uint8_t motor_status;



SemaphoreHandle_t ramped_velocity_mutex;
float velocity_ramp_limit = 10.0f;
float velocity_gain = 0.07f;  
float velocity_integrator_gain = 0.05f;


SemaphoreHandle_t joystick_mutex;
uint8_t joystick_reading_left=0; //values at joystick
uint8_t joystick_reading_right=0;


SemaphoreHandle_t motor_speeds_mutex;
uint8_t left_motor_speed=1; //values set in settings
uint8_t right_motor_speed=1;

SemaphoreHandle_t servo_status_mutex;
uint8_t servo_status = DETACHED;

//i2c bus config
i2c_master_bus_config_t i2c_mst_config;
i2c_master_bus_handle_t bus_handle;


//queue handles
QueueHandle_t other_controller_data_queue;


SemaphoreHandle_t new_controller_data;

std::atomic<uint8_t> controller_byte_2 = 0;
std::atomic<uint8_t> controller_byte_4 = 0;
std::atomic<uint8_t> controller_byte_5 = 0;
std::atomic<uint8_t> controller_byte_6 = 0;



#define bytes_in_float 4
#define minimum_battery_reading 0.0f
#define maximum_battery_reading 30.0f

#define send_data_task_stack_size 8092
#define ps4_task_stack_size 8092
#define display_task_stack_size 16384
#define ps4_data_processor_stack_sze 8092

#define send_data_task_priority 8
#define ps4_task_priority 10
#define display_task_priority 5
#define ps4_data_processor_priority 11

#define size_of_other_controller_data_queue 10

#define maximum_left_motor_speed_modifier 15
#define maximum_right_motor_speed_modifier 15

#define minimum_left_motor_speed_modifier 0
#define minimum_right_motor_speed_modifier 0

#define left_motor_speed_modifier_step_size 1
#define right_motor_speed_modifier_step_size 1

#define dpad_left_byte 0x06
#define dpad_right_byte 0x02
#define dpad_up_byte 0x00
#define dpad_down_byte 0x04

#define lower_four_bits 0x0F
#define upper_four_bits 0xF0



#define triangle_byte 0x80
#define square_byte 0x10
#define circle_byte 0x40
#define x_byte 0x20


#define options_byte 0x20
#define r2_byte 0x08
#define l2_byte 0x04




#define boot_screen_wait_time 1000

#define minimum_velocity_ramp_limit 0.0f
#define maximum_velocity_ramp_limit 50.0f
#define velocity_ramp_limit_step_size 0.5f

#define minimum_velocity_gain 0.0f
#define maximum_velocity_gain 0.7f
#define velocity_gain_step_size 0.01f

#define minimum_velocity_integrator_gain 0.0f
#define maximum_velocity_integrator_gain 1.0f
#define velocity_integrator_gain_step_size 0.01f

#define display_task_wait_time 10
#define ps4_data_processor_wait_time 10
#define main_task_wait_time 1000
#define send_data_task_wait_time 25

#define left_shift_4  << 4

#define mac_address_length 6

//call back function for when data is sent
void on_send(const uint8_t *mac_addr, esp_now_send_status_t status) {
    static bool last_transmit_state=false;
    if(status == ESP_NOW_SEND_SUCCESS){
            if(last_transmit_state == false){
                last_transmit_state = true;
                if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                    robot_connected = "Conn";
                    xSemaphoreGive(main_menu_values_mutex);
                    if(xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)){
                        update_main_display = true;
                        xSemaphoreGive(update_main_display_mutex);
                    }
                }
            }
        }else{
            if(last_transmit_state == true){
                last_transmit_state = false;
                if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                    robot_connected = "DisCon";
                    xSemaphoreGive(main_menu_values_mutex);
                    battery_voltage_string = "0";
                    if(xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)){
                        update_main_display = true;
                        xSemaphoreGive(update_main_display_mutex);
                    }
                }
            }
        }
}


// function that is called, when data is received
void on_receive(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len) {
    static float old_battery_reading=minimum_battery_reading;
    esp_now_data_to_receive received_data;
    memcpy(&received_data, data, sizeof(esp_now_data_to_receive));
    if((received_data.battery_voltage >= minimum_battery_reading) && (received_data.battery_voltage < maximum_battery_reading)){
        char temp_buffer[bytes_in_float] = "";
        std::string value_as_string = "";
        sprintf(temp_buffer, "%.1f", received_data.battery_voltage);
        value_as_string = temp_buffer;
        if(old_battery_reading != received_data.battery_voltage){
            if(xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                    battery_voltage_string = value_as_string;
                    xSemaphoreGive(main_menu_values_mutex);
                    if(xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)){
                        update_main_display = true;
                        xSemaphoreGive(update_main_display_mutex);
                }
            }
        }
        old_battery_reading = received_data.battery_voltage;
    }
}


//task that sends controller data
void send_data_task(void *pvParameter) {
    while (1) {
        bool ps4_connected = false;
        esp_now_data_to_send current_transmission;
        if(xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
            if(contoller_connected == "Conn"){
                ps4_connected = true;
            }
            xSemaphoreGive(main_menu_values_mutex);
        }

        if(ps4_connected){
            current_transmission.left_motor_speed = controller_byte_2;
            current_transmission.right_motor_speed = controller_byte_4;

            if(xSemaphoreTake(ramped_velocity_mutex, portMAX_DELAY)){
                current_transmission.velocity_ramp_limit = velocity_ramp_limit;
                current_transmission.velocity_gain = velocity_gain;
                current_transmission.velocity_integrator_gain = velocity_integrator_gain;
                xSemaphoreGive(ramped_velocity_mutex);
            }
            if(xSemaphoreTake(servo_status_mutex, portMAX_DELAY)){
                current_transmission.servo_and_motor_state = ((servo_status & lower_four_bits));
                xSemaphoreGive(servo_status_mutex);
            }
            if(xSemaphoreTake(motor_status_mutex, portMAX_DELAY)){
                current_transmission.servo_and_motor_state |= ((motor_status & lower_four_bits) left_shift_4);
                xSemaphoreGive(motor_status_mutex);
            }
            if(xSemaphoreTake(motor_speeds_mutex, portMAX_DELAY)){
                current_transmission.motor_speed_setting = ((left_motor_speed & lower_four_bits) left_shift_4) | (right_motor_speed & lower_four_bits);
                xSemaphoreGive(motor_speeds_mutex);
            }   
        }else{
            //if the controller is not connected, send 0 values
            current_transmission.left_motor_speed = 0;
            current_transmission.right_motor_speed = 0;
            current_transmission.servo_and_motor_state = 0;
            current_transmission.servo_and_motor_state |= ((DISABLED & lower_four_bits) left_shift_4);
        }
        (void)esp_now_send(robotMacAddress, (uint8_t *) &current_transmission, sizeof(esp_now_data_to_send));
        vTaskDelay(pdMS_TO_TICKS(send_data_task_wait_time));
    }
}


void esp_now_setup(){
   // setup nvs (non-volatile storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // set up wifi in station mode
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // setup esp-now
    ESP_ERROR_CHECK(esp_now_init());
    (void)esp_now_register_send_cb(on_send);
    (void)esp_now_register_recv_cb(on_receive);

    esp_now_peer_info_t peer_info = {};
    memcpy(peer_info.peer_addr, robotMacAddress, mac_address_length);
    peer_info.channel = 0;
    peer_info.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
}


void i2c_setup(){
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.i2c_port = I2C_NUM_0;
    i2c_mst_config.sda_io_num = sdaPin;
    i2c_mst_config.scl_io_num = sclPin;
    i2c_mst_config.glitch_ignore_cnt = 0;
    i2c_mst_config.flags.enable_internal_pullup = true;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}


extern "C" void app_main(void)
{
    i2c_setup();
    esp_now_setup();
    
    //create queues
    other_controller_data_queue = xQueueCreate(size_of_other_controller_data_queue, sizeof(display_controller_data));


    //create semaphores/mutuexes (espidf does not have mutexes directly)
    update_main_display_mutex = xSemaphoreCreateMutex();
    main_menu_values_mutex = xSemaphoreCreateMutex();
    ramped_velocity_mutex = xSemaphoreCreateMutex();
    motor_speeds_mutex = xSemaphoreCreateMutex();
    servo_status_mutex = xSemaphoreCreateMutex();
    joystick_mutex = xSemaphoreCreateMutex();
    motor_status_mutex = xSemaphoreCreateMutex();

    new_controller_data = xSemaphoreCreateBinary();

    //create tasks
    (void)xTaskCreate(send_data_task, "SendDataTask", send_data_task_stack_size, NULL, send_data_task_priority, NULL);
    (void)xTaskCreate(ps4_task, "ps4_task", ps4_task_stack_size, NULL, ps4_task_priority, NULL);
    (void)xTaskCreate(display_task, "display_task", display_task_stack_size, NULL, display_task_priority, NULL);
    (void)xTaskCreate(ps4_data_processor, "ps4_data_processor", ps4_data_processor_stack_sze, NULL, ps4_data_processor_priority, NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(main_task_wait_time));
    }
}


void ps4_task(void *pv){
    //this class creates all relavent tasks for usb host; the for loop here will never run;
    ps4 ps4_obj;
    
}


void ps4_data_processor(void *pv){
    for(;;){
        if(xSemaphoreTake(new_controller_data,portMAX_DELAY)){
            display_controller_data current_display_data = {0,0};
            uint8_t temp_buttons=0;
            
            uint8_t dpad = (controller_byte_5 & lower_four_bits);
            if((dpad == dpad_left_byte)){
                temp_buttons |= (1 <<processed_dpad_left_bit);//left
            }
            else if(dpad == dpad_down_byte){
                temp_buttons |= (1<<processed_dpad_down_bit);//down
            }
            else if(dpad == dpad_right_byte){
                temp_buttons |= (1<<processed_dpad_right_bit);//right
            }
            else if(dpad == dpad_up_byte){
                temp_buttons |= (1<<processed_dpad_up_bit);//up
            }


            if(controller_byte_5 & triangle_byte){
                temp_buttons |= (1<<processed_triangle_bit);//triangle
            }else if(controller_byte_5 & square_byte){
                temp_buttons |= (1<<processed_square_bit);//square
            }else if(controller_byte_5 & circle_byte){
                temp_buttons |= (1<<processed_circle_bit);//circle
            }else if(controller_byte_5 & x_byte){
                temp_buttons |= (1<<processed_x_bit);//x
            }

            current_display_data.buttons = temp_buttons;


            if(controller_byte_6 & options_byte){
                current_display_data.options_triggers |= (1<<processed_options_bit); //options
            }else if(controller_byte_6 & r2_byte){
                current_display_data.options_triggers |= (1<<processed_r2_bit); //r2
            }else if(controller_byte_6 & l2_byte){
                current_display_data.options_triggers |= (1<<processed_l2_bit); //l2
            }
        xQueueSend(other_controller_data_queue,&current_display_data,portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(ps4_data_processor_wait_time));
        }       
    }
}


void display_task(void *pv){
    //display setup
    i2c_device_config_t ssd1306_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SSD1306Address,
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

    //bools used for determining if a row has a numerical value + a selectable value
    bool all_false[values_4] = {false,false,false,false};
    bool all_true[values_4] = {true,true,true,true};
    bool one_false_two_true[values_3] = {false,true,true};

    //the startup menu 
    std::string startup_text[values_4] = {"Kicker","Robot","2025", "Trine"};

    //menu(uint8_t lines, bool *have_values, std::string *words, T **values, ssd1306 *display, bool *selectable_values);
    //<uint8_t> - does not matter here as there is no data displayed
    //4, indicates that there is 4 lines of text
    //all_false: the startup menu shows no values, only text constants
    //startup_text: a pointer to the array of the text to be displayed
    //nullptr: a pointer to an array of pointers with the values to be display (there are none in this case)
    //&display, a pointer to a ssd1306 class
    //all_false, a pointer to an array of bools, all false so no values are selectable
    menu<uint8_t> startup_menu(values_4,all_false,startup_text,nullptr,&display,all_false);
    display.clear_buffer();
    startup_menu.draw_to_display();
    display.write_buffer_SSD1306();
    vTaskDelay(pdMS_TO_TICKS(boot_screen_wait_time));
    
    std::string main_menu_text[values_4] = {"Bot","Ps4","Batt", ""};

    std::string *main_menu_values[values_4] = {&robot_connected,&contoller_connected,&battery_voltage_string,&status};
    menu<std::string> status_screen(values_4,all_true,main_menu_text,main_menu_values,&display,all_false);
    display.clear_buffer();
    status_screen.draw_to_display();
    display.write_buffer_SSD1306();

    changeable_values<uint8_t> left_motor_speed_values(&left_motor_speed,minimum_left_motor_speed_modifier,maximum_left_motor_speed_modifier,left_motor_speed_modifier_step_size);
    changeable_values<uint8_t> right_motor_speed_values(&right_motor_speed,minimum_right_motor_speed_modifier,maximum_right_motor_speed_modifier,right_motor_speed_modifier_step_size);
    changeable_values<uint8_t> *motor_speeds[values_3] = {nullptr,&left_motor_speed_values,&right_motor_speed_values};
    std::string motor_speed_text[values_3] = {"Speeds", "Left","Right"};
    
    menu<changeable_values<uint8_t>> motor_speed_menu(values_3,one_false_two_true,motor_speed_text,motor_speeds,&display,one_false_two_true);

    changeable_values<float> velocity_ramp_limit_value(&velocity_ramp_limit,minimum_velocity_ramp_limit,maximum_velocity_ramp_limit,velocity_ramp_limit_step_size);
    changeable_values<float> velocity_gain_value(&velocity_gain,minimum_velocity_gain,maximum_velocity_gain,velocity_gain_step_size);
    changeable_values<float> velocity_integrator_gain_value(&velocity_integrator_gain,minimum_velocity_integrator_gain,maximum_velocity_integrator_gain,velocity_integrator_gain_step_size);


    changeable_values<float> *ramped_velocity_settings[values_4] = {nullptr,&velocity_ramp_limit_value,&velocity_gain_value,&velocity_integrator_gain_value};
    std::string ramped_velocity_text[values_4] = {"Ramp Velo","RL", "VG", "VIG"};
    bool ramped_velocity_varible_settings[values_4] = {false,true,true,true};
    menu<changeable_values<float>> ramped_velocity_menu(values_4,ramped_velocity_varible_settings,ramped_velocity_text,ramped_velocity_settings,&display,ramped_velocity_varible_settings);

    std::string servo_latched_text[values_2] = {"Servo","Latched"};
    menu<uint8_t> servo_latched_menu(values_2,all_false,servo_latched_text,nullptr,&display,all_false);

    std::string servo_released_text[values_2] = {"Servo","Released"};
    menu<uint8_t> servo_released_menu(values_2,all_false,servo_released_text,nullptr,&display,all_false);

    std::string motors_calibrating_text[values_2] = {"Motors","Calib"};
    menu<uint8_t> motors_calibrating_menu(values_2,all_false,motors_calibrating_text,nullptr,&display,all_false);

    std::string motors_enable_text[values_2] = {"Motors","Enable"};
    menu<uint8_t> motors_enable_menu(values_2,all_false,motors_enable_text,nullptr,&display,all_false);

    std::string motors_disable_text[values_2] = {"Motors","Disable"};
    menu<uint8_t> motors_disable_menu(values_2,all_false,motors_disable_text,nullptr,&display,all_false);

    std::string errors_clear_text[values_2] = {"Errors","Clear"};
    menu<uint8_t> errors_clear_menu(values_2,all_false,errors_clear_text,nullptr,&display,all_false);

    menu_handler main_menu_handler(&display, &status_screen, &motor_speed_menu, &ramped_velocity_menu, &servo_latched_menu, &servo_released_menu, &motors_calibrating_menu,
        &motors_enable_menu, &motors_disable_menu, &errors_clear_menu);

    for(;;){
        main_menu_handler.update();
        vTaskDelay(pdMS_TO_TICKS(display_task_wait_time));
    }
}