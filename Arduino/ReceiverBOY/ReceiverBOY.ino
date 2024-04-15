#include "ESPNow.h"
#include "USBHIDGamepad.h"
#include <USB.h>

static ESPNow nowReceiver;
USBHIDGamepad gamepad;

void receiverCallback(const uint8_t* mac, const uint8_t* data, int dataLen) {
    if (dataLen == 3) {
        bool state = data[0];
        if (data[1]) {
            state ? gamepad.pressButton(data[2]) : gamepad.releaseButton(data[2]);
        } else {
            state ? gamepad.hat(data[2]) : gamepad.hat(HAT_CENTER);
        }
    }
}

void setup() {
    nowReceiver.setup(&receiverCallback);
    gamepad.begin();
    USB.begin();
    // Serial.begin(115200);
}

void loop() {
}
