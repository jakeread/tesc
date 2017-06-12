// circular buffers!
// https://en.wikipedia.org/wiki/Circular_buffer

/*
 * TODO: use Template ... make ringBuffer where object instantiation includes
 * type
 * list length
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <Arduino.h>
#include "config.h"

class RingBuffer {
  
  public:
  RingBuffer();

  void push(uint16_t item);
  uint16_t get(uint8_t relativePos);

  private:
  uint16_t _list[RBUF_LENGTH];
  uint8_t _listPosition;
  uint8_t _listEval;
  
};
#endif
