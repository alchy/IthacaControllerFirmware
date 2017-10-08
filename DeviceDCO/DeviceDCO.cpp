#include "mbed.h"

#include <stdio.h>
#include <stdint.h>
#include <globals.h>

#include "I2CHelper.h"
#include "I2CDevice.h"

#include "DeviceDCO.h"
#include "Frequencies.h"

extern Serial      pc;

DeviceDCO::DeviceDCO(PinName sda, PinName scl, uint8_t device_address) : I2CDevice(sda, scl, device_address)
{
    led_            = 0x00;
    gate_           = 0x00;
    r2r_            = 0x00;
    divider_        = 0x00;
    counter_        = 0x0000;
    force_update_   = false;
    for(uint8_t sysex_parameter_nr = 0; sysex_parameter_nr < 128; sysex_parameter_nr++) {
        sysex_parameter_[sysex_parameter_nr] = 0xffff;
    }
}

void DeviceDCO::set_led(uint8_t val)
{
    if(led_ != val) {
        led_ = val;
        force_update_ = true;
        write8(REG_B_LED, val);
    }
}

void DeviceDCO::set_r2r(uint8_t val)
{
    if(r2r_ != val) {
        r2r_ = val;
        force_update_ = true;
        write8(REG_B_R2R, val);
        //printf("[d] setting r2r %d\n\r", val);
    }
}

void DeviceDCO::set_velocity(uint8_t val)
{
    if(velocity_ != val) {
        velocity_ = val;
        force_update_ = true;
        //printf("[d] setting velocity %d\n\r", val);
        write8(REG_B_VELOCITY, val);
    }
}

void DeviceDCO::set_divider(uint8_t val)
{
    if(divider_ != val) {
        divider_ = val;
        force_update_ = true;
        write8(REG_B_DIVIDER, val);
    }
}

void DeviceDCO::set_counter(uint16_t val)
{
    if(counter_ != val) {
        counter_ = val;
        force_update_ = true;
        write16(REG_W_COUNTER, val);
    }
}

void DeviceDCO::set_gate(uint8_t val)
{
    if(gate_ != val) {
        gate_ = val;
        force_update_ = true;
    }
}

void DeviceDCO::apply(void)
{
    if(force_update_) {
        force_update_ = false;
        if(gate_) {
            write8(REG_B_CONTROL, CONTROL_APPLY | CONTROL_LED_TOGGLE | CONTROL_NOTE_ON);
        } else {
            write8(REG_B_CONTROL, CONTROL_APPLY | CONTROL_LED_TOGGLE | CONTROL_NOTE_OFF);
        }
    }
}

void DeviceDCO::set_sysex_parameter(uint8_t sysex_parameter_nr, int16_t val)
{
    if(sysex_parameter_nr < SYSEX_RANGE_DCO_END + 1) {                     // DCO sysex register range highest 
        if(sysex_parameter_nr > SYSEX_RANGE_DCO_START - 1) {               // DCO sysex register range lowest
            if(sysex_parameter_[sysex_parameter_nr] != val) {
                sysex_parameter_[sysex_parameter_nr] = val;
                //printf("[d] write to dco reg: %d, val: %d\n\r", sysex_parameter_nr, sysex_parameter_[sysex_parameter_nr]);
                write16(sysex_parameter_nr, val);
            }
        }
    }
}

void DeviceDCO::set_frequency(uint8_t note, int16_t pitchwheel, int16_t modulation)
{
    Frequencies frequency_;
    pitchwheel = pitchwheel >> 5;                                               // full pitchwheel range: shift by 5->2 notes, shift by 6-> 1 note
    set_counter(frequency_.get_counter((note * 0x80) + pitchwheel + modulation));
    set_divider(frequency_.get_divider((note * 0x80) + pitchwheel + modulation));
    set_r2r(frequency_.get_r2r((note * 0x80) + pitchwheel + modulation));
}
