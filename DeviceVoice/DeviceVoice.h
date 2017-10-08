#ifndef DEVICEVOICE_H_
#define DEVICEVOICE_H_

#include <stdint.h>

#include "DeviceDCO.h"

class DeviceVoice
{

public:
    DeviceVoice(uint8_t dco_count, uint8_t voice_queue);
    ~DeviceVoice();

    void    reset_dco_count(void);
    void    append_dco(uint8_t dco_number);
    
    void    play(uint8_t gate, uint8_t note);
    void    velocity(uint8_t velocity);
    void    pitchwheel(int16_t pitchwheel); 
    void    modulation(int16_t modulation);
    void    sysex(int8_t sysex_parameter_nr, int16_t val);
    void    refresh(void);
    
    uint8_t get_note(void);
    uint8_t get_gate(void);
    uint8_t get_queue(void);
    void    set_queue(uint8_t val);

private:
    void    set_pitchwheel(int16_t pitchwheel);    
    int16_t get_pitchwheel(void);
    void    set_modulation(int16_t modulation);
    int16_t get_modulation(void);
    void    set_note(uint8_t note);
    void    set_gate(uint8_t);
    void    set_velocity(uint8_t velocity);
    uint8_t get_velocity(void);
    
    
    uint8_t dco_[16];
    uint8_t dco_count_;
    uint8_t voice_note_;
    uint8_t voice_gate_;
    uint8_t voice_velocity_;
    int16_t pitchwheel_;
    int16_t modulation_;
    uint8_t voice_queue_;
};

#endif
