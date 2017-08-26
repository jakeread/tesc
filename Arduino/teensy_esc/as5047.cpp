// Does SPI Footwork, averaging

#include "kernel.h"
#include "as5047.h"
#include "rb.h"

AS5047::AS5047(){
  // cnstructor
}

void AS5047::init(){

  ringbuffer = new RingBuffer();
  timer = new IntervalTimer();
  
  SPI_AS5047.begin_MASTER(ALT_SCK, ALT_MOSI, ALT_MISO, CS1, CS_ActiveLOW);
  SPI_AS5047.setCTAR(CTAR_0, 16, SPI_MODE1, LSB_FIRST, SPI_CLOCK_DIV8); // DIV2 is no-go

  readWord = 0x3FFF | 0x4000 | 0x8000;
  noOpWord = 0x0000 | 0x4000 | 0x8000;

  readWords[0] = readWord;
  readWords[1] = noOpWord;

  _avgSumFloat = 0;
  _avgSumInt = 0;

  #if IS_BLDC_MACHINE
  _encoderOffset = AS5047_OFFSET_BLDC;
  _encoderReverse = AS5047_REV_BLDC;
  #else if IS_FOC_MACHINE
  _encoderOffset = AS5047_OFFSET_FOC;
  _encoderReverse = AS5047_REV_FOC;
  #endif
}

void AS5047::readNow(){
  // TODO: check parity bit, that readings make sense, etc
  SPI_AS5047.txrx16(readWords, returnWords, 2, CTAR_0, CS1);

  _reading = returnWords[1] << 2;
  _reading /= 4;

  _pardCheckBits = returnWords[1] << 1;
  _pardCheckBits /= 2;                  // if this # is even, _pardBitRx should be 0, if odd, 1
  _pardBitRx = returnWords[1] & 0x0001; // & acts like a 'mask' // https://en.wikipedia.org/wiki/Bitwise_operations_in_C
  
  if((_pardCheckBits - _pardBitRx)%2){
    _errBit = 1;
    Serial.println("ERROR: AS5047 Parity"); // something will catch this...
  } else {
    _errBit = 0;
  }

  /*
  Serial.print("returnWords[1]: ");
  Serial.print(returnWords[1]);
  Serial.print("\treturn << 2: ");
  Serial.print(_reading);
  Serial.print("\tparityCheckBits: ");
  Serial.print(_pardCheckBits);
  Serial.print("\tparity: ");
  Serial.print(_pardBitRx);
  Serial.print("\terr: ");
  Serial.println(_errBit);
  */
  
  if(_encoderReverse){
    ringbuffer->push(AS5047_RESOLUTION - _reading);
  } else {
    ringbuffer->push(_reading);
  }
}

uint16_t AS5047::mostRecent(){
  return _reading;
}

float AS5047::filtered(){
  _avgSumFloat = 0;
  
  noInterrupts();
  for(int i = 0; i < AS5047_AVERAGING; i++){
    _avgSumFloat += ringbuffer->get(-i);
  }
  interrupts();
  
  _filtered = _avgSumFloat / AS5047_AVERAGING + _encoderOffset;

  if(_filtered > AS5047_RESOLUTION){
    _filtered -= MOTOR_MODULO;
  }
  return _filtered;
}

uint32_t AS5047::filteredInt(){ // ~10us faster than float, above -> if you're going to do some conversions anyways *shrugman* use this
  _avgSumInt = 0;
  
  noInterrupts();
  for(int i = 0; i < AS5047_AVERAGING; i++){
    _avgSumInt += ringbuffer->get(-i);
  }
  interrupts();

  _filteredInt = _avgSumInt / AS5047_AVERAGING + _encoderOffset;
    
  if(_filteredInt > AS5047_RESOLUTION){
    _filteredInt -= MOTOR_MODULO;
  }
  
  return _filteredInt;
}

bool AS5047::setEncoderOffset(uint16_t newOffset){
  if(newOffset <= MOTOR_MODULO && newOffset >= 0){
    _encoderOffset = newOffset;
    return true;
  } else {
    return false;
  }
}

uint16_t AS5047::getEncoderOffset(){
  return _encoderOffset;
}

bool AS5047::setEncoderReverse(bool trueFalse){
  _encoderReverse = trueFalse;
  return true;
}

bool AS5047::getEncoderReverse(){
  return _encoderReverse;
}

void TIMER_HOOK_AS5047(){ // static unfuckery TODO
  KERNEL->as5047->readNow();
}

void AS5047::startSampling(){
  if(AS5047_SAMPLE_RATE){ // not-zero
    _usBetweenSamples = 1000000 / AS5047_SAMPLE_RATE;
  }
  timer->begin(TIMER_HOOK_AS5047, _usBetweenSamples);
}

void AS5047::stopSampling(){
  timer->end();
}

