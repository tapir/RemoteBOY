#ifndef BLE_REMOTE_H
#define BLE_REMOTE_H

#include "BLEStatus.h"
#include <NimBLECharacteristic.h>
#include <NimBLEHIDDevice.h>
#include <NimBLEServer.h>

typedef uint8_t ButtonReport[2];

static const ButtonReport BLE_REMOTE_POWER   = { 1, 0 };
static const ButtonReport BLE_REMOTE_SELECT  = { 2, 0 };
static const ButtonReport BLE_REMOTE_BACK    = { 4, 0 };
static const ButtonReport BLE_REMOTE_HOME    = { 8, 0 };
static const ButtonReport BLE_REMOTE_VOLUP   = { 16, 0 };
static const ButtonReport BLE_REMOTE_VOLDOWN = { 32, 0 };
static const ButtonReport BLE_REMOTE_MUTE    = { 64, 0 };
static const ButtonReport BLE_REMOTE_MENU    = { 128, 0 };
static const ButtonReport BLE_REMOTE_UP      = { 0, 1 };
static const ButtonReport BLE_REMOTE_DOWN    = { 0, 2 };
static const ButtonReport BLE_REMOTE_LEFT    = { 0, 4 };
static const ButtonReport BLE_REMOTE_RIGHT   = { 0, 8 };

class BLERemote {
private:
    ButtonReport          consumerButtons;
    BLEStatus*            connectionStatus;
    NimBLEHIDDevice*      hid;
    NimBLECharacteristic* inputConsumer;
    std::string           deviceManufacturer;
    std::string           deviceName;
    uint8_t               batteryLevel;
    static void           taskServer(void* pvParameter);
    void                  sendReport(void);

public:
    BLERemote(void);
    void setup(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel);
    void press(const ButtonReport b);
    void release(const ButtonReport b);
    void click(const ButtonReport b);
    bool isConnected(void);
    void setBatteryLevel(uint8_t level);
    void end(void);

    // protected:
    //     virtual void onStarted(NimBLEServer* pServer) {};
};

#endif // BLE_REMOTE_H