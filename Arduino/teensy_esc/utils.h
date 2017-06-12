// utilities for Teensy ESC
// needs a kernel

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

class Utils{
  
  public:
  Utils();
  void crush(double* valPtr);
  void doubleMap(double* valPtr, double fromLo, double fromHi, double toLo, double toHi);
  
};


#endif
