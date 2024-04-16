#include "USBHIDGamepad.h"
#include <USB.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <stdint.h>

static const char     USB_PRODUCT_NAME[] = "RemoteBOY";
static const char     USB_MFG_NAME[]     = "Lola Engineering";
static const uint16_t USB_DEV_VID        = 0x045E; // Microsoft
static const uint16_t USB_DEV_PID        = 0x02D1; // Xbox One Pad
static const uint8_t  WIFI_CHANNEL       = 6;
static USBHIDGamepad  gamepad;

// first bit:   pressed/released
// second bit:  button/hat
// last 6 bits: button or hat ID
void receiverCallback(const uint8_t* mac, const uint8_t* data, int dataLen) {
    if (dataLen != 1) {
        return;
    }

    bool    state    = data[0] & 0b00000001;
    bool    isButton = (data[0] & 0b00000010) >> 1;
    uint8_t button   = (data[0] & 0b11111100) >> 2;

    if (isButton) {
        state ? gamepad.pressButton(button) : gamepad.releaseButton(button);
    } else {
        state ? gamepad.hat(button) : gamepad.hat(HAT_CENTER);
    }
}

void setup() {
    // turn off LCD for T-Dongle-S3
    digitalWrite(38, HIGH);

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
    esp_now_register_recv_cb(&receiverCallback);

    // start gamepad and usb
    gamepad.begin();
    USB.VID(USB_VID);
    USB.PID(USB_PID);
    USB.productName(USB_PRODUCT_NAME);
    USB.manufacturerName(USB_MFG_NAME);
    USB.begin();
}

void loop() {
}
