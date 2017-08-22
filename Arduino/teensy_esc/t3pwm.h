#ifndef T3PWM_H
#define T3PWM_H

#include <Arduino.h>

class T3PWM {
  
  public:
  T3PWM();
  void init();
  void setupForBLDC();
  void setupForFOC();
  void setPhases(unsigned short phase_u, unsigned short phase_v, unsigned short phase_w); // TODO: values?
  void setPhasesDirect(unsigned short phase_u_lo, unsigned short phase_u_hi, unsigned short phase_v_lo, unsigned short phase_v_hi, unsigned short phase_w_lo, unsigned short phase_w_hi);
  void setBrake(uint16_t strength);
  void setPhasesLow();

  private:
  // not sure if we even need to / want to hold on to / retrieve current phase values?
  uint16_t _phase_u; 
  uint16_t _phase_v;
  uint16_t _phase_q;
  
};

#endif
