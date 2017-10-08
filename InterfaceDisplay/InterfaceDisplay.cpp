#include "mbed.h"

#include <stdio.h>
#include <stdint.h>
#include <globals.h>

#include "TextLCD.h"
#include "QEI.h"
#include "InterfaceDisplay.h"
#include "InterfaceSDCard.h"

// the lib used for TextLCD: https://developer.mbed.org/users/wim/code/TextLCD/docs/tip/
// the other, much simpler: https://developer.mbed.org/users/simon/code/TextLCD/docs/308d188a2d3a/classTextLCD.html

TextLCD lcd(PB_0, PB_1, PA_8, PA_9, PA_13, PA_14, TextLCD::LCD40x4, NC, PB_2, TextLCD::HD44780);    // 4bit bus: rs, e, d4-d7
QEI encoder0(PB_3, PB_4, NC, 24, QEI::X4_ENCODING);      // rotary encoders
QEI encoder1(PA_0, PA_1, NC, 24, QEI::X4_ENCODING);      // rotary encoders

DigitalIn content_0(PB_12);
DigitalIn content_1(PA_15);
DigitalIn content_2(PB_10);
DigitalIn assign(PC_5);

static const uint16_t patch_default[] = { 16, 196,
                                          17, 600,
                                          18, 424,
                                          19, 568,
                                          68, 0,
                                          72, 776,
                                          76, 0,
                                          20, 860,
                                          80, 0,
                                          81, 1023,
                                          82, 0,
                                          83, 704,
                                          31, 53,
                                          38, 104,
                                          27, 6,
                                          39, 504,
                                          40, 500,
                                          41, 564,
                                          15, 2,
                                          0, 511,
                                          14, 8,
                                          13, 16,
                                          1, 512,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 470,
                                          6, 0,
                                          7, 0,
                                          100, 472,
                                          104, 452,
                                          108, 0,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
                                         
                                         
InterfaceDisplay::InterfaceDisplay(MidiParser *midiParser) : _midiParser(midiParser)
{
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("                                     0.1");
    lcd.printf(" Ithaca SOUND MODULE                    ");
    lcd.printf("                           booting up...");
    lcd.printf("                                        ");

    _pos0[0] = 0;
    _pos0[1] = 0;
    _pos0[2] = 0;
    _enc0_data_historical = 0;
    _enc0_data_actual = 0;
    encoder0.reset();

    _pos1[0] = 0;
    _pos1[1] = 0;
    _pos1[2] = 0;
    _enc1_data_historical = 0;
    _enc1_data_actual = 0;
    encoder1.reset();

    _content_selection = 0;

    _patch_rom_load_nr = 0;
    _patch_rom_load_nr_historical = 1;
    _patch_rom_load = true;

    _patch_ram_load_nr = 0;
    _patch_ram_load_nr_historical = 1;
    _patch_ram_load = false;

    _patch_ram_save_nr = 0;
    _patch_ram_save_nr_historical = 1;

    /* PAGES */
    menu_item[0].page = 0;
    //nu_item[0].group_name = "12345678"
    menu_item[0].group_name = "WAVEFORM";
    //nu_item[0].sysex_name = "1234567890123456789012"
    menu_item[0].sysex_name = "Saw                   ";
    menu_item[0].sysex_address = SYSEX_W_WS_GAIN0_VIRTUAL_REG_A;
    menu_item[0].sysex_increment = 1;
    menu_item[0].sysex_min_value = 0;
    menu_item[0].sysex_max_value = 1023;
    menu_item[0].sysex_reset_value = 1023;

    menu_item[1].page = 0;
    //nu_item[1].group_name = "12345678"
    menu_item[1].group_name = "WAVEFORM";
    //nu_item[1].sysex_name = "1234567890123456789012"
    menu_item[1].sysex_name = "Sub Square            ";
    menu_item[1].sysex_address = SYSEX_W_WS_GAIN1_VIRTUAL_REG_A;
    menu_item[1].sysex_increment = 1;
    menu_item[1].sysex_min_value = 0;
    menu_item[1].sysex_max_value = 1023;
    menu_item[1].sysex_reset_value = 0;

    menu_item[2].page = 0;
    //nu_item[2].group_name = "12345678"
    menu_item[2].group_name = "WAVEFORM";
    //nu_item[2].sysex_name = "1234567890123456789012"
    menu_item[2].sysex_name = "Pulse                 ";
    menu_item[2].sysex_address = SYSEX_W_WS_GAIN2_VIRTUAL_REG_A;
    menu_item[2].sysex_increment = 1;
    menu_item[2].sysex_min_value = 0;
    menu_item[2].sysex_max_value = 1023;
    menu_item[2].sysex_reset_value = 0;

    menu_item[3].page = 0;
    //nu_item[3].group_name = "12345678"
    menu_item[3].group_name = "WAVEFORM";
    //nu_item[3].sysex_name = "1234567890123456789012"
    menu_item[3].sysex_name = "Noise                 ";
    menu_item[3].sysex_address = SYSEX_W_WS_GAIN3_VIRTUAL_REG_A;
    menu_item[3].sysex_increment = 1;
    menu_item[3].sysex_min_value = 0;
    menu_item[3].sysex_max_value = 1023;
    menu_item[3].sysex_reset_value = 0;

    menu_item[4].page = 0;
    //nu_item[4].group_name = "12345678"
    menu_item[4].group_name = "VOLUME  ";
    //nu_item[4].sysex_name = "1234567890123456789012"
    menu_item[4].sysex_name = "Attack                ";
    menu_item[4].sysex_address = SYSEX_W_FI_GAIN3_VIRTUAL_REG_A;
    menu_item[4].sysex_increment = 1;
    menu_item[4].sysex_min_value = 0;
    menu_item[4].sysex_max_value = 1023;
    menu_item[4].sysex_reset_value = 0;

    menu_item[5].page = 0;
    //nu_item[5].group_name = "12345678"
    menu_item[5].group_name = "VOLUME  ";
    //nu_item[5].sysex_name = "1234567890123456789012"
    menu_item[5].sysex_name = "Decay                 ";
    menu_item[5].sysex_address = SYSEX_W_FI_GAIN3_VIRTUAL_REG_B;
    menu_item[5].sysex_increment = 1;
    menu_item[5].sysex_min_value = 0;
    menu_item[5].sysex_max_value = 1023;
    menu_item[5].sysex_reset_value = 1023;

    menu_item[6].page = 0;
    //nu_item[6].group_name = "12345678"
    menu_item[6].group_name = "VOLUME  ";
    //nu_item[6].sysex_name = "1234567890123456789012"
    menu_item[6].sysex_name = "Sustain               ";
    menu_item[6].sysex_address = SYSEX_W_FI_GAIN3_VIRTUAL_REG_C;
    menu_item[6].sysex_increment = 1;
    menu_item[6].sysex_min_value = 0;
    menu_item[6].sysex_max_value = 1023;
    menu_item[6].sysex_reset_value = 0;

    menu_item[7].page = 0;
    //nu_item[7].group_name = "12345678"
    menu_item[7].group_name = "VOLUME  ";
    //nu_item[7].sysex_name = "1234567890123456789012"
    menu_item[7].sysex_name = "Release               ";
    menu_item[7].sysex_address = SYSEX_W_FI_GAIN3_VIRTUAL_REG_D;
    menu_item[7].sysex_increment = 1;
    menu_item[7].sysex_min_value = 0;
    menu_item[7].sysex_max_value = 1023;
    menu_item[7].sysex_reset_value = 0;

    menu_item[8].page = 0;
    //nu_item[8].group_name = "12345678"
    menu_item[8].group_name = "FILTER  ";
    //nu_item[8].sysex_name = "1234567890123456789012"
    menu_item[8].sysex_name = "Cutoff                ";
    menu_item[8].sysex_address = SYSEX_W_DCO_FILTER_CUTOFF;
    menu_item[8].sysex_increment = 1;
    menu_item[8].sysex_min_value = 0;
    menu_item[8].sysex_max_value = 1023;
    menu_item[8].sysex_reset_value = 1023;

    menu_item[9].page = 0;
    //nu_item[9].group_name = "12345678"
    menu_item[9].group_name = "FILTER  ";
    //nu_item[9].sysex_name = "1234567890123456789012"
    menu_item[9].sysex_name = "Resonance             ";
    menu_item[9].sysex_address = SYSEX_W_DCO_FILTER_RESONANCE;
    menu_item[9].sysex_increment = 1;
    menu_item[9].sysex_min_value = 0;
    menu_item[9].sysex_max_value = 1023;
    menu_item[9].sysex_reset_value = 0;

    menu_item[10].page = 0;
    //nu_item[10].group_name = "12345678"
    menu_item[10].group_name = "FILTER  ";
    //nu_item[10].sysex_name = "1234567890123456789012"
    menu_item[10].sysex_name = "Key Follow Depth      ";
    menu_item[10].sysex_address = SYSEX_W_DCO_FILTER_KEY_FOLLOW_DEPTH;
    menu_item[10].sysex_increment = 1;
    menu_item[10].sysex_min_value = 0;
    menu_item[10].sysex_max_value = 1023;
    menu_item[10].sysex_reset_value = 512;

    menu_item[11].page = 0;
    //nu_item[11].group_name = "12345678"
    menu_item[11].group_name = "FILTER  ";
    //nu_item[11].sysex_name = "1234567890123456789012"
    menu_item[11].sysex_name = "Key Velocity Depth    ";
    menu_item[11].sysex_address = SYSEX_W_DCO_FILTER_KEY_VELOCITY_DEPTH;
    menu_item[11].sysex_increment = 1;
    menu_item[11].sysex_min_value = 0;
    menu_item[11].sysex_max_value = 1023;
    menu_item[11].sysex_reset_value = 512;

    menu_item[12].page = 0;
    //nu_item[12].group_name = "12345678"
    menu_item[12].group_name = "FILTER  ";
    //nu_item[12].sysex_name = "1234567890123456789012"
    menu_item[12].sysex_name = "Filter Stage 3 Level  ";
    menu_item[12].sysex_address = SYSEX_W_FI_GAIN0_VIRTUAL_REG_A;
    menu_item[12].sysex_increment = 1;
    menu_item[12].sysex_min_value = 0;
    menu_item[12].sysex_max_value = 1023;
    menu_item[12].sysex_reset_value = 512;

    menu_item[13].page = 0;
    //nu_item[13].group_name = "12345678"
    menu_item[13].group_name = "FILTER  ";
    //nu_item[13].sysex_name = "1234567890123456789012"
    menu_item[13].sysex_name = "Filter Stage 5 Level  ";
    menu_item[13].sysex_address = SYSEX_W_FI_GAIN1_VIRTUAL_REG_A;
    menu_item[13].sysex_increment = 1;
    menu_item[13].sysex_min_value = 0;
    menu_item[13].sysex_max_value = 1023;
    menu_item[13].sysex_reset_value = 512;

    menu_item[14].page = 0;
    //nu_item[14].group_name = "12345678"
    menu_item[14].group_name = "FILTER  ";
    //nu_item[14].sysex_name = "1234567890123456789012"
    menu_item[14].sysex_name = "Filter Stage 2 Level  ";
    menu_item[14].sysex_address = SYSEX_W_FI_GAIN2_VIRTUAL_REG_A;
    menu_item[14].sysex_increment = 1;
    menu_item[14].sysex_min_value = 0;
    menu_item[14].sysex_max_value = 1023;
    menu_item[14].sysex_reset_value = 0;

    menu_item[15].page = 0;
    //nu_item[15].group_name = "12345678"
    menu_item[15].group_name = "FILTER  ";
    //nu_item[15].sysex_name = "1234567890123456789012"
    menu_item[15].sysex_name = "Filter ADSR Env Depth ";
    menu_item[15].sysex_address = SYSEX_W_DCO_ADSR0_DEPTH;
    menu_item[15].sysex_increment = 1;
    menu_item[15].sysex_min_value = 0;
    menu_item[15].sysex_max_value = 1023;
    menu_item[15].sysex_reset_value = 512;

    menu_item[16].page = 0;
    //nu_item[16].group_name = "12345678"
    menu_item[16].group_name = "FILTER  ";
    //nu_item[16].sysex_name = "1234567890123456789012"
    menu_item[16].sysex_name = "Filter Attack         ";
    menu_item[16].sysex_address = SYSEX_W_DCO_ADSR0_ATTACK;
    menu_item[16].sysex_increment = 1;
    menu_item[16].sysex_min_value = 0;
    menu_item[16].sysex_max_value = 1023;
    menu_item[16].sysex_reset_value = 0;

    menu_item[17].page = 0;
    //nu_item[17].group_name = "12345678"
    menu_item[17].group_name = "FILTER  ";
    //nu_item[17].sysex_name = "1234567890123456789012"
    menu_item[17].sysex_name = "Filter Decay          ";
    menu_item[17].sysex_address = SYSEX_W_DCO_ADSR0_DECAY;
    menu_item[17].sysex_increment = 1;
    menu_item[17].sysex_min_value = 0;
    menu_item[17].sysex_max_value = 1023;
    menu_item[17].sysex_reset_value = 1023;

    menu_item[18].page = 0;
    //nu_item[18].group_name = "12345678"
    menu_item[18].group_name = "FILTER  ";
    //nu_item[18].sysex_name = "1234567890123456789012"
    menu_item[18].sysex_name = "Filter Sustain        ";
    menu_item[18].sysex_address = SYSEX_W_DCO_ADSR0_SUSTAIN;
    menu_item[18].sysex_increment = 1;
    menu_item[18].sysex_min_value = 0;
    menu_item[18].sysex_max_value = 1023;
    menu_item[18].sysex_reset_value = 0;

    menu_item[19].page = 0;
    //nu_item[19].group_name = "12345678"
    menu_item[19].group_name = "FILTER  ";
    //nu_item[19].sysex_name = "1234567890123456789012"
    menu_item[19].sysex_name = "Filter Release        ";
    menu_item[19].sysex_address = SYSEX_W_DCO_ADSR0_RELEASE;
    menu_item[19].sysex_increment = 1;
    menu_item[19].sysex_min_value = 0;
    menu_item[19].sysex_max_value = 1023;
    menu_item[19].sysex_reset_value = 0;

    menu_item[20].page = 0;
    //nu_item[20].group_name = "12345678"
    menu_item[20].group_name = "FILTER  ";
    //nu_item[20].sysex_name = "1234567890123456789012"
    menu_item[20].sysex_name = "Filter LFO Speed      ";
    menu_item[20].sysex_address = SYSEX_W_DCO_LFO0_SPEED;
    menu_item[20].sysex_increment = 1;
    menu_item[20].sysex_min_value = 0;
    menu_item[20].sysex_max_value = 1023;
    menu_item[20].sysex_reset_value = 0;

    menu_item[21].page = 0;
    //nu_item[21].group_name = "12345678"
    menu_item[21].group_name = "FILTER  ";
    //nu_item[21].sysex_name = "1234567890123456789012"
    menu_item[21].sysex_name = "Filter LFO Depth      ";
    menu_item[21].sysex_address = SYSEX_W_DCO_LFO0_DEPTH;
    menu_item[21].sysex_increment = 1;
    menu_item[21].sysex_min_value = 0;
    menu_item[21].sysex_max_value = 1023;
    menu_item[21].sysex_reset_value = 0;

    menu_item[22].page = 0;
    //nu_item[22].group_name = "12345678"
    menu_item[22].group_name = "FILTER  ";
    //nu_item[22].sysex_name = "1234567890123456789012"
    menu_item[22].sysex_name = "                      ";
    menu_item[22].sysex_address = SYSEX_W_DCO_LFO0_DEPTH;
    menu_item[22].sysex_increment = 1;
    menu_item[22].sysex_min_value = 0;
    menu_item[22].sysex_max_value = 1023;
    menu_item[22].sysex_reset_value = 0;

    menu_item[23].page = 0;
    //nu_item[23].group_name = "12345678"
    menu_item[23].group_name = "FILTER  ";
    //nu_item[23].sysex_name = "1234567890123456789012"
    menu_item[23].sysex_name = "                      ";
    menu_item[23].sysex_address = SYSEX_W_DCO_LFO0_DEPTH;
    menu_item[23].sysex_increment = 1;
    menu_item[23].sysex_min_value = 0;
    menu_item[23].sysex_max_value = 1023;
    menu_item[23].sysex_reset_value = 0;

    menu_item[24].page = 0;
    //nu_item[24].group_name = "12345678"
    menu_item[24].group_name = "TUNING  ";
    //nu_item[24].sysex_name = "1234567890123456789012"
    menu_item[24].sysex_name = "Octave of DCO 0       ";
    menu_item[24].sysex_address = SYSEX_W_LOCAL_DCO0_TUNE_OCTAVE;
    menu_item[24].sysex_increment = 1;
    menu_item[24].sysex_min_value = 510;
    menu_item[24].sysex_max_value = 514;
    menu_item[24].sysex_reset_value = 512;

    menu_item[25].page = 0;
    //nu_item[25].group_name = "12345678"
    menu_item[25].group_name = "TUNING  ";
    //nu_item[25].sysex_name = "1234567890123456789012"
    menu_item[25].sysex_name = "Octave of DCO 1       ";
    menu_item[25].sysex_address = SYSEX_W_LOCAL_DCO1_TUNE_OCTAVE;
    menu_item[25].sysex_increment = 1;
    menu_item[25].sysex_min_value = 510;
    menu_item[25].sysex_max_value = 514;
    menu_item[25].sysex_reset_value = 512;

    menu_item[26].page = 0;
    //nu_item[26].group_name = "12345678"
    menu_item[26].group_name = "TUNING  ";
    //nu_item[26].sysex_name = "1234567890123456789012"
    menu_item[26].sysex_name = "Octave of DCO 2       ";
    menu_item[26].sysex_address = SYSEX_W_LOCAL_DCO2_TUNE_OCTAVE;
    menu_item[26].sysex_increment = 1;
    menu_item[26].sysex_min_value = 510;
    menu_item[26].sysex_max_value = 514;
    menu_item[26].sysex_reset_value = 512;

    menu_item[27].page = 0;
    //nu_item[27].group_name = "12345678"
    menu_item[27].group_name = "TUNING  ";
    //nu_item[27].sysex_name = "1234567890123456789012"
    menu_item[27].sysex_name = "Octave of DCO 3       ";
    menu_item[27].sysex_address = SYSEX_W_LOCAL_DCO3_TUNE_OCTAVE;
    menu_item[27].sysex_increment = 1;
    menu_item[27].sysex_min_value = 510;
    menu_item[27].sysex_max_value = 514;
    menu_item[27].sysex_reset_value = 512;

    menu_item[28].page = 0;
    //nu_item[28].group_name = "12345678"
    menu_item[28].group_name = "TUNING  ";
    //nu_item[28].sysex_name = "1234567890123456789012"
    menu_item[28].sysex_name = "Fine Tune DCO 0       ";
    menu_item[28].sysex_address = SYSEX_W_LOCAL_DCO0_TUNE_CENTS;
    menu_item[28].sysex_increment = 1;
    menu_item[28].sysex_min_value = 0;
    menu_item[28].sysex_max_value = 1023;
    menu_item[28].sysex_reset_value = 512;

    menu_item[29].page = 0;
    //nu_item[29].group_name = "12345678"
    menu_item[29].group_name = "TUNING  ";
    //nu_item[29].sysex_name = "1234567890123456789012"
    menu_item[29].sysex_name = "Fine Tune DCO 1       ";
    menu_item[29].sysex_address = SYSEX_W_LOCAL_DCO1_TUNE_CENTS;
    menu_item[29].sysex_increment = 1;
    menu_item[29].sysex_min_value = 0;
    menu_item[29].sysex_max_value = 1023;
    menu_item[29].sysex_reset_value = 512;

    menu_item[30].page = 0;
    //nu_item[30].group_name = "12345678"
    menu_item[30].group_name = "TUNING  ";
    //nu_item[30].sysex_name = "1234567890123456789012"
    menu_item[30].sysex_name = "Fine Tune DCO 2       ";
    menu_item[30].sysex_address = SYSEX_W_LOCAL_DCO2_TUNE_CENTS;
    menu_item[30].sysex_increment = 1;
    menu_item[30].sysex_min_value = 0;
    menu_item[30].sysex_max_value = 1023;
    menu_item[30].sysex_reset_value = 512;

    menu_item[31].page = 0;
    //nu_item[31].group_name = "12345678"
    menu_item[31].group_name = "TUNING  ";
    //nu_item[31].sysex_name = "1234567890123456789012"
    menu_item[31].sysex_name = "Fine Tune DCO 3       ";
    menu_item[31].sysex_address = SYSEX_W_LOCAL_DCO3_TUNE_CENTS;
    menu_item[31].sysex_increment = 1;
    menu_item[31].sysex_min_value = 0;
    menu_item[31].sysex_max_value = 1023;
    menu_item[31].sysex_reset_value = 512;

    menu_item[32].page = 0;
    //nu_item[32].group_name = "12345678"
    menu_item[32].group_name = "VOICE   ";
    //nu_item[32].sysex_name = "1234567890123456789012"
    menu_item[32].sysex_name = "Generators per Voice  ";
    menu_item[32].sysex_address = SYSEX_W_LOCAL_COUNT_OF_DCOS_PER_VOICE;
    menu_item[32].sysex_increment = 1;
    menu_item[32].sysex_min_value = 1;
    menu_item[32].sysex_max_value = 4;
    menu_item[32].sysex_reset_value = 1;

    menu_item[33].page = 0;
    //nu_item[33].group_name = "12345678"
    menu_item[33].group_name = "VOICE   ";
    //nu_item[33].sysex_name = "1234567890123456789012"
    menu_item[33].sysex_name = "Chorus Spread [n/a]   ";
    menu_item[33].sysex_address = SYSEX_W_LOCAL_CHORUS_SPREAD;
    menu_item[33].sysex_increment = 1;
    menu_item[33].sysex_min_value = 0;
    menu_item[33].sysex_max_value = 1023;
    menu_item[33].sysex_reset_value = 512;

    menu_item[34].page = 0;
    //nu_item[34].group_name = "12345678"
    menu_item[34].group_name = "VOICE   ";
    //nu_item[34].sysex_name = "1234567890123456789012"
    menu_item[34].sysex_name = "Pitch Mod Amplitude   ";
    menu_item[34].sysex_address = SYSEX_W_LOCAL_LFO_AMPLITUDE;
    menu_item[34].sysex_increment = 1;
    menu_item[34].sysex_min_value = 0;
    menu_item[34].sysex_max_value = 1023;
    menu_item[34].sysex_reset_value = 0;

    menu_item[35].page = 0;
    //nu_item[35].group_name = "12345678"
    menu_item[35].group_name = "VOICE   ";
    //nu_item[35].sysex_name = "1234567890123456789012"
    menu_item[35].sysex_name = "Pitch Mod Speed       ";
    menu_item[35].sysex_address = SYSEX_W_LOCAL_LFO_SPEED;
    menu_item[35].sysex_increment = 1;
    menu_item[35].sysex_min_value = 0;
    menu_item[35].sysex_max_value = 1023;
    menu_item[35].sysex_reset_value = 0;

    /* END OF PAGES */

    /* PATCHES */
    static const uint16_t patch_0000[] = {16, 196,
                                          17, 600,
                                          18, 424,
                                          19, 568,
                                          68, 0,
                                          72, 776,
                                          76, 0,
                                          20, 860,
                                          80, 0,
                                          81, 1023,
                                          82, 0,
                                          83, 704,
                                          31, 53,
                                          38, 104,
                                          27, 6,
                                          39, 504,
                                          40, 500,
                                          41, 564,
                                          15, 2,
                                          0, 511,
                                          14, 8,
                                          13, 16,
                                          1, 512,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 470,
                                          6, 0,
                                          7, 0,
                                          100, 472,
                                          104, 452,
                                          108, 0,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[0]._patch_data = (const uint16_t *) patch_0000;

    static const uint16_t patch_0001[] = {16, 436,
                                          17, 1023,
                                          18, 0,
                                          19, 656,
                                          68, 540,
                                          72, 1023,
                                          76, 0,
                                          20, 272,
                                          80, 336,
                                          81, 1023,
                                          82, 0,
                                          83, 568,
                                          31, 16,
                                          38, 432,
                                          27, 6,
                                          39, 0,
                                          40, 708,
                                          41, 512,
                                          15, 1,
                                          0, 511,
                                          14, 8,
                                          13, 16,
                                          1, 512,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 470,
                                          6, 0,
                                          7, 0,
                                          100, 704,
                                          104, 716,
                                          108, 0,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[1]._patch_data = (const uint16_t *) patch_0001;

    static const uint16_t patch_0002[] = {16, 472,
                                          17, 696,
                                          18, 0,
                                          19, 0,
                                          68, 868,
                                          72, 352,
                                          76, 0,
                                          20, 436,
                                          80, 0,
                                          81, 744,
                                          82, 896,
                                          83, 484,
                                          31, 0,
                                          38, 484,
                                          27, 0,
                                          39, 176,
                                          40, 584,
                                          41, 656,
                                          15, 4,
                                          0, 512,
                                          14, 0,
                                          13, 16,
                                          1, 511,
                                          2, 512,
                                          3, 512,
                                          4, 504,
                                          5, 546,
                                          6, 492,
                                          7, 529,
                                          100, 500,
                                          104, 300,
                                          108, 656,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[2]._patch_data = (const uint16_t *) patch_0002;

    static const uint16_t patch_0003[] = {16, 472,
                                          17, 732,
                                          18, 0,
                                          19, 492,
                                          68, 1023,
                                          72, 0,
                                          76, 0,
                                          20, 116,
                                          80, 116,
                                          81, 744,
                                          82, 896,
                                          83, 416,
                                          31, 0,
                                          38, 732,
                                          27, 0,
                                          39, 24,
                                          40, 584,
                                          41, 656,
                                          15, 4,
                                          0, 514,
                                          14, 484,
                                          13, 8,
                                          1, 513,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 485,
                                          6, 504,
                                          7, 496,
                                          100, 600,
                                          104, 376,
                                          108, 240,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[3]._patch_data = (const uint16_t *) patch_0003;

    static const uint16_t patch_0004[] = {16, 472,
                                          17, 732,
                                          18, 0,
                                          19, 492,
                                          68, 1023,
                                          72, 0,
                                          76, 0,
                                          20, 116,
                                          80, 116,
                                          81, 684,
                                          82, 220,
                                          83, 204,
                                          31, 0,
                                          38, 440,
                                          27, 0,
                                          39, 24,
                                          40, 584,
                                          41, 656,
                                          15, 1,
                                          0, 514,
                                          14, 484,
                                          13, 8,
                                          1, 513,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 485,
                                          6, 504,
                                          7, 496,
                                          100, 600,
                                          104, 416,
                                          108, 396,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[4]._patch_data = (const uint16_t *) patch_0004;

    static const uint16_t patch_0005[] = {16, 472,
                                          17, 696,
                                          18, 0,
                                          19, 348,
                                          68, 1023,
                                          72, 0,
                                          76, 0,
                                          20, 116,
                                          80, 116,
                                          81, 744,
                                          82, 896,
                                          83, 416,
                                          31, 0,
                                          38, 464,
                                          27, 0,
                                          39, 0,
                                          40, 584,
                                          41, 512,
                                          15, 4,
                                          0, 514,
                                          14, 484,
                                          13, 8,
                                          1, 513,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 485,
                                          6, 504,
                                          7, 496,
                                          100, 1023,
                                          104, 376,
                                          108, 240,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[5]._patch_data = (const uint16_t *) patch_0005;

    static const uint16_t patch_0006[] = {16, 516,
                                          17, 600,
                                          18, 0,
                                          19, 0,
                                          68, 512,
                                          72, 512,
                                          76, 0,
                                          20, 244,
                                          80, 116,
                                          81, 744,
                                          82, 896,
                                          83, 416,
                                          31, 0,
                                          38, 472,
                                          27, 0,
                                          39, 292,
                                          40, 1023,
                                          41, 512,
                                          15, 1,
                                          0, 514,
                                          14, 0,
                                          13, 0,
                                          1, 513,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 485,
                                          6, 504,
                                          7, 496,
                                          100, 1023,
                                          104, 376,
                                          108, 240,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[6]._patch_data = (const uint16_t *) patch_0006;

    static const uint16_t patch_0007[] = {16, 464,
                                          17, 664,
                                          18, 48,
                                          19, 664,
                                          68, 0,
                                          72, 1023,
                                          76, 48,
                                          20, 792,
                                          80, 172,
                                          81, 664,
                                          82, 0,
                                          83, 664,
                                          31, 0,
                                          38, 720,
                                          27, 0,
                                          39, 284,
                                          40, 484,
                                          41, 512,
                                          15, 2,
                                          0, 512,
                                          14, 0,
                                          13, 0,
                                          1, 513,
                                          2, 512,
                                          3, 510,
                                          4, 512,
                                          5, 516,
                                          6, 504,
                                          7, 512,
                                          100, 0,
                                          104, 124,
                                          108, 820,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[7]._patch_data = (const uint16_t *) patch_0007;

    static const uint16_t patch_0008[] = {16, 388,
                                          17, 780,
                                          18, 0,
                                          19, 580,
                                          68, 724,
                                          72, 579,
                                          76, 0,
                                          20, 896,
                                          80, 636,
                                          81, 1023,
                                          82, 0,
                                          83, 648,
                                          31, 54,
                                          38, 256,
                                          27, 8,
                                          39, 264,
                                          40, 484,
                                          41, 512,
                                          15, 4,
                                          0, 512,
                                          14, 700,
                                          13, 148,
                                          1, 512,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 512,
                                          6, 512,
                                          7, 512,
                                          100, 1023,
                                          104, 328,
                                          108, 0,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[8]._patch_data = (const uint16_t *) patch_0008;

    static const uint16_t patch_0009[] = {16, 0,
                                          17, 0,
                                          18, 1023,
                                          19, 0,
                                          68, 1023,
                                          72, 572,
                                          76, 0,
                                          20, 512,
                                          80, 0,
                                          81, 1023,
                                          82, 0,
                                          83, 648,
                                          31, 0,
                                          38, 848,
                                          27, 0,
                                          39, 248,
                                          40, 512,
                                          41, 512,
                                          15, 1,
                                          0, 512,
                                          14, 0,
                                          13, 0,
                                          1, 512,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 512,
                                          6, 512,
                                          7, 512,
                                          100, 1023,
                                          104, 0,
                                          108, 0,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[9]._patch_data = (const uint16_t *) patch_0009;

    static const uint16_t patch_0010[] = {16, 0,
                                          17, 0,
                                          18, 1023,
                                          19, 0,
                                          68, 512,
                                          72, 512,
                                          76, 0,
                                          20, 512,
                                          80, 0,
                                          81, 1023,
                                          82, 0,
                                          83, 0,
                                          31, 0,
                                          38, 1023,
                                          27, 0,
                                          39, 0,
                                          40, 512,
                                          41, 512,
                                          15, 1,
                                          0, 512,
                                          14, 0,
                                          13, 0,
                                          1, 512,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 512,
                                          6, 512,
                                          7, 512,
                                          100, 1023,
                                          104, 0,
                                          108, 0,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[10]._patch_data = (const uint16_t *) patch_0010;

    static const uint16_t patch_0011[] = {16, 0,
                                          17, 1023,
                                          18, 0,
                                          19, 0,
                                          68, 542,
                                          72, 0,
                                          76, 0,
                                          20, 512,
                                          80, 484,
                                          81, 1023,
                                          82, 0,
                                          83, 792,
                                          31, 0,
                                          38, 668,
                                          27, 0,
                                          39, 0,
                                          40, 520,
                                          41, 512,
                                          15, 2,
                                          0, 512,
                                          14, 216,
                                          13, 20,
                                          1, 511,
                                          2, 510,
                                          3, 512,
                                          4, 512,
                                          5, 512,
                                          6, 512,
                                          7, 512,
                                          100, 1023,
                                          104, 240,
                                          108, 0,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[11]._patch_data = (const uint16_t *) patch_0011;

    static const uint16_t patch_0012[] = {16, 0,
                                          17, 1023,
                                          18, 0,
                                          19, 0,
                                          68, 452,
                                          72, 1023,
                                          76, 0,
                                          20, 512,
                                          80, 0,
                                          81, 1023,
                                          82, 0,
                                          83, 0,
                                          31, 0,
                                          38, 1023,
                                          27, 0,
                                          39, 0,
                                          40, 512,
                                          41, 512,
                                          15, 1,
                                          0, 513,
                                          14, 0,
                                          13, 0,
                                          1, 513,
                                          2, 512,
                                          3, 512,
                                          4, 512,
                                          5, 483,
                                          6, 447,
                                          7, 512,
                                          100, 0,
                                          104, 1023,
                                          108, 0,
                                          112, 0,
                                          0xffff, 0xffff
                                         };
    _patch[12]._patch_data = (const uint16_t *) patch_0012;

    static const uint16_t patch_0013[] = { 16, 0,
                                           17, 1023,
                                           18, 0,
                                           19, 0,
                                           68, 452,
                                           72, 1023,
                                           76, 0,
                                           20, 512,
                                           80, 656,
                                           81, 1023,
                                           82, 0,
                                           83, 744,
                                           31, 0,
                                           38, 740,
                                           27, 0,
                                           39, 120,
                                           40, 512,
                                           41, 512,
                                           15, 1,
                                           0, 512,
                                           14, 0,
                                           13, 0,
                                           1, 512,
                                           2, 512,
                                           3, 512,
                                           4, 512,
                                           5, 483,
                                           6, 447,
                                           7, 512,
                                           100, 0,
                                           104, 512,
                                           108, 512,
                                           112, 0,
                                           0xffff, 0xffff
                                         };
    _patch[13]._patch_data = (const uint16_t *) patch_0013;

    static const uint16_t patch_0014[] = { 16, 280,
                                           17, 1023,
                                           18, 0,
                                           19, 404,
                                           68, 1023,
                                           72, 0,
                                           76, 0,
                                           20, 552,
                                           80, 124,
                                           81, 1023,
                                           82, 0,
                                           83, 136,
                                           31, 0,
                                           38, 740,
                                           27, 0,
                                           39, 120,
                                           40, 512,
                                           41, 512,
                                           15, 1,
                                           0, 512,
                                           14, 0,
                                           13, 0,
                                           1, 512,
                                           2, 512,
                                           3, 512,
                                           4, 512,
                                           5, 483,
                                           6, 447,
                                           7, 512,
                                           100, 0,
                                           104, 512,
                                           108, 512,
                                           112, 0,
                                           0xffff, 0xffff
                                         };
    _patch[14]._patch_data = (const uint16_t *) patch_0014;

    static const uint16_t patch_0015[] = { 16, 376,
                                           17, 716,
                                           18, 0,
                                           19, 0,
                                           68, 452,
                                           72, 1023,
                                           76, 0,
                                           20, 532,
                                           80, 0,
                                           81, 600,
                                           82, 0,
                                           83, 600,
                                           31, 98,
                                           38, 304,
                                           27, 0,
                                           39, 0,
                                           40, 548,
                                           41, 532,
                                           15, 2,
                                           0, 513,
                                           14, 0,
                                           13, 4,
                                           1, 513,
                                           2, 512,
                                           3, 512,
                                           4, 512,
                                           5, 483,
                                           6, 447,
                                           7, 512,
                                           100, 0,
                                           104, 772,
                                           108, 0,
                                           112, 0,
                                           0xffff, 0xffff
                                         };
    _patch[15]._patch_data = (const uint16_t *) patch_0015;
    /* END OF PATCHES */

    _sdcard = new InterfaceSDCard(midiParser);                                  // instantiate sdcard
}

// The printf(“:%15s:\n”, “Hello, world!”); statement prints the string, but print 15 characters. If the string is smaller the “empty” positions will be filled with “whitespace.”
// https://www.codingunit.com/printf-format-specifiers-format-conversions-and-formatted-output

bool InterfaceDisplay::getEncoder0(int16_t enc_min_value, int16_t enc_max_value)
{
    _enc0_data_actual = encoder0.getPulses();
    if((_enc0_data_actual != _enc0_data_historical) && ((_enc0_data_actual & 0x03) == 0x00)) {
        int16_t _enc0_change = _enc0_data_actual - _enc0_data_historical;
        _enc0_data_historical = _enc0_data_actual;
        _pos0[_content_selection] += _enc0_change >> 2;
        
        if(_pos0[_content_selection] > enc_max_value) {
            _pos0[_content_selection] = enc_max_value;
        } else if(_pos0[_content_selection] < enc_min_value) {
            _pos0[_content_selection] = enc_min_value;
        }
        //printf("[d] encoder0 historial %d, actual, %d, pos %d\n\r", _enc0_data_historical, _enc0_data_actual, _pos0[_content_selection]);
        return(true);
    }
    return(false);
}

bool InterfaceDisplay::getEncoder1(void)
{
    //printf("[d] encoder1 historial %d, actual, %d, pos %d\n\r", _enc1_data_historical, _enc1_data_actual, _pos1[_content_selection]);
    _enc1_data_actual = encoder1.getPulses();
    if((_enc1_data_actual != _enc1_data_historical) && ((_enc1_data_actual & 0x03) == 0x00)) {
        int16_t _enc1_change = _enc1_data_actual - _enc1_data_historical;
        _enc1_data_historical = _enc1_data_actual;
        _pos1[_content_selection] += _enc1_change >> 2;
        printf("[d] pos1[%d] = %d, getPulses = %d\n\r", _content_selection, _pos1[_content_selection], _enc1_data_actual);
        return(true);
    }
    return(false);
}

void InterfaceDisplay::assignCC(uint8_t sysex_register_to_assign) {
    if(!assign){
        if(_midiParser->getRecentCC() < 127) {
            printf("[d] CC %d assigned to sysex %d\n\r", _midiParser->getRecentCC(), sysex_register_to_assign);
            _midiParser->cc2sysex[_midiParser->getRecentCC()] = sysex_register_to_assign;
        } else {
            printf("[d] no recent CC\n\r");
        }
    }
}

void InterfaceDisplay::content(void)
{
    if(!content_0) {
        _content_selection = 0;
        _force_refresh =  true;
    } else if(!content_1) {
        _content_selection = 1;
        _force_refresh = true;
    } else if(!content_2) {
        _content_selection = 2;
        _force_refresh = true;
    }
}

#define PATCH_ROM_LAST 15
#define PATCH_RAM_LAST 15

void InterfaceDisplay::loadROMpatch(void)
{
    if(getEncoder1()) {                                                         // encoder changed
        if((_patch_rom_load_nr + _pos1[_content_selection]) > PATCH_ROM_LAST) {
            _patch_rom_load_nr = PATCH_ROM_LAST;
            _pos1[_content_selection] = 0;
            //printf("[d] patch rom nr > %d [_patch_rom_load_nr]\n\r", PATCH_ROM_LAST);
        } else if((_patch_rom_load_nr + _pos1[_content_selection]) < 0) {
            _patch_rom_load_nr = 0;
            _pos1[_content_selection] = 0;
            //printf("[d] patch rom nr < 0 [_patch_rom_load_nr]\n\r");
        }
        _force_refresh = true;
        _patch_rom_load = true;
        _patch_rom_load_nr += _pos1[_content_selection];
        _pos1[_content_selection] = 0;
        printf("[d] patch rom nr %d\n\r", _patch_rom_load_nr);
    }
    if(_force_refresh) {                                                         // process change
        lcd.locate(0,0);
        //          1234567890123456789012345678901234567890
        lcd.printf("PATCH   |>Load Factory Patch    <| %04d ", _patch_rom_load_nr);
        lcd.printf("        | Load Custom Patch      | %04d ", _patch_ram_load_nr);
        lcd.printf("        | Save Custom Patch      | %04d ", _patch_ram_save_nr);
        lcd.printf("*dial encoder(2) to load a factory patch");
    }
    if(_patch_rom_load) {                                                       // load patch in _patch_rom_load_nr
        printf("[d] about to load a rom patch nr %d\n\r", _patch_rom_load_nr);
        _patch_rom_load = false;
        if(_patch_rom_load_nr_historical != _patch_rom_load_nr) {
            printf("[d] loading rom patch nr %d\n\r", _patch_rom_load_nr);
            _patch_rom_load_nr_historical = _patch_rom_load_nr;
            uint16_t patch_data_ptr = 0; 
            uint16_t sysex_register = 0xffff;
            uint16_t sysex_value = 0xffff;
            while(_patch[0]._patch_data[patch_data_ptr] != 0xffff) {
                sysex_register = _patch[_patch_rom_load_nr]._patch_data[patch_data_ptr];
                patch_data_ptr++;
                sysex_value = _patch[_patch_rom_load_nr]._patch_data[patch_data_ptr];
                patch_data_ptr++;
                //printf("[d] patch data: %d, %d,\n\r", sysex_register, sysex_value);
                _midiParser->set_sysex_register(sysex_register, sysex_value);
            }
        }
    }
}

void InterfaceDisplay::loadRAMpatch(void)
{
    if(getEncoder1()) {                                                         // encoder changed
        if((_patch_ram_load_nr + _pos1[_content_selection]) > PATCH_RAM_LAST) {
            _patch_ram_load_nr = PATCH_RAM_LAST;
            _pos1[_content_selection] = 0;
            //printf("[d] patch ram nr > %d [_patch_ram_load_nr]\n\r", PATCH_RAM_LAST);
        } else if((_patch_ram_load_nr + _pos1[_content_selection]) < 0) {
            _patch_ram_load_nr = 0;
            _pos1[_content_selection] = 0;
            //printf("[d] patch ram nr < 0 [_patch_ram_load_nr]\n\r");
        }
        _force_refresh = true;
        _patch_ram_load = true;
        _patch_ram_load_nr += _pos1[_content_selection];
        _pos1[_content_selection] = 0;
        //printf("[d] patch ram nr %d\n\r", _patch_ram_load_nr);
    }
    if(_force_refresh) {                                                         // process change
        lcd.locate(0,0);
        //          1234567890123456789012345678901234567890
        lcd.printf("PATCH   | Load Factory Patch     | %04d ", _patch_rom_load_nr);
        lcd.printf("        |>Load Custom Patch     <| %04d ", _patch_ram_load_nr);
        lcd.printf("        | Save Custom Patch      | %04d ", _patch_ram_save_nr);
        lcd.printf("*dial encoder(2) to load a custom patch ");
    }
    if(_patch_ram_load) {                                                       // load patch in _patch_ram_load_nr
        printf("[d] about to load a ram patch nr %d\n\r", _patch_ram_load_nr);
        _patch_ram_load = false;
        if(_patch_ram_load_nr_historical != _patch_ram_load_nr) {
            printf("[d] loading ram patch nr %d\n\r", _patch_ram_load_nr);
            _patch_ram_load_nr_historical = _patch_ram_load_nr;
            _sdcard->readPatch(_patch_ram_load_nr);                             // test read patch
        }
    }
}

void InterfaceDisplay::saveRAMpatch(void)
{
    if(getEncoder1()) {                                                         // encoder changed
        if((_patch_ram_save_nr + _pos1[_content_selection]) > PATCH_RAM_LAST) {
            _patch_ram_save_nr = PATCH_RAM_LAST;
            _pos1[_content_selection] = 0;
            //printf("[d] patch ram nr > %d [_patch_ram_save_nr]\n\r", PATCH_RAM_LAST);
        } else if((_patch_ram_save_nr + _pos1[_content_selection]) < 0) {
            _patch_ram_save_nr = 0;
            _pos1[_content_selection] = 0;
            //printf("[d] patch ram nr < 0 [_patch_ram_save_nr]\n\r");
        }
        _force_refresh = true;
        _patch_ram_save_nr += _pos1[_content_selection];
        _pos1[_content_selection] = 0;
        //printf("[d] patch ram nr %d\n\r", _patch_ram_save_nr);
    }
    if(_force_refresh) {                                                         // process change
        lcd.locate(0,0);
        //          1234567890123456789012345678901234567890
        lcd.printf("PATCH   | Load Factory Patch     | %04d ", _patch_rom_load_nr);
        lcd.printf("        | Load Custom Patch      | %04d ", _patch_ram_load_nr);
        lcd.printf("        |>Save Custom Patch     <| %04d ", _patch_ram_save_nr);
        lcd.printf("press red button to save                ");
    }
    
    _assign_button_state_actual = assign;
    if(_assign_button_state_actual != _assign_button_state_historical) {        // button changed state
        _assign_button_state_historical = _assign_button_state_actual;          // react
        if(!_assign_button_state_actual) {                                      // button presset / pull-up
            printf("[d] about to save a ram patch nr %d\n\r", _patch_ram_save_nr);
            printf("[d] saving ram patch nr %d\n\r", _patch_ram_save_nr);
            _sdcard->writePatch(_patch_ram_save_nr, (const uint16_t *) patch_default);
        }
    }
}

void InterfaceDisplay::page(bool force_refresh)
{
    _force_refresh = force_refresh;
    content();
    switch(_content_selection) {
        case 0:
            if(getEncoder0(0,2)) {
                _force_refresh = true;
            }
            switch(_pos0[_content_selection]) {
                case 0:
                    loadROMpatch();
                    break;
                case 1:
                    loadRAMpatch();
                    break;
                case 2:
                    saveRAMpatch();
                    break;
            }
            break;

        case 1:                                                                 // global midi settings (receive, master tune)
            if(_force_refresh) {
                lcd.locate(0,0);
                //          1234567890123456789012345678901234567890
                lcd.printf("general midi settings                   ");
                lcd.printf("                                        ");
                lcd.printf("                                        ");
                lcd.printf("                                        ");
            }
            break;

        case 2:                                                                 // modify patch/sysex values
            if(getEncoder1()) {
                if(_midiParser->get_sysex_register(menu_item[_pos0[_content_selection]].sysex_address) + _pos1[_content_selection] > menu_item[_pos0[_content_selection]].sysex_max_value) {
                    _midiParser->set_sysex_register(menu_item[_pos0[_content_selection]].sysex_address, menu_item[_pos0[_content_selection]].sysex_max_value);
                    _pos1[_content_selection] = 0;
                }
                if(_midiParser->get_sysex_register(menu_item[_pos0[_content_selection]].sysex_address) + _pos1[_content_selection] < menu_item[_pos0[_content_selection]].sysex_min_value) {
                    _midiParser->set_sysex_register(menu_item[_pos0[_content_selection]].sysex_address, menu_item[_pos0[_content_selection]].sysex_min_value);
                    _pos1[_content_selection] = 0;
                }
                _midiParser->set_sysex_register(menu_item[_pos0[_content_selection]].sysex_address,
                                                _midiParser->get_sysex_register(menu_item[_pos0[_content_selection]].sysex_address) + _pos1[_content_selection]);
                _pos1[_content_selection] = 0;
                _force_refresh = true;
            }

            if(getEncoder0(0,35) || _force_refresh) {
                char highlight_l[4] = {' ', ' ', ' ', ' '};
                char highlight_r[4] = {' ', ' ', ' ', ' '};
                highlight_l[_pos0[_content_selection] & 0x03] = '>';
                highlight_r[_pos0[_content_selection] & 0x03] = '<';

                lcd.locate(0,0);
                lcd.printf("%s|%c%s%c| %04d ", menu_item[_pos0[_content_selection]+0].group_name, highlight_l[0],
                           menu_item[(_pos0[_content_selection] & 0xfffc)+0].sysex_name, highlight_r[0],
                           _midiParser->get_sysex_register(menu_item[(_pos0[_content_selection] & 0xfffc)+0].sysex_address));
                lcd.printf("        |%c%s%c| %04d ", highlight_l[1],
                           menu_item[(_pos0[_content_selection] & 0xfffc)+1].sysex_name, highlight_r[1],
                           _midiParser->get_sysex_register(menu_item[(_pos0[_content_selection] & 0xfffc)+1].sysex_address));
                lcd.printf("        |%c%s%c| %04d ", highlight_l[2],
                           menu_item[(_pos0[_content_selection] & 0xfffc)+2].sysex_name, highlight_r[2],
                           _midiParser->get_sysex_register(menu_item[(_pos0[_content_selection] & 0xfffc)+2].sysex_address));
                lcd.printf("        |%c%s%c| %04d ", highlight_l[3],
                           menu_item[(_pos0[_content_selection] & 0xfffc)+3].sysex_name, highlight_r[3],
                           _midiParser->get_sysex_register(menu_item[(_pos0[_content_selection] & 0xfffc)+3].sysex_address));
            }
            assignCC(menu_item[_pos0[_content_selection]].sysex_address);
            break;
    }
}