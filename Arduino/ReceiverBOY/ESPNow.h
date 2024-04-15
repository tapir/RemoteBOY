#ifndef ESPNOW_H
#define ESPNOW_H

#include <esp_now.h>

class ESPNow {
public:
    ESPNow(void);
    void setup(esp_now_recv_cb_t cb);
};

#endif // ESPNOW_H