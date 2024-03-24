#include "BLERemote.h"
#include "Pins.h"

BLERemote remote;

void setup() {
  Serial.begin(9600);
  setupPins();
  remote.begin();
}

void loop() {
  if (Serial.available() > 0) {
    int incoming = Serial.read();
    switch(incoming) {
      // CONSUMER
      case 'q':
      Serial.println("I received: REMOTE_POWER");
      remote.click(REMOTE_POWER);
      break;
      case 'w':
      Serial.println("I received: REMOTE_SELECT");
      remote.click(REMOTE_SELECT);
      break;
      case 'e':
      Serial.println("I received: REMOTE_BACK");
      remote.click(REMOTE_BACK);
      break;
      case 'r':
      Serial.println("I received: REMOTE_HOME");
      remote.click(REMOTE_HOME);
      break;
      case 't':
      Serial.println("I received: REMOTE_UP");
      remote.click(REMOTE_UP);
      break;
      case 'y':
      Serial.println("I received: REMOTE_DOWN");
      remote.click(REMOTE_DOWN);
      break;
      case 'u':
      Serial.println("I received: REMOTE_LEFT");
      remote.click(REMOTE_LEFT);
      break;
      case 'o':
      Serial.println("I received: REMOTE_RIGHT");
      remote.click(REMOTE_RIGHT);
      break;  
    }
  }
}
