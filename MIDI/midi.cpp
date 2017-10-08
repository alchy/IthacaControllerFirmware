#include "mbed.h"
#include "globals.h"
#include "ActiveKeys.h"
#include "midi.h"

MidiParser::MidiParser() : ActiveKeys()
{
    buffer.p_store = 0;
    buffer.p_parse = 0;
    for(uint8_t alpha = 0; alpha < 128; alpha++) {                              // clear cc2sysex 
        cc2sysex[alpha] = 0xff;                                                 // no assignment
    }
    setRecentCC(0xff);                                                          // no CC received yet
}

void MidiParser::Put(uint8_t byte)
{
    buffer.buffer[buffer.p_store] = byte;                                       // store data to buffer
    buffer.p_store++;                                                           // increment store pointer
}

void MidiParser::Parse(void)
{
    while(buffer.p_store != buffer.p_parse) {
        message.byte = buffer.buffer[buffer.p_parse];
        buffer.p_parse++;                                                       // increment buffer pointer (processed) to process new data byte
        if(message.byte >> 7) {                                                 // STATUS byte! STATUS byte determines the type of the message.
            message.count = 0;                                                  // a new message -> reset
            if(message.byte & 0xf0 == 0xf0) {                                   // --> SystemMessage, all four MSBits up
                message.type = message.byte;                                    // all bits are significant for SystemMessages
            } else {                                                            // --> ChannelMessage
                message.type = message.byte & 0xf0;                             // eventtype is the 4 MSBs
                message.channel = message.byte & 0x0f;                          // midi channel is the 4 LSBs (1-000, 1-001, 1-010, 1-011, 1-100, 1-101, 1-110)
            }                                                                   // there are 7 MIDI channel voice messages and each one is defined by the first four bits of its status byte.
        }                                                                       //
        if((message.type & 0xf0) == 0xf0) {                                     // msbit is 1 - if all four MSBits up
            SystemMessage();                                                    // process this as SystemMessage (global)
        } else {                                                                //
            ChannelMessage();                                                   // process as ChannelMessages (channel specific)
        }                                                                       //
    }
}

void MidiParser::ChannelMessage(void)
{
    // channel message is either:
    //                              Voice Message
    //                              Mode Message
    switch(message.type) {
        case NOTE_OFF_EVENT:                                                    // status byte 1000nnnn
            NoteOffEvent();
            break;
        case NOTE_ON_EVENT:                                                     // status byte 1001nnnn
            NoteOnEvent();
            break;
        case POLYPHONIC_KEY_PRESSURE:                                           // status byte 1010nnnn
            //printf( "[i] POLYPHONIC_KEY_PRESSURE (not yet implemented)\n\r" );
            //PolyphonicKeyPressure();
            break;
        case CONTROL_CHANGE:                                                    // status byte 1011nnnn --> Mode Messages are control change messages
            ControlChange();                                                    // Mode messages are the only "channel messages" that affect the entire MIDI device and aren't channel specific.
            break;
        case PROGRAM_CHANGE:                                                    // status byte 1100nnnn
            ProgramChange();
            break;
        case CHANNEL_PRESSURE:                                                  // status byte 1101nnnn
            // Voyager keyboard generates this
            //printf("[i] CHANNEL_PRESSURE (not yet implemented)\n\r");
            //ChannelPressure();
            break;
        case PITCH_WHELL_CHANGE:                                                // status byte 1110nnnn
            PitchWheelChange();
            break;
    }
}

