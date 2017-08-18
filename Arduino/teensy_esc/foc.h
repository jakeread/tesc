#ifndef FOC_H
#define FOC_H

#include <Arduino.h>
#include "svpwm.h"
#include "t3adc.h"
#include "rbf.h"
#include "constants.h"
#include "config.h"

class FOC {
  public:
    FOC();

    void init();

    IntervalTimer* timer;

    SVPWM* svpwm;
    T3ADC* t3adc;

    // current sensing
    void adcInterruptCheck();
    RingBufferFloat* rbfau;
    RingBufferFloat* rbfav;
    RingBufferFloat* rbfaw;

    static void onLoop();    
    static volatile long _loopCount;
    static volatile float _p;
    
    void doClarke();

  private:
    float _iu;
    float _iv;

    float _ia;
    float _ib;

};

#endif
