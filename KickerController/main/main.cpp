#include "../include/all_includes.h"

#include "../include/drivers/ps4.h"
#include "../include/drivers/ssd1306.h"

#include "../include/supporting/menus.h"
#include "../include/supporting/edge_dector.h"
#include "../include/supporting/menu_handler.h"
#include "../include/supporting/changeable_values.h"

#include "esp_timer.h"


#define CONFIG_ESPNOW_CHANNEL 11
#define CONFIG_ESPNOW_LMK "lmk1234567890123"
#define CONFIG_ESPNOW_PMK "pmk1234567890123"



#define CONFIG_ESPNOW_SEND_COUNT 100
#define CONFIG_ESPNOW_SEND_DELAY 1000
#define CONFIG_ESPNOW_SEND_LEN 10

#define SETABLE_PID


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



SemaphoreHandle_t pid_mutex;
float pid_kp=10.0f;
float pid_ki=0.07f;  
float pid_kd=0.05f;


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

//esp now config
//static uint8_t robotMacAddress[6] = {0xD8, 0x3B, 0xDA, 0xA0, 0xC5, 0xE0};
static uint8_t robotMacAddress[6] = {0xD8,0x3B,0xDA,0xA1,0x06,0x30};
//static uint8_t controller_mac_address[6] = {0xF0,0x9E,0x9E,0x12,0xA8,0xA4}; this devices mac address



//queue handles
QueueHandle_t other_controller_data_queue;






//struct for the data to be sent

typedef struct {
    uint8_t left_motor_speed;
    uint8_t right_motor_speed;
    #ifdef SETABLE_PID
    float kp; //top 8 bits = ones + tens, bottom 8 bits = decimal
    float ki;
    float kd;
    #endif
    uint8_t servo_and_motor_state;
    uint8_t motor_speed_setting;
} esp_now_data_to_send;


typedef struct {
    float battery_voltage; 
} esp_now_data_to_receive;


SemaphoreHandle_t new_controller_data;

std::atomic<uint8_t> controller_byte_2 = 0;
std::atomic<uint8_t> controller_byte_4 = 0;
std::atomic<uint8_t> controller_byte_5 = 0;
std::atomic<uint8_t> controller_byte_6 = 0;











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
    static float old_battery_reading=0;
    esp_now_data_to_receive received_data;
    memcpy(&received_data, data, sizeof(esp_now_data_to_receive));
    if((received_data.battery_voltage >= 0) && (received_data.battery_voltage < 30)){
        char temp_buffer[4] = "";
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

            #ifdef SETABLE_PID
            if(xSemaphoreTake(pid_mutex, portMAX_DELAY)){
                current_transmission.kp = pid_kp;
                current_transmission.ki = pid_ki;
                current_transmission.kd = pid_kd;
                xSemaphoreGive(pid_mutex);
            }
            #endif
            if(xSemaphoreTake(servo_status_mutex, portMAX_DELAY)){
                current_transmission.servo_and_motor_state = ((servo_status & 0x0F));
                xSemaphoreGive(servo_status_mutex);
            }
            if(xSemaphoreTake(motor_status_mutex, portMAX_DELAY)){
                current_transmission.servo_and_motor_state |= ((motor_status & 0x0F) << 4);
                xSemaphoreGive(motor_status_mutex);
            }
            if(xSemaphoreTake(motor_speeds_mutex, portMAX_DELAY)){
                current_transmission.motor_speed_setting = ((left_motor_speed & 0x0F) << 4) | (right_motor_speed & 0x0F);
                xSemaphoreGive(motor_speeds_mutex);
            }
            
        }else{
            //if the controller is not connected, send 0 values
            current_transmission.left_motor_speed = 0;
            current_transmission.right_motor_speed = 0;
            current_transmission.servo_and_motor_state = 0;
            current_transmission.servo_and_motor_state |= ((DISABLED & 0x0F) << 4);
        }
        (void)esp_now_send(robotMacAddress, (uint8_t *) &current_transmission, sizeof(esp_now_data_to_send));
        vTaskDelay(pdMS_TO_TICKS(25));
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
    memcpy(peer_info.peer_addr, robotMacAddress, 6);
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
    esp_now_setup();
    i2c_setup();

    //create queues
    other_controller_data_queue = xQueueCreate(10, sizeof(display_controller_data));


    //create semaphores/mutuexes (espidf does not have mutexes directly)
    update_main_display_mutex = xSemaphoreCreateMutex();
    main_menu_values_mutex = xSemaphoreCreateMutex();
    pid_mutex = xSemaphoreCreateMutex();
    motor_speeds_mutex = xSemaphoreCreateMutex();
    servo_status_mutex = xSemaphoreCreateMutex();
    joystick_mutex = xSemaphoreCreateMutex();
    motor_status_mutex = xSemaphoreCreateMutex();

    new_controller_data = xSemaphoreCreateBinary();

    //create tasks
    (void)xTaskCreate(send_data_task, "SendDataTask", 8092, NULL, 8, NULL);
    (void)xTaskCreate(ps4_task, "ps4_task", 8092, NULL, 10, NULL);
    (void)xTaskCreate(display_task, "display_task", 16384, NULL, 5, NULL);
    (void)xTaskCreate(ps4_data_processor, "ps4_data_processor", 8092, NULL, 11, NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}




void ps4_task(void *pv){
    //this class creates all relavent tasks for usb host; the for loop here will never run;
    ps4 ps4_obj;
    
}


