#ifndef ANALOGPOTREAD_H_
#define ANALOGPOTREAD_H_
#include "mbed.h"

class AnalogPotRead
{
    public:
        AnalogPotRead();
        AnalogPotRead(  PinName sw0, PinName sw1, PinName sw2, PinName sw3, 
                        PinName bs01, PinName bs23,
                        PinName adc0, PinName adc1, PinName adc2, PinName adc3);
        
        void readings(void);
        
    private:
        BusOut      analog_ctrl_;
        DigitalOut  analog_ctrl_bus01_; 
        DigitalOut  analog_ctrl_bus23_;                                          // 1 == no input, 0 == input active 
        AnalogIn    channel0_in_; 
        AnalogIn    channel1_in_; 
        AnalogIn    channel2_in_; 
        AnalogIn    channel3_in_;     
    
        struct AnalogPotRead_channel {
            uint16_t    value[16];
            uint16_t    value_prev[16];
            bool        processed[16];
        };

    struct AnalogPotRead_channel analog_[4];
};
#endif
