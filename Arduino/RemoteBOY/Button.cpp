#include "Button.h"

Button::Button(void) { }

void Button::setup(int buttonID, uint16_t stateMask, void (*stateChangeCallback)(int, bool), int (*readPinCallback)(int)) {
    this->stateChangeCallback = stateChangeCallback;
    this->readPinCallback     = readPinCallback;
    this->buttonID            = buttonID;
    this->samples             = 0;
    this->state               = false;
    this->stateMask           = stateMask;
}

// this simple debouncing routine is taken from
// https://www.e-tinkers.com/2021/05/the-simplest-button-debounce-solution/
// the idea is that you sample the button state multiple times until the sample looks like 0xFFFF
// depending on how stateMask looks you can get up to 16 consecutive samples or less
// 0x0000 -> 16 samples // 0xF000 -> 12 samples // 0xFF00 ->  8 samples
void Button::loop(void) {
    bool reading  = this->readPinCallback(this->buttonID);
    this->samples = (this->samples << 1) | (this->state ^ reading) | this->stateMask;
    if (state == 0xFFFF) {
        this->state = reading;
        this->stateChangeCallback(this->buttonID, this->state);
    }
}

bool Button::isPressed(void) {
    return this->state;
}