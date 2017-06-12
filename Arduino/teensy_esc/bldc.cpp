// does straightforward BLDC commutation

#include "bldc.h"

BLDC::BLDC(int pinHiA, int pinLoA, int pinHiB, int pinLoB, int pinHiC, int pinLoC){

  analogWriteResolution(LEG_PWMRES);
  // 8-bit analog (pwm) write resolution // values 0-255
  // & maching freq as per https://www.pjrc.com/teensy/td_pulse.html set in motorleg
  
  MLA = new MotorLeg(pinHiA, pinLoA); // TODO: rename to phases U V M
  MLB = new MotorLeg(pinHiB, pinLoB);
  MLC = new MotorLeg(pinHiC, pinLoC);

  _duty = 125;
  _dir = 1;

  _comloc = 0;
  _lastcom = micros();

  for(int i = 0; i < 7; i ++){
    _czc[i] = i*(MOTOR_MODULO / 6);
  }
  
}

void BLDC::init(){
}

void BLDC::prntCzc(){
  for(int i = 0; i < 7; i ++){
    Serial.print("_czc");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(_czc[i]);
    Serial.println(" ");
  }
  Serial.println(" ");
}

void BLDC::duty(int duty){
  _duty = duty;
}

void BLDC::dir(bool dir){
  _dir = dir;
}

void BLDC::advance(int advance){
  _advance = advance;
}


void BLDC::loop(uint16_t posNow){
  /*
   * OK: Have a Duty-Cycle (eventually Torque) setting, and a direction setting
   * SO: get a rolling-averaged position estimate (noInterrupts? -> or rollingAverage doesn't use most-recent value, so if it's in the process of being written we don't f...)
   * Check comm table / simple math: where are we in comm table? encoder resolution / poles % 6 ?
   * Set PWM's according
   */

  /*
   * Do Modulo: for splitting Encoder (0-AS5047_RESOLUTION Physical Period, into 0-BLDC_MODULO Electrical Period)
   */
  _posNow = posNow;
  
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
   *  Needs to update zone-values per pole-number
   */

  if(_modulo >= _czc[0] && _modulo < _czc[1]){
    _comZone = 0;
  } else if (_modulo >= _czc[1] && _modulo < _czc[2]){
    _comZone = 1;
  } else if (_modulo >= _czc[2] && _modulo < _czc[3]){
    _comZone = 2;
  } else if (_modulo >= _czc[3] && _modulo < _czc[4]){
    _comZone = 3;
  } else if (_modulo >= _czc[4] && _modulo < _czc[5]){
    _comZone = 4;
  } else if (_modulo >= _czc[5] && _modulo <= _czc[6]){
    _comZone = 5;
  }

  /*
  Serial.print(" comZone: ");
  Serial.print(comZone);
  */
  
  if(_dir == 1){
    _comCommand = _comZone + 2; // there is some falsity in this nomenclature: offset is not measured correctly atm
    if(_comCommand == 6){
      _comCommand = 0;
    }
    if(_comCommand == 7){
      _comCommand = 1;
    }
    if(_comCommand == 8){
      _comCommand = 2;
    }
  }
  
  if(_dir == 0){
    _comCommand = _comZone - 2;
    if(_comCommand == -1){
      _comCommand = 5;
    }
    if(_comCommand == -2){
      _comCommand = 4;
    }
    if(_comCommand == -3){
      _comCommand = 3;
    }
  }

  /*
  Serial.print(" comCommand: ");
  Serial.print(comCommand);
  
  Serial.println(" ");
  */
  
  commutate(_comCommand);

}

void BLDC::commutate(uint8_t comPos){

  /*
  _comloc += _dir;
  if(_comloc > 5){
    _comloc = 0;
  }
  if(_comloc < 0){
    _comloc = 5;
  }
  */

  _comloc = comPos % 6;

  MLA->set(_duty, comtable[_comloc][0]);
  MLB->set(_duty, comtable[_comloc][1]);
  MLC->set(_duty, comtable[_comloc][2]);
  
}

