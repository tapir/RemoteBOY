#ifndef BLE_STATUS_H
#define BLE_STATUS_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "nimconfig.h"
#if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)

#include "NimBLECharacteristic.h"
#include <NimBLEServer.h>

class BLEStatus : public NimBLEServerCallbacks {
public:
    BLEStatus(void);
    bool                  connected = false;
    void                  onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc);
    void                  onDisconnect(NimBLEServer* pServer);
    void                  onAuthenticationComplete(ble_gap_conn_desc* desc);
    NimBLECharacteristic* inputConsumer;
};

#endif // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif // CONFIG_BT_ENABLED
#endif // BLE_STATUS_H