#include "KeyMatrix.h"
#include <Arduino.h>

static const uint8_t PIN_COL_1   =  D6;
static const uint8_t PIN_COL_2   =  D10;
static const uint8_t PIN_ROW_1   =  D2;
static const uint8_t PIN_ROW_2   =  D1;
static const uint8_t PIN_ROW_3   =  D9;
static const uint8_t PIN_ROW_4   =  D8;
static const uint8_t PIN_ROW_5   =  D7;
static const int rowPins[NUM_ROWS] = {PIN_ROW_1, PIN_ROW_2, PIN_ROW_3, PIN_ROW_4, PIN_ROW_5};
static const int colPins[NUM_COLS] = {PIN_COL_1, PIN_COL_2};

KeyMatrix::KeyMatrix(void) {}

void KeyMatrix::setup(void) {
  // setup pins
  pinMode(PIN_COL_1, OUTPUT);
  pinMode(PIN_COL_2, OUTPUT);
  pinMode(PIN_ROW_1, INPUT_PULLUP);
  pinMode(PIN_ROW_2, INPUT_PULLUP);
  pinMode(PIN_ROW_3, INPUT_PULLUP);
  pinMode(PIN_ROW_4, INPUT_PULLUP);
  pinMode(PIN_ROW_5, INPUT_PULLUP);

  // key matrix initial state
  for (int c = 0; c < NUM_COLS; c++) {
    for (int r = 0; r < NUM_ROWS; r++) {
      this->state[r][c] = 0;
    }
    digitalWrite(colPins[c], HIGH);
  }
}

void KeyMatrix::loop(void) {
  for (int c = 0; c < NUM_COLS; c++) {
    digitalWrite(colPins[c], LOW);
    for (int r = 0; r < NUM_ROWS; r++) {
      // pressed = 1, released = 0
      this->state[r][c] = !digitalRead(rowPins[r]);
    }
    digitalWrite(colPins[c], HIGH);
  }
}

int KeyMatrix::getKeyState(size_t row, size_t col) {
  if (row < NUM_ROWS && col < NUM_COLS) {
    return this->state[row][col];
  }
  return 0;
}