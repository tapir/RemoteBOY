#ifndef BLE_STATUS_H
#define BLE_STATUS_H

#include <NimBLEServer.h>

class BLEStatus : public NimBLEServerCallbacks {
public:
    BLEStatus(void);
    bool connected = false;
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc);
    void onDisconnect(NimBLEServer* pServer);
};

#endif // BLE_STATUS_H