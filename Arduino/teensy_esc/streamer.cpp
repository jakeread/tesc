#include "streamer.h"
#include "kernel.h"

#include "sampler.h"
#include "ringBuffer.h"

/*
 * ! these functions should all actually stay independent of shell, 
 * where shell is handling chat-with-user, streamer.cpp operates the stream output ONLY
 */

float Streamer::sinSpot = 0;
float Streamer::sinStep = PI/60;
double Streamer::dv1 = 0;
double Streamer::dv2 = 0;
double Streamer::dv3 = 0;

Streamer::Streamer(int hz){
  _hz = hz;
  _isRunning = false;
}

void Streamer::init(){
  _Stream_Timer.priority(129);
  _varFn = Streamer::streamVarsCurrents;
  _varSet = VARSET_ID_CURRENTS;
}

void Streamer::streamVarsTest(){

  dv1 = sin(sinSpot - 2/3 * PI);
  dv2 = sin(sinSpot);
  dv3 = sin(sinSpot + 2/3 * PI);
  sinSpot += sinStep; // seems like these are all not evaluating...
  
  if(sinSpot > TWO_PI){
    sinSpot = 0;
  }

  Serial.print("#");
  Serial.print(millis());
  Serial.print(" dv1:");
  Serial.print(dv1);
  Serial.print(" dv2:");
  Serial.print(sin(sinSpot + 2/3 * PI));
  Serial.print(" dv3:");
  Serial.print(dv3);
  Serial.print(" sinSpot:");
  Serial.print(sinSpot);
  Serial.println("");
}

void Streamer::streamVarsVoltages(){
  Serial.print("#");
  Serial.print(millis());
  Serial.print("va:");
  Serial.print(KERNEL->sampler->rbva->latest());
  Serial.print("vb:");
  Serial.print(KERNEL->sampler->rbvb->latest());
  Serial.print("vc:");
  Serial.print(KERNEL->sampler->rbvc->latest());
  Serial.println("");
}

void Streamer::streamVarsCurrents(){
  Serial.print("#");
  Serial.print(millis());
  Serial.print(" aa:");
  Serial.print(KERNEL->sampler->rbaa->latest());
  Serial.print(" ab:");
  Serial.print(KERNEL->sampler->rbab->latest());
  Serial.print(" ac:");
  Serial.print(KERNEL->sampler->rbac->latest());
  Serial.println("");
}

bool Streamer::start(){
  if(_isRunning){
    return false;
  } else {
    _Stream_Timer.begin(_varFn, 1000000/_hz);
    _isRunning = true;
    return true;
  }
}

bool Streamer::stop(){
  if(!_isRunning){
    return false;
  } else {
    _Stream_Timer.end();
    _isRunning = false;
    return true;
  }
}

bool Streamer::restart(){
  if(!_isRunning){
    return false;
  } else {
    _Stream_Timer.end();
    _Stream_Timer.begin(_varFn, 1000000/_hz);
    return true;
  }
}

bool Streamer::setHz(int hz){
  if(hz == 0){
    return false; // no dice, shell will report error
   } else {
    _hz = hz;
    if(_isRunning){ // if already running we need to restart, else new _hz will be used on next restart
      _Stream_Timer.end();
      _Stream_Timer.begin(_varFn, 1000000/_hz);
    }
    return true;
  }
}

int Streamer::getHz(){
  return _hz;
}

bool Streamer::setVarSet(int varSetId){ // TODO: most shell commands should return true / f -> error reporting stays in shell.cpp
  switch(varSetId){
    case VARSET_ID_TEST:
      _varSet = VARSET_ID_TEST;
      _varFn = Streamer::streamVarsTest;
      restart();
      return true;
      break;
    case VARSET_ID_VOLTAGES:
      _varSet = VARSET_ID_VOLTAGES;
      _varFn = Streamer::streamVarsVoltages;
      restart();
      return true;
      break;
    case VARSET_ID_CURRENTS:
      _varSet = VARSET_ID_CURRENTS;
      _varFn = Streamer::streamVarsCurrents;
      restart();
      return true;
      break;
    default:
      return false;
  }
}

int Streamer::getVarSet(){
  return _varSet;
}

void Streamer::add(int varId){
  
}

void Streamer::remove(int varId){
  
}

