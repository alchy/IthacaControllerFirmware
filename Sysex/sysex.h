#ifndef SYSEX_H_
#define SYSEX_H_

#include <stdint.h>

class Sysex
{
    public:
        Sysex(void);
        
        void set_sysex_register(uint8_t sysex_register_nr, int16_t value);
        int16_t get_sysex_register(uint8_t sysex_register_nr);
        int16_t pop_sysex_register(uint8_t sysex_register_nr);
        bool is_sysex_register_processed(uint8_t sysex_register_nr);
        
    private:
        struct Sysex_register {
            int16_t value;
            bool    processed;
        };

    struct Sysex_register sysex_register_[128];
};
#endif