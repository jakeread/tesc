#include "foc.h"
#include "kernel.h"
#include "as5047.h"
#include "t3pwm.h"

#include "fastrig.h"
using namespace FASTRIG;

volatile long FOC::_loopCount = 0;
volatile float FOC::_p = 0;

FOC::FOC() {
  t3pwm = new T3PWM();
  t3adc = new T3ADC();

  analogWriteResolution(12);

  _id_err_int_rb = new RingBufferFloat();
  _iq_err_int_rb = new RingBufferFloat();
}

void FOC::init() {
  t3pwm->init(); // TODO: just T3PWM, pls
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

#define KP_D          6.0f    // volts / amp(error)
#define KP_Q          6.0f
#define KI_D          0.1f
#define KI_Q          0.1f
#define K_LIMIT_INT   V_BUS/4
#define K_LIMIT_TOTAL V_BUS/2 // carefully


void FOC::onMainLoop() {
  KERNEL->flashLed();
  _loopCount ++;

  /*
   * CURRENT MEASUREMENT
     all sampling runs 'in the background' on ADC / FTM0 interrupts
     so at first we load the values from their ringbuffers into the variables we'll use for the duration of the loop
  */
  float _iv = (t3adc->rbav->latest() - CURRENT_AMP_OFFSET_V) * _i_scalar; // 1-2-3, 10us
  float _iw = (t3adc->rbaw->latest() - CURRENT_AMP_OFFSET_W) * _i_scalar;
  float _iu = -_iv - _iw;

  /*
   * POSITION MEASUREMENT
   * we sample this 'directly' by talking to the encoder class. this triggers SPI footwork to read from the encoder.
   * inside the encoder class we do some pretty low-tech filtering, and we use a filtered output rather than the direct reading (which is noisy!)
   * TODO: in other versions, the encoder sampled in the background on a timer at ~30kHz, and then samples were loaded out of a ringbuffer a-la the 
   * current sensing above. I think this might be a better solution, and it allows for better filtering.
   * Good position data seems critical.
   */
  KERNEL->as5047->readNow();
  float _elecpos = KERNEL->as5047->filteredInt() % MOTOR_MODULO;
  float _theta = (float)_elecpos / MOTOR_MODULO * TWO_PI; // this float conversion takes ~10us :/

  /*
     the Clarke transform takes these u, v, w current values and transforms them into the static AB frame
  */
  float _ia = TWO_OVER_THREE * ( _iu - 0.5f * _iv - 0.5f * _iw );
  float _ib = TWO_OVER_THREE * ( SQRT_THREE_OVER_TWO * _iv - SQRT_THREE_OVER_TWO * _iw );

  /*
     now we do a Parke transform, which results in a current measurement in the DQ frame that rotates along with the rotor. 
     note on electric vs. mechanical rotor-position:
     _theta is the 'electric' position of the rotor, as in how far long one phase-cycle it is. for example, if a motor had only two poles,
     one mechanical revolution would be equal to one electrical revolution, or phase. if a motor has four poles, it would make two electric
     revolutions per complete mechanical revolution, etc. high torque motors tend to have high pole counts (and high speed motors, low pole counts)
  */
  float _ptermsin = ftSin(_theta);
  float _ptermcos = ftCos(_theta);
  // d current, q current at-this-time is
  float _id = _ia * _ptermcos + _ib * _ptermsin;
  float _iq = _ib * _ptermcos - _ia * _ptermsin;

  /*
     then we do PID control
     we want all of the torque on the 'quadrature' axis, the Q axis. 
     because we've done this in a DQ frame, which rotates along with the rotor, we've abstracted away the complications of rotating target-vectors
     and we can write a PI control loop that will use a Voltage Output to control the current flowing through the phases.
  */
  float _id_ref = 0.0;            // target values
  float _iq_ref = 1.0;            // 1 amp on the quadrature axis

  float _id_err = _id_ref - _id;  // error -> how far, currently, are we from the target?
  _id_err_int_rb->push(_id_err);  // adding this error to the integral ring buffer, where we keep our past mistakes (haha)
  float _iq_err = _iq_ref - _iq;
  _iq_err_int_rb->push(_iq_err);

  float _id_err_int;
  _id_err_int_rb->sum(&_id_err_int);  // summing the past errors -> this is just a super simple way of 'integrating' over some chunk of the controller's history.
  float _iq_err_int;
  _iq_err_int_rb->sum(&_iq_err_int);

  float _id_term_p = _id_err * KP_D;        // KP and KI (one for each axis) are somewhat 'arbitrary' values... as in, no hard and fast units.
  float _id_term_int = _id_err_int * KI_D;  // technically they are like 'volts (of control output) per amp (of error) - but in reality these just get the heck tuned out of them until they work.
  float _iq_term_p = _iq_err * KP_Q;
  float _iq_term_int = _iq_err_int * KI_Q;

  float _comm_mag_int = abs(_id_term_int) + abs(_iq_term_int); // then we limit the total I-term output, so that we avoid runaway conditions where the controller decides that it's time to blow some fets
  if(_comm_mag_int > K_LIMIT_INT){
    _id_term_int = _id_term_int * K_LIMIT_INT / _comm_mag_int;
    _iq_term_int = _iq_term_int * K_LIMIT_INT / _comm_mag_int;
  }

  float _vd_comm = _id_term_p + _id_term_int;
  float _vq_comm = _iq_term_p + _iq_term_int;

  float _comm_mag_total = abs(_vd_comm) + abs(_vq_comm); //sqrt(_vd_comm * _vd_comm + _vq_comm * _vq_comm); // limit total control output, as above, no blown electronics please
  if (_comm_mag_total > K_LIMIT_TOTAL) {
    _vd_comm = _vd_comm * K_LIMIT_TOTAL / _comm_mag_total;
    _vq_comm = _vq_comm * K_LIMIT_TOTAL / _comm_mag_total;
  }

  /*
    coming back to ground-truth reality, we bring the rotating-frame vector back into static-frame AB space.
    notice I am using the previously-computed sin and cos terms
  */
  float va = _vd_comm * _ptermcos - _vq_comm * _ptermsin;
  float vb = _vd_comm * _ptermsin + _vq_comm * _ptermcos;

  /*
     and right on the metal, here we are taking these AB terms and plotting them back to the three phases, UVW
  */
  float vu = va;
  float vv = THREE_OVER_TWO * (SQRT_THREE_OVER_THREE * vb - va / 3.0);
  float vw = THREE_OVER_TWO * (-SQRT_THREE_OVER_THREE * vb - va / 3.0);

  /*
   * and writing those values to the PWM registers. notice we're 'normalizing' vu, vv, vw, to -1 -> 1, whereas previously they represented 'real' voltages
   */
  uint16_t vpwmu_hi = 1024 + 1024 * ( vu / V_BUS );
  uint16_t vpwmv_hi = 1024 + 1024 * ( vv / V_BUS );
  uint16_t vpwmw_hi = 1024 + 1024 * ( vw / V_BUS );

  t3pwm->setPhases(vpwmu_hi, vpwmv_hi, vpwmw_hi);

  /*
     END OF LOOP
  */

  /*
   * Reporting and Tracking
   * Using DAC to direct-to-scope current tracking...
   */
  float report = _iq * 2046 + 2048; // range is -1 -> 1
  if(report > 4096){
    report = 4096;
  } else if (report < 1){
    report = 1;
  }
  
  analogWrite(A14, (int)report);

  if (!(_loopCount % MACHINE_LOOP_SECONDARY)) {
    int loopDistance = micros();
    float hz = 1000000.0 / ((loopDistance - _lastLoops) / MACHINE_LOOP_SECONDARY);
    Serial.print(_iq);
    Serial.print("\thz:\t");
    Serial.println(hz);
    /*
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
    */
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


