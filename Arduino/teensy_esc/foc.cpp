#include "foc.h"
#include "kernel.h"
#include "as5047.h"
#include "svpwm.h"
#include "t3pwm.h"

#include "fastrig.h"
using namespace FASTRIG;

volatile long FOC::_loopCount = 0;
volatile float FOC::_p = 0;

FOC::FOC() {
  svpwm = new SVPWM();
  t3adc = new T3ADC();
  timer = new IntervalTimer();

  _id_err_int_rb = new RingBufferFloat();
  _iq_err_int_rb = new RingBufferFloat();
}

void FOC::init() {
  svpwm->init();
  t3adc->init();

  ftTableInit();

  _id_err_int_rb->clear();
  _iq_err_int_rb->clear();

  _i_scalar = ( V_REF / 2048 ) / ( CURRENT_SHUNT_RES * CURRENT_AMP_GAIN );

  _loopCount = 0;
  KERNEL->gateEnableOn();
}

//float mapfloat(float val, float fromLo, float fromHi, float toLo, float toHi){
//return ((val - fromLo) / (fromHi - fromLo)) * (toHi - toLo);
//}

#define KP_D          5.0f    // volts / amp(error)
#define KP_Q          5.0f
#define KI_D          0.01f
#define KI_Q          0.01f
#define K_LIMIT_INT   V_BUS/4
#define K_LIMIT_TOTAL V_BUS/2 // carefully


