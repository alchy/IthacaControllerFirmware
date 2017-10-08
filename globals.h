#define BIN(b7,b6,b5,b4,b3,b2,b1,b0) \
            ((b7 << 7)+(b6 << 6)+\
             (b5 << 5)+(b4 << 4)+\
             (b3 << 3)+(b2 << 2)+\
             (b1 << 1)+b0)
             

struct lfo_struct {
    bool        process_irq;
    uint16_t    speed_parameter;
    uint16_t    amplitude_parameter;
    float       step_persistent;
    int16_t     modulation;                             
    int16_t     tune[16];                                              // precompute for all 16 DCOs, only first N is used
};

// default configuration parameters
#define DCO_PER_VOICE_DEFAULT                   0x04

#define I2C_FREQUENCY                           400000

// i2c address space description
#define DCO_ADDRESS_SPACE_BEGIN                 0x08
#define DCO_ADDRESS_SPACE_END                   DCO_ADDRESS_SPACE_BEGIN + 0x0f

#define CHAIN_ADDRESS_SPACE_BEGIN               0x18
#define CHAIN_ADDRESS_SPACE_END                 CHAIN_ADDRESS_SPACE_BEGIN + 0x20

// device type request address
#define REG_B_DEVTYPE                           0xfe

// device type response
#define DEV_TYPE_DCO                            0x10
#define DEV_TYPE_FILTER_GAIN                    0x11
#define DEV_TYPE_SHAPER_GAIN                    0x12

// 111 various sysex messages from 0x10(16) - 0x7f(127) *7bit data register
// DCO UNIT parameters

// --------------------------------------------------
// LOCAL UNIT: public sysex register addresses (16 registers max)
#define SYSEX_RANGE_LOCAL_START            0
#define SYSEX_RANGE_LOCAL_END              15
// --------------------------------------------------
#define SYSEX_W_LOCAL_DCO0_TUNE_OCTAVE          0
#define SYSEX_W_LOCAL_DCO1_TUNE_OCTAVE          1
#define SYSEX_W_LOCAL_DCO2_TUNE_OCTAVE          2
#define SYSEX_W_LOCAL_DCO3_TUNE_OCTAVE          3

#define SYSEX_W_LOCAL_DCO0_TUNE_CENTS           4
#define SYSEX_W_LOCAL_DCO1_TUNE_CENTS           5
#define SYSEX_W_LOCAL_DCO2_TUNE_CENTS           6
#define SYSEX_W_LOCAL_DCO3_TUNE_CENTS           7


#define SYSEX_W_LOCAL_CHORUS_SPREAD             2

#define SYSEX_W_LOCAL_LFO_AMPLITUDE             13
#define SYSEX_W_LOCAL_LFO_SPEED                 14
#define SYSEX_W_LOCAL_COUNT_OF_DCOS_PER_VOICE   15

// --------------------------------------------------
// DCO UNITS: public sysex register addresses (46 registers max)
#define SYSEX_RANGE_DCO_START              16
#define SYSEX_RANGE_DCO_END                62
// --------------------------------------------------
#define SYSEX_W_DCO_ADSR0_ATTACK                16
#define SYSEX_W_DCO_ADSR0_DECAY                 17
#define SYSEX_W_DCO_ADSR0_SUSTAIN               18
#define SYSEX_W_DCO_ADSR0_RELEASE               19
#define SYSEX_W_DCO_ADSR0_DEPTH                 20

#define SYSEX_W_DCO_ADSR1_ATTACK                21
#define SYSEX_W_DCO_ADSR1_DECAY                 22
#define SYSEX_W_DCO_ADSR1_SUSTAIN               23
#define SYSEX_W_DCO_ADSR1_RELEASE               24
#define SYSEX_W_DCO_ADSR1_DEPTH                 25

#define SYSEX_W_DCO_LFO0_MODE                   26
#define SYSEX_W_DCO_LFO0_SPEED                  27
#define SYSEX_W_DCO_LFO0_SHAPE                  28  
#define SYSEX_W_DCO_LFO0_ATTACK                 29
#define SYSEX_W_DCO_LFO0_RELEASE                30
#define SYSEX_W_DCO_LFO0_DEPTH                  31

