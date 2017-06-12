// Utils for Teensy ESC
// Todo: get into kernel, how to kernel?

#include "utils.h"

Utils::Utils(){
  // ain't no thang
}

void Utils::crush(double* valPtr){
  if(*valPtr > 1){ *valPtr = 1; } // resolving valPtr pointer to value
  if(*valPtr < -1){ *valPtr = -1; }
}

void Utils::doubleMap(double* valPtr, double fromLo, double fromHi, double toLo, double toHi){
  // maths
  *valPtr = (*valPtr-fromLo) * (toHi - toLo) / (fromHi - fromLo) + toLo;
}

