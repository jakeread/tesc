#include "svpwm.h"
#include "t3pwm.h"
#include "constants.h"

SVPWM::SVPWM() {
  //
}

void SVPWM::init() {
  this->t3pwm = new T3PWM();
  this->t3pwm->init();
}

/*
   theta = angle-from-u-phase (0 -> 2PI)
   mag = |VREF|, 0 -> maxv
*/

int SVPWM::doAngular(double theta, double mag) {
  this->doVector((mag * cos(theta)), (mag * sin(theta)));
  return 0;
}

/*
   a, b, componenets of vector VREF ! real #'s, corresponding to voltage delivered to phase
*/
int SVPWM::doVector(double a, double b) {
  _a = a; // TODO: CHECK VALS
  _b = b;

  _sector = 0; // TODO: need this? could leave floating
  _theta = 0;
  _omega = 0;

  /*
     get theta, need this to determine sector, timing-within-sector
  */
  if (_b >= 0) {
    _theta = PI / 2 - atan(_a / _b);
  } else {
    _theta = THREE_PI / 2 - atan(_a / _b);
  }

  if (_theta > PI) {
    if (_theta > FIVE_PI_OVER_THREE) {
      _sector = 5;
      _omega = _theta - FIVE_PI_OVER_THREE;
    } else {
      if (_theta > FOUR_PI_OVER_THREE) {
        _sector = 4;
        _omega = _theta - FOUR_PI_OVER_THREE;
      } else {
        _sector = 3;
        _omega = _theta - PI;
      }
    }
  } else { // sec 1 - 3
    if (_theta > TWO_PI_OVER_THREE) {
      _sector = 2;
      _omega = _theta - TWO_PI_OVER_THREE;
    } else {
      if (_theta > PI_OVER_THREE) {
        _sector = 1;
        _omega = _theta - PI_OVER_THREE;
      } else {
        _sector = 0;
        _omega = _theta;
      }
    }
  }

  _mag = sqrt(a*a + b*b);

  _tz_a = TZ*(_mag/TWO_OVER_THREE_VDC);

  _t1 = _tz_a * (sin(PI_OVER_THREE - _omega) / SIN_PI_OVER_THREE);
  _t2 = _tz_a * (sin(_omega) / SIN_PI_OVER_THREE);
  _t0 = TZ - (_t1 + _t2);

  _sa = _t1 + _t2 + _t0/2; 
  _sb1 = _t2 + _t0/2;
  _sb2 = _t1 + _t0/2;
  _sc = _t0/2;

  switch (_sector){
    case 0:
      _tu = _sa;
      _tv = _sb1;
      _tw = _sc;
      break;
    case 1:
      _tu = _sb2;
      _tv = _sa;
      _tw = _sc;
      break;
    case 2:
      _tu = _sc;
      _tv = _sa;
      _tw = _sb1;
      break;
    case 3:
      _tu = _sc;
      _tv = _sb2;
      _tw = _sa;
      break;
    case 4:
      _tu = _sb1;
      _tv = _sc;
      _tw = _sa;
      break;
    case 5:
      _tu = _sa;
      _tv = _sc;
      _tw = _sb2;
      break;
    default:
      // TODO: SHUTDOWN / EMERGY CODE for errors!
      _tu = 0;
      _tv = 0;
      _tw = 0;
  }

  //#define DO_HELLA_PRINT
  #ifdef DO_HELLA_PRINT
  Serial.print("_omega: ");
  Serial.print(_omega);
  Serial.print("\t_sector: ");
  Serial.print(_sector);
  Serial.print("\t_mag: ");
  Serial.print(_mag);
  Serial.print("\t|| t0:\t");
  Serial.print(_t0);
  Serial.print("\tt1:\t");
  Serial.print(_t1);
  Serial.print("\tt2:\t");
  Serial.print(_t2);
  Serial.print("\t|| tu:\t");
  Serial.print(_tu);
  Serial.print("\ttv:\t");
  Serial.print(_tv);
  Serial.print("\ttw:\t");
  Serial.print(_tw);
  Serial.println("");
  #endif

  // do current sampling serial print?
  // do dq(encoder, get dq ref?) -> etc ... draw your loop, do no-pid test w/ just encoder reading / interrupt & using all transforms, but w/o PID controller, just desiredAVector->Vvector

  this->t3pwm->setPhases(_tu, _tv, _tw);
  
  /*
    if (_b >= 0) {
    if (abs(_a) > abs(_b / SQRT3)) {
      if (_a >= 0) {
        _sector = 1;
      } else {
        _sector = 3;
      }
    } else {
      _sector = 2;
    }
    } else {
    if (abs(_a) > abs(_b / SQRT3)) {
      if (_a >= 0) {
        _sector = 6;
      } else {
        _sector = 4;
      }
    } else {
      _sector = 5;
    }
    }
  */

  //
  return 0;
}

int SVPWM::directPhases(int u, int v, int w) {
  _tu = u; // TODO: CHECK VALS
  _tv = v;
  _tw = w;
  this->t3pwm->setPhases(_tu, _tv, _tw);
  return 1;
}

int SVPWM::killPhases() {
  //
  return 0;
}

