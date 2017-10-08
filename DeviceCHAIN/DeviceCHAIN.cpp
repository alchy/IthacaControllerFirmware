#include "mbed.h"

#include <stdio.h>
#include <stdint.h>
#include <globals.h>

#include "I2CHelper.h"
#include "I2CDevice.h"

#include "DeviceCHAIN.h"

extern Serial      pc;

DeviceCHAIN::DeviceCHAIN(PinName sda, PinName scl, uint8_t device_address) : I2CDevice(sda, scl, device_address)
{
    for(uint8_t sysex_parameter_nr = 0; sysex_parameter_nr < 128; sysex_parameter_nr++) {
        sysex_parameter_[sysex_parameter_nr] = 0xffff;
    }
}

void DeviceCHAIN::set_sysex_parameter(uint8_t sysex_parameter_nr, int16_t val)
{
    if(getDeviceType() == DEV_TYPE_FILTER_GAIN || getDeviceType() == DEV_TYPE_SHAPER_GAIN) 
    {
        if(sysex_parameter_nr < SYSEX_RANGE_CHAIN_END + 1) {               // DCO sysex register range highest
            if(sysex_parameter_nr > SYSEX_RANGE_CHAIN_START - 1) {         // DCO sysex register range lowest
                if(sysex_parameter_[sysex_parameter_nr] != val) {
                    sysex_parameter_[sysex_parameter_nr] = val;
                    //printf("[d] write to chain reg: %d, val: %d\n\r", sysex_parameter_nr, sysex_parameter_[sysex_parameter_nr]);
                    write16(sysex_parameter_nr, val);
                }
            }
        }
    }
}