void FOC::onMainLoop() {
  // TODO: SWITCH (or in main, main?)
  KERNEL->flashLed();
  _loopCount ++;

  /*
     all sampling runs 'in the background' on ADC / FTM0 interrupts
     so at first we load the values from their ringbuffers into the variables we'll use during calcs
  */

  float _iv = (t3adc->rbav->latest() - CURRENT_AMP_OFFSET_V) * _i_scalar; // 1-2-3, 10us
  float _iw = (t3adc->rbaw->latest() - CURRENT_AMP_OFFSET_W) * _i_scalar;
  float _iu = -_iv - _iw;

  /*
     CURRENTLY:
     does closed loop voltage FOC
     implement DQ-framework, and do voltage FOC from there
     then insert PI controller inside DQ
  */

  KERNEL->as5047->readNow();
  float _elecpos = KERNEL->as5047->filteredInt() % MOTOR_MODULO;
  float _theta = (float)_elecpos / MOTOR_MODULO * TWO_PI; // this float conversion takes ~10us :/

  /*
     the Clarke transform takes these u, v, w current values and transforms them into an AB frame
  */
  float _ia = TWO_OVER_THREE * ( _iu - 0.5f * _iv - 0.5f * _iw );
  float _ib = TWO_OVER_THREE * ( SQRT_THREE_OVER_TWO * _iv - SQRT_THREE_OVER_TWO * _iw );

  /*
     now we do Parke, and get a read on current IQ currents. this requires the encoder value ... electrical position of the rotor
  */
  float _ptermsin = ftSin(_theta);
  float _ptermcos = ftCos(_theta);
  // d current, q current at-this-time is
  float _id = _ia * _ptermcos + _ib * _ptermsin;
  float _iq = _ib * _ptermcos - _ia * _ptermsin;

  /*
     then we do PID control (do this part last)
     CURRENTLY: units? when are they amps (targets should be, outputs, in volts... should be 0 - 1 for PWM, but ? p-term, i-term does conversion, basically?
  */
  float _id_ref = 0.0;            // target values
  float _iq_ref = 1.0;            // 1 amp on the quadrature axis

  float _id_err = _id_ref - _id;  // error
  _id_err_int_rb->push(_id_err);
  float _iq_err = _iq_ref - _iq;
  _iq_err_int_rb->push(_iq_err);

  /*
     currently:
     FIRST: AB / DQ Axis Alignment?
     INTEGRATING: need to ONLY pull ~0.5s of samples
     ALSO: need some load to actually hit any torque... like free-wheeling you will only ever hit ~0.4 amps, you need to be able to target ~3-4
     ALSO: this click, try different motors?
  */

  float _id_err_int;
  _id_err_int_rb->sum(&_id_err_int);
  float _iq_err_int;
  _iq_err_int_rb->sum(&_iq_err_int);

  float _id_term_p = _id_err * KP_D;
  float _id_term_int = _id_err_int * KI_D;
  float _iq_term_p = _iq_err * KP_Q;
  float _iq_term_int = _iq_err_int * KI_Q;

  float _comm_mag_int = abs(_id_term_int) + abs(_iq_term_int);
  if(_comm_mag_int > K_LIMIT_INT){
    _id_term_int = _id_term_int * K_LIMIT_INT / _comm_mag_int;
    _iq_term_int = _iq_term_int * K_LIMIT_INT / _comm_mag_int;
  }

  float _vd_comm = _id_term_p + _id_term_int;
  float _vq_comm = _iq_term_p + _iq_term_int;

  float _comm_mag_total = abs(_vd_comm) + abs(_vq_comm); //sqrt(_vd_comm * _vd_comm + _vq_comm * _vq_comm);
  if (_comm_mag_total > K_LIMIT_TOTAL) {
    _vd_comm = _vd_comm * K_LIMIT_TOTAL / _comm_mag_total;
    _vq_comm = _vq_comm * K_LIMIT_TOTAL / _comm_mag_total;
  }

  /*
     and go PID command vectors (in IQ space) -> clarke vectors (in AB space) -> UVW space -> phases, and the loop loops again!
    back to AB space, and normalize -V_BUS -> V_BUS towards -1 -> 1
  */
  float va = (_vd_comm * _ptermcos - _vq_comm * _ptermsin) / V_BUS;
  float vb = (_vd_comm * _ptermsin + _vq_comm * _ptermcos) / V_BUS;

  /*
     doing AB -> UVW
  */

  float vu = va;
  float vv = THREE_OVER_TWO * (SQRT_THREE_OVER_THREE * vb - va / 3.0);
  float vw = THREE_OVER_TWO * (-SQRT_THREE_OVER_THREE * vb - va / 3.0);

  uint16_t vpwmu_hi = 1024 + 1024 * vu;
  uint16_t vpwmv_hi = 1024 + 1024 * vv;
  uint16_t vpwmw_hi = 1024 + 1024 * vw;

  this->svpwm->t3pwm->setPhases(vpwmu_hi, vpwmv_hi, vpwmw_hi);

  /*
     END OF LOOP
  */
  /*
      currently:
      RIP SVPWM, long live SINUPWM
      -> do encoder-voltagemode FOC
  */

  if (!(_loopCount % MACHINE_LOOP_SECONDARY)) {
    int loopDistance = micros();
    float hz = 1000000.0 / ((loopDistance - _lastLoops) / MACHINE_LOOP_SECONDARY);
    Serial.print("pos:\t");
    Serial.print(_elecpos);
    Serial.print("\tid:\t");
    Serial.print(_id);
    Serial.print("\tiq:\t");
    Serial.print(_iq);
    Serial.print("\tid_err:\t");
    Serial.print(_id_err);
    Serial.print("\tiq_err:\t");
    Serial.print(_iq_err);
    Serial.print("\tid_e_int:\t");
    Serial.print(_id_err_int);
    Serial.print("\tiq_e_int:\t");
    Serial.print(_iq_err_int);
    Serial.print("\tvd_com:\t");
    Serial.print(_vd_comm);
    Serial.print("\tvq_comm:\t");
    Serial.print(_vq_comm);
    Serial.print("\thz:\t");
    Serial.print(hz);
    Serial.println("");
    _lastLoops = micros();
  }

  //#define ENCODERSETUP
#ifdef ENCODERSETUP
  KERNEL->as5047->readNow();
  uint16_t elecpos = KERNEL->as5047->filteredInt() % MOTOR_MODULO;
  float thetaReading = (float)elecpos / MOTOR_MODULO * TWO_PI;
  if (_p == 0) {
    Serial.print("zero: \t");
    Serial.print(elecpos);
    Serial.print("\t");
    Serial.println(_p, 3);
  }
  Serial.print("pos: \t");
  Serial.print(elecpos);
  Serial.print("\treading: \t");
  Serial.print(thetaReading, 2);
  Serial.print("\tcommand: ");
  Serial.print(_p, 2);
  Serial.print("\terror: ");
  Serial.println(_p - thetaReading, 2);
#endif

}


