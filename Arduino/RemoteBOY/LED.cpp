#include "LED.h"
#include <Arduino.h>

static const uint32_t LED_TOTAL_ON_TIME   = 5000; // stay on for 5 seconds
static const uint32_t LED_BLINK_FREQUENCY = 500;  // blink every 0.5 seconds
static const uint8_t  PIN_LED_2           = D3;
static const uint8_t  PIN_LED_1           = D4;
static const uint8_t  ledPins[NUM_LEDS]   = { PIN_LED_2, PIN_LED_1 };

LEDs::LEDs(void) { }

void LEDs::setup(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
        this->state[i].state           = false;
        this->state[i].blink           = false;
        this->state[i].endless         = false;
        this->state[i].lastUpdateTotal = millis();
        this->state[i].lastUpdateBlink = millis();
    }
}

void LEDs::loop(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t currentTime = millis();
        if (this->state[i].state) {
            // check if it's time to turn LED off
            if (!this->state[i].endless && (currentTime - this->state[i].lastUpdateTotal > LED_TOTAL_ON_TIME)) {
                this->turnOff(i);
            }
            // check if it's time to toggle for blink
            if (this->state[i].blink && (currentTime - this->state[i].lastUpdateBlink > LED_BLINK_FREQUENCY)) {
                digitalWrite(ledPins[i], !digitalRead(ledPins[i]));
                this->state[i].lastUpdateBlink += LED_BLINK_FREQUENCY;
            }
        }
    }
}

void LEDs::turnOn(uint8_t ledNum) {
    digitalWrite(ledPins[ledNum], HIGH);
    this->state[ledNum].state           = true;
    this->state[ledNum].lastUpdateTotal = millis();
}

void LEDs::turnOff(uint8_t ledNum) {
    digitalWrite(ledPins[ledNum], LOW);
    this->state[ledNum].state = false;
}

void LEDs::setBlinkFlag(bool blink) {
    for (int i = 0; i < NUM_LEDS; i++) {
        this->state[i].blink           = blink;
        this->state[i].lastUpdateBlink = millis();
    }
}

bool LEDs::getBlinkFlag(void) {
    bool indicator = true;
    for (int i = 0; i < NUM_LEDS; i++) {
        indicator &= this->state[i].blink;
    }
    return indicator;
}

bool LEDs::getPairing(void) {
    bool indicator = true;
    for (int i = 0; i < NUM_LEDS; i++) {
        indicator &= this->state[i].endless;
        indicator &= this->state[i].blink;
        indicator &= this->state[i].state;
    }
    return indicator;
}

void LEDs::setPairing(bool pairing) {
    for (int i = 0; i < NUM_LEDS; i++) {
        this->state[i].endless = pairing;
        this->state[i].blink   = pairing;
        this->state[i].state   = pairing;
        digitalWrite(ledPins[i], pairing);
    }
}