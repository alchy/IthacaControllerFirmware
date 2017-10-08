#ifndef ACTIVEKEYS_H_
#define ACTIVEKEYS_H_

#include <stdint.h>
#include "sysex.h"

class ActiveKeys : public Sysex
{
    public:
        ActiveKeys(void);
        
        void    put_map_note_on(uint8_t channel, uint8_t key, uint8_t velocity);
        uint8_t get_map_velocity(uint8_t channel, uint8_t key);
        uint8_t pop_map_note_on(uint8_t channel);
        
        void    put_map_note_off(uint8_t channel, uint8_t key);
        uint8_t pop_map_note_off(uint8_t channel);
        
        void    set_map_pitchwheel(int16_t pitchwheel);
        int16_t get_map_pitchwheel(void);
        
        void    mark_as_processed(uint8_t channel, uint8_t key);      
        void    printAllActiveKeys(void);
        
    private:   
        uint8_t interface_;
        
        struct  Key {
            uint8_t velocity;
            bool    processed;
        };
        
        struct  Key     keymap_[16][128];
        int16_t         pitchwheel_;
        uint8_t         pop_map_note_on_ptr_;
        uint8_t         pop_map_note_off_ptr_;
};

#endif