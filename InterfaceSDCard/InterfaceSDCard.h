#ifndef INTERFACESDCARD_H_
#define INTERFACESDCARD_H_

#include <stdint.h>
#include <string>
#include <vector>

#include "midi.h"

class InterfaceSDCard
{
    public:
        InterfaceSDCard(MidiParser *midiParser);
        void read_directory_list(char *dir);
        bool writePatch(uint16_t patch_number, const uint16_t *patch_data);     // patch_data is a template from ROM
        bool readPatch(uint16_t patch_number);
        void test(void); 
        
    private:
        vector<string> _directory_list;                                         // directory listing stored in a vector string
        MidiParser *_midiParser;
        uint16_t _load_buffer[128];
};
    
#endif
