#include "foc.h"
#include "kernel.h"
#include "as5047.h"

volatile long FOC::_loopCount = 0;
volatile float FOC::_p = 0;

FOC::FOC(){
  svpwm = new SVPWM();
  t3adc = new T3ADC();
  timer = new IntervalTimer();
}

void FOC::init(){
  this->svpwm->init();
  this->t3adc->init();

  _loopCount = 0;
  this->timer->begin(FOC::onLoop, 1000000/MACHINE_LOOP_DEFAULT_HZ);
}

void FOC::onLoop(){
  _loopCount ++;
  if(!(_loopCount % MACHINE_LOOP_SECONDARY)){
    Serial.print("u: ");
    Serial.print(KERNEL->foc->t3adc->rbau->latest()); // adc result data register
    Serial.print("\t v: ");
    Serial.print(KERNEL->foc->t3adc->rbav->latest());
    Serial.print("\t enc: ");
    Serial.print(KERNEL->as5047->filtered());
    Serial.println("");
    //KERNEL->foc->svpwm->doAngular(_p, 3.5);
    _p += 0.1;
    if(_p > TWO_PI){
      _p = 0;
    }
  }
}