void MidiParser::SystemMessage(void)
{
    switch(message.type) {
        case SYSTEM_EXCLUSIVE:
            //printf( "[i] SYSTEM_EXCLUSIVE\n\r" );
            SystemExclusive();
            break;
        case END_OF_EXCLUSIVE:
            //printf( "[i] END_OF_EXCLUSIVE\n\r" );
            SystemExclusiveEnd();
            break;
        case MIDI_TIME_CODE_QUARTER_FRAME:
            printf( "[i] MIDI_TIME_CODE_QUARTER_FRAME\n\r" );
            break;
        case SONG_POSITION_POINTER:
            printf( "[i] SONG_POSITION_POINTER\n\r" );
            break;
        case SONG_SELECT:
            printf( "[i] SONG_SELECT\n\r" );
            break;
        case RESERVED1:
            printf( "[i] RESERVED1\n\r" );
            break;
        case RESERVED2:
            printf( "[i] RESERVED2\n\r" );
            break;
        case TUNE_REQUEST:
            printf( "[i] TUNE_REQUEST\n\r" );
            break;
        case TIMING_CLOCK:
            printf( "[i] TIMING_CLOCK\n\r" );
            break;
        case UNDEFINED3:
            printf( "[i] UNDEFINED3\n\r" );
            break;
        case START:
            printf( "[i] START\n\r" );
            break;
        case CONTINUE:
            printf( "[i] CONTINUE\n\r" );
            break;
        case STOP:
            printf( "[i] STOP\n\r" );
            break;
        case UNDEFINED4:
            printf( "[i] UNDEFINED4\n\r" );
            break;
        case ACTIVE_SENSING:
            printf( "[i] ACTIVE_SENSING\n\r" );
            break;
        case RESET:
            printf( "[i] RESET\n\r" );
            break;
        default:
            printf("[i] UNKNOWN ChannelVoiceMessages\n\r");
            break;
    }
}

void MidiParser::NoteOffEvent()
{
    switch(message.count) {
        case 0:
            //printf( "[i] NOTE_OFF_EVENT\n\r" );
            break;
        case 1:
            note.key = message.byte;
            break;
        case 2:
            note.velocity = message.byte;
            //printf( "[d] CH %d, KEY %d, VEL: %d\n\r", message.channel, note.key, note.velocity );
            put_map_note_off(message.channel, note.key);
            break;
    }
    message.count++;
}

void MidiParser::NoteOnEvent()
{
    switch(message.count) {
        case 0:
            //printf( "[i] NOTE_ON_EVENT\n\r" );
            break;
        case 1:
            note.key = message.byte;
            break;
        case 2:
            note.velocity = message.byte;
            //printf( "[d] CH %d, KEY %d, VEL: %d\n\r", message.channel, note.key, note.velocity);
            if(note.velocity == 0) {
                put_map_note_off(message.channel, note.key);
            } else {
                put_map_note_on(message.channel, note.key, note.velocity);
            }
            break;
    }
    message.count++;
}

void MidiParser::PitchWheelChange()
{
    switch(message.count) {
        case 0:
            //printf("[i] PITCH_WHELL_CHANGE\n\r");
            break;
        case 1:
            note.pitchwheel_lsb = message.byte;
            break;
        case 2:
            note.pitchwheel_msb = message.byte;
            note.pitchwheel = note.pitchwheel_msb;
            note.pitchwheel = note.pitchwheel << 7;
            note.pitchwheel |= note.pitchwheel_lsb;
            note.pitchwheel = note.pitchwheel - 0x2000;
            //printf("[d] PITCHWHEEL: %d\n\r", note.pitchwheel);
            set_map_pitchwheel(note.pitchwheel);                                // inherited from ActiveKeys
            break;
    }
    message.count++;
}

void MidiParser::ProgramChange()                                                // change instrument on a given channel
{
    switch(message.count) {
        case 0:
            printf("[i] PROGRAM_CHANGE\n\r");
            break;
        case 1:
            printf("[i] channel: %d, val: %d\n\r", message.channel, message.byte);
            break;
    }
    message.count++;
}

void MidiParser::setRecentCC(uint8_t recent_cc) {
    cc.recent_cc = recent_cc;
}

uint8_t MidiParser::getRecentCC(void) {
    return(cc.recent_cc);
}

