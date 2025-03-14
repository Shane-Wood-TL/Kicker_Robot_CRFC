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
  (void) esp_now_register_send_cb(on_send);
  (void) esp_now_register_recv_cb(on_receive);

  esp_now_peer_info_t peer_info = {};
  memcpy(peer_info.peer_addr, robotMacAddress, mac_address_length);
  peer_info.channel = 0;
  peer_info.encrypt = false;
  ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
}