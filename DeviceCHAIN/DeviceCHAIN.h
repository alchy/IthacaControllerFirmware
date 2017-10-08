#ifndef DEVICECHAIN_H_
#define DEVICECHAIN_H_

#include <stdint.h>
#include "I2CHelper.h"
#include "I2CDevice.h"

class DeviceCHAIN : public I2CDevice
{
public:
    DeviceCHAIN(PinName sda, PinName scl, uint8_t device_address);

    void set_sysex_parameter(uint8_t sysex_parameter_nr, int16_t val);
    
private:
    int16_t     sysex_parameter_[128];
};
#endif
