#ifndef T3PWM_H
#define T3PWM_H

#include <Arduino.h>

class T3PWM {
  public:

  T3PWM();
  void init();
  void setPhases(uint16_t phase_a, uint16_t phase_b, uint16_t phase_c); // TODO: values?
  void setBrake(uint16_t strength);
  void setPhasesLow();

  private:
  // not sure if we even need to / want to hold on to / retrieve current phase values?
  uint16_t _phase_a; 
  uint16_t _phase_b;
  uint16_t _phase_c;
};

#endif
