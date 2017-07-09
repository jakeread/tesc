#ifndef CONFIG_H
#define CONFIG_H

#define TRUE                        1
#define FALSE                       0

// STARTUP

#define GO_ON_STARTUP               TRUE

// COM

#define THE_BAUDRATE                115200

// PINS

#define PIN_HI_A                    10
#define PIN_LO_A                    9
#define PIN_HI_B                    6
#define PIN_LO_B                    5
#define PIN_HI_C                    4
#define PIN_LO_C                    3

#define PIN_SENSV_A                 17
#define PIN_SENSV_B                 18
#define PIN_SENSV_C                 19

#define PIN_SENSA_A                 21
#define PIN_SENSA_B                 22
#define PIN_SENSA_C                 23

// ENCODER SETUPS

#define AS5047_RESOLUTION           16384 
#define AS5047_SAMPLES              256             // keep lots of these around so that we can do filtering, derivatives
#define AS5047_AVERAGING            8

#define RBUF_LENGTH 256

#define AS5047_OFFSET               1931           //1395
#define AS5047_REV                  FALSE          // set TRUE to reverse values, 0 to retain original // MT4108 = TRUE // MT5208 = FALSE

#define ENCODER_REVSEARCH_NUM       5
#define ENCODER_REVSEARCH_MULTIPLE  25 // _NUM * 6 ... TODO: evaluations in #defines?
#define ENCODER_REVSEARCH_LESS      24
#define ENCODER_SEARCH_DEFAULT_DUTY 190

// MOTOR SPECIFIC

#define MOTOR_POLEPAIRS             11 // MT5208 = 7 // MT4108 = 11
#define MOTOR_MODULO                2340 // AS5047_RESOLUTION / MOTOR_POLEPAIRS // // MT4108 = 1489 || MT5208 = 2340 = resolution / poles / 2

// DRIVER

#define LEG_PWMRES                  8
#define LEG_PWMFREQ                 187500
#define LEG_MINPWM                  64


// LOOP SPEEDS

#define MACHINE_LOOP_DEFAULT_HZ     15000
#define MACHINE_LOOP_SECONDARY      500      // so we get 1000hz & 25000hz w/ one timer
#define SAMPLE_DEFAULT_HZ           6000

// SVM

#define PHASE_ADVANCE               1.45 // RADIANS: optimum is 90*, or PI/2, or 1.57
#define POS_TO_THETA                0.0042197349

// BLDC

#define IS_BLDC_MACHINE             1
#define IS_FOC_MACHINE              0

#if IS_BLDC_MACHINE && IS_FOC_MACHINE
#warning WTF
#endif

// DEBUG POT

#define DEBUG_POT_PIN               A14

// SP

#define ARGWORDS                    4

#endif
