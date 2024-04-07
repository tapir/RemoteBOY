#include "Button.h"
#include <Arduino.h>

static const uint32_t BTN_DEBOUNCE_DELAY = 15;

Button::Button(void) { }

void Button::setup(const uint8_t buttonID, int (*stateChangeCallback)(const uint8_t, bool), int (*readPinCallback)(const uint8_t)) {
    this->stateChangeCallback = stateChangeCallback;
    this->readPinCallback     = readPinCallback;
    this->buttonID            = buttonID;
    this->state               = false;
}

int Button::loop(void) {
    static bool     hold             = false;
    static uint32_t lastDebounceTime = 0;

    if (!hold) {
        bool reading = this->readPinCallback(this->buttonID);
        if (reading != this->state) {
            this->state      = reading;
            lastDebounceTime = millis();
            hold             = true;
            int err          = this->stateChangeCallback(this->buttonID, reading);
            if (err < BTN_EXIT_SUCCESS) {
                return err;
            }
        }
    } else {
        if (millis() - lastDebounceTime > BTN_DEBOUNCE_DELAY) {
            hold = false;
        }
    }
    return BTN_EXIT_SUCCESS;
}

bool Button::isPressed(void) {
    return this->state;
}