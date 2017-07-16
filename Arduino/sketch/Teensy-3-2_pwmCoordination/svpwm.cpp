#include "svpwm.h"
#include "t3pwm.h"

SVPWM::SVPWM(){
  //
}

void SVPWM::init(){
  this->t3pwm = new T3PWM();
}

int SVPWM::doVector(float a, float b){
  //
  return 0;
}

int SVPWM::directPhases(int u, int v, int w){
  _u = u;
  _v = v;
  _w = w;
  this->t3pwm->setPhases(_u, _v, _w);
  return 1;
}

int SVPWM::killPhases(){
  //
  return 0;
}

