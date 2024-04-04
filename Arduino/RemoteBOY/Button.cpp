#include "Button.h"
#include <Arduino.h>

static const uint32_t BTN_DEBOUNCE_DELAY = 15;

Button::Button(void) { }

void Button::setup(const uint8_t buttonID, int (*stateChangeCallback)(const uint8_t, bool), int (*readPinCallback)(const uint8_t)) {
    this->stateChangeCallback = stateChangeCallback;
    this->readPinCallback     = readPinCallback;
    this->buttonID            = buttonID;
    this->lastDebounceTime    = 0;
    this->state               = false;
    this->lastState           = false;
}

int Button::loop(void) {
    bool reading = this->readPinCallback(this->buttonID);
    if (reading != this->lastState) {
        this->lastDebounceTime = millis();
    }
    if (millis() - this->lastDebounceTime > BTN_DEBOUNCE_DELAY) {
        if (reading != this->state) {
            this->state = reading;
            int err     = this->stateChangeCallback(this->buttonID, reading);
            if (err < BTN_EXIT_SUCCESS) {
                return err;
            }
        }
    }
    this->lastState = reading;
    return BTN_EXIT_SUCCESS;
}

bool Button::isPressed(void) {
    return this->state;
}