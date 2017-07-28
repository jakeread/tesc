/*
 * does foc, associated transforms
 * gets encoder
 * gets u, v currents
 * does u, v, w currents
 * does u, v, w -> a, b currents (clarke transform)
 * does a, b -> d, q currents (encoder data, park transform)
 * does PID on d, q currents
 * does d, q commands -> a, b commands (inverse park transform)
 * does a, b commands -> u, v, w PID commands (inverse clarke transform)
 */

#include <Arduino.h>
#include "config.h"
#include "constants.h"
#include "svpwm.h"

class FOC{
  public:

  FOC();

  void init();
  void doLoop(); // fire from ADC conversion-complete interrupt, every / other depending on hz of pwm ?

  private:
  // phase current values
  int32_t _u; // and are offset to -ve, +ve from mid-points ?
  int32_t _v;
  int32_t _w;
  void doWCalc(); // u+v+w = 0;
  // clarke
  void doClarke(); // acts on internal vars
  void doInvClarke();
  // a, b current readings
  double _aReading;
  double _bReading;
  double _aCommand;
  double _bCommand;
  // encoder
  uint16_t _encoder;  // raw digits
  double _theta;      // do modulo, do ticks -> electric degrees
  double _recession;  // back off on quadrature command, so that no accidental reversal
  // park
  void doPark();
  void doInvPark();
  // d, q axis readings
  double _dReading; // will need to be a ringbuffer type (to filter, do integrals, during PID)
  double _qReading;
  double _dCommand;
  double _qCommand;
  // pid
  void runDQPID();
  // outputs
  uint16_t _uCommand;
  uint16_t _vCommand;
  uint16_t _wCommand;
};

