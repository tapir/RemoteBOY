#include "IRRemote.h"
#include <Arduino.h>

static const uint8_t PIN_LED_IR = D5;
static const uint8_t IR_DELAY   = 50;

IRRemote::IRRemote(void) { }

IRRemote::~IRRemote(void) {
    delete this->irSender;
}

void IRRemote::setup(void) {
    this->irSender = new IRsend(PIN_LED_IR);
    this->irSender->begin();
    this->lastSend = 0;
}

void IRRemote::send(uint32_t irCode) {
    if (millis() - this->lastSend > IR_DELAY) {
        this->irSender->sendNEC(irCode);
        this->lastSend = millis();
    }
}
