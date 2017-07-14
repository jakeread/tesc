// static obj, does ADC register footwork, isr, ringbuffering. all adc's are here...

#ifndef T3ADC_H
#define T3ADC_H

#include <mk20dx128.h>
#include "ringbuffer.h"

class T3ADC {
  public:

  T3ADC(uint8_t pin_sensa_a, uint8_t pin_sensa_b, uint8_t pin_sensa_c, uint8_t pin_sensv_a, uint8_t pin_sensv_b, uint8_t pin_sensv_c);

  void init();

  RingBuffer RB_SENSA_A; // holdin on' to raw adc values from these pins
  RingBuffer RB_SENSA_B;
  RingBuffer RB_SENSA_C;

  RingBuffer RB_SENSV_A; // yar, lots of adc's to handle !
  RingBuffer RB_SENSV_B;
  RingBuffer RB_SENSV_C;

  private:

  uint8_t _pin_sensa_a;
  uint8_t _pin_sensa_b;
  uint8_t _pin_sensa_c;
  uint8_t _pin_sensv_a;
  uint8_t _pin_sensv_b;
  uint8_t _pin_sensv_c;

  // readings holders -> but perhaps we can write register direct -> ringBuffer
  uint16_t _sensa_a_reading;
  uint16_t _sensa_b_reading;
  uint16_t _sensa_c_reading;
  uint16_t _sensv_a_reading;
  uint16_t _sensv_b_reading;
  uint16_t _sensv_c_reading;

  // tick (to track timing on scope) 
  // are const & use digitalWriteFast to use optimized code w/ PRJC so that scope traces are best aligned w/ adc reading
  const uint8_t _tickPin = 14;
  const bool _tickOn = HIGH;
  const bool _tickOff = LOW;

};

#endif
