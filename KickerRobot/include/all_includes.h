#ifndef __allIncludes__
#define __allIncludes__
#include <string>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/twai.h"
#include "driver/ledc.h"


#include "esp_system.h"
#include "esp_wifi.h"
#include "espnow.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "espnow_ctrl.h"
#include "espnow_utils.h"

#include "esp_timer.h"
#include "led_strip.h"

#define assertS(x) while((x)){}
#define map(a,b,c,w,q) ((w)+(((a) - (b)) * ((q) - (w)))/((c)-(b)))


enum motor_status_list{ENABLED, DISABLED, ERRORLESS,CALIBRATING, IDLE};
enum servo_status_list{LATCHED, RELEASED, DETACHED};
enum controller_status_list{CONNECTED, DISCONNECTED};

typedef struct {
    float battery_voltage; 
} esp_now_data_to_send;


typedef struct {
    uint8_t left_motor_speed;
    uint8_t right_motor_speed;
    float kp;
    float ki;
    float kd;
    uint8_t servo_and_motor_state;
    uint8_t motor_speed_setting;
} esp_now_data_to_receive;

#endif