#ifndef IRREMOTE_H
#define IRREMOTE_H

#include <IRremoteESP8266.h>
#include <IRsend.h>

static const uint32_t IR_REMOTE_POWER   = 0x20DF10EF;
static const uint32_t IR_REMOTE_VOLUP   = 0x20DF40BF;
static const uint32_t IR_REMOTE_VOLDOWN = 0x20DFC03F;
static const uint32_t IR_REMOTE_VOLMUTE = 0x20DF906F;

class IRRemote {
public:
    IRRemote(void);
    ~IRRemote(void);
    void setup(void);
    void press(const uint32_t irCode, bool repeat);
    void release(void);
    void loop(void);

private:
    IRsend*  irSender;
    uint32_t repeatCommand;
};

#endif // IRREMOTE_H