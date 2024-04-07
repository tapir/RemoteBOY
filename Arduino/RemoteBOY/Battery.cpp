#include "Battery.h"
#include <Arduino.h>

static const uint8_t  PIN_BATTERY           = A0;
static const uint32_t BATT_UPDATE_FREQUENCY = 20000; // only update every 20s
static const float    BATT_MAX_VOLTAGE      = 3.9;   // tests show most of a NiMH battery's charge is inbetween 3.3 - 3.9V
static const float    BATT_MIN_VOLTAGE      = 3.3;   // https://lygte-info.dk/review/batteries2012/Ikea%20Ladda%20AAA%20900mAh%20%28White%29%20UK.html

Battery::Battery(void) { }

void Battery::setup(void) {
    pinMode(PIN_BATTERY, INPUT);
    this->level   = 100;
    this->voltage = 0;
    this->loop();
}

void Battery::loop(void) {
    static uint32_t lastUpdate = 0;

    // we only take voltage reading every BATT_UPDATE_FREQUENCY
    if (millis() - lastUpdate > BATT_UPDATE_FREQUENCY) {
        lastUpdate += BATT_UPDATE_FREQUENCY;

        // take 16 samples
        uint32_t v = 0;
        for (int i = 0; i < 16; i++) {
            v += analogReadMilliVolts(PIN_BATTERY);
        }

        // we have voltage divider of 2 and 16 samples, converted from mV to V
        float vf      = v / 8000.0 * 1.0108; // correction factor
        this->voltage = vf;
        if (vf >= BATT_MAX_VOLTAGE) {
            this->level = 100;
        } else if (vf <= BATT_MIN_VOLTAGE) {
            this->level = 0;
        } else {
            this->level = (vf - BATT_MIN_VOLTAGE) * 100 / (BATT_MAX_VOLTAGE - BATT_MIN_VOLTAGE);
        }
    }
}

uint8_t Battery::getLevel(void) {
    return this->level;
}

float Battery::getVoltage(void) {
    return this->voltage;
}