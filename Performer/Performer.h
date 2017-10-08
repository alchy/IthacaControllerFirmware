#ifndef Performer_H_
#define Performer_H_

#include <stdint.h>

//#include "I2CDevice.h"
#include "DeviceDCO.h"
#include "DeviceVoice.h"

class Performer {
    public:
        Performer(void);

        uint8_t get_voice_count(void);
        uint8_t get_dco_count(void);
        void    assign_dcos_to_voices(uint8_t dco_per_voice);
                
        void    play(uint8_t gate, uint8_t note, uint8_t velocity);
        void    pitchwheel(int16_t pitchwheel);
        void    modulation(int16_t modulation);
        void    sysex(uint8_t sysex_parameter_nr, int16_t val);
        void    refresh(void);
        
    private:
        void    scan_bus(void);
        void    build_dumb_voices(void);
        void    set_dco_count(uint8_t dco_count);
        void    set_voice_count(uint8_t voice_count);
        uint8_t get_free_voice(uint8_t note);
        void    mixle_queue(uint8_t queue_number);
        
        DeviceVoice *   voice_[16];
        uint8_t         voice_count_;
        uint8_t         dco_count_;
};

#endif