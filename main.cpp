#include "mbed.h"
#include <stdint.h>

#include "midi.h"
#include "globals.h"
#include "Performer.h"
#include "DeviceCHAIN.h"
#include "sysex.h"
#include "analogpotread.h"
#include "InterfaceDisplay.h"

Serial              pc(USBTX, USBRX);                                           // Virtual serial port over USB (takes Serial2), 9600 bauds, 8-bit, no parity
Serial              ser1(PB_6, PA_10);                                          // MIDI port on Serial1(tx, rx)

MidiParser          midiParser;                                                 // MidiParser class
DigitalOut          midi_led(PA_5);                                             // LD2 for 411RE is PA_5

DeviceDCO           *dco[16];                                                   // globally shared dco structures
DeviceCHAIN         *chain[64];                                                 // other non dco devices (4 chains to 1 dco)

InterfaceDisplay    *display;                                                   // pointer on display class

Performer           performer;                                                  // master player           
AnalogPotRead       potentiometers;                                             // read pots from external board

Ticker              lfo_ticker;                                                 // define ticker
struct              lfo_struct      lfo;

// ******************************************************************************
// LFO irq
// ******************************************************************************
void lfo_irq(void) {
    lfo.process_irq = true;
}

void lfo_process(void) {
    if(lfo.process_irq == true) {
        float step = 0;
        if(lfo.speed_parameter != 0) {    
            step = (0.003141592 * lfo.speed_parameter) / 2;
        }                                                                       
        lfo.step_persistent += step;                                            // increment persistent_step
        if(lfo.step_persistent > 2 * 3.141592) {                                // 2pi absolute 
            lfo.step_persistent = 0;                                            // reset
        }                                                                       // modulation is common for all voices
        lfo.modulation = sin(lfo.step_persistent) * (lfo.amplitude_parameter / 2);
        
        for(uint8_t a = 0; a < 4; a++) 
        {
            if(! midiParser.is_sysex_register_processed(a) || ! midiParser.is_sysex_register_processed(a+4) ) {
                uint16_t tune_octave;
                uint16_t tune_fine;
                tune_octave = midiParser.pop_sysex_register(a);                 // pop for processing
                if(tune_octave < 510 || tune_octave > 514) {                    // +/-2 octaves only (512 == 0)
                    tune_octave = 512;                                          // if outside the range, reset 
                }                                                               // +/- 512 == +/- 4 note range
                tune_fine = midiParser.pop_sysex_register(a+4);                 // 0x80 == 128 'cents' per semitone                          
                lfo.tune[a] = ((tune_octave - 512) * (0x80 * 12)) + (tune_fine - 512);
                //printf("[d] DCO %d tune octave set to: %d\n\r ", a, lfo.tune[a]);
            }
        }
        lfo.process_irq = false;
    }
}

void MidiInterrupt (void)                                                       // interrupts
{
    midiParser.Put(ser1.getc());
}

void setupMIDI(void)                                                            // setup MIDI
{
    pc.baud(9600);
    ser1.format(8, SerialBase::None, 1);                                        // bits, parity, stop_bits
    ser1.attach(&MidiInterrupt);                                                // IRQ on callback
    ser1.baud(31250);                                                           // speed
    printf("[i] SERIAL3 <-> MIDI initialized\n\r");
}
    
int main()
{
    display = new InterfaceDisplay(&midiParser);
    printf("\n\n\n[i] Ithaca OS 0.010 booting up...\n\r");    
    setupMIDI();
    printf("[i] attaching lfo ticker...\n\r");
    lfo_ticker.attach(lfo_irq, 0.025);                                          // attach routine, invoke 25msec

    uint8_t channel = 0;
    display->page(true);
    while(1) {
        display->page(false);
        midiParser.Parse();
        uint8_t key;
        while((key = midiParser.pop_map_note_off(channel)) != 0xff) {           // read note-off changes for specific channel
            performer.play(0, key, 0);                                          // note is off when it has velocity set to 0 (MIDI note on with 0 velocity == NOTE_OFF)
            midi_led = 0;
        }                                                                       // velocity will not be changed in Performer::play
        while((key = midiParser.pop_map_note_on(channel)) != 0xff) {            // read note-on changes for specific channel
            performer.play(1, key, midiParser.get_map_velocity(channel, key));  // velocity will be changed in Performer::play            
            midi_led = 1;
        }
        performer.pitchwheel(midiParser.get_map_pitchwheel());
        performer.modulation(lfo.modulation);
        performer.refresh();
        lfo_process();
                                                                                // process local (controller) sysex, no need to send outside the controller
        if(! midiParser.is_sysex_register_processed(SYSEX_W_LOCAL_COUNT_OF_DCOS_PER_VOICE)) {
            performer.assign_dcos_to_voices(midiParser.pop_sysex_register(SYSEX_W_LOCAL_COUNT_OF_DCOS_PER_VOICE));
        }
        lfo.speed_parameter = midiParser.pop_sysex_register(SYSEX_W_LOCAL_LFO_SPEED);
        lfo.amplitude_parameter = midiParser.pop_sysex_register(SYSEX_W_LOCAL_LFO_AMPLITUDE);
        // process sysex for DCO & CHAIN and send the changes
        for(uint8_t register_nr = SYSEX_RANGE_DCO_START; register_nr < SYSEX_RANGE_CHAIN_END; register_nr++) {        
            if(! midiParser.is_sysex_register_processed(register_nr)) {
                //printf("[i] sysex register %d[%d]\n\r", register_nr, midiParser.pop_sysex_register(register_nr));
                performer.sysex(register_nr, midiParser.pop_sysex_register(register_nr));
            }
        } 
    }
}
