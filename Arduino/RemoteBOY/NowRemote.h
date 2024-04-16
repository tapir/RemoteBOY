#ifndef NOW_REMOTE_H
#define NOW_REMOTE_H

#include <stdint.h>

static const uint8_t NOW_REMOTE_SELECT[2] = { 1, 0 };  // Menu Pick (Select)
static const uint8_t NOW_REMOTE_BACK[2]   = { 1, 1 };  // Menu Escape (Back)
static const uint8_t NOW_REMOTE_UP[2]     = { 0, 1 };  // Menu Up
static const uint8_t NOW_REMOTE_DOWN[2]   = { 0, 5 };  // Menu Down
static const uint8_t NOW_REMOTE_LEFT[2]   = { 0, 7 };  // Menu Left
static const uint8_t NOW_REMOTE_RIGHT[2]  = { 0, 3 };  // Menu Right
static const uint8_t NOW_REMOTE_HOME[2]   = { 1, 12 }; // Home
static const uint8_t NOW_REMOTE_NONE[2]   = { 0, 0 };  // None

class NowRemote {
public:
    NowRemote(void);
    void setup(void);
    void press(const uint8_t* buttonID);
    void release(const uint8_t* buttonID);
    void click(const uint8_t* buttonID);
    void prepareSleep(void);
    void prepareWakeUp(void);

private:
    void send(const uint8_t* buttonID, bool state);
};

#endif // NOW_REMOTE_H