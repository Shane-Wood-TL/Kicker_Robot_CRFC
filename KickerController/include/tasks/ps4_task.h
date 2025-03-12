#ifndef __ps4_task__
#define __ps4_task__

#include "../all_includes.h"
#include "../pinout.h"
#include "../drivers/ps4.h"

extern std::atomic<uint8_t> controller_byte_2;
extern std::atomic<uint8_t> controller_byte_4;
extern std::atomic<uint8_t> controller_byte_5;
extern std::atomic<uint8_t> controller_byte_6;

/**
 * @brief This is the task that handles the usb for the ps4 controller
 */
void ps4_task(void *pv);
#endif


