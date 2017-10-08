#ifndef I2CHELPER_H_
#define I2CHELPER_H_

#include "mbed.h"

class I2CHelper {
public:
    I2CHelper();
    I2CHelper(PinName sda, PinName scl);
    
    void setFrequency(int hz);
    
    bool readBit(const uint8_t devAddr, const uint8_t regAddr, const uint8_t bit, uint8_t *data);
    bool readBits(const uint8_t devAddr, const uint8_t regAddr, const uint8_t startBit, const uint8_t length, uint8_t *data);
    bool readByte(const uint8_t devAddr, const uint8_t regAddr, uint8_t *data);
    bool readBytes(const uint8_t devAddr, const uint8_t regAddr, uint8_t *data, const uint8_t length);
    bool readWord(const uint8_t devAddr, const uint8_t regAddr, uint16_t *data);
    bool readWords(const uint8_t devAddr, const uint8_t regAddr, uint16_t *data, const uint8_t length);
    
    bool writeBit(const uint8_t devAddr, const uint8_t regAddr, const uint8_t bit, const uint8_t data);
    bool writeBits(const uint8_t devAddr, const uint8_t regAddr, const uint8_t startBit, const uint8_t length, const uint8_t data);
    bool writeByte(const uint8_t devAddr, const uint8_t regAddr, const uint8_t data);
    bool writeBytes(const uint8_t devAddr, const uint8_t regAddr, const uint8_t *data, const uint8_t length);
private:
    I2C i2c_;
};

#endif