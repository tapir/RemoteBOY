#ifndef IRREMOTE_H
#define IRREMOTE_H

#include <stdint.h>

// address + data
const uint16_t IR_REMOTE_POWER    = 0x0408;
const uint16_t IR_REMOTE_SELECT   = 0x0444;
const uint16_t IR_REMOTE_BACK     = 0x0428;
const uint16_t IR_REMOTE_HOME     = 0x047C;
const uint16_t IR_REMOTE_INPUT    = 0x040B;
const uint16_t IR_REMOTE_SETTINGS = 0x0443;
const uint16_t IR_REMOTE_VOLUP    = 0x0402;
const uint16_t IR_REMOTE_VOLDOWN  = 0x0403;
const uint16_t IR_REMOTE_VOLMUTE  = 0x0409;
const uint16_t IR_REMOTE_UP       = 0x0440;
const uint16_t IR_REMOTE_DOWN     = 0x0441;
const uint16_t IR_REMOTE_LEFT     = 0x0407;
const uint16_t IR_REMOTE_RIGHT    = 0x0406;

class IRRemote {
public:
    IRRemote(void);
    void setup(void);
    void press(uint16_t irCode, bool repeat);
    void release(void);
    void loop(void);

private:
    uint32_t lastRepeat;
    bool     repeat;
    bool     getBits(uint16_t irCode, bool address, uint8_t index);
    void     modulateOne(void);
    void     modulateZero(void);
    void     modulateStart(void);
    void     modulateEnd(void);
    void     modulateRepeat(void);
    void     modulateAddress(uint16_t irCode);
    void     modulateData(uint16_t irCode);
};

#endif // IRREMOTE_H