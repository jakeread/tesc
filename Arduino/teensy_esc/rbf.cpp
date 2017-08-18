#include "rbf.h"

RingBufferFloat::RingBufferFloat(){
  _listPosition = 0;
}

void RingBufferFloat::push(float item){
  // puts a new item into the buffer, overwriting least-recent entry
  
  _listPosition ++;
  _list[_listPosition] = item;
  if(_listPosition >= RBUF_LENGTH){
    _listPosition = 0;
  }
}

float RingBufferFloat::get(uint8_t relativePos){
  // returns item at this position
  _listEval = _listPosition + relativePos;
  if(_listEval < 0){
    _listEval = RBUF_LENGTH - _listEval;
  } else if(_listEval > RBUF_LENGTH){
    _listEval -= RBUF_LENGTH;
  }

  return _list[_listEval];
}

float RingBufferFloat::latest(){
  return _list[_listPosition];
}


