#ifndef BLDC_H
#define BLDC_H

#include <Arduino.h>
#include "ml.h"
#include "config.h"

class T3PWM;

class BLDC {

  public:
    BLDC(); // constructor

    T3PWM* t3pwm;

    void init();          // starts timers

    bool duty(int duty);  // set rms pwm val
    int getDuty();

    bool dir(bool dir);   // set direction for commutation
    bool getDir();

    bool freq(unsigned int hz);
    int getFreq();

    void clcommutate();
    void olcommutate();
    
    void pot_input_update();

    void commutateStep(int dir);
    void commutate(uint8_t comPos);
    void lockPhase(uint8_t lockSpot);

    int getInputMode();
    bool setInputMode(int mode);

    bool killAllPower();

    void prntCzc();

  private:

    // vars for update fc'n
    uint16_t _dutyUser;
    uint16_t _dutyDished;

    int _duty;
    int _dir; // -1 or 1
    unsigned int _freq;
    unsigned int _maxFreq;
    int _period; // us between commutation cycles
    int _advance;

    unsigned short _phase_u_lo;
    unsigned short _phase_u_hi;
    unsigned short _phase_v_lo;
    unsigned short _phase_v_hi;
    unsigned short _phase_w_lo;
    unsigned short _phase_w_hi;

    int _mode;
    int _check;
    int _inputMode;

    int _comloc;
    unsigned long _lastcom;

    uint8_t _lockSpot;

    uint16_t _czc[7]; // comZoneCaps

    int _modulo;
    int _posNow;
    int _comZone;
    int _comCommand;

    int comtable[6][3] = {
      {  1, -1, 0 }, //0
      {  1, 0, -1 }, //1
      {  0, 1, -1 }, //2
      { -1, 1, 0  }, //3
      { -1, 0, 1  }, //4
      {  0, -1, 1 }, //5
    };

    int comPhaseTable[3][6][2] = {
      { // U
        { 1, 0 }, // lo, hi
        { 1, 0 },
        { 0, 0 },
        { 0, 1 },
        { 0, 1 },
        { 0, 0 },
      }, {                        // V
        { 0, 1 },
        { 0, 0 },
        { 1, 0 },
        { 1, 0 },
        { 0, 0 },
        { 0, 1 },
      }, {                        // W
        { 0, 0 },
        { 0, 1 },
        { 0, 1 },
        { 0, 0 },
        { 1, 0 },
        { 1, 0 },
      },
    };

    float phaseLockTable[3][3] = { // indeterminite currents in -1, -1 phases -> doesnotwork
      {  1, -1, -1  },
      { -1,  1, -1  },
      { -1, -1,  1  },
    };

};

#endif

