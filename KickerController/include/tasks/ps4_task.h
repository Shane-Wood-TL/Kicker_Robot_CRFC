/**
 * @file ps4_task.h
 * @brief This is the file that contains the ps4 task
 * @details This task is responsible for handling the usb for the ps4 controller
 * @author Shane Wood
 */
#ifndef __ps4_task__
#define __ps4_task__

#include "../all_includes.h"
#include "../drivers/ps4.h"
#include "../pinout.h"

extern std::atomic<uint8_t> controller_byte_2;
extern std::atomic<uint8_t> controller_byte_3;
extern std::atomic<uint8_t> controller_byte_5;
extern std::atomic<uint8_t> controller_byte_6;

/**
 * @brief This is the task that handles the usb for the ps4 controller
 */
void ps4_task(void *pv);
#endif
