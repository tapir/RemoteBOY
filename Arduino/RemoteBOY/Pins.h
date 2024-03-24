#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

const int PIN_BATTERY =  A0;
const int PIN_COL_1   =  D6;
const int PIN_COL_2   =  D10;
const int PIN_ROW_1   =  D2;
const int PIN_ROW_2   =  D1;
const int PIN_ROW_3   =  D9;
const int PIN_ROW_4   =  D8;
const int PIN_ROW_5   =  D7;
const int PIN_LED_1   =  D3;
const int PIN_LED_2   =  D4;
const int PIN_LED_IR  =  D5;

// key matrix
const int numRows = 5;
const int numCols = 2;
const int rowPins[numRows] = {PIN_ROW_1, PIN_ROW_2, PIN_ROW_3, PIN_ROW_4, PIN_ROW_5};
const int colPins[numCols] = {PIN_COL_1, PIN_COL_2};

// key matrix state
int keyMap[numRows][numCols] = {
  {0, 0}, // up, power
  {0, 0}, // left, select
  {0, 0}, // down, right
  {0, 0}, // back, volup
  {0, 0}  // home, voldown
};

void setupPins(void);
void scanMatrix(void);

#endif // PINS_H