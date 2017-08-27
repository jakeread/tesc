#include "rbf.h"

RingBufferFloat::RingBufferFloat(){
  _listPosition = 0;
}

void RingBufferFloat::clear(){
  for(int i = 0; i < RBUF_FLOAT_LENGTH; i ++){
    _list[i] = 0.0f;
  }
}

void RingBufferFloat::push(float item){
  // puts a new item into the buffer, overwriting least-recent entry
  
  _listPosition ++;
  _list[_listPosition] = item;
  if(_listPosition >= RBUF_FLOAT_LENGTH){
    _listPosition = 0;
  }
}

void RingBufferFloat::get(float *item, uint8_t from){
  // returns item at this position
  _listEval = _listPosition + from;
  if(_listEval < 0){
    _listEval = RBUF_FLOAT_LENGTH - _listEval;
  } else if(_listEval > RBUF_FLOAT_LENGTH){
    _listEval -= RBUF_FLOAT_LENGTH;
  }

  *item = _list[_listEval];
}

void RingBufferFloat::latest(float* latest){
  *latest = _list[_listPosition];
}

void RingBufferFloat::sum(float* sum){
  noInterrupts();
  *sum = 0;
  for(int i = 0; i < RBUF_FLOAT_LENGTH; i ++){
    *sum += _list[i];
  }
  interrupts();
}


