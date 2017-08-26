#ifndef FASTRIG_H
#define FASTRIG_H

#include <Arduino.h>

#define NUM_TABLE_ENTRIES 2048

namespace FASTRIG{
  void ftTableInit();
  float ftSin(float theta);
  float ftCos(float theta);
};

#endif

