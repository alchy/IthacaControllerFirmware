#include "mbed.h"
#include <stdint.h>
#include "sysex.h"

Sysex::Sysex()
{
    for(uint8_t reg_number = 0; reg_number < 128; reg_number++) {
        sysex_register_[reg_number].value = 0;
        sysex_register_[reg_number].processed = true;
    }
}

void Sysex::set_sysex_register(uint8_t sysex_register_nr, int16_t value) {
    if(sysex_register_nr < 128) {
        sysex_register_[sysex_register_nr].value = value;
        sysex_register_[sysex_register_nr].processed = false;    
    } else {
        printf("[!] set_sysex_register got register %d, this is over 127\n\r", sysex_register_nr);
    }
}
    
int16_t Sysex::get_sysex_register(uint8_t sysex_register_nr) {
    return(sysex_register_[sysex_register_nr].value);
}
    
int16_t Sysex::pop_sysex_register(uint8_t sysex_register_nr) {
    sysex_register_[sysex_register_nr].processed = true;
    return(sysex_register_[sysex_register_nr].value);
}

bool Sysex::is_sysex_register_processed(uint8_t sysex_register_nr) {
    return(sysex_register_[sysex_register_nr].processed);
}
