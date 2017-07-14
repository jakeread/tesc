#ifndef ENCODER_AS5047_H
#define ENCODER_AS5047_H

#include <Arduino.h>
#include "t3spi.h"
#include "ringbuffer.h"
#include "config.h"

class AS5047 {

  public:
  AS5047();
  
  T3SPI SPI_AS5047;
  
  void init();
  void readNow(); // read 1 value
  uint16_t mostRecent(); // TODO: should both be the same type (this and filtered)
  float filtered();
  uint32_t filteredInt();

  bool setEncoderOffset(uint16_t newOffset);
  uint16_t getEncoderOffset();

  bool setEncoderReverse(bool trueFalse);
  bool getEncoderReverse();

  RingBuffer Readings;

  private:
  volatile float _filtered; // TODO: check all of these & as well as their read / writes are all interrupt safe :|
  volatile uint32_t _filteredInt;
  volatile float _offset;
  volatile uint32_t _offsetInt;
  volatile uint16_t _reading;
  volatile uint16_t _pardCheckBits;
  volatile uint16_t _pardBitRx;
  volatile uint16_t _errBit;

  uint16_t          _encoderOffset;
  bool              _encoderReverse;

  volatile uint16_t readWord;
  volatile uint16_t noOpWord;
  volatile uint16_t readWords[2] = {};
  volatile uint16_t returnWords[2] = {};

  float _avgSumFloat;
  uint32_t _avgSumInt;
};

#endif
