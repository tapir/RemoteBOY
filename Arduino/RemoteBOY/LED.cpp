#include "LED.h"
#include <Arduino.h>

static const uint8_t PIN_LED_2         = D3;
static const uint8_t PIN_LED_1         = D4;
static const uint8_t ledPins[NUM_LEDS] = { PIN_LED_2, PIN_LED_1 };

LEDs::LEDs(void) { }

void LEDs::setup(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
        this->state[i] = false;
    }
}

void LEDs::toggle(const uint8_t ledNum) {
    this->state[ledNum] = !this->state[ledNum];
    digitalWrite(ledPins[ledNum], this->state[ledNum]);
}

void LEDs::turnOn(const uint8_t ledNum) {
    if (!this->state[ledNum]) {
        digitalWrite(ledPins[ledNum], HIGH);
        this->state[ledNum] = true;
    }
}

void LEDs::turnOff(const uint8_t ledNum) {
    if (this->state[ledNum]) {
        digitalWrite(ledPins[ledNum], LOW);
        this->state[ledNum] = false;
    }
}