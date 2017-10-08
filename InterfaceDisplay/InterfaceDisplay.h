#ifndef INTERFACEDISPLAY_H_
#define INTERFACEDISPLAY_H_

#include <stdint.h>
#include "midi.h"
#include "InterfaceSDCard.h"

class InterfaceDisplay
{
public:
    InterfaceDisplay(MidiParser *midiParser);
    void page(bool force_refresh);
private:
    bool getEncoder0(int16_t enc_min_value, int16_t enc_max_value);
    bool getEncoder1(void);
    void content(void);
    void loadROMpatch(void);
    void loadRAMpatch(void);
    void saveRAMpatch(void);
    void assignCC(uint8_t sysex_register_to_assign);
    
    MidiParser          *_midiParser;
    InterfaceSDCard     *_sdcard;
    
    int16_t _value;
    bool    _force_refresh, _patch_rom_load, _patch_ram_load;
    bool    _assign_button_state_historical, _assign_button_state_actual;
    int16_t _patch_rom_load_nr, _patch_rom_load_nr_historical; 
    int16_t _patch_ram_load_nr, _patch_ram_load_nr_historical;
    int16_t _patch_ram_save_nr, _patch_ram_save_nr_historical;
    uint8_t _content_selection;
    int16_t _pos0[3], _enc0_data_historical, _enc0_data_actual;
    int16_t _pos1[3], _enc1_data_historical, _enc1_data_actual;

    struct menu_item {
        uint8_t  page;
        char    *group_name;
        char    *sysex_name;
        uint8_t  sysex_address;
        uint8_t  sysex_increment;
        int16_t  sysex_min_value;
        int16_t  sysex_max_value;
        int16_t  sysex_reset_value;
    } menu_item[256];
    
    struct patch {
        const uint16_t *_patch_data;
    } _patch[256];

};

#endif