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

class RingBufferFloat {
  
  public:
  RingBufferFloat();

  void push(float item);
  float get(uint8_t relativePos);
  float latest();

  private:
  float _list[RBUF_LENGTH]; // TODO: Volatile? !
  uint8_t _listPosition;
  uint8_t _listEval;
  
};
#endif
