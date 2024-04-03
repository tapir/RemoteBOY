#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

const uint32_t BTN_EXIT_BT_DISCONNECT = -2;
const uint32_t BTN_EXIT_LAYER_CHANGE  = -1;
const uint32_t BTN_EXIT_SUCCESS       = 0;

class Button {
public:
    Button(void);
    void setup(uint8_t buttonID, uint16_t stateMask, int (*stateChangeCallback)(uint8_t, bool), int (*readPinCallback)(uint8_t));
    int  loop(void);
    bool isPressed(void);

private:
    int      buttonID;
    bool     state;
    uint16_t samples;
    uint16_t stateMask;
    int (*stateChangeCallback)(uint8_t buttonID, bool state);
    int (*readPinCallback)(uint8_t buttonID);
};

#endif // BUTTON_H