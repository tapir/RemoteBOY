#include "HIDKeyboardTypes.h"
#include "HIDTypes.h"
#include "NimBLEHIDDevice.h"
#include "sdkconfig.h"
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <driver/adc.h>

#include "BLERemote.h"
#include "BLEStatus.h"

static const uint8_t  BLE_CONSUMER_REPORT_ID    = 0x01;
static const uint16_t BLE_HID_REMOTE_APPEARANCE = 0x0180;
static const uint16_t BLE_VEND_ID               = 0x0000;
static const uint8_t  BLE_VEND_SRC              = 0x02;
static const uint16_t BLE_PROD_ID               = 0x0000;
static const uint16_t BLE_PROD_VER              = 0x0001;

static const uint8_t _hidReportDescriptor[]
    = {
          USAGE_PAGE(1), 0x0C,                  // USAGE_PAGE (Consumer)
          USAGE(1), 0x01,                       // USAGE (Consumer Control)
          COLLECTION(1), 0x01,                  // COLLECTION (Application)
          REPORT_ID(1), BLE_CONSUMER_REPORT_ID, //   REPORT_ID (1)
          LOGICAL_MINIMUM(1), 0x00,             //   LOGICAL_MINIMUM (0)
          LOGICAL_MAXIMUM(1), 0x01,             //   LOGICAL_MAXIMUM (1)
          REPORT_SIZE(1), 0x01,                 //   REPORT_SIZE (1)
          REPORT_COUNT(1), 0x0C,                //   REPORT_COUNT (12)
          USAGE(1), 0x30,                       //   Power
          USAGE(1), 0x41,                       //   Menu Pick (Select)
          USAGE(1), 0x46,                       //   Menu Escape (Back)
          USAGE(2), 0x23, 0x02,                 //   Home
          USAGE(1), 0xE9,                       //   Volume Increase
          USAGE(1), 0xEA,                       //   Volume Decrease
          USAGE(1), 0xE2,                       //   Volume Mute
          USAGE(1), 0x40,                       //   Menu
          USAGE(1), 0x42,                       //   Menu Up
          USAGE(1), 0x43,                       //   Menu Down
          USAGE(1), 0x44,                       //   Menu Left
          USAGE(1), 0x45,                       //   Menu Right
          HIDINPUT(1), 0x02,                    //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
          REPORT_SIZE(1), 0x04,                 //   REPORT_SIZE (4) - Padding 4 bit
          REPORT_COUNT(1), 0x01,                //   REPORT_COUNT (1)
          HIDINPUT(1), 0x03,                    //   INPUT
          END_COLLECTION(0)                     // END COLLECTION
      };

BLERemote::BLERemote(void) { }

void BLERemote::setup(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) {
    this->_consumerButtons[0] = 0;
    this->_consumerButtons[1] = 0;
    this->hid                 = 0;
    this->deviceName          = deviceName;
    this->deviceManufacturer  = deviceManufacturer;
    this->batteryLevel        = batteryLevel;
    this->connectionStatus    = new BLEStatus();
    xTaskCreate(this->taskServer, "server", 20000, (void*)this, 1, NULL);
}

void BLERemote::press(const ButtonReport b) {
    uint16_t b_16            = b[1] | (b[0] << 8);
    uint16_t buttonReport_16 = this->_consumerButtons[1] | (this->_consumerButtons[0] << 8);

    buttonReport_16 |= b_16;
    this->_consumerButtons[0] = (uint8_t)((buttonReport_16 & 0xFF00) >> 8);
    this->_consumerButtons[1] = (uint8_t)(buttonReport_16 & 0x00FF);

    this->sendReport();
}

void BLERemote::release(const ButtonReport b) {
    uint16_t b_16            = b[1] | (b[0] << 8);
    uint16_t buttonReport_16 = this->_consumerButtons[1] | (this->_consumerButtons[0] << 8);

    buttonReport_16 &= ~b_16;
    this->_consumerButtons[0] = (uint8_t)((buttonReport_16 & 0xFF00) >> 8);
    this->_consumerButtons[1] = (uint8_t)(buttonReport_16 & 0x00FF);

    this->sendReport();
}

void BLERemote::click(const ButtonReport b) {
    this->press(b);
    delay(80);
    this->release(b);
}

void BLERemote::sendReport(void) {
    if (this->isConnected()) {
        this->inputConsumer->setValue((uint8_t*)this->_consumerButtons, sizeof(ButtonReport));
        this->inputConsumer->notify();
    }
}

bool BLERemote::isConnected(void) {
    return this->connectionStatus->connected;
}

void BLERemote::setDisconnected(void) {
    this->connectionStatus->connected = false;
}

void BLERemote::setBatteryLevel(uint8_t level) {
    if (this->isConnected()) {
        if (level != this->batteryLevel) {
            this->batteryLevel = level;
            if (hid != 0) {
                this->hid->setBatteryLevel(level);
            }
        }
    }
}

void BLERemote::taskServer(void* pvParameter) {
    BLERemote* BLERemoteInstance = (BLERemote*)pvParameter;
    NimBLEDevice::init(BLERemoteInstance->deviceName);

    NimBLEServer* pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(BLERemoteInstance->connectionStatus);
    pServer->advertiseOnDisconnect(true);
    BLERemoteInstance->bleServer                       = pServer;
    BLERemoteInstance->hid                             = new NimBLEHIDDevice(pServer);
    BLERemoteInstance->inputConsumer                   = BLERemoteInstance->hid->inputReport(BLE_CONSUMER_REPORT_ID);
    BLERemoteInstance->connectionStatus->inputConsumer = BLERemoteInstance->inputConsumer;
    BLERemoteInstance->hid->manufacturer()->setValue(BLERemoteInstance->deviceManufacturer);
    BLERemoteInstance->hid->pnp(BLE_VEND_SRC, BLE_VEND_ID, BLE_PROD_ID, BLE_PROD_VER);
    BLERemoteInstance->hid->hidInfo(0x00, 0x01);

    NimBLEDevice::setSecurityAuth(true, false, false);

    BLERemoteInstance->hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
    BLERemoteInstance->hid->startServices();
    BLERemoteInstance->onStarted(pServer);

    NimBLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(BLE_HID_REMOTE_APPEARANCE);
    pAdvertising->addServiceUUID(BLERemoteInstance->hid->hidService()->getUUID());
    pAdvertising->start();
    BLERemoteInstance->hid->setBatteryLevel(BLERemoteInstance->batteryLevel);

    vTaskDelay(portMAX_DELAY);
}

void BLERemote::disconnect(void) {
    size_t peersNum = this->bleServer->getConnectedCount();
    for (int i = 0; i < peersNum; i++) {
        uint16_t connID = this->bleServer->getPeerInfo(i).getConnHandle();
        this->bleServer->disconnect(connID);
    }
}