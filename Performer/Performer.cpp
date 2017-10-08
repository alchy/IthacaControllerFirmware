#include "mbed.h"

#include <stdio.h>
#include <stdint.h>

#include "globals.h"
#include "TextLCD.h"
#include "Performer.h"
#include "DeviceCHAIN.h"

extern class DeviceDCO   *dco[];
extern class DeviceCHAIN *chain[];

Performer::Performer(void) {
    set_dco_count(0);
    scan_bus();                                                                 // create global DeviceDCO *dco[];
    build_dumb_voices();                                                        // create DeviceVoice *voice_[] 
    if(get_dco_count()) {
        printf("[i] %d dco(s) created\n\r", get_dco_count());                                                      
    } else {
        printf("[!] no dco, can't continue...\n\r");
        //for(;;);
    }
    assign_dcos_to_voices(DCO_PER_VOICE_DEFAULT);    
    if(get_voice_count()) {
        printf("[i] %d voices created\n\r", get_voice_count());
    } else {
        printf("[!] no voices, can't continue...\n\r");
        //for(;;);
    }
}

void Performer::scan_bus(void)
{
    uint8_t     i2c_address;
    for(i2c_address = DCO_ADDRESS_SPACE_BEGIN; i2c_address < DCO_ADDRESS_SPACE_END + 1; i2c_address++) {
        dco[i2c_address - DCO_ADDRESS_SPACE_BEGIN] = new DeviceDCO(PB_9, PB_8, i2c_address);
        printf("[i] testing address [%2.2X]: device type [%2.2X] \n\r", i2c_address, \
                  dco[i2c_address - DCO_ADDRESS_SPACE_BEGIN]->getDeviceType());
    }
    for(i2c_address = CHAIN_ADDRESS_SPACE_BEGIN; i2c_address < CHAIN_ADDRESS_SPACE_END + 1; i2c_address++) {
        chain[i2c_address - CHAIN_ADDRESS_SPACE_BEGIN] = new DeviceCHAIN(PB_9, PB_8, i2c_address);
        printf("[i] testing address [%2.2X]: device type [%2.2X] \n\r", i2c_address, \
                  chain[i2c_address - CHAIN_ADDRESS_SPACE_BEGIN]->getDeviceType());
    }
}

void Performer::build_dumb_voices(void)
{
    uint8_t     dco_count;
    uint8_t     dco_count_real = 0;
    for(dco_count = 0; dco_count < DCO_ADDRESS_SPACE_END - DCO_ADDRESS_SPACE_BEGIN + 1; dco_count++) { 
        printf("[i] identifying device nr. %d\n\r", dco_count);
        if(dco[dco_count]->getDeviceType() == DEV_TYPE_DCO) {
            printf("[i] device %d identified as dco\n\r", dco_count);
            voice_[dco_count] = new DeviceVoice(0, dco_count);                  // create voice with 0 dcos assigned, but give it an queue position
            dco_count_real++;
            printf("[!] incrementing dco count real, value is %d\n\r", dco_count_real);
        } else {
            printf("[i] device %d is not identified as dco\n\r", dco_count);
        }
    }
    set_dco_count(dco_count_real);
}

void Performer::assign_dcos_to_voices(uint8_t dco_per_voice) {
    if(dco_per_voice > get_dco_count()) {
        dco_per_voice = get_dco_count();
    }
    uint8_t     voice_count, dco_count, alpha = 0;    
    for(voice_count = 0; voice_count < get_dco_count() / dco_per_voice; voice_count++) {
        voice_[voice_count]->reset_dco_count();
        for(dco_count = 0; dco_count < dco_per_voice; dco_count++) {
            while(dco[alpha]->getDeviceType() != DEV_TYPE_DCO) {
                alpha++;
            }
            //printf("[i] dco %d assigned to voice %d\n\r", alpha, voice_count);
            voice_[voice_count]->append_dco(alpha);                             // DeviceVoice *   voice_[16], 
            alpha++;        
        }
    }
    set_voice_count(voice_count);
}

