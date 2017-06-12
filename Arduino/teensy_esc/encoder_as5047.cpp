// Does SPI Footwork, averaging

#include "encoder_as5047.h"

AS5047::AS5047(){
  // cnstructor
}

void AS5047::init(){
  
  SPI_AS5047.begin_MASTER(ALT_SCK, ALT_MOSI, ALT_MISO, CS4, CS_ActiveLOW);
  SPI_AS5047.setCTAR(CTAR_0, 16, SPI_MODE1, LSB_FIRST, SPI_CLOCK_DIV8); // DIV2 is no-go

  readWord = 0x3FFF | 0x4000 | 0x8000;
  noOpWord = 0x0000 | 0x4000 | 0x8000;

  readWords[0] = readWord;
  readWords[1] = noOpWord;

  readingPosition = 0;

  avgSumFloat = 0;
  avgSumInt = 0;
  
}

void AS5047::readNow(){
  // TODO: check parity bit, that readings make sense, etc
  SPI_AS5047.txrx16(readWords, returnWords, 2, CTAR_0, CS4);

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
  
  #if AS5047_REV
    Readings.push(AS5047_RESOLUTION - _reading);
  #else
    Readings.push(_reading);
  #endif
}

uint16_t AS5047::mostRecent(){
  return _reading;
}

float AS5047::filtered(){
  avgSumFloat = 0;
  
  noInterrupts();
  for(int i = 0; i < AS5047_AVERAGING; i++){
    avgSumFloat += Readings.get(-i);
  }
  interrupts();
  
  _filtered = avgSumFloat / AS5047_AVERAGING;

  _offset = _filtered + AS5047_AVERAGING;
  if(_offset > AS5047_RESOLUTION){
    _offsetInt -= MOTOR_MODULO;
  }
  return _filtered;
}

uint32_t AS5047::filteredInt(){
  avgSumInt = 0;
  
  noInterrupts();
  for(int i = 0; i < AS5047_AVERAGING; i++){
    avgSumInt += Readings.get(-i);
  }
  interrupts();

  _filteredInt = avgSumInt / AS5047_AVERAGING;
  
  _offsetInt = _filteredInt + AS5047_OFFSET_UP;
  if(_offsetInt > AS5047_RESOLUTION){
    _offsetInt -= MOTOR_MODULO;
  }
  
  return _offsetInt;
}

