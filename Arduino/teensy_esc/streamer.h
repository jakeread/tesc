/*
 * does serial-streaming for graphing / analysis off-board
 */

#ifndef STREAMER_H
#define STREAMER_H

#define STREAMER_DEFAULT_HZ   60
#define VARSET_ID_TEST        0
#define VARSET_ID_VOLTAGES    1
#define VARSET_ID_CURRENTS    2

#include <Arduino.h>

class Streamer{

  public: Streamer(int hz = STREAMER_DEFAULT_HZ);

  void init();

  bool start();
  bool stop();
  bool restart();
  bool setHz(int hz);
  int getHz();
  bool setVarSet(int setId);
  int getVarSet();
  void add(int varId);
  void remove(int varId);

  static float sinSpot;
  static float sinStep;
  static double dv1;
  static double dv2;
  static double dv3;

  IntervalTimer _Stream_Timer;
  static void streamVarsTest();
  static void streamVarsVoltages();
  static void streamVarsCurrents();

  private:
  
  int _hz;
  bool _isRunning;
  int _varSet;
  void (*_varFn)(void);
  
};

#endif
