#ifndef I2CDEVICE_H_
#define I2CDEVICE_H_

#include "mbed.h"
#include <stdint.h>
#include "I2CHelper.h"

class I2CDevice : private I2CHelper
{
    public:
        I2CDevice(PinName sda, PinName scl, uint8_t device_address);
        
        uint8_t     getDeviceType(void);
        
        uint8_t     read8(uint8_t reg);
        uint16_t    read16(uint8_t reg);
        void        write8(uint8_t reg, uint8_t val);
        void        write16(uint8_t reg, uint16_t val);
        
    private:
        uint8_t     device_address_;
        uint8_t     device_type_;
        I2CHelper   device_bus_;
};
    
#endif
