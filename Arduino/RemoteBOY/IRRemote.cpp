#include "IRRemote.h"
#include <Arduino.h>

static const uint8_t  PIN_LED_IR          = D5;
static const uint32_t IR_REPEAT_FREQUENCY = 108; // ms

IRRemote::IRRemote(void) { }

void IRRemote::setup(void) {
    pinMode(PIN_LED_IR, OUTPUT);
    digitalWrite(PIN_LED_IR, LOW);
    this->lastRepeat = 0;
    this->irCode     = IR_REMOTE_NONE;
    this->repeat     = false;
}

void IRRemote::press(uint16_t irCode, bool repeat) {
    this->lastRepeat = millis();
    this->modulateStart();
    this->modulateAddress(irCode);
    this->modulateData(irCode);
    this->modulateEnd();
    this->repeat = repeat;
}

void IRRemote::release(void) {
    this->repeat = false;
}

void IRRemote::loop(void) {
    if (millis() - this->lastRepeat >= IR_REPEAT_FREQUENCY) {
        this->lastRepeat += IR_REPEAT_FREQUENCY;
        this->modulateRepeat();
    }
}

// gets bits of a byte from LSB (index = 0) to MSB (index = 7)
bool IRRemote::getBits(uint16_t irCode, bool address, uint8_t index) {
    uint8_t c = irCode & 0xFF;
    if (address) {
        c = (irCode >> 8) & 0xFF;
    }
    return (c >> index) & 1;
}

void IRRemote::modulateOne(void) {
    digitalWrite(PIN_LED_IR, HIGH);
    delayMicroseconds(562);
    digitalWrite(PIN_LED_IR, LOW);
    delayMicroseconds(1687);
}

void IRRemote::modulateZero(void) {
    digitalWrite(PIN_LED_IR, HIGH);
    delayMicroseconds(562);
    digitalWrite(PIN_LED_IR, LOW);
    delayMicroseconds(562);
}

void IRRemote::modulateStart(void) {
    digitalWrite(PIN_LED_IR, HIGH);
    delayMicroseconds(9000);
    digitalWrite(PIN_LED_IR, LOW);
    delayMicroseconds(4500);
}

void IRRemote::modulateEnd(void) {
    digitalWrite(PIN_LED_IR, HIGH);
    delayMicroseconds(562);
    digitalWrite(PIN_LED_IR, LOW);
}

void IRRemote::modulateRepeat(void) {
    digitalWrite(PIN_LED_IR, HIGH);
    delayMicroseconds(9000);
    digitalWrite(PIN_LED_IR, LOW);
    delayMicroseconds(2250);
    digitalWrite(PIN_LED_IR, HIGH);
    delayMicroseconds(562);
    digitalWrite(PIN_LED_IR, LOW);
}

void IRRemote::modulateAddress(uint16_t irCode) {
    // modulate address
    for (int i = 0; i < 8; i++) {
        this->getBits(irCode, true, i) ? this->modulateOne() : this->modulateZero();
    }
    // modulate bit inverse of address
    for (int i = 0; i < 8; i++) {
        this->getBits(irCode, true, i) ? this->modulateZero() : this->modulateOne();
    }
}

void IRRemote::modulateData(uint16_t irCode) {
    // modulate data
    for (int i = 0; i < 8; i++) {
        this->getBits(irCode, false, i) ? this->modulateOne() : this->modulateZero();
    }
    // modulate bit inverse of data
    for (int i = 0; i < 8; i++) {
        this->getBits(irCode, false, i) ? this->modulateZero() : this->modulateOne();
    }
}