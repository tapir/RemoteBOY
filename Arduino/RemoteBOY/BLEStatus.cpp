#include "BLEStatus.h"

BLEStatus::BLEStatus(void) { }

void BLEStatus::onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
    this->connected = true;
}

void BLEStatus::onDisconnect(NimBLEServer* pServer) {
    this->connected = false;
}