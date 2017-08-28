/*
 * a class, of which only one instance,
 * containing the instances of all other classes
 * a nice package for instance crosstalk ... our central node
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <Arduino.h>

#define KERNEL Kernel::instance

/*
 * trix // so that we can write pointers below. actual constructors are called in the kernel cpp file
 */
  
class BLDC;
class FOC;

class AS5047;
class VSens;

class SP;
class Shell;

// a good place to parse config values?
// how to auto-instance ? so that on #including Kernel.h it's available?

class Kernel{
  public: 
  Kernel();
  static Kernel* instance; // only 1 pls

  void init();
  void onMainLoop();
  void flashLed();

  void gateEnableOff(); // shuts down DRV8302
  void gateEnableOn();

  int simpleInput();

  bool encoderValueSearch(int duty);

  /*
   * pointers to modules
   */
  BLDC* bldc; //->motorleg
  FOC* foc;   //->svpwm->t3pwm

  AS5047* as5047;
  VSens* vsens;

  SP* sp;
  Shell* shell;

  static int _loopCount;
  
};

#endif
