#include "BLERemote.h"
#include "IRRemote.h"
#include "KeyMatrix.h"
#include "Button.h"
#include "Battery.h"

static const size_t NUM_BUTTONS = NUM_ROWS * NUM_COLS;

// button IDs
static const int BTN_ID_POWER   = 0;
static const int BTN_ID_SELECT  = 1;
static const int BTN_ID_UP      = 2;
static const int BTN_ID_LEFT    = 3;
static const int BTN_ID_RIGHT   = 4;
static const int BTN_ID_DOWN    = 5;
static const int BTN_ID_BACK    = 6;
static const int BTN_ID_VOLUP   = 7;
static const int BTN_ID_HOME    = 8;
static const int BTN_ID_VOLDOWN = 9;

// mapping of button IDs to key matrix {row, col}
static const size_t BTN_MATRIX_MAP[NUM_BUTTONS][2] = {
  {0, 1},
  {1, 1},
  {0, 0},
  {1, 0},
  {2, 1},
  {2, 0},
  {3, 0},
  {3, 1},
  {4, 0},
  {4, 1}
};

// battery state
Battery battery;
uint8_t prevBatteryLevel = 100;

// key matrix state
KeyMatrix matrix;

// button states
Button buttons[NUM_BUTTONS];

// bluetooth sender
BLERemote blRemote;

// infrared sender
IRRemote irRemote;

// this func is called by each button to read the state instead of digitalRead() because
// we operate with a key matrix and not "a pin per button". key matrix will keep all
// states up to date as long as KeyMatrix::loop() is called.
int readPinFunc(int buttonID) {
  return matrix.getKeyState(BTN_MATRIX_MAP[buttonID][0], BTN_MATRIX_MAP[buttonID][1]);
}

// this is where button event and button layer related logic happens
// it's called buy each button whenever there is a state change (pressed&released)
// is detected (debounced on both press and release).
void stateChangeFunc(int buttonID, bool state) {
  // TODO
}

void setup() {
  battery.setup();
  blRemote.setup("RemoteBOY", "LolaEngineering", battery.getLevel());
  irRemote.setup();
  matrix.setup();
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].setup(i, 0, &stateChangeFunc, &readPinFunc);
  }
}

void loop() {
  // power management sleep and wake-up point
  // TODO

  // update battery level
  battery.loop();
  uint8_t currBatteryLevel = battery.getLevel();
  if (currBatteryLevel != prevBatteryLevel) {
    prevBatteryLevel = currBatteryLevel;
    blRemote.setBatteryLevel(currBatteryLevel);
  }
  // low battery indicator
  // TODO

  // scan key matrix
  matrix.loop();

  // process each button
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].loop();
  }
}
