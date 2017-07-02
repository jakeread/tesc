#include "kernel.h"
#include "config.h"
/*
   includes for all modules the Kernel will include
*/
// motor control
#include "bldc.h"
#include "svm.h"
// sampling
#include "encoder_as5047.h"
// -> adc
// communication
#include "sp.h"
#include "shell.h"
#include "streamer.h"

Kernel* Kernel::instance;

int Kernel::_loopCount = 0;

Kernel::Kernel() {

  instance = this;

  /*
      call constructors on modules
  */
  this->bldc = new BLDC(10, 9, 6, 5, 4, 3);
  this->svm = new SVM(10, 9, 6, 5, 4, 3);
  this->as5047 = new AS5047();

  this->sp = new SP(THE_BAUDRATE);
  this->shell = new Shell();
  this->streamer = new Streamer();
}

void Kernel::init() {
  pinMode(13, OUTPUT);
  /*
     call inits on all modules, in proper order, when necessary
  */
  
  this->as5047->init();
  this->sp->init();
  this->streamer->init();
  this->bldc->init();

  _Sample_Timer.begin(Kernel::onSample, 1000000/SAMPLE_DEFAULT_HZ); // as of yet unclear if all of this structure works, w/ complications due to static & non-static members
  _Sample_T_isRunning = true;
  _Sample_T_hz = SAMPLE_DEFAULT_HZ;
  _Sample_Timer.priority(126);

  _Machine_Timer.begin(Kernel::onMachineLoop, 1000000/MACHINE_LOOP_DEFAULT_HZ); // TODO: wrap timers, put them in array to make "timers" command simpler
  _Machine_T_isRunning = true;
  _Machine_T_hz = MACHINE_LOOP_DEFAULT_HZ;
  _Machine_Timer.priority(127);
}

void Kernel::onSample() {
  KERNEL->as5047->readNow();
}

void Kernel::onMachineLoop() {
#if IS_BLDC_MACHINE
  if(!(_loopCount % MACHINE_LOOP_SECONDARY) && KERNEL->bldc->getInputMode() == BLDC_INPUTMODE_POT){ // search for double -ve !0 
    KERNEL->bldc->pot_input_update(); // update control variables TODO: this check to BLDC object
    KERNEL->flashLed();
    KERNEL->bldc->clcommutate();
  } else {
    KERNEL->bldc->clcommutate();
  }
#else if IS_FOC_MACHINE
  KERNEL->svm->loop();
#endif
  _loopCount ++;
}

void Kernel::onMainLoop() {
  this->sp->onMainLoop();
  // serial check
  // perhaps interval timer functions should be much shorter
  // and we do check-based operations here ?
}

void Kernel::flashLed() {
  digitalWrite(13, !digitalRead(13));
}

