#include "Battery.h"
#include "Button.h"
#include "IRRemote.h"
#include "KeyMatrix.h"
#include "LED.h"
#include "NowRemote.h"

static const uint32_t BLINK_DELAY = 2000;
static const size_t   NUM_BUTTONS = NUM_ROWS * NUM_COLS;

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

// mapping of button IDs to HID CC codes
static const uint8_t* BTN_NOWREMOTE_MAP[NUM_BUTTONS] = {
    NOWREMOTE_NONE, NOWREMOTE_SELECT, NOWREMOTE_UP, NOWREMOTE_LEFT, NOWREMOTE_RIGHT,
    NOWREMOTE_DOWN, NOWREMOTE_BACK, NOWREMOTE_NONE, NOWREMOTE_HOME, NOWREMOTE_NONE
};

static Battery   battery;              // battery state
static LEDs      leds;                 // led state
static KeyMatrix matrix;               // key matrix state
static Button    buttons[NUM_BUTTONS]; // button states
static IRRemote  irRemote;             // IR sender
static NowRemote nowRemote;            // espnow sender

void setup() {
    battery.setup();
    leds.setup();
    nowRemote.setup();
    irRemote.setup();
    matrix.setup();
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].setup(i, &onButtonStateChange, &onReadPin);
    }

    // set row pins to wakeup
    gpio_wakeup_enable(GPIO_NUM_3, GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable(GPIO_NUM_4, GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable(GPIO_NUM_8, GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable(GPIO_NUM_9, GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable(GPIO_NUM_20, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();

    // Serial.begin(115200);
    // Serial.flush();
}

void loop() {
    // scan key matrix
    matrix.loop();

    // process each button
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i].loop() < BTN_EXIT_SUCCESS) {
            break;
        }
    }

    // ir repeat codes
    irRemote.loop();

    // update battery level
    battery.loop();

    // low battery indicator
    if (battery.getLevel() < 10) {
        static uint32_t lastTime = 0;
        if (millis() - lastTime > BLINK_DELAY) {
            leds.toggle(LED2);
            lastTime += BLINK_DELAY;
        }
    }
}

// void sleep() {
//     digitalWrite(GPIO_NUM_10, LOW);
//     digitalWrite(GPIO_NUM_21, LOW);
//     gpio_hold_en(GPIO_NUM_3);
//     gpio_hold_en(GPIO_NUM_4);
//     gpio_hold_en(GPIO_NUM_8);
//     gpio_hold_en(GPIO_NUM_9);
//     gpio_hold_en(GPIO_NUM_20);
//     gpio_hold_en(GPIO_NUM_10);
//     gpio_hold_en(GPIO_NUM_21);

//     // cleanups
//     btConnectOnce = false;
//     blRemote.end();
//     matrix.reset();
//     delay(SLEEP_DELAY);
//     esp_light_sleep_start();
// }

// void wakeup() {
//     // for some reason restart doesn't clear hold states
//     gpio_hold_dis(GPIO_NUM_3);
//     gpio_hold_dis(GPIO_NUM_4);
//     gpio_hold_dis(GPIO_NUM_8);
//     gpio_hold_dis(GPIO_NUM_9);
//     gpio_hold_dis(GPIO_NUM_20);
//     gpio_hold_dis(GPIO_NUM_10);
//     gpio_hold_dis(GPIO_NUM_21);
//     digitalWrite(GPIO_NUM_10, HIGH);
//     digitalWrite(GPIO_NUM_21, HIGH);
//     matrix.loop();
//     for (int i = 0; i < NUM_BUTTONS; i++) {
//         if (matrix.getKeyState(BTN_MATRIX_MAP[i][0], BTN_MATRIX_MAP[i][1])) {
//             wakeUpButton = i;
//             break;
//         }
//     }
//     ESP.restart();
// }

// this func is called by each button to read the pin state instead of
// digitalRead() because we operate with a key matrix and not "a pin per
// button". key matrix object will keep all states up to date as long as
// KeyMatrix::loop() is called.
int onReadPin(const uint8_t buttonID) {
    return matrix.getKeyState(BTN_MATRIX_MAP[buttonID][0], BTN_MATRIX_MAP[buttonID][1]);
}

// this is where button event logic happens
// it's called by each button whenever there is a state change (press & release)
// is detected (debounced on both press and release).
int onButtonStateChange(const uint8_t buttonID, bool state) {
    if (state) {
        leds.turnOn(LED1);
    } else {
        bool anyPressed = false;
        for (int i = 0; i < NUM_BUTTONS; i++) {
            anyPressed |= buttons[i].isPressed();
        }
        if (!anyPressed) {
            leds.turnOff(LED1);
        }
    }

    switch (buttonID) {
    case BTN_ID_VOLUP:
        state ? irRemote.press(IR_REMOTE_VOLUP, true) : irRemote.release();
        break;
    case BTN_ID_VOLDOWN:
        state ? irRemote.press(IR_REMOTE_VOLDOWN, true) : irRemote.release();
        break;
    case BTN_ID_POWER:
        if (state) {
            irRemote.press(IR_REMOTE_POWER, false);
        }
        break;
    default:
        state ? nowRemote.press(BTN_NOWREMOTE_MAP[buttonID]) : nowRemote.release(BTN_NOWREMOTE_MAP[buttonID]);
    }

    // irRemote.release();
    // irRemote.press(IR_REMOTE_VOLMUTE, false);

    return BTN_EXIT_SUCCESS;
}