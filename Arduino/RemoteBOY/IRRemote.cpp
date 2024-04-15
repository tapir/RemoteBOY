#include "IRRemote.h"
#include <Arduino.h>

static const uint8_t  PIN_LED_IR     = D5;
static const uint8_t  IR_DELAY       = 50;
static const uint32_t IR_REMOTE_NONE = 0;

IRRemote::IRRemote(void) { }

IRRemote::~IRRemote(void) {
    delete this->irSender;
}

void IRRemote::setup(void) {
    this->irSender = new IRsend(PIN_LED_IR);
    this->irSender->begin();
}

void IRRemote::release(void) {
    this->repeatCommand = IR_REMOTE_NONE;
}

void IRRemote::loop(void) {
    if (this->repeatCommand != IR_REMOTE_NONE) {
        this->press(this->repeatCommand, false);
    }
}

void IRRemote::press(uint32_t irCode, bool repeat) {
    if (repeat) {
        this->repeatCommand = irCode;
    }
    static uint32_t lastSend = 0;
    if (millis() - lastSend > IR_DELAY) {
        this->irSender->sendNEC(irCode);
        lastSend = millis();
    }
}