#define SYSEX_W_DCO_LFO1_MODE                   32
#define SYSEX_W_DCO_LFO1_SPEED                  33
#define SYSEX_W_DCO_LFO1_SHAPE                  34  
#define SYSEX_W_DCO_LFO1_ATTACK                 35
#define SYSEX_W_DCO_LFO1_RELEASE                36
#define SYSEX_W_DCO_LFO1_DEPTH                  37

#define SYSEX_W_DCO_FILTER_CUTOFF               38
#define SYSEX_W_DCO_FILTER_RESONANCE            39

#define SYSEX_W_DCO_FILTER_KEY_FOLLOW_DEPTH     40
#define SYSEX_W_DCO_FILTER_KEY_VELOCITY_DEPTH   41
#define SYSEX_W_DCO_FILTER_KEY_AFTERTOUCH_DEPTH 42

// --------------------------------------------------
// CHAIN DEVICES: public register sysex addresses (64 registers max)
#define SYSEX_RANGE_CHAIN_START            63
#define SYSEX_RANGE_CHAIN_END              127

#define MODE_HOLD                               0
#define MODE_SINE                               1
#define MODE_ADSR                               2
#define MODE_RNDSAH                             3
// --------------------------------------------------

#define SYSEX_W_FI_GAIN0_MODE               64
#define SYSEX_W_FI_GAIN1_MODE               65
#define SYSEX_W_FI_GAIN2_MODE               66
#define SYSEX_W_FI_GAIN3_MODE               67

#define SYSEX_W_FI_GAIN0_VIRTUAL_REG_A      68
#define SYSEX_W_FI_GAIN0_VIRTUAL_REG_B      69
#define SYSEX_W_FI_GAIN0_VIRTUAL_REG_C      70
#define SYSEX_W_FI_GAIN0_VIRTUAL_REG_D      71

#define SYSEX_W_FI_GAIN1_VIRTUAL_REG_A      72
#define SYSEX_W_FI_GAIN1_VIRTUAL_REG_B      73
#define SYSEX_W_FI_GAIN1_VIRTUAL_REG_C      74
#define SYSEX_W_FI_GAIN1_VIRTUAL_REG_D      75

#define SYSEX_W_FI_GAIN2_VIRTUAL_REG_A      76
#define SYSEX_W_FI_GAIN2_VIRTUAL_REG_B      77
#define SYSEX_W_FI_GAIN2_VIRTUAL_REG_C      78
#define SYSEX_W_FI_GAIN2_VIRTUAL_REG_D      79

#define SYSEX_W_FI_GAIN3_VIRTUAL_REG_A      80
#define SYSEX_W_FI_GAIN3_VIRTUAL_REG_B      81
#define SYSEX_W_FI_GAIN3_VIRTUAL_REG_C      82
#define SYSEX_W_FI_GAIN3_VIRTUAL_REG_D      83

#define SYSEX_W_WS_GAIN0_MODE               96
#define SYSEX_W_WS_GAIN1_MODE               97
#define SYSEX_W_WS_GAIN2_MODE               98
#define SYSEX_W_WS_GAIN3_MODE               99

#define SYSEX_W_WS_GAIN0_VIRTUAL_REG_A      100
#define SYSEX_W_WS_GAIN0_VIRTUAL_REG_B      101
#define SYSEX_W_WS_GAIN0_VIRTUAL_REG_C      102
#define SYSEX_W_WS_GAIN0_VIRTUAL_REG_D      103

#define SYSEX_W_WS_GAIN1_VIRTUAL_REG_A      104
#define SYSEX_W_WS_GAIN1_VIRTUAL_REG_B      105
#define SYSEX_W_WS_GAIN1_VIRTUAL_REG_C      106
#define SYSEX_W_WS_GAIN1_VIRTUAL_REG_D      107

#define SYSEX_W_WS_GAIN2_VIRTUAL_REG_A      108
#define SYSEX_W_WS_GAIN2_VIRTUAL_REG_B      109
#define SYSEX_W_WS_GAIN2_VIRTUAL_REG_C      110
#define SYSEX_W_WS_GAIN2_VIRTUAL_REG_D      111

#define SYSEX_W_WS_GAIN3_VIRTUAL_REG_A      112
#define SYSEX_W_WS_GAIN3_VIRTUAL_REG_B      113
#define SYSEX_W_WS_GAIN3_VIRTUAL_REG_C      114
#define SYSEX_W_WS_GAIN3_VIRTUAL_REG_D      115