void Performer::play(uint8_t gate, uint8_t key, uint8_t velocity) {
    uint8_t free_voice;
    if(gate == 0) {
        for(uint8_t voice = 0; voice < get_voice_count(); voice++) {
            if(voice_[voice]->get_note() == key) {
                voice_[voice]->play(0, key);
            }
        }
    } else {
        free_voice = get_free_voice(key);
        voice_[free_voice]->play(1, key);                                       // get unused voice or steal
        voice_[free_voice]->velocity(velocity);                                 // velocity set only on note on event
    }
}

void Performer::refresh(void) {
    for(uint8_t voice = 0; voice < get_voice_count(); voice++) {
        voice_[voice]->refresh();
    }
}

void Performer::pitchwheel(int16_t pitchwheel) {
    for(uint8_t voice = 0; voice < get_voice_count(); voice++) {
        voice_[voice]->pitchwheel(pitchwheel);
    }
}

void Performer::modulation(int16_t modulation) {
    for(uint8_t voice = 0; voice < get_voice_count(); voice++) {
        voice_[voice]->modulation(modulation);
    }
}

void Performer::sysex(uint8_t sysex_parameter_nr, int16_t val) 
{
    for(uint8_t voice = 0; voice < get_voice_count(); voice++) {                
        voice_[voice]->sysex(sysex_parameter_nr, val);                          // update DCO units (if sysex belongs to this type of unit is handled by the class)
    }
    for(uint8_t chain_device = 0; chain_device < (CHAIN_ADDRESS_SPACE_END - CHAIN_ADDRESS_SPACE_BEGIN) + 1; chain_device++) {
        chain[chain_device]->set_sysex_parameter(sysex_parameter_nr, val);      // update CHAIN devices (if sysex belongs to this type of unit is handled by the class)
    }
}

void Performer::set_voice_count(uint8_t voice_count) {
    voice_count_ = voice_count;
}

uint8_t Performer::get_voice_count(void) {
    return(voice_count_);
}

void Performer::set_dco_count(uint8_t dco_count) {
    dco_count_ = dco_count;
}

uint8_t Performer::get_dco_count(void) {
    return(dco_count_);
}

void Performer::mixle_queue(uint8_t queue_number) {
    for(uint8_t alpha = 0; alpha < get_voice_count(); alpha++) {
        if(voice_[alpha]->get_queue() == queue_number) {
            voice_[alpha]->set_queue(0);
        } else {
            voice_[alpha]->set_queue(voice_[alpha]->get_queue() + 1);  
        }
    }
    for(uint8_t alpha = 0; alpha < get_voice_count(); alpha++) {
        if(voice_[alpha]->get_queue() > queue_number) {
            voice_[alpha]->set_queue(voice_[alpha]->get_queue() - 1);
        }
    }
}

uint8_t Performer::get_free_voice(uint8_t note) {
    uint8_t voice, voice_candidate = 0xff; 
    for(voice = 0; voice < get_voice_count(); voice++) {                        // easy, note plays already or stopped playing and not replaced by other note yet
        if(voice_[voice]->get_note() == note) {                                 // no magic
            return voice;                                                       // return voice 
        }
    }
    for(voice = 0; voice < get_voice_count(); voice++) {                        // select unused voice with highest queue nr.
        if(voice_[voice]->get_gate() == 0) {                                    // *** key-off must change the queue number to the latest ***
            if(voice_candidate == 0xff) {                                       // no candidate yet
                voice_candidate = voice;                                        // pick first candidate
            } else {
                if(voice_[voice]->get_queue() > voice_[voice_candidate]->get_queue()) { // actual is more ontop stack than previous
                    voice_candidate = voice;                                    // use one which is higher
                }
            }
        }
    }
    if(voice_candidate != 0xff) {                                               // podarilo se vybrat
        mixle_queue(voice_[voice_candidate]->get_queue());                      // mixle that, put selected on bottom of queue              
        return voice_candidate;                                                 // return best voice candidate
    }
    for(voice = 0; voice < get_voice_count(); voice++) {                        // no free voice, must steal one - pick one with highest queue number
        if(voice_[voice]->get_queue() == get_voice_count() - 1) {               // voice on top of the stack
            mixle_queue(voice_[voice]->get_queue());                            // mixle that
            return voice;
        }
    }   
    return voice;                                                               // this code will never be reached
}
