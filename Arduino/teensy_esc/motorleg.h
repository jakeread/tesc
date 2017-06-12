// single phase of our three-phase BLDC
// when a phase is set in -ves, it is a Source of electrons (transistor connects it to ground), 
// when High it is a Drain (transistor connects it to voltage)

#ifndef MOTORLEG_H
#define MOTORLEG_H

#include <Arduino.h>
#include "config.h"

class MotorLeg{
  
  public:
  MotorLeg(int pinHi, int pinLo); // constructor
  //~MotorLeg(); // destructor, should have, don't
  void set(uint8_t duty, int dir); // val, -255 <-> 255 (duty cycle, direction)
  void setSVM(double dutyDir);
  void assert();
  void kill();
  void report();

  private:
  int _pinHi;
  int _pinLo;
  uint8_t _duty;
  uint8_t _dir;
};

#endif
