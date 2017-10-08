#include "mbed.h"
#include "TextLCD.h"
#include <stdint.h>
#include "analogpotread.h"

extern TextLCD lcd;

/* Constructors */
AnalogPotRead::AnalogPotRead() : analog_ctrl_(PA_1, PA_4, PH_0, PH_1), 
                                 analog_ctrl_bus01_(PA_6), 
                                 analog_ctrl_bus23_(PA_7), 
                                 channel0_in_(PA_0), 
                                 channel1_in_(PC_0), 
                                 channel2_in_(PB_0), 
                                 channel3_in_(PB_0) {}

AnalogPotRead::AnalogPotRead(PinName sw0,                                       
                             PinName sw1, 
                             PinName sw2, 
                             PinName sw3, 
                             PinName bs01, 
                             PinName bs23,
                             PinName adc0, 
                             PinName adc1, 
                             PinName adc2, 
                             PinName adc3) : \
                             analog_ctrl_(sw0, sw1, sw2, sw3),
                             analog_ctrl_bus01_(bs01), 
                             analog_ctrl_bus23_(bs23),  
                             channel0_in_(adc0), 
                             channel1_in_(adc1), 
                             channel2_in_(adc2), 
                             channel3_in_(adc3) {
    
    BusOut      analog_ctrl_(sw0, sw1, sw2, sw3);                               // select channel_register                                 
    DigitalOut  analog_ctrl_bus01_(bs01);                                       // 1 == no input, 0 == input active
    DigitalOut  analog_ctrl_bus23_(bs23);                                       // 1 == no input, 0 == input active
    AnalogIn    channel0_in_(adc0);                                             // dac channel 0
    AnalogIn    channel1_in_(adc1);                                             // dac channel 1
    AnalogIn    channel2_in_(adc2);                                             // dac channel 2
    AnalogIn    channel3_in_(adc3);                                             // dac channel 3

    for(uint8_t channel = 0; channel < 4; channel++) {
        for(uint8_t channel_register = 0; channel_register < 16; channel_register++) {
            analog_[channel].value[channel_register] = 0;
            analog_[channel].value_prev[channel_register] = 0;
            analog_[channel].processed[channel_register] = false;
        }
    }
}

/* Read Function */
void AnalogPotRead::readings() {
    uint16_t    reading;
    analog_ctrl_bus01_ = 0;
    for(uint8_t channel_register = 0; channel_register < 4; channel_register++) {
        analog_ctrl_ = channel_register;
        wait(0.025);
        reading = channel0_in_.read_u16();
        //reading = reading >> 8;
        analog_[0].value[channel_register] = reading;
    }
    lcd.locate(0, 0);
    lcd.printf("1 : %5d  \n", analog_[0].value[0]);
    lcd.printf("2 : %5d  \n", analog_[0].value[1]);
    lcd.printf("3 : %5d  \n", analog_[0].value[2]);
    lcd.printf("4 : %5d  \n", analog_[0].value[3]);
    //lcd.printf("%3d %3d %3d %3d\n", analog_[0].value[0], analog_[0].value[1], analog_[0].value[2], analog_[0].value[3]);
}
