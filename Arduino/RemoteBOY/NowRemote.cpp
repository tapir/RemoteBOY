#include "NowRemote.h"
#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <string.h>

static const uint32_t      ESPNOW_CLICK_DELAY = 100;
static const uint8_t       WIFI_CHANNEL       = 6;
static const uint8_t       peerMac[6]         = { 0xF4, 0x12, 0xFA, 0x7A, 0x6A, 0xF8 };
static esp_now_peer_info_t peerInfo;

NowRemote::NowRemote(void) { }

void NowRemote::setup(void) {
    // init wifi
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

    // init espnow
    esp_now_init();
    memcpy(peerInfo.peer_addr, peerMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
}

void NowRemote::press(const uint8_t* buttonID) {
    this->send(buttonID, true);
}

void NowRemote::release(const uint8_t* buttonID) {
    this->send(buttonID, false);
}

void NowRemote::click(const uint8_t* buttonID) {
    this->send(buttonID, true);
    delay(ESPNOW_CLICK_DELAY);
    this->send(buttonID, false);
}

void NowRemote::send(const uint8_t* buttonID, bool state) {
    uint8_t data = state | buttonID[0] << 1 | buttonID[1] << 2;
    esp_now_send(peerMac, &data, 1);
}

void NowRemote::prepareSleep(void) {
    esp_now_deinit();
    esp_wifi_stop();
}

void NowRemote::prepareWakeUp(void) {
    esp_wifi_start();
    esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_now_init();
    esp_now_add_peer(&peerInfo);
}