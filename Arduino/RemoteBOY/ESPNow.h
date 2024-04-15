#ifndef ESPNOW_H
#define ESPNOW_H

#include <stdint.h>

static const uint8_t ESPNOW_HID_SELECT[2] = { 1, 0 };  // Menu Pick (Select)
static const uint8_t ESPNOW_HID_BACK[2]   = { 1, 1 };  // Menu Escape (Back)
static const uint8_t ESPNOW_HID_UP[2]     = { 0, 1 };  // Menu Up
static const uint8_t ESPNOW_HID_DOWN[2]   = { 0, 5 };  // Menu Down
static const uint8_t ESPNOW_HID_LEFT[2]   = { 0, 7 };  // Menu Left
static const uint8_t ESPNOW_HID_RIGHT[2]  = { 0, 3 };  // Menu Right
static const uint8_t ESPNOW_HID_HOME[2]   = { 1, 12 }; // Home
static const uint8_t ESPNOW_HID_NONE[2]   = { 0, 0 };  // None

class ESPNow {
public:
    ESPNow(void);
    void setup(void);
    void press(const uint8_t* buttonHIDCode);
    void release(const uint8_t* buttonHIDCode);
    void prepareSleep(void);
    void prepareWakeUp(void);

private:
    void send(const uint8_t* buttonHIDCode, bool state);
};

#endif // ESPNOW_H