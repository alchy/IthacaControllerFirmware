#include "I2CHelper.h"

/* Constructors */
I2CHelper::I2CHelper() : i2c_(I2C_SDA, I2C_SCL) {}

I2CHelper::I2CHelper(PinName sda, PinName scl) : i2c_(sda, scl) {}

void I2CHelper::setFrequency(int hz) {
    i2c_.frequency(hz);
}


/* Read Functions */
bool I2CHelper::readBit(const uint8_t devAddr, const uint8_t regAddr, const uint8_t bit, uint8_t *data) {
    uint8_t buf;
    i2c_.write((int)(devAddr << 1), (char *)&regAddr, 1, true);
    i2c_.read((int)(devAddr << 1), (char *)&buf, 1);
    *data = buf & (1 << bit);
    return true;
}

bool I2CHelper::readBits(const uint8_t devAddr, const uint8_t regAddr, const uint8_t startBit, const uint8_t length, uint8_t *data) {
    uint8_t buf;
    int status = i2c_.write((int)(devAddr << 1), (char *)&regAddr, 1, true);
    status = i2c_.read((int)(devAddr << 1), (char *)&buf, 1);
    buf >>= (startBit - length + 1); 
    buf &= ((1 << length) - 1);
    *data = buf;
    return true;
}

bool I2CHelper::readByte(const uint8_t devAddr, const uint8_t regAddr, uint8_t *data) {
    return readBytes(devAddr, regAddr, data, 1);
}

bool I2CHelper::readBytes(const uint8_t devAddr, const uint8_t regAddr, uint8_t *data, const uint8_t length) {
    i2c_.write((int)(devAddr << 1), (char *)&regAddr, 1, true);
    return i2c_.read((int)(devAddr << 1), (char *)data, (int)length);
}

bool I2CHelper::readWord(const uint8_t devAddr, const uint8_t regAddr, uint16_t *data) {
    return readWords(devAddr, regAddr, data, 1);
}

bool I2CHelper::readWords(const uint8_t devAddr, const uint8_t regAddr, uint16_t *data, const uint8_t length) {
    i2c_.write((int)(devAddr << 1), (char *)&regAddr, 1, true);
    return i2c_.read((int)(devAddr << 1), (char *)data, length * 2);
}

/* Write Functions */
bool I2CHelper::writeBit(const uint8_t devAddr, const uint8_t regAddr, const uint8_t bit, const uint8_t data) {
    uint8_t buf;
    i2c_.write((int)(devAddr << 1), (char *)&regAddr, 1, true);
    i2c_.read((int)(devAddr << 1), (char *)&buf, 1, true);
    buf = (data != 0) ? (buf | (1 << bit)) : (buf & ~(1 << bit));
    i2c_.write((int)(devAddr << 1), (char *)&buf, 1);
    return true;
}

bool I2CHelper::writeBits(const uint8_t devAddr, const uint8_t regAddr, const uint8_t startBit, const uint8_t length, const uint8_t data) {
    uint8_t buf;
    i2c_.write((int)(devAddr << 1), (char *)&regAddr, 1, true);
    i2c_.read((int)(devAddr << 1), (char *)&buf, 1, true);
    uint8_t dat = data << (startBit - length + 1);
    uint8_t mask = ((1 << length) - 1) << (startBit - length + 1);
    dat &= mask;
    buf &= ~mask;
    buf |= dat;
    i2c_.write((int)(devAddr << 1), (char *)&buf, 1);
    return true;
}

bool I2CHelper::writeByte(const uint8_t devAddr, const uint8_t regAddr, const uint8_t data) {
    return writeBytes(devAddr, regAddr, &data, 1);
}

bool I2CHelper::writeBytes(const uint8_t devAddr, const uint8_t regAddr, const uint8_t *data, const uint8_t length) {
    /*i2c_.start();
    i2c_.write((int)(devAddr << 1));
    i2c_.write((int)regAddr);
    for(int i = 0; i < length; i++) {
        i2c_.write(data[i]);
    }
    i2c_.stop();*/
    char *buf = (char *)malloc(length + 1);
    buf[0] = regAddr;
    for(int i = 0; i < length; i++) {
        buf[i + 1] = data[i];
    }
    i2c_.write((int)(devAddr << 1), buf, length + 1);
    free(buf);
    return true;
}