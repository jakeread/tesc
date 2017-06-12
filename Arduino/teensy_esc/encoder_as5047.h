#ifndef ENCODER_AS5047_H
#define ENCODER_AS5047_H

#include <Arduino.h>
#include "t3spi.h"
#include "ringBuffer.h"
#include "config.h"

class AS5047 {

  public:
  AS5047();
  void init();
  void readNow(); // read 1 value
  uint16_t mostRecent();
  float filtered();
  uint32_t filteredInt();
  T3SPI SPI_AS5047;

  RingBuffer Readings;

  private:
  uint8_t readingPosition;
  volatile float _filtered;
  volatile uint32_t _filteredInt;
  volatile float _offset;
  volatile uint32_t _offsetInt;
  volatile uint16_t _reading;
  volatile uint16_t _pardCheckBits;
  volatile uint16_t _pardBitRx;
  volatile uint16_t _errBit;

  volatile uint16_t readWord;
  volatile uint16_t noOpWord;
  volatile uint16_t readWords[2] = {};
  volatile uint16_t returnWords[2] = {};

  float avgSumFloat;
  uint32_t avgSumInt;
};

#endif
