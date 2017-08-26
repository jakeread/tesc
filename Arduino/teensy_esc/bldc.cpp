// does straightforward BLDC commutation
#include "bldc.h"
#include "kernel.h"
#include "rb.h"
#include "as5047.h"
#include "t3pwm.h"

BLDC::BLDC() {

  this->t3pwm = new T3PWM();

  _duty = BLDC_DEFAULT_DUTY;
  _dir = BLDC_DEFAULT_DIR;

  _comloc = 0;
  _lastcom = micros();

  for (int i = 0; i < 7; i ++) {
    _czc[i] = i * (MOTOR_MODULO / 6);
  }

  _phase_u_lo = 0;
  _phase_u_hi = 0;
  _phase_v_lo = 0;
  _phase_v_hi = 0;
  _phase_w_lo = 0;
  _phase_w_hi = 0;

}

void BLDC::init() {
  this->t3pwm->init();
  this->t3pwm->setupForBLDC();
  KERNEL->as5047->startSampling();
  _inputMode = BLDC_INPUTMODE_POT;
  freq(BLDC_DEFAULT_HZ);
  KERNEL->gateEnableOn();
  _ctr = 0;
}

void BLDC::onMainLoop(){
  clcommutate();
}

bool BLDC::killAllPower() {
  KERNEL->gateEnableOff();
  this->t3pwm->setPhasesLow();
  return true;
}

bool BLDC::wakeUp(){
  KERNEL->gateEnableOn();
  return true;
}

void BLDC::prntCzc() {
  for (int i = 0; i < 7; i ++) {
    Serial.print("_czc");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(_czc[i]);
    Serial.println(" ");
  }
  Serial.println(" ");
}

bool BLDC::duty(int duty) {
  if (duty > 0 && duty < 4096) {
    _duty = duty;
    return true;
  } else {
    KERNEL->gateEnableOff();
    return false;
  }
}

int BLDC::getDuty() {
  return _duty;
}

bool BLDC::dir(bool dir) {
  _dir = dir;
  return true;
}

bool BLDC::getDir() {
  return _dir;
}

int BLDC::getInputMode() {
  return _inputMode;
}

bool BLDC::setInputMode(int mode) {
  if (mode == BLDC_INPUTMODE_POT || mode == BLDC_INPUTMODE_SHELL) {
    _inputMode = mode;
    return true;
  } else {
    return false;
  }
}

void BLDC::clcommutate() {
  /*
     OK: Have a Duty-Cycle (eventually Torque) setting, and a direction setting
     SO: get a rolling-averaged position estimate (noInterrupts? -> or rollingAverage doesn't use most-recent value, so if it's in the process of being written we don't f...)
     Check comm table / simple math: where are we in comm table? encoder resolution / poles % 6 ?
     Set PWM's according
  */

  if(_ctr % 500){
    _ctr ++;
  } else {
    _ctr = 0;
    KERNEL->flashLed();
    duty(map(KERNEL->simpleInput(), 0, 1024, 1, 2096));
  }

  /*
     Do Modulo: for splitting Encoder (0-AS5047_RESOLUTION Physical Period, into 0-BLDC_MODULO Electrical Period)
  */
  _posNow = KERNEL->as5047->filteredInt();
  _modulo = _posNow % MOTOR_MODULO;
  /*
    Serial.print("Duty: ");
    Serial.print(_duty);
    Serial.print(" Dir: ");
    Serial.print(_dir);
    Serial.print(" posNow: ");
    Serial.print(_posNow);
    Serial.print(" Modulo: ");
    Serial.println(_modulo);
  */
  /* ENTER CONFIG
      Needs to update zone-values per pole-number
  */
  if (_modulo >= _czc[0] && _modulo < _czc[1]) {
    _comZone = 0;
  } else if (_modulo >= _czc[1] && _modulo < _czc[2]) {
    _comZone = 1;
  } else if (_modulo >= _czc[2] && _modulo < _czc[3]) {
    _comZone = 2;
  } else if (_modulo >= _czc[3] && _modulo < _czc[4]) {
    _comZone = 3;
  } else if (_modulo >= _czc[4] && _modulo < _czc[5]) {
    _comZone = 4;
  } else if (_modulo >= _czc[5] && _modulo <= _czc[6]) {
    _comZone = 5;
  }
  /*
    Serial.print(" comZone: ");
    Serial.print(comZone);
  */
  if (_dir == 1) {
    _comCommand = _comZone + 1; // there is some falsity in this nomenclature: offset is not measured correctly atm
    if (_comCommand == 6) {
      _comCommand = 0;
    }
  }
  if (_dir == -1) {
    _comCommand = _comZone - 1;
    if (_comCommand == -1) {
      _comCommand = 5;
    }
  }
  /*
    Serial.print(" comCommand: ");
    Serial.print(comCommand);
    Serial.println(" ");
  */
  commutate(_comCommand);
}

bool BLDC::freq(unsigned int hz) {
  if (hz > BLDC_MAX_OL_FREQ) {
    _freq  = BLDC_MAX_OL_FREQ;
  } else if (hz == 0) {
    _freq = 1;
  } else {
    _freq = hz;
  }
  _period = 1000000 / _freq;
  return true;
}

int BLDC::getFreq() {
  return _freq;
}

void BLDC::olcommutate() {
  if (micros() - _lastcom > _period) {
    if (analogRead(A14) < 1000) {
      KERNEL->gateEnableOff();
      KERNEL->flashLed();
    } else {
      KERNEL->gateEnableOn();
      commutateStep(_dir);
    }
    freq(map(KERNEL->simpleInput(), 0, 1024, 3, BLDC_MAX_OL_FREQ));
    //Serial.println(_period);
    _lastcom = micros();
  }
}

void BLDC::commutateStep(int dir) {
  _comloc += dir;
  if (_comloc > 5) {
    _comloc = 0;
  } else if (_comloc < 0) {
    _comloc = 5;
  }
  commutate(_comloc); // TODO: as structured, passing to self
}

void BLDC::commutate(uint8_t comPos) {
  _comloc = comPos % 6;

  // comPhaseTable[3][6][2]
  // comPhaseTable[phase][comPos][switch]

  _phase_u_lo = _duty * comPhaseTable[0][comPos][0];
  _phase_u_hi = _duty * comPhaseTable[0][comPos][1];
  _phase_v_lo = _duty * comPhaseTable[1][comPos][0];
  _phase_v_hi = _duty * comPhaseTable[1][comPos][1];
  _phase_w_lo = _duty * comPhaseTable[2][comPos][0];
  _phase_w_hi = _duty * comPhaseTable[2][comPos][1];

  this->t3pwm->setPhasesDirect(_phase_u_lo, _phase_u_hi, _phase_v_lo, _phase_v_hi, _phase_w_lo, _phase_w_hi);
}

