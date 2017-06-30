/*
 * does serial-streaming for graphing / analysis off-board
 */

#ifndef STREAMER_H
#define STREAMER_H

#define STREAMER_DEFAULT_HZ 60

#include <Arduino.h>

class Streamer{

  public: Streamer(int hz = STREAMER_DEFAULT_HZ);

  void start();
  void stop();
  void setHz(int hz);
  int getHz();
  void pickVarSet(int setId);

  static float sinSpot;
  static float sinStep;
  static double dv1;
  static double dv2;
  static double dv3;

  IntervalTimer _Stream_Timer;
  static void onStreamLoop();

  private:
  
  int _hz;
  bool _isRunning;
  
};

#endif
