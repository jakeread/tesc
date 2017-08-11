#ifndef FOC_H
#define FOC_H

#include <Arduino.h>
#include "svpwm.h"
#include "t3adc.h"
#include "constants.h"
#include "config.h"

class FOC {
  public:
    FOC();

    void init();

    IntervalTimer* timer;

    SVPWM* svpwm;
    T3ADC* t3adc;

    static void onLoop();
    static volatile long _loopCount;
    static volatile float _p;

  private:

};

#endif
