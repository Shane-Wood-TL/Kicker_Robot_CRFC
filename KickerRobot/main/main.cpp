#include "../include/all_includes.h"
#include "../include/pinout.h"

#include "../include/setup/esp_now_setup.h"
#include "../include/setup/esp_now_callback_setup.h"
#include "../include/setup/i2c_setup.h"
#include "../include/setup/led_strip_setup.h"


#include "../include/drivers/ssd1306.h"
#include "../include/drivers/servo.h"
#include "../include/drivers/oDrive.h"

#include "../include/supporting/font.h"
#include "../include/supporting/menus.h"
#include "../include/supporting/odrive_commands.h"

#include "../include/tasks/esp_now_task.h"
#include "../include/tasks/servo_driver_task.h"
#include "../include/tasks/display_driver_task.h"
#include "../include/tasks/odrive_driver_task.h"



//display battery value + mutex
SemaphoreHandle_t battery_voltage_mutex;
float battery_voltage=0;

//controller connected status value + mutex
SemaphoreHandle_t controller_connected_mutex;
uint8_t controller_connected = DISCONNECTED;

//motor status + mutex
SemaphoreHandle_t motor_status_mutex;
uint8_t motor_status;

//servo status + mutex
SemaphoreHandle_t servo_status_mutex;
uint8_t servo_status = DETACHED;

//ramped values + values changed + mutexes
SemaphoreHandle_t ramped_values_updated;
bool ramped_values_updated_bool = true;
SemaphoreHandle_t ramped_mutex;
float velocity_ramp_limit=0.5f;
float velocity_gain=0.0f;  
float velocity_integrator_gain=0.0f;

//motor speed settings + mutex
SemaphoreHandle_t motor_speeds_settings_mutex;
uint8_t left_motor_speed_mult=0; //values set in settings
uint8_t right_motor_speed_mult=0;

//need to update display + mutex
SemaphoreHandle_t update_main_display_mutex;
bool update_main_display = false;

//strings for the display + mutex
SemaphoreHandle_t main_menu_values_mutex;
std::string controller_connected_string="DisCon"; 
std::string battery_voltage_string="0";

//current motor speeds + mutex (0-255, centered at 127)
SemaphoreHandle_t motor_speeds;
uint8_t left_motor_speed=127;
uint8_t right_motor_speed=127;


//i2c bus tags
i2c_master_bus_config_t i2c_mst_config;
i2c_master_bus_handle_t bus_handle;


extern "C" void app_main(void)
{
    battery_voltage_mutex = xSemaphoreCreateMutex();
    controller_connected_mutex = xSemaphoreCreateMutex();
    motor_status_mutex = xSemaphoreCreateMutex();
    servo_status_mutex = xSemaphoreCreateMutex();
    ramped_values_updated = xSemaphoreCreateMutex();
    motor_speeds_settings_mutex = xSemaphoreCreateMutex();
    update_main_display_mutex = xSemaphoreCreateMutex();
    main_menu_values_mutex = xSemaphoreCreateMutex();
    motor_speeds = xSemaphoreCreateMutex();
    ramped_mutex = xSemaphoreCreateMutex();

    esp_now_setup();
    i2c_setup();
    led_setup();

    xTaskCreate(displayDriver,"displayDriver",10000,NULL,1,NULL);
    xTaskCreatePinnedToCore(oDriveDriver, "oDriveTask", 4096, NULL, configMAX_PRIORITIES - 1, NULL, 0);
    xTaskCreate(servoDriver,"servoDriver",4096,NULL,2,NULL);
    xTaskCreate(send_data_task, "SendDataTask", 2048, NULL, 5, NULL);

    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}