/*
 * a class, of which only one instance,
 * containing the instances of all other classes
 * a nice package for instance crosstalk ... our central node
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <Arduino.h>

#define KERN Kernel::instance

class SBP;
class Shell;
class Streamer;

// a good place to parse config values?
// how to auto-instance ? so that on #including Kernel.h it's available?

class Kernel{
  public: 
  Kernel();
  static Kernel* instance;

  void init();
  void onMainLoop();
  void flashLed();

  SBP* sbp;
  Shell* shell;
  Streamer* streamer;
  
};

#endif
