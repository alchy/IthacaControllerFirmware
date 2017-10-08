// ****************************
// midi code definitions:
// http://www.midi.org/techspecs/midimessages.php
// physical layer:
// http://www.personal.kent.edu/~sbirch/Music_Production/MP-II/MIDI/midi_physical_layer.htm
// excelent midi protocol coding tutorial:
// http://www.music-software-development.com/midi-tutorial.html
// ****************************

#ifndef MIDI_H
#define MIDI_H

#include <stdint.h>
#include "ActiveKeys.h"

#define SYSEX_MAX_LENGTH    512
#define SYSEX_ID_LENGTH     4
static uint8_t sysex_id[4] = {0x00, 0x01, 0x04, 0x01};                          // SGI reserved :)

class MidiParser : public ActiveKeys 
{
public:
    MidiParser();
    void    Put(uint8_t byte);
    void    Parse(void);
    uint8_t getRecentCC(void);
    uint8_t cc2sysex[127];                                                      // control change to sysex mapping
    
private:
    struct Buffer {
        uint8_t     buffer[256];                                                // 16byte circular buffer
        uint8_t     p_store;                                                    // circular buffer pointer to store new data
        uint8_t     p_parse;                                                    // circular buffer pointer on data currently processed
    } buffer;

    struct Message {
        uint8_t byte;                                           // midi in byte
        uint8_t count;                                          // how many bytest midi_event_type has
        uint8_t type;                                           // type of event
        uint8_t channel;                                        // channel
    } message;

    struct Note {
        uint8_t key;
        uint8_t velocity;
        uint8_t pitchwheel_lsb;
        uint8_t pitchwheel_msb;
        int16_t pitchwheel;

    } note;

    struct ControlChange {
        uint8_t    cc[128];                                 // persistent slots where all the incoming CC values are written to, used in midi.cpp
        uint8_t    slot;                                    // persistent storage for CC value while reading CC by midi.cpp
        uint8_t    recent_cc;                               // recent cc used (control change register)
    } cc;
    
    void    setRecentCC(uint8_t);
    
    struct  SystemExclusive {
        uint8_t     data[SYSEX_MAX_LENGTH];
        uint16_t    last_index;
    } sysex;
    
    void ChannelMessage(void);
    void SystemMessage(void);
        
    void NoteOffEvent(void);
    void NoteOnEvent(void);
    void PitchWheelChange(void);
    void ProgramChange(void);
    void ControlChange(void);

    void    SystemExclusive(void);
    void    SystemExclusiveEnd(void);
    bool    SystemExclusive_CompareID(void);
    uint8_t SystemExclusive_getParameterType(void);
    uint16_t SystemExclusive_getParameterValue(void);
};

//
// midi
//



// ***************************************
// Handling 60H, 61H inc/dec MIDI messages 
// http://www.midi.org/techspecs/rp18.php
// http://www.doepfer.de/pd_tec_e.htm
// ***************************************


// ***************************************
// Registered Parameter Number (RPN)
// http://www.blitter.com/~russtopia/MIDI/~jglatt/tech/midispec/rpn.htm
// ***************************************
#define RPN_MSB                     101                 // 0x65, For controllers 6, 38, 96, and 97, it selects the RPN parameter. 
#define RPN_LSB                     100                 // 0x64, For controllers 6, 38, 96, and 97, it selects the RPN parameter.

#define RPN_VALUE_MSB               6                   // PARAM aka coarse (MSB) value               
#define RPN_VALUE_LSB               38                  // PARAM aka fine (LSB) value 


#define RPN_Pitch_Bend_Sensitivity  0                   // Pitch Bend Range (ie, Sensitivity) 0x0000
#define RPN_Master_Fine_Tuning      1                   // Master Fine Tuning (ie, in cents) 0x0001
#define RPN_Master_Coarse_Tuning    2                   // Master Coarse Tuning (in half-steps) 0x0002
#define RPN_Reset                   0x3FFF              // No coarse or fine adjustments are applicable. This is a "dummy" parameter.


// ***************************************
// Non-Registered Parameter Number (RPN)
// ***************************************
#define NRPN_MSB                    99                  // For controllers 6, 38, 96, and 97, it selects the NRPN parameter.
#define NRPN_LSB                    98                  // For controllers 6, 38, 96, and 97, it selects the NRPN parameter.


// ***************************************
// CHANNEL VOICE MESSAGES
// ***************************************

//                                        M S G E|c h n l
#define NOTE_OFF_EVENT                BIN(1,0,0,0,0,0,0,0)
#define NOTE_OFF_EVENT_BC             2                       // Note Off event. 
                                                              // 0kkkkkkk + 0vvvvvvv
                                                              // This message is sent when a note is released (ended).
                                                              // (kkkkkkk) is the key (note) number. (vvvvvvv) is the velocity.

//                                        M S G E|c h n l
#define NOTE_ON_EVENT                 BIN(1,0,0,1,0,0,0,0)
#define NOTE_ON_EVENT_BC              2                       // Note On event. 
                                                              // 0kkkkkkk + 0vvvvvvv
                                                              // This message is sent when a note is depressed (start).
                                                              // (kkkkkkk) is the key (note) number. (vvvvvvv) is the velocity.

//                                        M S G E|c h n l
#define POLYPHONIC_KEY_PRESSURE       BIN(1,0,1,0,0,0,0,0)
#define POLYPHONIC_KEY_PRESSURE_BC    2                       // Polyphonic Key Pressure (Aftertouch). 
                                                              // 0kkkkkkk + 0vvvvvvv
                                                              // This message is most often sent by pressing down on the
                                                              // key after it "bottoms out". (kkkkkkk) is the key (note)
                                                              // number. (vvvvvvv) is the pressure value.

