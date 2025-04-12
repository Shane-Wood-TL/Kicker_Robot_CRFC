/**
 * @file esp_now_setup.cpp
 * @brief source file that sets up esp-now
 * @author Shane Wood
 * @date 1/15/2025
 */

#include "../../include/setup/esp_now_setup.h"



void esp_now_setup() {
  // setup nvs (non-volatile storage)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // set up wifi in station mode
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  //get the current network channel
  if (xSemaphoreTake(network_channel_mutex, portMAX_DELAY) == pdTRUE) {
    esp_wifi_set_channel(current_network_channel, WIFI_SECOND_CHAN_NONE);
    xSemaphoreGive(network_channel_mutex);
  }


  // setup esp-now
  ESP_ERROR_CHECK(esp_now_init());

  // set up callback functions for sending and receiving data
  (void) esp_now_register_send_cb(on_send);
  (void) esp_now_register_recv_cb(on_receive);


  // set up the peer info for the kicker
  esp_now_peer_info_t peer_info = {};
  memcpy(peer_info.peer_addr, robotMacAddress, mac_address_length);
  if (xSemaphoreTake(network_channel_mutex, portMAX_DELAY) == pdTRUE) {
    peer_info.channel = current_network_channel;
    xSemaphoreGive(network_channel_mutex);
  }
  peer_info.encrypt = false;
  ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
}