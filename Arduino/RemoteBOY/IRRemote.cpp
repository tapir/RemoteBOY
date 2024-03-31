#include "IRRemote.h"

#include <Arduino.h>

static const uint8_t PIN_LED_IR = D5;

IRRemote::IRRemote() { }

void IRRemote::setup(void) {
    pinMode(PIN_LED_IR, OUTPUT);
}