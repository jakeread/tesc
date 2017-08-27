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

    static volatile long _loopCount;
    static volatile float _p;
    
    void doClarke();

  private:

    RingBufferFloat* _id_err_int_rb;
    RingBufferFloat* _iq_err_int_rb;

    float _i_scalar;
    int _lastLoops;

};

#endif
