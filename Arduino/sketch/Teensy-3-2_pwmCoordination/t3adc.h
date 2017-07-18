// static obj, does ADC register footwork, isr, ringbuffering. all adc's are here...

#ifndef T3ADC_H
#define T3ADC_H

#include <mk20dx128.h>
#include "ringbuffer.h"

class T3ADC {
  public:

  T3ADC(uint8_t pin_sensa_u, uint8_t pin_sensa_v, uint8_t pin_sensv_u, uint8_t pin_sensv_v, uint8_t pin_sensv_w);

  void init();

  RingBuffer RB_SENSA_U; // holdin on' to raw adc values from these pins
  RingBuffer RB_SENSA_V;

  RingBuffer RB_SENSV_U; // yar, lots of adc's to handle !
  RingBuffer RB_SENSV_V;
  RingBuffer RB_SENSV_W;

  private:

  uint8_t _pin_sensa_u;
  uint8_t _pin_sensa_v;
  uint8_t _pin_sensv_u;
  uint8_t _pin_sensv_v;
  uint8_t _pin_sensv_w;

  // readings holders -> but perhaps we can write register direct -> ringBuffer
  uint16_t _sensa_u_reading;
  uint16_t _sensa_v_reading;
  uint16_t _sensv_u_reading;
  uint16_t _sensv_v_reading;
  uint16_t _sensv_w_reading;

  // tick (to track timing on scope) 
  // are const & use digitalWriteFast to use optimized code w/ PRJC so that scope traces are best aligned w/ adc reading
  const uint8_t _tickPin = 14;
  const bool _tickOn = HIGH;
  const bool _tickOff = LOW;

};

#endif
