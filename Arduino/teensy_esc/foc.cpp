#include "foc.h"
#include "kernel.h"
#include "as5047.h"

volatile long FOC::_loopCount = 0;
volatile float FOC::_p = 0;

FOC::FOC() {
  svpwm = new SVPWM();
  t3adc = new T3ADC();
  timer = new IntervalTimer();

  rbfau = new RingBufferFloat();
  rbfav = new RingBufferFloat();
  rbfaw = new RingBufferFloat();
}

void FOC::init() {
  this->svpwm->init();
  this->t3adc->init();

  pinMode(14, OUTPUT);

  _loopCount = 0;
  this->timer->begin(FOC::onLoop, 1000000 / MACHINE_LOOP_DEFAULT_HZ);
}

void FOC::adcInterruptCheck() {
  if (this->t3adc->adc0_flag && this->t3adc->adc1_flag) {
    digitalWriteFast(14, HIGH);
    _iu = (this->t3adc->rbau->latest() - CURRENT_AMP_OFFSET) * (V_REF / 4095) / (CURRENT_SHUNT_RES * CURRENT_AMP_GAIN);
    _iv = (this->t3adc->rbav->latest() - CURRENT_AMP_OFFSET) * (V_REF / 4095) / (CURRENT_SHUNT_RES * CURRENT_AMP_GAIN);
    digitalWriteFast(14, LOW);
    this->t3adc->adc0_flag = false;
    this->t3adc->adc1_flag = false;
    rbfau->push(_iu);
    rbfav->push(_iv);
    rbfaw->push(-_iu - _iv);
    KERNEL->as5047->readNow();
  }
}

void FOC::doClarke() {
  _ia = (2.0 / 3.0) * rbfau->latest() - (1.0 / 3.0) * rbfav->latest() - (1.0 / 3.0) * rbfaw->latest();
  _ib = ONE_OVER_SQRT_THREE * rbfav->latest() - ONE_OVER_SQRT_THREE * rbfaw->latest();
}

void FOC::onLoop() {
  _loopCount ++;

  KERNEL->foc->doClarke();
  /*
    if(!(_loopCount % MACHINE_LOOP_SECONDARY)){
    Serial.print("u:\t");
    Serial.print(KERNEL->foc->t3adc->rbau->latest()); // adc result data register
    Serial.print("\t v:\t");
    Serial.print(KERNEL->foc->t3adc->rbav->latest());
    Serial.print("\t w:\t");
    Serial.print(KERNEL->foc->rbfaw->latest());
    Serial.print("\t enc: ");
    Serial.print(KERNEL->as5047->filtered());
    //Serial.println("");
    Serial.print("\tiadr:\t");
    Serial.print(KERNEL->foc->rbfau->latest());
    Serial.print("\t ibdr:\t");
    Serial.print(KERNEL->foc->rbfau->latest() * ONE_OVER_SQRT_THREE + KERNEL->foc->rbfav->latest() * TWO_OVER_SQRT_THREE);
    Serial.print("\tiafc:\t");
    Serial.print(KERNEL->foc->_ia);
    Serial.print("\tibfc:\t");
    Serial.print(KERNEL->foc->_ib);
    Serial.println("");
    }
  */
  KERNEL->foc->svpwm->doAngular(_p, 3.5);
  _p += 0.25;
  if (_p > TWO_PI) {
    _p = 0;
  }
}

