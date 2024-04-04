#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

static const uint32_t BTN_EXIT_BT_DISCONNECT = -1;
static const uint32_t BTN_EXIT_SUCCESS       = 0;

class Button {
public:
    Button(void);
    void setup(const uint8_t buttonID, int (*stateChangeCallback)(const uint8_t, bool), int (*readPinCallback)(const uint8_t));
    int  loop(void);
    bool isPressed(void);

private:
    int      buttonID;
    bool     state;
    bool     lastState;
    uint32_t lastDebounceTime;
    int (*stateChangeCallback)(const uint8_t buttonID, bool state);
    int (*readPinCallback)(const uint8_t buttonID);
};

#endif // BUTTON_H