//                                        M S G E|c h n l
#define CONTROL_CHANGE                BIN(1,0,1,1,0,0,0,0)
#define CONTROL_CHANGE_BC             2                       // Control Change. 
                                                              // 0ccccccc + 0vvvvvvv
                                                              // This message is sent when a controller value changes.
                                                              // Controllers include devices such as pedals and levers.
                                                              // Controller numbers 120-127 are reserved as "Channel Mode
                                                              // Messages" (below). (ccccccc) is the controller number
                                                              // (0-119). (vvvvvvv) is the controller value (0-127).

//                                        M S G E|c h n l
#define PROGRAM_CHANGE                BIN(1,1,0,0,0,0,0,0)
#define PROGRAM_CHANGE_BC             1                       // Program Change.
                                                              // 0ppppppp
                                                              // This message sent when the patch number
                                                              // changes. (ppppppp) is the new program number.

//                                        M S G E|c h n l
#define CHANNEL_PRESSURE              BIN(1,1,0,1,0,0,0,0)
#define CHANNEL_PRESSURE_BC           1                       // Channel Pressure (After-touch).
                                                              // (vvvvvvv) is the pressure value.

//                                        M S G E|c h n l
#define PITCH_WHELL_CHANGE            BIN(1,1,1,0,0,0,0,0)
#define PITCH_WHELL_CHANGE_BC         2                       // Pitch Wheel Change. 
                                                              // 0lllllll + 0mmmmmmm
                                                              // This message is sent to
                                                              // indicate a change in the pitch wheel. The pitch wheel is
                                                              // measured by a fourteen bit value. Center (no pitch change) is
                                                              // 2000H. Sensitivity is a function of the transmitter. (llllll) are
                                                              // the least significant 7 bits. (mmmmmm) are the most
                                                              // significant 7 bits.




// ***************************************
// SYSTEM COMMON MESSAGES
// ***************************************

#define SYSTEM_EXCLUSIVE                BIN(1,1,1,1,0,0,0,0)
#define SYSTEM_EXCLUSIVE_BC             65535                 // not supported yet
                                                              // !!! stream ends with END_OF_EXCLUSIVE.

#define MIDI_TIME_CODE_QUARTER_FRAME    BIN(1,1,1,1,0,0,0,1)
#define MIDI_TIME_CODE_QUARTER_FRAME_BC 1                     // 0nnndddd, where:
                                                              // nnn = Message Type
                                                              // dddd = Values

#define SONG_POSITION_POINTER           BIN(1,1,1,1,0,0,1,0)
#define SONG_POSITION_POINTER_BC        2                     // 0lllllll + 0mmmmmmm 
                                                              // 14 bit register, l is the LSB, m the MSB

#define SONG_SELECT                     BIN(1,1,1,1,0,0,1,1)
#define SONG_SELECT_BC                  1                     // 0sssssss
                                                              // (sssssss) specifies which sequence
                                                              // or song is to be played.

#define RESERVED1                       BIN(1,1,1,1,0,1,0,0)
#define RESERVED1_BC                    0

#define RESERVED2                       BIN(1,1,1,1,0,1,0,1)
#define RESERVED2_BC                    0

#define TUNE_REQUEST                    BIN(1,1,1,1,0,1,1,0)  // Tune Request. 
                                                              // Upon receiving a Tune Request,
                                                              // all analog synthesizers should
                                                              // tune their oscillators.
#define TUNE_REQUEST_BC                 0

#define END_OF_EXCLUSIVE                BIN(1,1,1,1,0,1,1,1)   
#define END_OF_EXCLUSIVE_BC             0                     // End of Exclusive. Used to terminate
                                                              // a System Exclusive dump (see above).
// System Real-Time Messages
#define TIMING_CLOCK                    BIN(1,1,1,1,1,0,0,0)
#define TIMING_CLOCK_BC                 0                     // Timing Clock. Sent 24 times per quarter note when 
                                                              // synchronization is required (see text).

#define UNDEFINED3                      BIN(1,1,1,1,1,0,0,1)
#define UNDEFINED3_BC                   0

#define START                           BIN(1,1,1,1,1,0,1,0)
#define START_BC                        0                     // Start. Start the current sequence playing. 
                                                              // This message will be followed with Timing Clocks.

#define CONTINUE                        BIN(1,1,1,1,1,0,1,1)
#define CONTINUE_BC                     0                     // Continue. Continue at the point the sequence was Stopped.

#define STOP                            BIN(1,1,1,1,1,1,0,0)
#define STOP_BC                         0                     // Stop. Stop the current sequence. 

#define UNDEFINED4                      BIN(1,1,1,1,1,1,0,1)
#define UNDEFINED4_BC                   0                     // Undefined. (Reserved)

#define ACTIVE_SENSING                  BIN(1,1,1,1,1,1,1,0)
#define ACTIVE_SENSING_BC               0                     // Active Sensing. This message is intended to be sent 
                                                              // repeatedly to tell the receiver that a connection is alive. Use of
                                                              // this message is optional. When initially received, the receiver
                                                              // will expect to receive another Active Sensing message each
                                                              // 300ms (max), and if it does not then it will assume that the
                                                              // connection has been terminated. At termination, the receiver
                                                              // will turn off all voices and return to normal (non- active
                                                              // sensing) operation.


#define RESET                           BIN(1,1,1,1,1,1,1,1)
#define RESET_BC                        0                     // Reset. Reset all receivers in the system to power-up status. 
                                                              // This should be used sparingly, preferably under manual
                                                              // control. In particular, it should not be sent on power-up.

#endif // MIDI_H