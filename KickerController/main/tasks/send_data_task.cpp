#include "../../include/tasks/send_data_task.h"

// task that sends controller data
void send_data_task(void *pvParameter) {
    TickType_t last_wake_time = xTaskGetTickCount();
    while (1) {
      bool ps4_connected = false;
      static esp_now_data_to_send current_transmission;
      if (xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)) {
        if (contoller_connected == "Conn") { ps4_connected = true; }
        xSemaphoreGive(main_menu_values_mutex);
      }
  
      if (ps4_connected) {
        current_transmission.left_motor_speed = controller_byte_2;
        current_transmission.right_motor_speed = controller_byte_4;
  
        if (xSemaphoreTake(ramped_velocity_mutex, portMAX_DELAY)) {
          current_transmission.velocity_ramp_limit = velocity_ramp_limit;
          current_transmission.velocity_gain = velocity_gain;
          current_transmission.velocity_integrator_gain = velocity_integrator_gain;
          xSemaphoreGive(ramped_velocity_mutex);
        }
        if (xSemaphoreTake(servo_status_mutex, portMAX_DELAY)) {
          current_transmission.servo_and_motor_state = (servo_status & lower_four_bits);
          xSemaphoreGive(servo_status_mutex);
        }
        if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY)) {
          current_transmission.servo_and_motor_state |= ((motor_status & lower_four_bits) left_shift_4);
          xSemaphoreGive(motor_status_mutex);
        }
        if (xSemaphoreTake(motor_speeds_mutex, portMAX_DELAY)) {
          current_transmission.motor_speed_setting = ((left_motor_speed & lower_four_bits) left_shift_4) | (right_motor_speed & lower_four_bits);
          xSemaphoreGive(motor_speeds_mutex);
        }
      } else {
        // if the controller is not connected, send 0 values
        current_transmission.left_motor_speed = 0;
        current_transmission.right_motor_speed = 0;
        current_transmission.servo_and_motor_state = 0;
        current_transmission.servo_and_motor_state |= ((DISABLED & lower_four_bits) left_shift_4);
      }
      (void) esp_now_send(robotMacAddress, (uint8_t *) &current_transmission, sizeof(esp_now_data_to_send));

      TickType_t end_time = xTaskGetTickCount();
      TickType_t time_taken = end_time - last_wake_time;
      
      // If time taken was less than 25ms, adjust wait time
      if (time_taken < pdMS_TO_TICKS(send_data_task_wait_time)) {
          vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(send_data_task_wait_time) - time_taken);
      } else {
          vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(send_data_task_wait_time)); // Still maintain 25ms wait
      }
    }
  }
  