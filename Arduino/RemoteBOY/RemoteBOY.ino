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
int onReadPin(uint8_t buttonID) {
    return matrix.getKeyState(BTN_MATRIX_MAP[buttonID][0], BTN_MATRIX_MAP[buttonID][1]);
}

// this is where button event and button layer related logic happens
// it's called by each button whenever there is a state change (press & release)
// is detected (debounced on both press and release).
void onButtonStateChange(uint8_t buttonID, bool state) {
    static bool layer = false; // default layer
    if (!layer) {
        // default layer
        // power    BT
        // select   BT
        // d-pad    BT
        // back     BT
        // home     BT
        // volume   IR
        // mute     IR

        leds.turnOff(LED2);
        leds.turnOn(LED1);

        switch (buttonID) {
        case BTN_ID_POWER:
            if (state && buttons[BTN_ID_SELECT].isPressed()) {
                blRemote.pairingMode = true;
            } else {
                state ? blRemote.press(BLE_REMOTE_POWER) : blRemote.release(BLE_REMOTE_POWER);
            }
            break;
        case BTN_ID_SELECT:
            if (state && buttons[BTN_ID_POWER].isPressed()) {
                blRemote.pairingMode = true;
            } else {
                state ? blRemote.press(BLE_REMOTE_SELECT) : blRemote.release(BLE_REMOTE_SELECT);
            }
            break;
        case BTN_ID_BACK:
            if (state && buttons[BTN_ID_HOME].isPressed()) {
                layer = !layer;
            } else {
                state ? blRemote.press(BLE_REMOTE_BACK) : blRemote.release(BLE_REMOTE_BACK);
            }
            break;
        case BTN_ID_HOME:
            if (state && buttons[BTN_ID_BACK].isPressed()) {
                layer = !layer;
            } else {
                state ? blRemote.press(BLE_REMOTE_HOME) : blRemote.release(BLE_REMOTE_HOME);
            }
            break;
        case BTN_ID_VOLUP:
            if (state && buttons[BTN_ID_VOLDOWN].isPressed()) {
                // TODO: IR vol mute
            } else {
                // TODO: IR vol up
            }
            break;
        case BTN_ID_VOLDOWN:
            if (state && buttons[BTN_ID_VOLUP].isPressed()) {
                // TODO: IR vol mute
            } else {
                // TODO: IR vold down
            }
            break;
        case BTN_ID_UP:
            state ? blRemote.press(BLE_REMOTE_UP) : blRemote.release(BLE_REMOTE_UP);
            break;
        case BTN_ID_LEFT:
            state ? blRemote.press(BLE_REMOTE_LEFT) : blRemote.release(BLE_REMOTE_LEFT);
            break;
        case BTN_ID_RIGHT:
            state ? blRemote.press(BLE_REMOTE_RIGHT) : blRemote.release(BLE_REMOTE_RIGHT);
            break;
        case BTN_ID_DOWN:
            state ? blRemote.press(BLE_REMOTE_DOWN) : blRemote.release(BLE_REMOTE_DOWN);
            break;
        }
    } else {
        // secondary layer
        // power    IR
        // select   IR
        // d-pad    IR
        // back     IR
        // home     IR
        // volume   IR
        // mute     IR

        leds.turnOff(LED1);
        leds.turnOn(LED2);

        // for IR we're only interested in "press" action
        if (!state) {
            return;
        }

        switch (buttonID) {
        case BTN_ID_POWER:
            if (buttons[BTN_ID_SELECT].isPressed()) {
                blRemote.pairingMode = true;
            } else {
                // TODO: IR
            }
            break;
        case BTN_ID_SELECT:
            if (buttons[BTN_ID_POWER].isPressed()) {
                blRemote.pairingMode = true;
            } else {
                // TODO: IR
            }
            break;
        case BTN_ID_BACK:
            if (buttons[BTN_ID_HOME].isPressed()) {
                layer = !layer;
            } else {
                // TODO: IR
            }
            break;
        case BTN_ID_HOME:
            if (buttons[BTN_ID_BACK].isPressed()) {
                layer = !layer;
            } else {
                // TODO: IR
            }
            break;
        case BTN_ID_VOLUP:
            if (buttons[BTN_ID_VOLDOWN].isPressed()) {
                // TODO: IR
            } else {
                // TODO: IR
            }
            break;
        case BTN_ID_VOLDOWN:
            if (buttons[BTN_ID_VOLUP].isPressed()) {
                // TODO: IR
            } else {
                // TODO: IR
            }
            break;
        case BTN_ID_UP:
            // TODO: IR
            break;
        case BTN_ID_LEFT:
            // TODO: IR
            break;
        case BTN_ID_RIGHT:
            // TODO: IR
            break;
        case BTN_ID_DOWN:
            // TODO: IR
            break;
        }
    }
}

void setup() {
    battery.setup();
    leds.setup();
    blRemote.setup("RemoteBOY", "Lola Engineering", battery.getLevel());
    irRemote.setup();
    matrix.setup();
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].setup(i, 0, &onButtonStateChange, &onReadPin);
    }
}

void loop() {
    // TODO: power management sleep and wake-up point

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
