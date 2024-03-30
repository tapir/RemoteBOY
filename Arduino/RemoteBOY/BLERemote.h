#ifndef BLE_REMOTE_H
#define BLE_REMOTE_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "BLEStatus.h"
#include "NimBLEHIDDevice.h"
#include "NimBLECharacteristic.h"

typedef uint8_t ButtonReport[2];

const ButtonReport REMOTE_POWER   = {1, 0};
const ButtonReport REMOTE_SELECT  = {2, 0};
const ButtonReport REMOTE_BACK    = {4, 0};
const ButtonReport REMOTE_HOME    = {8, 0};
const ButtonReport REMOTE_VOLUP   = {16, 0};
const ButtonReport REMOTE_VOLDOWN = {32, 0};
const ButtonReport REMOTE_VOLMUTE = {64, 0};
const ButtonReport REMOTE_UP      = {128, 0};
const ButtonReport REMOTE_DOWN    = {0, 1};
const ButtonReport REMOTE_LEFT    = {0, 2};
const ButtonReport REMOTE_RIGHT   = {0, 4};

class BLERemote {
private:
  ButtonReport _consumerButtons;
  BLEStatus* connectionStatus;
  NimBLEHIDDevice* hid;
  NimBLECharacteristic* inputConsumer;
  std::string deviceManufacturer;
  std::string deviceName;
  uint8_t batteryLevel;
  static void taskServer(void* pvParameter);
public:
  BLERemote(void);
  void setup(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel);
  void click(const ButtonReport b);
  void press(const ButtonReport b);
  void release(const ButtonReport b);
  bool isConnected(void);
  void setBatteryLevel(uint8_t level);
  void sendReport(void);
  
protected:
  virtual void onStarted(NimBLEServer *pServer) {};
};

#endif // CONFIG_BT_ENABLED
#endif // BLE_REMOTE_H