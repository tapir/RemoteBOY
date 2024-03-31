#include "BLERemote.h"
#include "Battery.h"
#include "Button.h"
#include "IRRemote.h"
#include "KeyMatrix.h"
#include "LED.h"

static const size_t NUM_BUTTONS = NUM_ROWS * NUM_COLS;

// button IDs
static const uint8_t BTN_ID_POWER   = 0;
static const uint8_t BTN_ID_SELECT  = 1;
static const uint8_t BTN_ID_UP      = 2;
static const uint8_t BTN_ID_LEFT    = 3;
static const uint8_t BTN_ID_RIGHT   = 4;
static const uint8_t BTN_ID_DOWN    = 5;
static const uint8_t BTN_ID_BACK    = 6;
static const uint8_t BTN_ID_VOLUP   = 7;
static const uint8_t BTN_ID_HOME    = 8;
static const uint8_t BTN_ID_VOLDOWN = 9;

// mapping of button IDs to key matrix {row, col}
//        col0    col1
// row0   up      power on
// row1   left    select
// row2   down    right
// row3   back    volup
// row4   home    voldown
static const size_t BTN_MATRIX_MAP[NUM_BUTTONS][2] = {
    { 0, 1 }, { 1, 1 }, { 0, 0 }, { 1, 0 }, { 2, 1 },
    { 2, 0 }, { 3, 0 }, { 3, 1 }, { 4, 0 }, { 4, 1 }
};

// battery state
Battery battery;

// led state
LED leds;

// key matrix state
KeyMatrix matrix;

// button states
Button buttons[NUM_BUTTONS];

// bluetooth sender
BLERemote blRemote;

// infrared sender
IRRemote irRemote;

// this func is called by each button to read the pin state instead of
// digitalRead() because we operate with a key matrix and not "a pin per
// button". key matrix object will keep all states up to date as long as
// KeyMatrix::loop() is called.
int readPinFunc(int buttonID) {
    return matrix.getKeyState(BTN_MATRIX_MAP[buttonID][0], BTN_MATRIX_MAP[buttonID][1]);
}

// this is where button event and button layer related logic happens
// it's called by each button whenever there is a state change (press & release)
// is detected (debounced on both press and release).
void stateChangeFunc(int buttonID, bool state) {
    // TODO
}

void setup() {
    battery.setup();
    leds.setup();
    blRemote.setup("RemoteBOY", "Lola Engineering", battery.getLevel());
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
    blRemote.setBatteryLevel(battery.getLevel());
    // low battery indicator
    if (battery.getLevel() <= 10) {
        leds.setBlinkFlag(LED1, true);
        leds.setBlinkFlag(LED2, true);
    }

    // scan key matrix
    matrix.loop();

    // process each button
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].loop();
    }

    // led process
    leds.loop();
}
