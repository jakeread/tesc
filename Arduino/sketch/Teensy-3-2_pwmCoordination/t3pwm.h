#ifndef T3PWM_H
#define T3PWM_H

#include <Arduino.h>

class T3PWM {
  public:

  T3PWM();
  void init();
  void setPhases(uint16_t phase_u, uint16_t phase_v, uint16_t phase_w); // TODO: values?
  void setBrake(uint16_t strength);
  void setPhasesLow();

  private:
  // not sure if we even need to / want to hold on to / retrieve current phase values?
  uint16_t _phase_u; 
  uint16_t _phase_v;
  uint16_t _phase_q;
};

#endif
