#ifndef IRREMOTE_H
#define IRREMOTE_H

#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint8_t IR_REMOTE_POWER    = 0x04FB08F7;
const uint8_t IR_REMOTE_SELECT   = 0x04FB44BB;
const uint8_t IR_REMOTE_BACK     = 0x04FB28D7;
const uint8_t IR_REMOTE_HOME     = 0x04FB7C83;
const uint8_t IR_REMOTE_INPUT    = 0x04FB0BF4;
const uint8_t IR_REMOTE_SETTINGS = 0x04FB43BC;
const uint8_t IR_REMOTE_VOLUP    = 0x04FB02FD;
const uint8_t IR_REMOTE_VOLDOWN  = 0x04FB03FC;
const uint8_t IR_REMOTE_VOLMUTE  = 0x04FB09F6;
const uint8_t IR_REMOTE_UP       = 0x04FB40BF;
const uint8_t IR_REMOTE_DOWN     = 0x04FB41BE;
const uint8_t IR_REMOTE_LEFT     = 0x04FB07FB;
const uint8_t IR_REMOTE_RIGHT    = 0x04FB06F9;

class IRRemote {
public:
    IRRemote(void);
    ~IRRemote(void);
    void setup(void);
    void send(uint8_t irCode);

private:
    IRsend*  irSender;
    uint32_t lastSend;
};

#endif // IRREMOTE_H