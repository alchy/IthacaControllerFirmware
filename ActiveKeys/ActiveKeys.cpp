#include "mbed.h"

#include <stdio.h>
#include <stdint.h>
#include <globals.h>

#include "ActiveKeys.h"

ActiveKeys::ActiveKeys() : Sysex() {
    pop_map_note_on_ptr_ = 0;
    pop_map_note_off_ptr_ = 0;
    for(uint8_t channel = 0; channel < 16; channel++) {
        for(uint8_t key = 0; key < 128; key++) {
            keymap_[channel][key].velocity = 0;
            keymap_[channel][key].processed = true;
        }
    }
    set_map_pitchwheel(0);
}

void ActiveKeys::put_map_note_on(uint8_t channel, uint8_t key, uint8_t velocity)
{
    keymap_[channel][key].velocity = velocity;
    keymap_[channel][key].processed = false;
}

uint8_t ActiveKeys::pop_map_note_on(uint8_t channel)
{
    while(pop_map_note_on_ptr_ < 128) {
        if(keymap_[channel][pop_map_note_on_ptr_].velocity > 0 && keymap_[channel][pop_map_note_on_ptr_].processed == false) {
            keymap_[channel][pop_map_note_on_ptr_].processed = true;
            return(pop_map_note_on_ptr_);
        }
        pop_map_note_on_ptr_++;
    }
    pop_map_note_on_ptr_ = 0;
    return(0xff);
}

uint8_t ActiveKeys::get_map_velocity(uint8_t channel, uint8_t key)
{
    return(keymap_[channel][key].velocity);
}

void ActiveKeys::put_map_note_off(uint8_t channel, uint8_t key)
{
    keymap_[channel][key].velocity = 0;
    keymap_[channel][key].processed = false;
}

uint8_t ActiveKeys::pop_map_note_off(uint8_t channel)
{
    while(pop_map_note_off_ptr_ < 128) {
        if(keymap_[channel][pop_map_note_off_ptr_].velocity == 0 && keymap_[channel][pop_map_note_off_ptr_].processed == false) {
            keymap_[channel][pop_map_note_off_ptr_].processed = true;
            return(pop_map_note_off_ptr_);
        }
        pop_map_note_off_ptr_++;
    }
    pop_map_note_off_ptr_ = 0;
    return(0xff);
}

void ActiveKeys::mark_as_processed(uint8_t channel, uint8_t key)
{
    keymap_[channel][key].processed = true;
}

void ActiveKeys::set_map_pitchwheel(int16_t pitchwheel)
{
    pitchwheel_ = pitchwheel;
}

int16_t ActiveKeys::get_map_pitchwheel(void)
{
    return(pitchwheel_);
}

