#include "ESPNow.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include <string.h>

static const uint8_t       WIFI_CHANNEL = 6;
static const uint8_t       peerMac[6]   = { 0xF4, 0x12, 0xFA, 0x7A, 0x6A, 0xF8 };
static esp_now_peer_info_t peerInfo;

ESPNow::ESPNow(void) { }

void ESPNow::setup(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR);
    esp_now_init();
    memcpy(peerInfo.peer_addr, peerMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
}

void ESPNow::press(const uint8_t* buttonHIDCode) {
    this->send(buttonHIDCode, true);
}

void ESPNow::release(const uint8_t* buttonHIDCode) {
    this->send(buttonHIDCode, false);
}

void ESPNow::send(const uint8_t* buttonHIDCode, bool state) {
    uint8_t data[3];
    data[0] = state;
    data[1] = buttonHIDCode[0];
    data[2] = buttonHIDCode[1];
    esp_now_send(peerMac, data, 3);
}

void ESPNow::prepareSleep(void) {
}

void ESPNow::prepareWakeUp(void) {
}