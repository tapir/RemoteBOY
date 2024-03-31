#include "LED.h"
#include <Arduino.h>

static const uint32_t LED_TOTAL_ON_TIME	  = 5000; // stay on for 5 seconds
static const uint32_t LED_BLINK_FREQUENCY = 500;  // blink every 0.5 seconds
static const uint8_t  PIN_LED_2			  = D3;
static const uint8_t  PIN_LED_1			  = D4;
static const uint8_t  ledPins[2]		  = { PIN_LED_2, PIN_LED_1 };

LED::LED(void) { }

void LED::setup(void) {
	pinMode(PIN_LED_2, OUTPUT);
	pinMode(PIN_LED_1, OUTPUT);
	digitalWrite(PIN_LED_2, LOW);
	digitalWrite(PIN_LED_1, LOW);
	for (int i = 0; i < 2; i++) {
		this->state[i].state		   = false;
		this->state[i].blink		   = false;
		this->state[i].lastUpdateTotal = millis();
		this->state[i].lastUpdateBlink = millis();
	}
}

void LED::loop(void) {
	for (int i = 0; i < 2; i++) {
		uint32_t currentTime = millis();
		if (this->state[i].state) {
			// check if it's time to turn LED off
			if (currentTime - this->state[i].lastUpdateTotal > LED_TOTAL_ON_TIME) {
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

void LED::turnOn(int ledNum) {
	digitalWrite(ledPins[ledNum], HIGH);
	this->state[ledNum].state			= true;
	this->state[ledNum].lastUpdateTotal = millis();
}

void LED::turnOff(int ledNum) {
	digitalWrite(ledPins[ledNum], LOW);
	this->state[ledNum].state = false;
}

void LED::setBlinkFlag(int ledNum, bool blink) {
	this->state[ledNum].blink			= blink;
	this->state[ledNum].lastUpdateBlink = millis();
}