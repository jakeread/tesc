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
class SVM;
class AS5047;
class Sampler;

class SP;
class Shell;
class Streamer;

// a good place to parse config values?
// how to auto-instance ? so that on #including Kernel.h it's available?

class Kernel{
  public: 
  Kernel();
  static Kernel* instance; // only 1 pls

  void init();
  void onMainLoop();
  void flashLed();

  static void onSampleLoop(); // timer f'ns; also only 1 pls
  static void onMachineLoop();

  bool findEncoderReverse(int duty);
  uint16_t findEncoderOffset(int duty);

  /*
   * timers
   */

  IntervalTimer _Sample_Timer; // TODO: do these need to be pointered-to ? rather than 'direct' ? isn't this only a semantic difference anyways?
  int _Sample_T_hz;
  bool _Sample_T_isRunning;
  IntervalTimer _Machine_Timer;
  int _Machine_T_hz;
  bool _Machine_T_isRunning;

  void stopSampleTimer();
  void stopMachineTimer();
  void startSampleTimer();
  void startMachineTimer();

  /*
   * pointers to modules
   */
  BLDC* bldc;
  SVM* svm;
  AS5047* as5047;

  SP* sp;
  Shell* shell;
  Streamer* streamer;
  Sampler* sampler;

  static int _loopCount;
  
};

#endif
