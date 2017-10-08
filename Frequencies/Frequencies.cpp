#include <stdio.h>
#include <stdint.h>
#include "Frequencies.h"

Frequencies::Frequencies() {};

uint8_t Frequencies::get_r2r(int16_t val) {
    uint8_t r2r;
    if(val < 0) {
        //printf("[!] r2r out of 8bit bound, val is %d, val range is 0 - 16512\n\r", val);
        return(0);
    } else if(val > 16512 - 1) {
        //printf("[!] r2r out of 8bit bound, val is %d, val range is 0 - 16512\n\r", val);
        return(255);
    } else {
        r2r = val >> 6;
    };
    return(r2r);
};

uint16_t Frequencies::get_divider(int16_t val) {
    if(val < 0) {
        return(divider[0]);
    } else if(val > 16512 - 1) {
        return(divider[16512 - 1]);
    };
    return(divider[val]);
};

uint16_t Frequencies::get_counter(int16_t val) {
    if(val < 0) {
        return(counter[0]);
    } else if(val > 16512 - 1) {
        return(counter[16512 - 1]);
    } 
    return(counter[val]);
};
