#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

class Battery {
public:
    Battery(void);
    void    setup(void);
    void    loop(void);
    uint8_t getLevel(void);
    float   getVoltage(void);

private:
    float   voltage;
    uint8_t level;
};

#endif // BATTERY_H