/**
 * @file ps4_data_processor_task.h
 * @brief This is the file that contains the ps4 data processor task
 * @author Shane Wood
 */
#ifndef __ps4_data_processor_task__
#define __ps4_data_processor_task__

#include "../all_includes.h"
#include "../pinout.h"

#define dpad_left_byte 0x06 ///< dpad left byte
#define dpad_right_byte 0x02 ///< dpad right byte
#define dpad_up_byte 0x00 ///< dpad up byte
#define dpad_down_byte 0x04 ///< dpad down byte

#define triangle_byte 0x80 ///< triangle byte
#define square_byte 0x10 ///< square byte
#define circle_byte 0x40 ///< circle byte
#define x_byte 0x20 ///< x byte


#define options_byte 0x20 ///< options byte
#define r2_byte 0x08 ///< r2 byte
#define l2_byte 0x04 ///< l2 byte

#define ps4_data_processor_wait_time 10 ///< time to wait in the ps4 data processor task

extern std::atomic<uint8_t> controller_byte_2;
extern std::atomic<uint8_t> controller_byte_3;
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