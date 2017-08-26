#ifndef FOC_H
#define FOC_H

#include <Arduino.h>
#include "svpwm.h"
#include "t3adc.h"
#include "rbf.h"
#include "rb.h"
#include "constants.h"
#include "config.h"

class FOC {
  public:
    FOC();

    void init();
    void onMainLoop();    

    IntervalTimer* timer;

    SVPWM* svpwm;
    T3ADC* t3adc;

    // current sensing
    RingBufferFloat* rbfau;
    RingBufferFloat* rbfav;
    RingBufferFloat* rbfaw;

    static volatile long _loopCount;
    static volatile float _p;
    
    void doClarke();

  private:
    uint16_t _riv;
    uint16_t _riw;
    
    float _iu;
    float _iv;
    float _iw;

    float _ia;
    float _ib;

    int _lastLoops;

};

#endif
