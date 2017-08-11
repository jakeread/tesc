#ifndef SVPWM_H
#define SVPWM_H

/*
 * input is vector in a-b frame
 * does sector -> space vector pwm's on phases u v w
 */

#include <Arduino.h>

class T3PWM;

class SVPWM {
  public:
  SVPWM();
  void init();

  int doAngular(double theta, double mag);
  int doVector(double a, double b);
  int directPhases(int u, int v, int w); // b/c T3PWM belongs to SVPWM, only access thru kernel is thru here
  int killPhases(); // kills all phases

  T3PWM* t3pwm;

  private:
  double _a;
  double _b;
  double _mag;
  double _theta;
  double _omega;
  
  uint8_t _sector;

  double _tz_a;

  double _t0;
  double _t1;
  double _t2;
  
  double _sa;
  double _sb1;
  double _sb2;
  double _sc;

  uint16_t _tu; // = tu
  uint16_t _tv; // = tv
  uint16_t _tw; // = tw
};

#endif

