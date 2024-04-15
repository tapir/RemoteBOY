#include "ESPNow.h"
#include <esp_wifi.h>

static const uint8_t WIFI_CHANNEL = 6;
static const uint8_t peerMac[6]   = { 0xF4, 0x12, 0xFA, 0x7A, 0x6A, 0xF8 };

ESPNow::ESPNow(void) { }

void ESPNow::setup(esp_now_recv_cb_t cb) {
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR);
    esp_now_init();
    esp_now_register_recv_cb(cb);
}