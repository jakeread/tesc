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
#include "sampler.h"
// -> adc
// communication
#include "sp.h"
#include "shell.h"
#include "streamer.h"
#include "sampler.h"

Kernel* Kernel::instance;

int Kernel::_loopCount = 0;

Kernel::Kernel() {

  instance = this;
  /*
      call constructors on modules
  */
  this->bldc = new BLDC(PIN_HI_A, PIN_LO_A, PIN_HI_B, PIN_LO_B, PIN_HI_C, PIN_LO_C);
  this->svm = new SVM(PIN_HI_A, PIN_LO_A, PIN_HI_B, PIN_LO_B, PIN_HI_C, PIN_LO_C);
  this->as5047 = new AS5047();
  this->sampler = new Sampler(PIN_SENSV_A, PIN_SENSV_B, PIN_SENSV_C, PIN_SENSA_A, PIN_SENSA_B, PIN_SENSA_C);

  this->sp = new SP(THE_BAUDRATE);
  this->shell = new Shell();
  this->streamer = new Streamer();
}

void Kernel::init() {
  pinMode(13, OUTPUT);
  /*
     call inits on all modules, in proper order, when necessary
  */

  this->sampler->init();
  this->as5047->init();
  this->sp->init();
  this->streamer->init();
  this->bldc->init();

#if GO_ON_STARTUP
  _Sample_Timer.begin(Kernel::onSampleLoop, 1000000/SAMPLE_DEFAULT_HZ); // as of yet unclear if all of this structure works, w/ complications due to static & non-static members
  _Sample_T_isRunning = true;
#else
  _Sample_T_isRunning = false;
#endif
  _Sample_T_hz = SAMPLE_DEFAULT_HZ;
  _Sample_Timer.priority(126);

#if GO_ON_STARTUP
  _Machine_Timer.begin(Kernel::onMachineLoop, 1000000/MACHINE_LOOP_DEFAULT_HZ); // TODO: wrap timers, put them in array to make "timers" command simpler
  _Machine_T_isRunning = true;
#else
  _Machine_T_isRunning = false;
#endif
  _Machine_T_hz = MACHINE_LOOP_DEFAULT_HZ;
  _Machine_Timer.priority(127);
}

void Kernel::onSampleLoop() {
  KERNEL->as5047->readNow();
  //KERNEL->sampler->readVNow();
  //KERNEL->sampler->readANow(); // TODO: this fires only on pwm pin on
}

void Kernel::onMachineLoop() {
#if IS_BLDC_MACHINE
  if (!(_loopCount % MACHINE_LOOP_SECONDARY) && KERNEL->bldc->getInputMode() == BLDC_INPUTMODE_POT) { // search for double -ve !0
    KERNEL->bldc->pot_input_update(); // update control variables TODO: this check to BLDC object
    KERNEL->flashLed();
    KERNEL->bldc->clcommutate();
  } else {
    KERNEL->bldc->clcommutate();
  }
#else
#if IS_FOC_MACHINE
  KERNEL->svm->loop();
#endif
#endif
  _loopCount ++;
}

void Kernel::onMainLoop() {
  this->sp->onMainLoop();
  // serial check
  // perhaps interval timer functions should be much shorter
  // and we do check-based operations here ?
}

void Kernel::stopSampleTimer() {
  _Sample_Timer.end();
  _Sample_T_isRunning = false;
}

void Kernel::stopMachineTimer() {
  _Machine_Timer.end();
  _Machine_T_isRunning = false;
}

void Kernel::startSampleTimer() {
  _Sample_Timer.begin(Kernel::onSampleLoop, 1000000 / _Sample_T_hz);
  _Sample_T_isRunning = true;
}

void Kernel::startMachineTimer() {
  _Machine_Timer.begin(Kernel::onMachineLoop, 1000000 / _Machine_T_hz);
  _Machine_T_isRunning = false;
}

