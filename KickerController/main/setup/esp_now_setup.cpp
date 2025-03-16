/**
 * @file esp_now_setup.cpp
 * @brief source file that sets up esp-now
 * @author Shane Wood
 * @date 1/15/2025
 */

#include "../../include/setup/esp_now_setup.h"

void esp_now_setup(){
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

  // setup esp-now
  ESP_ERROR_CHECK(esp_now_init());

  // esp_now_rate_config_t rate_config = {
  //   .phymode = WIFI_PHY_MODE_HT40,
  //   .rate = WIFI_PHY_RATE_6M,
  //   .ersu = false,
  //   .dcm = false
  // };

//  esp_now_set_peer_rate_config(robotMacAddress, &rate_config);

  (void) esp_now_register_send_cb(on_send);
  (void) esp_now_register_recv_cb(on_receive);

  esp_now_peer_info_t peer_info = {};
  memcpy(peer_info.peer_addr, robotMacAddress, mac_address_length);
  peer_info.channel = 0;
  peer_info.encrypt = false;
  ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
}