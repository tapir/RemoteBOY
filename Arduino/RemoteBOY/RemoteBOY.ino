#include "Battery.h"
#include "Button.h"
#include "IRRemote.h"
#include "KeyMatrix.h"
#include "LED.h"
#include "NowRemote.h"

static const uint8_t LOW_BATTERY_LEVEL = 10;
static const size_t  NUM_BUTTONS       = NUM_ROWS * NUM_COLS;

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
static const uint8_t* BTN_NOW_REMOTE_MAP[NUM_BUTTONS] = {
    NOW_REMOTE_NONE, NOW_REMOTE_SELECT, NOW_REMOTE_UP, NOW_REMOTE_LEFT, NOW_REMOTE_RIGHT,
    NOW_REMOTE_DOWN, NOW_REMOTE_BACK, NOW_REMOTE_NONE, NOW_REMOTE_HOME, NOW_REMOTE_NONE
};

static Battery   battery;              // battery state
static LEDs      leds;                 // led state
static KeyMatrix matrix;               // key matrix state
static Button    buttons[NUM_BUTTONS]; // button states
static IRRemote  irRemote;             // IR sender
static NowRemote nowRemote;            // espnow sender

void sleep() {
    digitalWrite(GPIO_NUM_10, LOW);
    digitalWrite(GPIO_NUM_21, LOW);
    gpio_hold_en(GPIO_NUM_3);
    gpio_hold_en(GPIO_NUM_4);
    gpio_hold_en(GPIO_NUM_8);
    gpio_hold_en(GPIO_NUM_9);
    gpio_hold_en(GPIO_NUM_20);
    gpio_hold_en(GPIO_NUM_10);
    gpio_hold_en(GPIO_NUM_21);

    // cleanups
    leds.turnOff(LED1);
    leds.turnOff(LED2);
    matrix.reset();
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].reset();
    }
    nowRemote.prepareSleep();
    esp_light_sleep_start();
}

void wakeup() {
    nowRemote.prepareWakeUp();
    // for some reason restart doesn't clear hold states
    gpio_hold_dis(GPIO_NUM_3);
    gpio_hold_dis(GPIO_NUM_4);
    gpio_hold_dis(GPIO_NUM_8);
    gpio_hold_dis(GPIO_NUM_9);
    gpio_hold_dis(GPIO_NUM_20);
    gpio_hold_dis(GPIO_NUM_10);
    gpio_hold_dis(GPIO_NUM_21);
    digitalWrite(GPIO_NUM_10, HIGH);
    digitalWrite(GPIO_NUM_21, HIGH);
}

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

    // programming helper for when in sleep
    leds.turnOn(LED1);
    leds.turnOff(LED2);
    for (int i = 0; i < 10; i++) {
        leds.toggle(LED1);
        leds.toggle(LED2);
        delay(500);
    }
    leds.turnOff(LED1);
    leds.turnOff(LED2);
}

void loop() {
    // sleep as soon as possible if buttons are not kept pressed
    bool anyButton = false;
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i].isPressed()) {
            anyButton = true;
            break;
        }
    }
    if (!anyButton) {
        sleep();
        wakeup();
    }

    // scan key matrix
    matrix.loop();

    // process each button
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i].loop() < BTN_EXIT_SUCCESS) {
            break;
        }
    }

    // repeat IR codes if any
    irRemote.loop();

    // battery level update
    battery.loop();
}

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
        if (battery.getLevel() < LOW_BATTERY_LEVEL) {
            leds.turnOn(LED2);
        }
    } else {
        bool anyButton = false;
        for (int i = 0; i < NUM_BUTTONS; i++) {
            if (buttons[i].isPressed()) {
                anyButton = true;
                break;
            }
        }
        if (!anyButton) {
            leds.turnOff(LED1);
            leds.turnOff(LED2);
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
        state ? irRemote.press(IR_REMOTE_POWER, false) : irRemote.release();
        break;
    case BTN_ID_SELECT:
        state ? nowRemote.press(NOW_REMOTE_SELECT) : nowRemote.release(NOW_REMOTE_SELECT);
        break;
    case BTN_ID_UP:
        state ? nowRemote.press(NOW_REMOTE_UP) : nowRemote.release(NOW_REMOTE_UP);
        break;
    case BTN_ID_LEFT:
        state ? nowRemote.press(NOW_REMOTE_LEFT) : nowRemote.release(NOW_REMOTE_LEFT);
        break;
    case BTN_ID_RIGHT:
        state ? nowRemote.press(NOW_REMOTE_RIGHT) : nowRemote.release(NOW_REMOTE_RIGHT);
        break;
    case BTN_ID_DOWN:
        state ? nowRemote.press(NOW_REMOTE_DOWN) : nowRemote.release(NOW_REMOTE_DOWN);
        break;
    case BTN_ID_BACK:
        state ? nowRemote.press(NOW_REMOTE_BACK) : nowRemote.release(NOW_REMOTE_BACK);
        break;
    case BTN_ID_HOME:
        state ? nowRemote.press(NOW_REMOTE_HOME) : nowRemote.release(NOW_REMOTE_HOME);
        break;
    }

    return BTN_EXIT_SUCCESS;
}