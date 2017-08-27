// circular buffers!
// https://en.wikipedia.org/wiki/Circular_buffer

/*
 * TODO: use Template ... make ringBuffer where object instantiation includes
 * type
 * list length
 */

#ifndef RINGBUFFERFLOAT_H
#define RINGBUFFERFLOAT_H

#include <Arduino.h>
#include "config.h"

#define RBUF_FLOAT_LENGTH 32

class RingBufferFloat {
  
  public:
  RingBufferFloat();

  void clear();
  void push(float item);
  void get(float* item, uint8_t relativePos);
  void latest(float* latest);
  void sum(float* sum);

  private:
  float _list[RBUF_FLOAT_LENGTH]; // TODO: Volatile? !
  uint8_t _listPosition;
  uint8_t _listEval;
  
};
#endif
