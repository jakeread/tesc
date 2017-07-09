/*
 * reads adc's, maintains ringbuffers
 */

#ifndef SAMPLER_H
#define SAMPLER_H

#include <Arduino.h>

class RingBuffer;

class Sampler{
  public: 
  
  Sampler(int pinva, int pinvb, int pinvc, int pinaa, int pinab, int pinac);

  void init();
  void readANow(); // TODO: will have to coordinate on PWM-on phases? some missed understanding there as well?
  void readVNow();

  // ringbuffer, voltage readings
  RingBuffer* rbva; // TODO: RingBuffers & type-specified ? how
  RingBuffer* rbvb;
  RingBuffer* rbvc;

  // ringbuffer, amperage readings
  RingBuffer* rbaa;
  RingBuffer* rbab;
  RingBuffer* rbac;

  private:
  int _pinva;
  int _pinvb;
  int _pinvc;
  int _pinaa;
  int _pinab;
  int _pinac;
};

#endif
