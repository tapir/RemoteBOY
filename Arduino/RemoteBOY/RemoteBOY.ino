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

Battery   battery;              // battery state
LEDs      leds;                 // led state
KeyMatrix matrix;               // key matrix state
Button    buttons[NUM_BUTTONS]; // button states
BLERemote blRemote;             // bluetooth sender
IRRemote  irRemote;             // infrared sender

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

    // wait until bluetooth is connected
    bool pairingIndicator = leds.getPairing();
    if (!blRemote.isConnected()) {
        // bluetooth advertising indicator
        if (!pairingIndicator) {
            leds.setPairing(true);
        }
        return;
    }
    // bluetooth connected
    if (pairingIndicator) {
        leds.setPairing(false);
    }

    // update battery level
    battery.loop();
    blRemote.setBatteryLevel(battery.getLevel());

    // low battery indicator
    bool blinkIndicator = leds.getBlinkFlag();
    if (battery.getLevel() <= 10 && !blinkIndicator) {
        leds.setBlinkFlag(true);
    } else if (blinkIndicator) {
        leds.setBlinkFlag(false);
    }

    // scan key matrix
    matrix.loop();

    // process each button
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].loop();
    }

    // process LEDs
    leds.loop();

    // process IR repeats
    irRemote.loop();
}

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
        // power        BT
        // select       BT
        // d-pad        BT
        // back         BT
        // home         BT
        // volume up    IR
        // volume down  IR
        // mute         IR

        leds.turnOff(LED2);
        leds.turnOn(LED1);

        switch (buttonID) {
        case BTN_ID_POWER:
            if (state && buttons[BTN_ID_SELECT].isPressed()) {
                blRemote.disconnect();
            } else {
                state ? blRemote.press(BLE_REMOTE_POWER) : blRemote.release(BLE_REMOTE_POWER);
            }
            break;
        case BTN_ID_SELECT:
            if (state && buttons[BTN_ID_POWER].isPressed()) {
                blRemote.disconnect();
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
                irRemote.press(IR_REMOTE_VOLMUTE, false);
            } else {
                state ? irRemote.press(IR_REMOTE_VOLUP, true) : irRemote.release();
            }
            break;
        case BTN_ID_VOLDOWN:
            if (state && buttons[BTN_ID_VOLUP].isPressed()) {
                irRemote.press(IR_REMOTE_VOLMUTE, false);
            } else {
                state ? irRemote.press(IR_REMOTE_VOLDOWN, true) : irRemote.release();
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
        // power            IR
        // select           IR
        // d-pad            IR
        // back             IR
        // home             IR
        // mute             IR
        // settings (volup) IR
        // input (voldown)  IR
        // mute             IR

        leds.turnOff(LED1);
        leds.turnOn(LED2);

        switch (buttonID) {
        case BTN_ID_POWER:
            if (state) {
                if (buttons[BTN_ID_SELECT].isPressed()) {
                    blRemote.disconnect();
                } else {
                    irRemote.press(IR_REMOTE_POWER, false);
                }
            }
            break;
        case BTN_ID_SELECT:
            if (state) {
                if (buttons[BTN_ID_POWER].isPressed()) {
                    blRemote.disconnect();
                } else {
                    irRemote.press(IR_REMOTE_SELECT, false);
                }
            }
            break;
        case BTN_ID_BACK:
            if (state) {
                if (buttons[BTN_ID_HOME].isPressed()) {
                    layer = !layer;
                } else {
                    irRemote.press(IR_REMOTE_BACK, false);
                }
            }
            break;
        case BTN_ID_HOME:
            if (state) {
                if (buttons[BTN_ID_BACK].isPressed()) {
                    layer = !layer;
                } else {
                    irRemote.press(IR_REMOTE_HOME, false);
                }
            }
            break;
        case BTN_ID_VOLUP:
            if (state) {
                if (buttons[BTN_ID_VOLDOWN].isPressed()) {
                    irRemote.press(IR_REMOTE_VOLMUTE, false);
                } else {
                    irRemote.press(IR_REMOTE_SETTINGS, false);
                }
            }
            break;
        case BTN_ID_VOLDOWN:
            if (state) {
                if (buttons[BTN_ID_VOLUP].isPressed()) {
                    irRemote.press(IR_REMOTE_VOLMUTE, false);
                } else {
                    irRemote.press(IR_REMOTE_INPUT, false);
                }
            }
            break;
        case BTN_ID_UP:
            state ? irRemote.press(IR_REMOTE_UP, true) : irRemote.release();
            break;
        case BTN_ID_LEFT:
            state ? irRemote.press(IR_REMOTE_LEFT, true) : irRemote.release();
            break;
        case BTN_ID_RIGHT:
            state ? irRemote.press(IR_REMOTE_RIGHT, true) : irRemote.release();
            break;
        case BTN_ID_DOWN:
            state ? irRemote.press(IR_REMOTE_DOWN, true) : irRemote.release();
            break;
        }
    }
}