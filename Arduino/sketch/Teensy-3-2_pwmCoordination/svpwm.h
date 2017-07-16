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

  int doVector(float a, float b);
  int directPhases(int u, int v, int w); // b/c T3PWM belongs to SVPWM, only access thru kernel is thru here
  int killPhases(); // kills all phases

  T3PWM* t3pwm;

  private:
  float _a;
  float _b;

  int _u;
  int _v;
  int _w;
};

#endif

