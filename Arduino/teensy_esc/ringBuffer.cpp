
#include "ringBuffer.h"

RingBuffer::RingBuffer(){
  _listPosition = 0;
}

void RingBuffer::push(uint16_t item){
  // puts a new item into the buffer, overwriting least-recent entry
  
  _listPosition ++;
  
  _list[_listPosition] = item;

  if(_listPosition >= RBUF_LENGTH){
    _listPosition = 0;
  }
}

uint16_t RingBuffer::get(uint8_t relativePos){
  // returns item at this position
  _listEval = _listPosition - 1 + relativePos;
  if(_listEval < 0){
    _listEval = RBUF_LENGTH - _listEval;
  } else if(_listEval > RBUF_LENGTH){
    _listEval -= RBUF_LENGTH;
  }

  return _list[_listEval];
}


