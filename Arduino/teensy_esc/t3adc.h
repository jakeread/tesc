// static obj, does ADC register footwork, isr, ringbuffering. all adc's are here...

#ifndef T3ADC_H
#define T3ADC_H

#include <mk20dx128.h>
#include "rb.h"

class T3ADC {
  public:

  T3ADC();

  void init();

  RingBuffer* rbav; // holdin on' to raw adc values from these pins
  RingBuffer* rbaw;
  
  bool adc0_flag;
  bool adc1_flag;

  private:
};

#endif