bool Kernel::findEncoderReverse(int duty) {
  // kill timers & switches
  if (_Sample_T_isRunning) {
    this->stopSampleTimer();
  }
  if (_Machine_T_isRunning) {
    this->stopMachineTimer();
  }
  this->bldc->killAllPower();

  // set offsets & reverse to 0
  //this->as5047->setEncoderReverse(false);
  this->as5047->setEncoderOffset(0);

  int encoderResults[ENCODER_REVSEARCH_MULTIPLE][3]; // [result][com, raw, modulo]
  int com = 0;

  this->bldc->duty(duty);

  for (int i = 0; i < ENCODER_REVSEARCH_MULTIPLE; i ++) {
    com = i % 6;
    this->bldc->commutate(com);
    encoderResults[i][0] = com;
    Serial.print("commutate: ");
    Serial.print(com);
    delay(500);                              // spring restitution
    Serial.print("|\t");
    for (int s = 0; s < AS5047_AVERAGING; s++) {
      this->as5047->readNow();
      Serial.print("\t");
      Serial.print(this->as5047->mostRecent());
      delay(10);
    }
    Serial.print(" |\t\t");
    Serial.print(i);
    Serial.print(": ");
    encoderResults[i][1] = this->as5047->filteredInt();
    Serial.println(encoderResults[i][1]);
  }

  this->bldc->killAllPower();

  // ---------------------------------------------------- REVERSE ANALYSIS
  Serial.println("");
  int differences[ENCODER_REVSEARCH_LESS];              // one less val, bc we are differencing away
  
  // finding step between commutations
  Serial.print("differences:\t\t");
  for (int i = 0; i < ENCODER_REVSEARCH_LESS; i ++) {      // looping down
    differences[i] = encoderResults[i + 1][1] - encoderResults[i][1];
    Serial.print(i);
    Serial.print(": ");
    Serial.print(differences[i]);
    Serial.print(", ");
  }

  // excluding steps over-the-ring, and averaging
  Serial.println(" ");
  Serial.print("vals for avg:\t\t");
  int averageCount = ENCODER_REVSEARCH_LESS;   // the max # of values we could *potentially* have to average, we will be excluding cross-ring differences
  int averageComTicks = 0;
  for (int i = 0; i < ENCODER_REVSEARCH_LESS; i ++) { // discounting item [0], as, above, it never had a friend to difference from
    if (differences[i] > AS5047_RESOLUTION / 2) {
      averageComTicks += 0; // add nothing
      averageCount --;      // divide by 1 less
    } else {
      averageComTicks += differences[i];
      Serial.print(i);
      Serial.print(": ");
      Serial.print(differences[i]);
      Serial.print(", ");
    }
  }

  Serial.println("");
  averageComTicks /= averageCount;
  Serial.print("averageComTicks:\t");
  Serial.println(averageComTicks);
  bool reverseResult;
  if (averageComTicks > 0) {
    reverseResult = false;
  } else {
    reverseResult = true;
  }
  Serial.print("should reverse?\t\t");
  Serial.println(reverseResult);

  // ---------------------------------------------------- MODULO ANALYSIS
  Serial.println("");
  int moduloGuess = abs(averageComTicks) * 6;
  Serial.print("moduloGuess:\t\t");
  Serial.println(moduloGuess);
  int moduloSet[32][2];
  for (int i = 3; i < 35; i ++) {
    moduloSet[i][0] = AS5047_RESOLUTION / i;
    moduloSet[i][1] = abs(moduloSet[i][0] - moduloGuess); // separation from guess, will use this to choose
  }
  int moduloPick = 0;
  int bestDif = 100;
  for (int i = 3; i < 35; i ++) {
    if (moduloSet[i][1] < bestDif) {
      bestDif = moduloSet[i][1];
      moduloPick = moduloSet[i][0];
    }
  }
  Serial.print("moduloPick:\t\t");
  Serial.println(moduloPick);

  // ---------------------------------------------------- OFFSET ANALYSIS
  // now we can figure what the modulo-friendly offset value is
  Serial.println("");
  Serial.println("results array: ");

  int offsetGuess = 0;
  int offsetAverages = 0;
  for (int i = 0; i < ENCODER_REVSEARCH_MULTIPLE; i ++) {
    encoderResults[i][2] = encoderResults[i][1] % moduloPick;
    if (encoderResults[i][0] == 0) {
      offsetGuess += encoderResults[i][2];
      offsetAverages ++;
    }
    Serial.print(i);
    Serial.print(": ");
    Serial.print(encoderResults[i][0]);
    Serial.print("\t|\t");
    Serial.print(encoderResults[i][1]);
    Serial.print("\t|\t");
    Serial.println(encoderResults[i][2]);
  }

  Serial.println("");
  Serial.print(offsetGuess);
  Serial.print(" ");
  Serial.print(offsetAverages);
  offsetGuess /= offsetAverages;
  Serial.print("\t offsetGuess:\t\t");
  Serial.println(offsetGuess);

  // if reverse flip necessary, set reverse, run again
  
  return false;
}

uint16_t Kernel::findEncoderOffset(int duty) {
  //
}

void Kernel::flashLed() {
  digitalWrite(13, !digitalRead(13));
}

