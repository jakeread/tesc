#include "foc.h"

FOC::FOC() {
  // initialize vars?
}

void FOC::init() {
  // phases
  _u = 0;
  _v = 0;
  _w = 0;
  // a, b current readings
  _aReading = 0;
  _bReading = 0;
  _aCommand = 0;
  _bCommand = 0;
  // encoder
  _encoder = 0;
  _theta = 0;
  _recession = PI / 8;
  // d, q axis readings
  _dReading = 0; // will need to be a ringbuffer type (to filter, do integrals, during PID)
  _qReading = 0;
  _dCommand = 0;
  _qCommand = 0;

  // outputs
  _uCommand = 0;
  _vCommand = 0;
  _wCommand = 0;
}

void FOC::doLoop(){ // runs on hz
  // now,
  // encoder -> dq command -> ab command -> u, v, w command -> pids
}

void FOC::doClarke(){ // does u, v, w -> a, b
  // not until current measurement impl.
}

void FOC::doInvClarke(){ // does a, b -> u, v, w (this is actually SVPWM's job) ?
  // using a, b, write u, v, w -
  _u = _aCommand; // and type changes?
  _v = -0.5*_aCommand + SQRT_TWO_OVER_THREE*_bCommand;
  _w = -0.5*_aCommand - SQRT_TWO_OVER_THREE*_bCommand;
  // this->svpwm->doVector(_a, _b);
}

void FOC::runDQPID(){
  // 
}

void FOC::doPark(){ // does a, b + encoder -> d, q;
    // wrong -> do direct encoder -> vector -> vector + phaseAdvance -> svpwm
    // do search for phase advance? auto setup closed loop?
    /*
  _encoder = KERNEL->as5047->getFiltered();
  _theta = (_encoder % MOTOR_MODULO) * MOTOR_MODULO_OVER_TWO_PI;
  _aReading = cos(_theta); // position vector, will be current vector in FOC, is no bc only encoder at the moment
  _bReading = sin(_theta);
  _dCommand = cos(_theta-_recession)*_bReading - sin(_theta-_recession)*_aReading; // quadrature
  _qCommand = sin(_theta-_recession)*_bReading) + cos(_theat-_recession)*_aReading; // direct, these are flipped in mag. bc we are direct-commutating
  */
}

void FOC::doInvPark(){ // does d, q + encoder -> a, b;
  //
}

