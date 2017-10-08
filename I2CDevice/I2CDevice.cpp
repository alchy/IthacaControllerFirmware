#include "mbed.h"

#include <stdio.h>
#include <stdint.h>
#include <globals.h>

#include "I2CHelper.h"
#include "I2CDevice.h"

extern Serial      pc;

I2CDevice::I2CDevice(PinName sda, PinName scl, uint8_t device_address) :  I2CHelper(sda, scl) {
    setFrequency(I2C_FREQUENCY);
    device_address_ = device_address;                                           // remember device_address_
    device_type_ = 0xff;
    readByte(device_address_, REG_B_DEVTYPE, & device_type_);                   // remember device_type_
};
        
uint8_t I2CDevice::getDeviceType(void) {
    return(device_type_);                                                       // return device_type_
};

uint8_t I2CDevice::read8(uint8_t reg) {
    uint8_t data[1]; 
    readByte(device_address_, reg, data);
    return(data[0]);
};

uint16_t I2CDevice::read16(uint8_t reg) {
    uint16_t data;  
    readWord(device_address_, reg, & data);                                     // catch? returns MSB first
    return(data);
};

void I2CDevice::write8(uint8_t reg, uint8_t val) {
    writeByte(device_address_, reg, val);    
};

void I2CDevice::write16(uint8_t reg, uint16_t val) { 
    uint8_t data[2];
    data[0] = uint8_t(val & 0x00ff);                                            // LSB first
    data[1] = uint8_t(val >> 8);                                                // MSB then
    writeBytes(device_address_, reg, data, 2);  
};
