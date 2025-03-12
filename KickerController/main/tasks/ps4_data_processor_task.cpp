#include "../../include/tasks/ps4_data_processor_task.h"

void ps4_data_processor(void *pv) {
    for (;;) {
      if (xSemaphoreTake(new_controller_data, portMAX_DELAY)) {
        display_controller_data current_display_data = {0, 0};
        uint8_t temp_buttons = 0;
  
        uint8_t dpad = (controller_byte_5 & lower_four_bits);
        if (dpad == dpad_left_byte) {
          temp_buttons |= (1 << processed_dpad_left_bit); // left
        } else if (dpad == dpad_down_byte) {
          temp_buttons |= (1 << processed_dpad_down_bit); // down
        } else if (dpad == dpad_right_byte) {
          temp_buttons |= (1 << processed_dpad_right_bit); // right
        } else if (dpad == dpad_up_byte) {
          temp_buttons |= (1 << processed_dpad_up_bit);// up
        }
  
        if (controller_byte_5 & triangle_byte) {
          temp_buttons |= (1 << processed_triangle_bit); // triangle
        } else if (controller_byte_5 & square_byte) {
          temp_buttons |= (1 << processed_square_bit);   // square
        } else if (controller_byte_5 & circle_byte) {
          temp_buttons |= (1 << processed_circle_bit);   // circle
        } else if (controller_byte_5 & x_byte) {
          temp_buttons |= (1 << processed_x_bit);        // x
        }
  
        current_display_data.buttons = temp_buttons;
  
        if (controller_byte_6 & options_byte) {
          current_display_data.options_triggers |= (1 << processed_options_bit); // options
        } else if (controller_byte_6 & r2_byte) {
          current_display_data.options_triggers |= (1 << processed_r2_bit); // r2
        } else if (controller_byte_6 & l2_byte) {
          current_display_data.options_triggers |= (1 << processed_l2_bit);      // l2
        }
        xQueueSend(other_controller_data_queue, &current_display_data, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(ps4_data_processor_wait_time));
      }
    }
  }