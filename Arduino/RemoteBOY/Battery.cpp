#include "Battery.h"
#include <Arduino.h>

static const uint32_t BATT_UPDATE_FREQUENCY = 20000; // only update every 20s
static const float	  BATT_MAX_VOLTAGE		= 3.6;
static const float	  BATT_MIN_VOLTAGE		= 3.2;
static const uint8_t  PIN_BATTERY			= A0;

Battery::Battery(void) { }

void Battery::setup(void) {
	pinMode(PIN_BATTERY, INPUT);
	this->level		 = 100;
	this->lastUpdate = 0;
	this->loop();
}

void Battery::loop(void) {
	// we only take voltage reading every UPDATE_FREQUENCY
	uint32_t currentTime = millis();
	if (currentTime - this->lastUpdate > BATT_UPDATE_FREQUENCY) {
		this->lastUpdate += BATT_UPDATE_FREQUENCY;

		// take 16 samples
		uint32_t v = 0;
		for (int i = 0; i < 16; i++) {
			v += analogReadMilliVolts(PIN_BATTERY);
		}

		// we have voltage divider of 2 and 16 samples, converted from mV to V
		float vf = v * 8000;

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