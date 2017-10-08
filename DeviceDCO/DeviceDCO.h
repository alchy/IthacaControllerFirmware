#ifndef DEVICEDCO_H_
#define DEVICEDCO_H_

#include <stdint.h>
#include "I2CHelper.h"
#include "I2CDevice.h"


// dco specific controls
#define REG_B_LED                           0x00
#define REG_B_R2R                           0x01
#define REG_B_DIVIDER                       0x02
#define REG_W_COUNTER                       0x03
#define REG_B_CONTROL                       0x04
#define REG_B_VELOCITY                      0x05
#define REG_B_CALIBRATE                     0xf0

#define CONTROL_NOTE_ON                     0x80
#define CONTROL_NOTE_OFF                    0x00
#define CONTROL_LED_TOGGLE                  0x40
#define CONTROL_APPLY                       0x20

class DeviceDCO : public I2CDevice
{
public:
    DeviceDCO(PinName sda, PinName scl, uint8_t device_address);

    void set_led(uint8_t val);
    void set_gate(uint8_t val);
    void set_r2r(uint8_t val);
    void set_velocity(uint8_t val);
    void set_divider(uint8_t val);
    void set_counter(uint16_t val);
    void set_frequency(uint8_t note, int16_t pitchwheel, int16_t modulation);
    void set_sysex_parameter(uint8_t sysex_parameter_nr, int16_t val);
    void apply(void);
    
private:
    uint8_t     led_;
    uint8_t     gate_;
    uint8_t     r2r_;
    uint8_t     velocity_;
    uint8_t     divider_;
    uint16_t    counter_;
    uint8_t     force_update_;
    int16_t     sysex_parameter_[128];
};


#endif
