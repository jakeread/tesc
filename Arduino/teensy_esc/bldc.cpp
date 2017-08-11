// does straightforward BLDC commutation
#include "bldc.h"
#include "kernel.h"
#include "as5047.h"

BLDC::BLDC() {

  analogWriteResolution(LEG_PWMRES);
  // 8-bit analog (pwm) write resolution // values 0-255
  // & maching freq as per https://www.pjrc.com/teensy/td_pulse.html set in motorleg

  MLU = new MotorLeg(PIN_HI_U, PIN_LO_U); // TODO: rename to phases U V M
  MLV = new MotorLeg(PIN_HI_V, PIN_LO_V);
  MLW = new MotorLeg(PIN_HI_W, PIN_LO_W);

  _duty = 125;
  _dir = 1;

  _comloc = 0;
  _lastcom = micros();

  for (int i = 0; i < 7; i ++) {
    _czc[i] = i * (MOTOR_MODULO / 6);
  }

}

void BLDC::init() {
  _inputMode = BLDC_INPUTMODE_POT;
}

bool BLDC::killAllPower() {
  MLU->kill();
  MLV->kill();
  MLW->kill();
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
  if (duty > 0 && duty < 256) {
    _duty = duty;
    return true;
  } else {
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

void BLDC::pot_input_update() {
  _dutyUser = analogRead(DEBUG_POT_PIN);
  if (_dutyUser < 512) {
    this->dir(0);
    this->duty(map(_dutyUser, 512, 0, 110, 255));
  } else if (_dutyUser >= 512) {
    this->dir(1);
    this->duty(map(_dutyUser, 512, 1024, 110, 255));
  } else {
    this->duty(0);
  }
}

void BLDC::clcommutate() {
  /*
     OK: Have a Duty-Cycle (eventually Torque) setting, and a direction setting
     SO: get a rolling-averaged position estimate (noInterrupts? -> or rollingAverage doesn't use most-recent value, so if it's in the process of being written we don't f...)
     Check comm table / simple math: where are we in comm table? encoder resolution / poles % 6 ?
     Set PWM's according
  */

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
  if (_dir == 0) {
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

void BLDC::commutate(uint8_t comPos) {

  _comloc = comPos % 6;

  MLU->set(_duty, comtable[_comloc][0]);
  MLV->set(_duty, comtable[_comloc][1]);
  MLW->set(_duty, comtable[_comloc][2]);

}

