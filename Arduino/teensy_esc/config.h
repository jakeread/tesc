#ifndef CONFIG_H
#define CONFIG_H

#define TRUE                        1
#define FALSE                       0

// STARTUP

#define GO_ON_STARTUP               TRUE

// COM

#define THE_BAUDRATE                115200

// PINS

// BLDC Specific (uses abstracted pwm) (SVPWM / FOC PWM modules use register-written pin configs in t3pwm.cpp)
#define PIN_HI_U                    22
#define PIN_LO_U                    23
#define PIN_HI_V                    21
#define PIN_LO_V                    5
#define PIN_HI_W                    6
#define PIN_LO_W                    20

// ADC for Voltages (adc for amperage measurement is linked to t3adc.cpp register-writing)
#define PIN_VSENS_U                 A2
#define PIN_VSENS_V                 A4
#define PIN_VSENS_W                 A10

// ENCODER SETUPS

#define AS5047_SAMPLE_RATE          20000

#define AS5047_RESOLUTION           16384 
#define AS5047_SAMPLES              256             // keep lots of these around so that we can do filtering, derivatives
#define AS5047_AVERAGING            4

#define RBUF_LENGTH                 256

#define AS5047_OFFSET_BLDC          364           // but ~ 1700 seems better -> this is roughly one 'com zone' away
#define AS5047_OFFSET_FOC           999
#define AS5047_REV_BLDC             TRUE          // set TRUE to reverse values, 0 to retain original // MT4108 = TRUE // MT5208 = FALSE
#define AS5047_REV_FOC              TRUE

#define ENCODER_REVSEARCH_NUM       5
#define ENCODER_REVSEARCH_MULTIPLE  25 // _NUM * 6 ... TODO: evaluations in #defines?
#define ENCODER_REVSEARCH_LESS      24
#define ENCODER_SEARCH_DEFAULT_DUTY 1024

// MOTOR SPECIFIC

#define MOTOR_POLEPAIRS             11    // MT5208 = 7 // MT4108 = 11
#define MOTOR_MODULO                1489  // AS5047_RESOLUTION / MOTOR_POLEPAIRS // // MT4108 = 1489 || MT5208 = 2340 = resolution / poles / 2

// DRIVER

#define LEG_PWMRES                  8
#define LEG_PWMFREQ                 187500
#define LEG_MINPWM                  64


// LOOP SPEEDS

#define MACHINE_LOOP_DEFAULT_HZ     8000
#define MACHINE_LOOP_SECONDARY      10000
#define SAMPLE_DEFAULT_HZ           10000

// SVM

#define PHASE_ADVANCE               1.45 // RADIANS: optimum is 90*, or PI/2, or 1.57
#define POS_TO_THETA                0.0042197349

// FOC

#define V_REF                       1.65f // half of VREF from Teensy, see DRV8302 datasheet, REF pin
#define V_BUS                       12.0f
#define CURRENT_SHUNT_RES           0.002
#define CURRENT_AMP_GAIN            40
#define CURRENT_AMP_OFFSET_V        2048 // observed amp output at 0 current, in raw adc num
#define CURRENT_AMP_OFFSET_W        2048

// BLDC

#define IS_BLDC_MACHINE             0
#define IS_FOC_MACHINE              1

#define BLDC_MAX_OL_FREQ            10000

#define BLDC_INPUTMODE_POT          1
#define BLDC_INPUTMODE_SHELL        0

#define BLDC_DEFAULT_DUTY           1500 // of 4096
#define BLDC_DEFAULT_DIR            1
#define BLDC_DEFAULT_HZ             124

#if IS_BLDC_MACHINE && IS_FOC_MACHINE
#warning WTF
#endif

// DEBUG POT

#define DEBUG_POT_PIN               A14

// SP

#define ARGWORDS                    4

#endif
