#ifndef CONFIG_H
#define CONFIG_H

#define TRUE                        1
#define FALSE                       0

// COM

#define THE_BAUDRATE                115200

// ENCODER SETUPS

#define AS5047_RESOLUTION           16384 
#define AS5047_SAMPLES              256             // keep lots of these around so that we can do filtering, derivatives
#define AS5047_AVERAGING            8

#define RBUF_LENGTH 256

#define AS5047_OFFSET_UP            672 //1395           // MOTOR_MODULO - MeasuredAt0 + (0.5*(MOTOR_MODULO/6))
#define AS5047_REV                  TRUE          // set TRUE to reverse values, 0 to retain original // MT4108 = TRUE // MT5208 = FALSE


// MOTOR SPECIFIC

#define MOTOR_POLEPAIRS             11 // MT5208 = 7 // MT4108 = 11
#define MOTOR_MODULO                1489 // AS5047_RESOLUTION / MOTOR_POLEPAIRS // // MT4108 = 1489 || MT5208 = 2340 = resolution / poles / 2

// DRIVER

#define LEG_PWMRES                  8
#define LEG_PWMFREQ                 187500
#define LEG_MINPWM                  64


// LOOP SPEEDS

#define MACHINE_LOOP_DEFAULT_HZ     15000
#define MACHINE_LOOP_SECONDARY      25      // so we get 1000hz & 25000hz w/ one timer
#define SAMPLE_DEFAULT_HZ           30000

// SVM

#define PHASE_ADVANCE               1.45 // RADIANS: optimum is 90*, or PI/2, or 1.57
#define POS_TO_THETA                0.0042197349

// BLDC

#define IS_BLDC_MACHINE             1

// DEBUG POT

#define DEBUG_POT_PIN               A14

// SP

#define ARGWORDS                    4

#endif
