#ifndef LED_H
#define LED_H

#include <stdint.h>

static const uint8_t LED1 = 0;
static const uint8_t LED2 = 1;

typedef struct LEDState {
  uint32_t lastUpdateTotal;
  uint32_t lastUpdateBlink;
  bool state;
  bool blink;
};

class LED {
public:
  LED(void);
  void setup(void);
  void loop(void);
  void turnOn(int ledNum);
  void turnOff(int ledNum);
  void setBlinkFlag(int ledNum, bool blink);
private:
  LEDState state[2];
};

#endif // LED_H