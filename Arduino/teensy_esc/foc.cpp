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

  rbfau = new RingBufferFloat();
  rbfav = new RingBufferFloat();
  rbfaw = new RingBufferFloat();
}

void FOC::init() {
  this->svpwm->init();
  this->t3adc->init();

  ftTableInit();

  _loopCount = 0;
  KERNEL->gateEnableOn();
}

float magmagmag = 0.15;
//float mapfloat(float val, float fromLo, float fromHi, float toLo, float toHi){
//return ((val - fromLo) / (fromHi - fromLo)) * (toHi - toLo);
//}

void FOC::onMainLoop() {
  // TODO: SWITCH (or in main, main?)
  KERNEL->flashLed();
  _loopCount ++;

  /*
     all sampling runs 'in the background' on ADC / FTM0 interrupts
     so at first we load the values from their ringbuffers into the variables we'll use during calcs
  */

  rbfav->push((this->t3adc->rbav->latest() - CURRENT_AMP_OFFSET_V) * (V_REF / 2048) / (CURRENT_SHUNT_RES * CURRENT_AMP_GAIN)); // 1-2-3, 10us
  rbfaw->push((this->t3adc->rbaw->latest() - CURRENT_AMP_OFFSET_W) * (V_REF / 2048) / (CURRENT_SHUNT_RES * CURRENT_AMP_GAIN));
  rbfau->push(-rbfav->latest() - rbfaw->latest());

  //KERNEL->as5047->readNow();

  _iu = rbfau->latest();
  _iv = rbfav->latest();
  _iw = rbfaw->latest();

 KERNEL->as5047->readNow();
  uint16_t elecpos = KERNEL->as5047->filteredInt() % MOTOR_MODULO;
  float thetaReading = (float)elecpos / MOTOR_MODULO * TWO_PI;
  float thetaTarget = thetaReading + PI_OVER_TWO; // can blindly do this b/c ftSin & ftCos will adjust / set back in bounds

  /*
     the Clarke transform takes these u, v, w current values and transforms them into an AB frame
  */
  doClarke();

  /*
     now we do Parke, and get a read on current IQ currents. this requires the encoder value ... electrical position of the rotor
  */

  /*
     then we do PID control (do this part last)
  */

  /*
     and go PID command vectors (in IQ space) -> clarke vectors (in AB space) -> UVW space -> phases, and the loop loops again!
  */

  /*
     at the moment, we are blind commutating w/ svpwm
  */

  /*
  _p += 0.1;
  if (_p > TWO_PI) {
    _p = 0;
  }
  */

  float va = ftCos(thetaTarget) * magmagmag;
  float vb = ftSin(thetaTarget) * magmagmag;

  /*
     doing AB -> UVW
  */

  float vu = va;
  float vv = THREE_OVER_TWO * (SQRT_THREE_OVER_THREE * vb - va / 3.0);
  float vw = THREE_OVER_TWO * (-SQRT_THREE_OVER_THREE * vb - va / 3.0);

  uint16_t vpwmu = 2048 + 2048 * vu;
  uint16_t vpwmv = 2048 + 2048 * vv;
  uint16_t vpwmw = 2048 + 2048 * vw;

  this->svpwm->t3pwm->setPhases(vpwmu, vpwmv, vpwmw);

  //delay(100);

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
  /*
      currently:
      RIP SVPWM, long live SINUPWM
      -> do encoder-voltagemode FOC
  */

  if (!(_loopCount % MACHINE_LOOP_SECONDARY)) {
    int loopDistance = micros();
    float hz = 1000000.0 / ((loopDistance - _lastLoops) / MACHINE_LOOP_SECONDARY);
    //magmagmag = mapfloat(KERNEL->simpleInput(), 0.0, 1024.0, 0.0, 0.25);
    //Serial.print("\t enc: ");
    //Serial.print(KERNEL->as5047->filtered());
    //Serial.println("");
    /*
      Serial.print("\tiu:\t");
      Serial.print(_iu);
      Serial.print("\tiv:\t");
      Serial.print(_iv);
      Serial.print("\tiw:\t");
      Serial.print(_iw);
      Serial.print("\tiafc:\t");
      Serial.print(_ia);
      Serial.print("\tibfc:\t");
      Serial.print(_ib);
    */
    Serial.print("\tva:\t");
    Serial.print(va);
    Serial.print("\tvb:\t");
    Serial.print(vb);
    Serial.print("\tvu:\t");
    Serial.print(vpwmu);
    Serial.print("\tvv:\t");
    Serial.print(vpwmv);
    Serial.print("\tvw:\t");
    Serial.print(vpwmw);
    Serial.print("\thz:\t");
    Serial.print(hz);
    Serial.println("");
    _lastLoops = micros();
  }
}

void FOC::doClarke() {
  _ia = TWO_OVER_THREE * _iu - ONE_OVER_THREE * _iv - ONE_OVER_THREE * _iw;
  _ib = ONE_OVER_SQRT_THREE * _iv - ONE_OVER_SQRT_THREE * _iw;
}


