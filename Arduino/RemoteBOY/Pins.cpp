#include "Pins.h"

void setupPins(void) {
  pinMode(PIN_BATTERY, INPUT);
  pinMode(PIN_COL_1, OUTPUT);
  pinMode(PIN_COL_2, OUTPUT);
  pinMode(PIN_ROW_1, INPUT_PULLUP);
  pinMode(PIN_ROW_2, INPUT_PULLUP);
  pinMode(PIN_ROW_3, INPUT_PULLUP);
  pinMode(PIN_ROW_4, INPUT_PULLUP);
  pinMode(PIN_ROW_5, INPUT_PULLUP);
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_IR, OUTPUT);

  digitalWrite(PIN_COL_1, HIGH);
  digitalWrite(PIN_COL_2, HIGH);
}