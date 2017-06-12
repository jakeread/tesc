#ifndef CONFIG_H
#define CONFIG_H

#define TRUE                        1
#define FALSE                       0

// ENCODER SETUPS

#define AS5047_RESOLUTION           16384 
#define AS5047_SAMPLES              256    // keep lots of these around so that we can do filtering, derivatives
#define AS5047_AVERAGING            8

#define RBUF_LENGTH 256

#define AS5047_OFFSET_UP            1590 //1395           // MOTOR_MODULO - MeasuredAt0 + (0.5*(MOTOR_MODULO/6))
#define AS5047_REV                  FALSE          // set TRUE to reverse values, 0 to retain original // MT4108 = FALSE


// MOTOR SPECIFIC

#define MOTOR_POLEPAIRS             7 // MT5208 = 7 // MT4108 = 11
#define MOTOR_MODULO                2340 // AS5047_RESOLUTION / MOTOR_POLEPAIRS // // MT4108 = 1489 || MT5208 = 2340 = resolution / poles / 2

// DRIVER

#define LEG_PWMRES                  8
#define LEG_PWMFREQ                 187500
#define LEG_MINPWM                  64


// LOOP SPEEDS

#define AS5047_LOOP_HZ              60000
#define SVM_LOOP_HZ                 30000
#define BLDC_LOOP_HZ                25000
#define DEBUG_LOOP_HZ               60

// SVM

#define PHASE_ADVANCE               1.45 // RADIANS: optimum is 90*, or PI/2, or 1.57
#define POS_TO_THETA                0.0042197349

// BBESC_GRINDER

#define FWD_PIN                     12
#define REV_PIN                     11

#endif
