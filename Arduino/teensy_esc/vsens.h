/*
 * reads adc's, maintains ringbuffers
 */

#ifndef VSENS_H
#define VSENS_H

#include <Arduino.h>

class RingBuffer;

class VSens{
  public: 
  
  VSens();

  void init();
  void readVNow();

  // ringbuffer, amperage readings
  RingBuffer* rbav;
  RingBuffer* rbau;
  
  // ringbuffer, voltage readings
  RingBuffer* rbvu; // TODO: RingBuffers & type-specified ? how
  RingBuffer* rbvv;
  RingBuffer* rbvw;

  private:

};

#endif

