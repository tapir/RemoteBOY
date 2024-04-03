#ifndef LED_H
#define LED_H

#include <stddef.h>
#include <stdint.h>

static const size_t  NUM_LEDS = 2;
static const uint8_t LED1     = 0;
static const uint8_t LED2     = 1;

typedef struct {
    uint32_t lastUpdateTotal;
    bool     state;
} LEDState;

class LEDs {
public:
    LEDs(void);
    void setup(void);
    void loop(void);
    void turnOn(uint8_t ledNum);
    void turnOff(uint8_t ledNum);
    void setBlinkFlag(bool blink);
    bool getBlinkFlag(void);
    void setEndlessFlag(bool endless);
    bool getEndlessFlag(void);

private:
    uint32_t lastUpdateBlink;
    bool     blink;
    bool     endless; // LEDs never turn off on their own
    LEDState state[NUM_LEDS];
};

#endif // LED_H