void ps4_data_processor(void *pv){
    for(;;){
            display_controller_data current_display_data = {0,0};
            uint8_t temp_buttons=0;
            
            uint8_t dpad = (controller_byte_5 & 0x0F);
            if((dpad == 0x06)){
                temp_buttons |= (1 <<3);//left
            }
            else if(dpad == 0x04){
                temp_buttons |= (1<<2);//down
            }
            else if(dpad == 0x02){
                temp_buttons |= (1<<1);//right
            }
            else if(dpad == 0x00){
                temp_buttons |= (1<<0);//up
            }


            if(controller_byte_5 & 0x80){
                temp_buttons |= (1<<4);//up //triangle
            }else if(controller_byte_5 & 0x10){
                temp_buttons |= (1<<5);//square
            }else if(controller_byte_5 & 0x40){
                temp_buttons |= (1<<6);//circle
            }else if(controller_byte_5 & 0x20){
                temp_buttons |= (1<<7);//x
            }

            current_display_data.buttons = temp_buttons;


            if(controller_byte_6 & 0x20){
                current_display_data.options_triggers |= (1<<0); //options
            }else if(controller_byte_6 & 0x08){
                current_display_data.options_triggers |= (1<<1); //r2
            }else if(controller_byte_6 & 0x04){
                current_display_data.options_triggers |= (1<<2); //l2
            }
        xQueueSend(other_controller_data_queue,&current_display_data,portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

}


void display_task(void *pv){
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

    //bools used for determining if a row has a numerical value + a selectable value
    bool all_false[4] = {false,false,false,false};
    bool all_true[4] = {true,true,true,true};
    bool one_false_two_true[3] = {false,true,true};


    //the startup menu 
    std::string startup_text[4] = {"Kicker","Robot","2025", "Trine"};

    //menu(uint8_t lines, bool *have_values, std::string *words, T **values, ssd1306 *display, bool *selectable_values);
    //<uint8_t> - does not matter here as there is no data displayed
    //4, indicates that there is 4 lines of text
    //all_false: the startup menu shows no values, only text constants
    //startup_text: a pointer to the array of the text to be displayed
    //nullptr: a pointer to an array of pointers with the values to be display (there are none in this case)
    //&display, a pointer to a ssd1306 class
    //all_false, a pointer to an array of bools, all false so no values are selectable
    menu<uint8_t> startup_menu(4,all_false,startup_text,nullptr,&display,all_false);
    display.clear_buffer();
    startup_menu.draw_to_display();
    display.write_buffer_SSD1306();


    vTaskDelay(pdMS_TO_TICKS(1000));





    
    std::string main_menu_text[4] = {"Bot","Ps4","Batt", ""};

    std::string *main_menu_values[4] = {&robot_connected,&contoller_connected,&battery_voltage_string,&status};
    menu<std::string> status_screen(4,all_true,main_menu_text,main_menu_values,&display,all_false);
    display.clear_buffer();
    status_screen.draw_to_display();
    display.write_buffer_SSD1306();

    changeable_values<uint8_t> left_motor_speed_values(&left_motor_speed,0,15,1);
    changeable_values<uint8_t> right_motor_speed_values(&right_motor_speed,0,15,1);
    changeable_values<uint8_t> *motor_speeds[3] = {nullptr,&left_motor_speed_values,&right_motor_speed_values};
    std::string motor_speed_text[3] = {"Speeds", "Left","Right"};
    
    menu<changeable_values<uint8_t>> motor_speed_menu(3,one_false_two_true,motor_speed_text,motor_speeds,&display,one_false_two_true);

    changeable_values<float> velocity_ramp_limit(&pid_kp,0.0f,50.0f,0.5f);
    changeable_values<float> velocity_gain(&pid_ki,0.0f,0.7f,0.01f);
    changeable_values<float> velocity_integrator_gain(&pid_kd,0.0f,25.0f,0.01f);


    changeable_values<float> *ramped_velocity_settings[4] = {nullptr,&velocity_ramp_limit,&velocity_gain,&velocity_integrator_gain};
    std::string pid_text[4] = {"Ramp Velo","RL", "VG", "VIG"};
    bool pid_varible_settings[4] = {false,true,true,true};
    menu<changeable_values<float>> ramped_velocity_menu(4,pid_varible_settings,pid_text,ramped_velocity_settings,&display,pid_varible_settings);



    std::string servo_latched_text[2] = {"Servo","Latched"};
    menu<uint8_t> servo_latched_menu(2,all_false,servo_latched_text,nullptr,&display,all_false);

    std::string servo_released_text[2] = {"Servo","Released"};
    menu<uint8_t> servo_released_menu(2,all_false,servo_released_text,nullptr,&display,all_false);

    std::string motors_calibrating_text[2] = {"Motors","Calib"};
    menu<uint8_t> motors_calibrating_menu(2,all_false,motors_calibrating_text,nullptr,&display,all_false);




    std::string motors_enable_text[2] = {"Motors","Enable"};
    menu<uint8_t> motors_enable_menu(2,all_false,motors_enable_text,nullptr,&display,all_false);

    std::string motors_disable_text[2] = {"Motors","Disable"};
    menu<uint8_t> motors_disable_menu(2,all_false,motors_disable_text,nullptr,&display,all_false);

    std::string errors_clear_text[2] = {"Errors","Clear"};
    menu<uint8_t> errors_clear_menu(2,all_false,errors_clear_text,nullptr,&display,all_false);


    menu_handler main_menu_handler(&display, &status_screen, &motor_speed_menu, &ramped_velocity_menu, &servo_latched_menu, &servo_released_menu, &motors_calibrating_menu,
        &motors_enable_menu, &motors_disable_menu, &errors_clear_menu);

    for(;;){
        main_menu_handler.update();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}