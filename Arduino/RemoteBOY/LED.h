#ifndef LED_H
#define LED_H

#include <stddef.h>
#include <stdint.h>

static const size_t  NUM_LEDS = 2;
static const uint8_t LED1     = 0;
static const uint8_t LED2     = 1;

class LEDs {
public:
    LEDs(void);
    void setup(void);
    void turnOn(const uint8_t ledNum);
    void turnOff(const uint8_t ledNum);
    void toggle(const uint8_t ledNum);

private:
    bool state[NUM_LEDS];
};

#endif // LED_H