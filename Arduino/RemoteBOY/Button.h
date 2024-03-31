#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

class Button {
public:
    Button(void);
    void setup(uint8_t buttonID, uint16_t stateMask, void (*stateChangeCallback)(uint8_t, bool), int (*readPinCallback)(uint8_t));
    void loop(void);
    bool isPressed(void);

private:
    int      buttonID;
    bool     state;
    uint16_t samples;
    uint16_t stateMask;
    void (*stateChangeCallback)(uint8_t buttonID, bool state);
    int (*readPinCallback)(uint8_t buttonID);
};

#endif // BUTTON_H