void MidiParser::ControlChange()                                                // Mode messages are the only "channel messages" that affect the entire MIDI device and aren't channel specific.
{
    switch(message.count) {
        case 0:
            //printf( "[i] CONTROL_CHANGE\n\r" );
            break;
        case 1:
            cc.slot = message.byte;
            break;
        case 2:
            setRecentCC(cc.slot);
            cc.cc[cc.slot] = message.byte;
            
            if(cc2sysex[cc.slot] < 128) {                                       // is this CC assingned to sysex?
                set_sysex_register(cc2sysex[cc.slot], message.byte * 8);        // send value to sysex register
            }
            /*
            printf("[i] channel: %d, slot: %d, val: %d\n\r", message.channel, cc.slot, cc.cc[cc.slot]);
                                                                // CC:  0 = Sound bank selection (MSB)
                                                                //      1 = Modulation wheel, often assigned to a vibrato or tremolo effect.
                                                                //      7 = Volume level of the instrument
                                                                //      10 = Panoramic (0 = left; 64 = center; 127 = right)
                                                                //      11 = Expression
                                                                //      32 = Sound bank selection (LSB)
                                                                //      64 = Sustain pedal (0 = no pedal; >= 64 => pedal ON)
                                                                //      121 = All controllers off (this message clears all the controller values for this channel, back to their default values)
                                                                //      123 = All notes off (this message stops all the notes that are currently playing)
            */
            break;
    }
    message.count++;
}

void MidiParser::SystemExclusive()                                              // midi manufacturers IDs:
{                                                                               // https://www.midi.org/specifications/item/manufacturer-id-numbers
    switch(message.count) {                             
        case 0:
            //printf("[i] SYSTEM_EXCLUSIVE START\n\r");
            break;
        default:
            if(message.count > SYSEX_MAX_LENGTH) {                              // sysex data larger than buffer
                printf("[i] SYSTEM_EXCLUSIVE byte %d exceeded SYSEX_MAX_LENGTH!\n\r", message.byte);
            } else {
                //printf("[i] SYSTEM_EXCLUSIVE[%d] received: %d\n\r", message.count - 1, message.byte);
                sysex.data[message.count - 1] = message.byte;                   // save data to buffer,1st MSB, 2nd LSB
                sysex.last_index = message.count - 1;
            }                                                           
            break;
    } 
    message.count++;
}

void MidiParser::SystemExclusiveEnd() 
{
    switch(message.count) {
        case 0:
            //printf("[i] SYSTEM_EXCLUSIVE_END, data[%d] is last data\n\r", sysex.last_index);
            break;
        case 1:
            if(SystemExclusive_CompareID()) {
                //printf("[i] sysex param %d[%d]\n\r", SystemExclusive_getParameterType(), SystemExclusive_getParameterValue());
                printf("[d] sysex: %d, %d, \n\r", SystemExclusive_getParameterType(), SystemExclusive_getParameterValue());
                if(SystemExclusive_getParameterType() < 128) {
                    set_sysex_register(SystemExclusive_getParameterType(), SystemExclusive_getParameterValue());
                } else {
                    printf("[!] systemExclusive_getParameterType() > 128\n\r");
                }
            } else {
                printf("[i] SystemExclusive_CompareID(): sysex is not for us...\n\r");
                for(uint8_t compare = 0; compare < SYSEX_ID_LENGTH; compare++) {
                    printf(":%x", sysex.data[compare]);
                }
                printf("\n\r");
            }
            //printf("[i] SYSTEM_EXCLUSIVE_END COMPLETED...\n\r");
            break;
        default:
            //printf("[i] SYSTEM_EXCLUSIVE_END received extra bytes...\n\r");
            break;
    }
    message.count++;
}

bool MidiParser::SystemExclusive_CompareID() {
    uint8_t match = 0;
    for(uint8_t compare = 0; compare < SYSEX_ID_LENGTH; compare++) {           // compare first four bytes if they fit our ID
        if(sysex.data[compare] == sysex_id[compare]) {
            match++;
        }
    }    
    if(match == SYSEX_ID_LENGTH) {
        return(true);
    };
    return(false);
}

uint8_t MidiParser::SystemExclusive_getParameterType() {
    return(sysex.data[5]);
}

uint16_t MidiParser::SystemExclusive_getParameterValue() {
    uint16_t param_value = 0;
    param_value = sysex.data[6];
    param_value = param_value << 7;
    param_value |= sysex.data[7];
    return(param_value);
}
