#include <stdio.h>
#include <stdint.h>

#include "globals.h"
#include "DeviceDCO.h"
#include "DeviceVoice.h"

extern struct lfo_struct lfo;
extern class DeviceDCO   * dco[];

DeviceVoice::DeviceVoice(uint8_t dco_count, uint8_t voice_queue) : dco_count_(dco_count), voice_note_(0), voice_gate_(0) , voice_queue_(voice_queue)
{
    set_modulation(0);
    set_pitchwheel(0);
    set_velocity(0);
    set_gate(0);
    set_note(0);
}

DeviceVoice::~DeviceVoice(void)
{
                                                                                // empty
}

void DeviceVoice::reset_dco_count() {
    dco_count_ = 0;
}

void DeviceVoice::append_dco(uint8_t dco_number)
{
    dco_[dco_count_] = dco_number;                                              // each DeviceVoice instance has its own private dco_count
    set_gate(0);                                                                // mute voices when changing polyphony
    dco_count_++;
}

void DeviceVoice::play(uint8_t gate, uint8_t note)
{
    set_gate(gate);
    set_note(note);
}

void DeviceVoice::velocity(uint8_t velocity) {
    set_velocity(velocity);
}

void DeviceVoice::pitchwheel(int16_t pitchwheel) {
    set_pitchwheel(pitchwheel);
}

void DeviceVoice::modulation(int16_t modulation) {
    set_modulation(modulation);
}

void DeviceVoice::sysex(int8_t sysex_parameter_nr, int16_t val) {
    for(uint8_t alpha = 0; alpha < dco_count_; alpha++) {
        dco[dco_[alpha]]->set_sysex_parameter(sysex_parameter_nr, val);
    }
}

void DeviceVoice::refresh(void) {
    for(uint8_t alpha = 0; alpha < dco_count_; alpha++) {
        //printf("[d] set_frequency[%d](%d, %d, %d)\n\r", alpha, get_note(),  get_pitchwheel(), get_modulation());
        dco[dco_[alpha]]->set_frequency(get_note(), get_pitchwheel(), get_modulation() + lfo.tune[alpha]);        
        dco[dco_[alpha]]->set_velocity(get_velocity());
        dco[dco_[alpha]]->set_gate(get_gate());
        dco[dco_[alpha]]->apply();
    }
}

void DeviceVoice::set_pitchwheel(int16_t pitchwheel) {
    pitchwheel_ = pitchwheel;
}

int16_t DeviceVoice::get_pitchwheel() {
    return(pitchwheel_);
}

void DeviceVoice::set_modulation(int16_t modulation) {
    modulation_ = modulation;
}
    
int16_t DeviceVoice::get_modulation() {
    return(modulation_);
};

void DeviceVoice::set_note(uint8_t note)
{
    voice_note_ = note;
}

uint8_t DeviceVoice::get_note(void)
{
    return(voice_note_);
}

void DeviceVoice::set_velocity(uint8_t velocity)
{
    voice_velocity_ = velocity;
}

uint8_t DeviceVoice::get_velocity(void)
{
    return(voice_velocity_);
}

void DeviceVoice::set_gate(uint8_t gate)
{
    voice_gate_ = gate;
}

uint8_t DeviceVoice::get_gate(void)
{
    return(voice_gate_);
}

void DeviceVoice::set_queue(uint8_t queue)
{
    voice_queue_ = queue;
}

uint8_t DeviceVoice::get_queue(void)
{
    return(voice_queue_);
}
