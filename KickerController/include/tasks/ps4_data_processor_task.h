#ifndef __ps4_data_processor_task__
#define __ps4_data_processor_task__

#include "../all_includes.h"
#include "../pinout.h"

#define dpad_left_byte 0x06
#define dpad_right_byte 0x02
#define dpad_up_byte 0x00
#define dpad_down_byte 0x04

#define triangle_byte 0x80
#define square_byte 0x10
#define circle_byte 0x40
#define x_byte 0x20


#define options_byte 0x20
#define r2_byte 0x08
#define l2_byte 0x04

#define ps4_data_processor_wait_time 10

extern std::atomic<uint8_t> controller_byte_2;
extern std::atomic<uint8_t> controller_byte_4;
extern std::atomic<uint8_t> controller_byte_5;
extern std::atomic<uint8_t> controller_byte_6;


extern SemaphoreHandle_t new_controller_data;

// queue handles
extern QueueHandle_t other_controller_data_queue;

/**
 * @brief This is the task that processes the ps4 controller data, but does not deal with usb directly
 */
void ps4_data_processor(void *pv);
#endif