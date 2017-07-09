#ifndef BLDC_H
#define BLDC_H

#include <Arduino.h>
#include "motorleg.h"
#include "config.h"

#define BLDC_INPUTMODE_POT        1
#define BLDC_INPUTMODE_SHELL      0

class BLDC{

  public:
  BLDC(int pinHiA, int pinLoA, int pinHiB, int pinLoB, int pinHiC, int pinLoC); // constructor

  MotorLeg* MLA;
  MotorLeg* MLB;
  MotorLeg* MLC;
  
  void init();          // starts timers
  
  bool duty(int duty);  // set rms pwm val
  int getDuty();
  bool dir(bool dir);   // set direction for commutation
  bool getDir();
  
  void clcommutate();
  void pot_input_update();
  void commutate(uint8_t comPos);

  int getInputMode();
  bool setInputMode(int mode);

  bool killAllPower();

  void prntCzc();

  private:

  // vars for update fc'n
  uint16_t _dutyUser;
  uint16_t _dutyDished;
  
  
  int _duty;
  int _dir; // -1 or 1
  int _advance;
  
  int _mode;
  int _check;
  int _inputMode;

  int _comloc;
  unsigned long _lastcom;

  uint16_t _czc[7]; // comZoneCaps

  int _modulo;
  int _posNow;
  int _comZone;
  int _comCommand;

  int comtable[6][3] = {
    {  1,-1, 0 }, //0
    {  1, 0,-1 }, //1
    {  0, 1,-1 }, //2
    { -1, 1, 0 }, //3
    { -1, 0, 1 }, //4
    {  0,-1, 1 }, //5
  };
  
};

#endif
