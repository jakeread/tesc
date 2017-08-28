#ifndef FOC_H
#define FOC_H

#include <Arduino.h>
#include "t3adc.h"
#include "t3pwm.h"
#include "rbf.h"
#include "rb.h"
#include "constants.h"
#include "config.h"

class FOC {
  public:
    FOC();

    void init();
    void onMainLoop();    

    T3PWM* t3pwm;
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
