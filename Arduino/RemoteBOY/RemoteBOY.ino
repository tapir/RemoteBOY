#include "BLERemote.h"
#include "Battery.h"
#include "Button.h"
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

static const ButtonReport* BTN_BLE_MAP[NUM_BUTTONS] = {
    &BLE_REMOTE_POWER, &BLE_REMOTE_SELECT, &BLE_REMOTE_UP, &BLE_REMOTE_LEFT, &BLE_REMOTE_RIGHT,
    &BLE_REMOTE_DOWN, &BLE_REMOTE_BACK, &BLE_REMOTE_VOLUP, &BLE_REMOTE_HOME, &BLE_REMOTE_VOLDOWN
};

static const uint32_t SLEEP_DELAY   = 5000;
static const uint32_t BT_CONN_DELAY = 700;
static const uint32_t TOGGLE_DELAY  = 500;
static const uint32_t BLINK_DELAY   = 2000;
static const uint32_t IDLE_TIMEOUT  = 60000;

static RTC_NOINIT_ATTR uint8_t wakeUpButton  = NUM_BUTTONS; // button that is pressed to wake device up
static bool                    btConnectOnce = false;       // single run code on bt connect
static uint32_t                idleTimer     = millis();    // sleep timer
static Battery                 battery;                     // battery state
static LEDs                    leds;                        // led state
static KeyMatrix               matrix;                      // key matrix state
static Button                  buttons[NUM_BUTTONS];        // button states
static BLERemote               blRemote;                    // bluetooth sender

void setup() {
    battery.setup();
    leds.setup();
    blRemote.setup("RemoteBOY", "Lola Engineering", battery.getLevel());
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
    static uint32_t lastBlinkTime = 0;

    // power management sleep and wake-up point
    if (millis() - idleTimer > IDLE_TIMEOUT) {
        sleep();
        wakeup();
    }

    // wait until bluetooth is connected
    if (!blRemote.isConnected()) {
        if (!btConnectOnce) {
            leds.turnOn(LED1);
            leds.turnOff(LED2);
            btConnectOnce = true;
            lastBlinkTime = millis();
        }
        if (millis() - lastBlinkTime > TOGGLE_DELAY) {
            leds.toggle(LED1);
            leds.toggle(LED2);
            lastBlinkTime += TOGGLE_DELAY;
        }
        return;
    }
    // bluetooth connected
    if (btConnectOnce) {
        leds.turnOff(LED1);
        leds.turnOff(LED2);
        btConnectOnce = false;
        delay(BT_CONN_DELAY);
    }

    // process the button that was pressed during sleep
    if (wakeUpButton < NUM_BUTTONS) {
        blRemote.click(*BTN_BLE_MAP[wakeUpButton]);
        wakeUpButton = NUM_BUTTONS;
        matrix.reset();
    }

    // scan key matrix
    matrix.loop();

    // process each button
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i].loop() < BTN_EXIT_SUCCESS) {
            break;
        }
    }

    // update battery level
    battery.loop();
    blRemote.setBatteryLevel(battery.getLevel());

    // low battery indicator
    if (battery.getLevel() < 10) {
        static uint32_t lastTime = 0;
        if (millis() - lastTime > BLINK_DELAY) {
            leds.toggle(LED2);
            lastTime += BLINK_DELAY;
        }
    }
}

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
    btConnectOnce = false;
    blRemote.end();
    matrix.reset();
    delay(SLEEP_DELAY);
    esp_light_sleep_start();
}

void wakeup() {
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
    matrix.loop();
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (matrix.getKeyState(BTN_MATRIX_MAP[i][0], BTN_MATRIX_MAP[i][1])) {
            wakeUpButton = i;
            break;
        }
    }
    ESP.restart();
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
        if (state && buttons[BTN_ID_VOLDOWN].isPressed()) {
            blRemote.click(BLE_REMOTE_MUTE);
        } else {
            state ? blRemote.press(BLE_REMOTE_VOLUP) : blRemote.release(BLE_REMOTE_VOLUP);
        }
        break;
    case BTN_ID_VOLDOWN:
        if (state && buttons[BTN_ID_VOLUP].isPressed()) {
            blRemote.click(BLE_REMOTE_MUTE);
        } else {
            state ? blRemote.press(BLE_REMOTE_VOLDOWN) : blRemote.release(BLE_REMOTE_VOLDOWN);
        }
        break;
    // case BTN_ID_SELECT:
    //     if (state && buttons[BTN_ID_HOME].isPressed()) {
    //         blRemote.click(BLE_REMOTE_MENU);
    //     } else {
    //         state ? blRemote.press(BLE_REMOTE_SELECT) : blRemote.release(BLE_REMOTE_SELECT);
    //     }
    //     break;
    default:
        state ? blRemote.press(*BTN_BLE_MAP[buttonID]) : blRemote.release(*BTN_BLE_MAP[buttonID]);
        // case BTN_ID_POWER:
        //     state ? blRemote.press(BLE_REMOTE_POWER) : blRemote.release(BLE_REMOTE_POWER);
        //     break;
        // case BTN_ID_BACK:
        //     state ? blRemote.press(BLE_REMOTE_BACK) : blRemote.release(BLE_REMOTE_BACK);
        //     break;
        // case BTN_ID_HOME:
        //     state ? blRemote.press(BLE_REMOTE_HOME) : blRemote.release(BLE_REMOTE_HOME);
        //     break;
        // case BTN_ID_UP:
        //     state ? blRemote.press(BLE_REMOTE_UP) : blRemote.release(BLE_REMOTE_UP);
        //     break;
        // case BTN_ID_LEFT:
        //     state ? blRemote.press(BLE_REMOTE_LEFT) : blRemote.release(BLE_REMOTE_LEFT);
        //     break;
        // case BTN_ID_RIGHT:
        //     state ? blRemote.press(BLE_REMOTE_RIGHT) : blRemote.release(BLE_REMOTE_RIGHT);
        //     break;
        // case BTN_ID_DOWN:
        //     state ? blRemote.press(BLE_REMOTE_DOWN) : blRemote.release(BLE_REMOTE_DOWN);
        //     break;
    }

    // reset sleep timer
    idleTimer = millis();

    return BTN_EXIT_SUCCESS;
}