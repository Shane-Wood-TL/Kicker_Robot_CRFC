#ifndef __all_includes__
#define __all_includes__
//standard C includes
#include <stdio.h>
#include <string>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <atomic>

typedef struct {
    uint8_t buttons;
} transmitter_controller_data;

typedef struct {
    uint8_t buttons;
    uint8_t options_triggers;
} display_controller_data;

//rtos includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

//esp includes
#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "errno.h"
#include "esp_system.h"
#include "esp_mac.h"

//esp now includes
#include "esp_wifi.h"
#include "espnow.h"
#include "espnow_ctrl.h"
#include "espnow_utils.h"
#include "esp_crc.h"
#include "esp_netif.h"
#include "nvs_flash.h"

//usb includes
#include "usb/usb_host.h"
#include "usb/hid_host.h"

//drives includes
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#include "pinout.h"



enum motor_status_list{ENABLED, DISABLED, ERRORLESS,CALIBRATING, IDLE};
enum servo_status_list{LATCHED, RELEASED, DETACHED};
#endif


