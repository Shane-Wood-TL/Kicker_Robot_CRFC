/**
 * @file send_data_task.cpp
 * @brief This is the file that contains the send data task
 * @details This task is responsible for sending the data to the robot
 * from the controller, it sends the motor speeds, servo status, and other
 * values that are set in the controller menu
 * @author Shane Wood
 */
#include "../../include/tasks/send_data_task.h"



// task that sends controller data
void send_data_task(void *pvParameter) {
  static esp_now_data_to_send current_transmission;


  //get the initial network channel
  static uint8_t last_network_channel = 0;
  if (xSemaphoreTake(network_channel_mutex, portMAX_DELAY) == pdTRUE) {
    last_network_channel = current_network_channel;
    xSemaphoreGive(network_channel_mutex);
  }

  while (1) {
    if (ps4_controller_connected) {
      //only controller data if the ps4 controller is connected

      //raw data from the joysticks
      current_transmission.driving_speed = controller_byte_2;
      current_transmission.turning_speed = controller_byte_3;

      //ramped velocity values
      if (xSemaphoreTake(ramped_velocity_mutex, portMAX_DELAY) == pdTRUE) {
        current_transmission.velocity_ramp_limit = velocity_ramp_limit;
        current_transmission.velocity_gain = velocity_gain;
        current_transmission.velocity_integrator_gain = velocity_integrator_gain;
        xSemaphoreGive(ramped_velocity_mutex);
      }

      //servo status value
      if (xSemaphoreTake(servo_status_mutex, portMAX_DELAY) == pdTRUE) {
        current_transmission.servo_and_motor_state = (servo_status & lower_four_bits);
        xSemaphoreGive(servo_status_mutex);
      }

      //motor status value
      if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY) == pdTRUE) {
        if (boost && motor_status == IDLE) {
          current_transmission.servo_and_motor_state |= ((BOOSTED & lower_four_bits) left_shift_4);
        } else {
          current_transmission.servo_and_motor_state |= ((motor_status & lower_four_bits) left_shift_4);
        }
        xSemaphoreGive(motor_status_mutex);
      }

      //motor speed values
      if (xSemaphoreTake(motor_speeds_mutex, portMAX_DELAY) == pdTRUE) {
        current_transmission.motor_speed_setting = ((drive_motor_speed & lower_four_bits) left_shift_4) | (turn_motor_speed & lower_four_bits);
        xSemaphoreGive(motor_speeds_mutex);
      }
    } else {
      // if the controller is not connected, send 0 values and disable motors
      current_transmission.driving_speed = 0;
      current_transmission.turning_speed = 0;
      current_transmission.servo_and_motor_state = (LATCHED & lower_four_bits);
      current_transmission.servo_and_motor_state |= ((DISABLED & lower_four_bits) left_shift_4);
    }

    //update the network channel even if the ps4 controller is not connected
    if (xSemaphoreTake(network_channel_mutex, portMAX_DELAY) == pdTRUE) {
      current_transmission.esp_now_channel = current_network_channel;
      if (current_network_channel != last_network_channel) {
        //set the new network channel
        esp_wifi_set_channel(current_network_channel, WIFI_SECOND_CHAN_NONE);
        last_network_channel = current_network_channel;

        //update peer settings
        esp_now_del_peer(robotMacAddress);
        esp_now_peer_info_t peer_info = {};
        memcpy(peer_info.peer_addr, robotMacAddress, mac_address_length);
        peer_info.channel = current_network_channel;
        peer_info.encrypt = false;
        ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
      }
      xSemaphoreGive(network_channel_mutex);
    }

    //transmit the message to the robot
    (void) esp_now_send(robotMacAddress, (uint8_t *) &current_transmission, sizeof(esp_now_data_to_send));

    vTaskDelay(pdMS_TO_TICKS(send_data_task_wait_time));
  }
}
