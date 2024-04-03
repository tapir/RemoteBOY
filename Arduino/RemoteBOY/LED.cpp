#include "LED.h"
#include <Arduino.h>

static const uint32_t LED_TOTAL_ON_TIME   = 3000; // stay on for 5 seconds
static const uint32_t LED_BLINK_FREQUENCY = 500;  // blink every 0.5 seconds
static const uint8_t  PIN_LED_2           = D3;
static const uint8_t  PIN_LED_1           = D4;
static const uint8_t  ledPins[NUM_LEDS]   = { PIN_LED_2, PIN_LED_1 };

LEDs::LEDs(void) { }

void LEDs::setup(void) {
    this->endless         = false;
    this->blink           = false;
    this->lastUpdateBlink = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
        this->state[i].state           = false;
        this->state[i].lastUpdateTotal = 0;
    }
}

void LEDs::loop(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        // check if it's time to turn LED off
        if (this->state[i].state && !this->endless && (millis() - this->state[i].lastUpdateTotal > LED_TOTAL_ON_TIME)) {
            this->setBlinkFlag(false);
            this->turnOff(i);
            return;
        }
    }
    // check if it's time to toggle for blink
    if (this->blink && (millis() - this->lastUpdateBlink > LED_BLINK_FREQUENCY)) {
        for (int i = 0; i < NUM_LEDS; i++) {
            if (this->state[i].state) {
                digitalWrite(ledPins[i], !digitalRead(ledPins[i]));
            }
        }
        this->lastUpdateBlink += LED_BLINK_FREQUENCY;
    }
}

void LEDs::turnOn(uint8_t ledNum) {
    if (!this->state[ledNum].state) {
        digitalWrite(ledPins[ledNum], HIGH);
        this->state[ledNum].state           = true;
        this->state[ledNum].lastUpdateTotal = millis();
    }
}

void LEDs::turnOff(uint8_t ledNum) {
    if (this->state[ledNum].state) {
        digitalWrite(ledPins[ledNum], LOW);
        this->state[ledNum].state = false;
    }
}

// sets all LEDs blinking
void LEDs::setBlinkFlag(bool blink) {
    if (blink && !this->blink) {
        this->lastUpdateBlink = millis();
    }
    this->blink = blink;
}

bool LEDs::getBlinkFlag(void) {
    return this->blink;
}

// sets all LEDs endless
void LEDs::setEndlessFlag(bool endless) {
    this->endless = endless;
}

bool LEDs::getEndlessFlag(void) {
    return this->endless;
}

// // set all flags necessary to indicate for bluetooth pairing
// // both LEDs will blink at the same time endlessly
// void LEDs::setPairing(bool pairing) {
//     for (int i = 0; i < NUM_LEDS; i++) {
//         this->state[i].endless = pairing;
//         this->setBlinkFlag(pairing);
//         pairing ? this->turnOn(ledPins[i]) : this->turnOff(ledPins[i]);
//         // this->state[i].blink   = pairing;
//         // this->state[i].state   = pairing;
//         // digitalWrite(ledPins[i], pairing);
//     }
// }

// bool LEDs::getPairing(void) {
//     bool indicator = true;
//     for (int i = 0; i < NUM_LEDS; i++) {
//         indicator &= this->state[i].endless;
//         indicator &= this->state[i].blink;
//         indicator &= this->state[i].state;
//     }
//     return indicator;
// }