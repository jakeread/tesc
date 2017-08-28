#include "kernel.h"
#include "config.h"
/*
   includes for all modules the Kernel will include
*/
// motor modes
#include "bldc.h"
#include "foc.h"

// hardware wrappers
#include "as5047.h"
#include "vsens.h"

// communication
#include "sp.h"
#include "shell.h"

Kernel* Kernel::instance;

int Kernel::_loopCount = 0;

Kernel::Kernel() {

  instance = this;

  #if IS_BLDC_MACHINE
  this->bldc = new BLDC();
  #else if IS_FOC_MACHINE
  this->foc = new FOC();
  #endif;

  this->as5047 = new AS5047();
  this->vsens = new VSens(); // holds ringbuffers for sampl'd adcs

  this->sp = new SP();
  this->shell = new Shell();
}

void Kernel::init() {

  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT); // gate enable
  /*
     call inits on all modules, in proper order, when necessary
  */
  this->as5047->init();
  this->vsens->init();
  
  this->sp->init();
  this->shell->init();
  
  #if IS_BLDC_MACHINE
  this->bldc->init();
  #else if IS_FOC_MACHINE
  this->foc->init();
  #endif

}

void Kernel::onMainLoop() {
  this->sp->onMainLoop(); // serial line-check
  #if IS_FOC_MACHINE
  this->foc->onMainLoop();
  #else if IS_BLDC_MACHINE
  this->bldc->onMainLoop();
  #endif
}

void Kernel::gateEnableOff(){
  digitalWriteFast(2, LOW);
}

void Kernel::gateEnableOn(){
  digitalWriteFast(2, HIGH);
}

void Kernel::flashLed() {
  digitalWriteFast(13, !digitalRead(13));
}

int Kernel::simpleInput(){
  return analogRead(A11); // WARNING this crashes system when ADC's for FOC are initiated 
}

bool Kernel::encoderValueSearch(int duty) {
  // kill timers & switches
  this->bldc->killAllPower();

  // set offsets & reverse to 0
  //this->as5047->setEncoderReverse(false);
  this->as5047->setEncoderOffset(0);

  int encoderResults[ENCODER_REVSEARCH_MULTIPLE][3]; // [result][com, raw, modulo]
  int com = 0;

  this->bldc->duty(duty);
  this->bldc->wakeUp();

  for (int i = 0; i < ENCODER_REVSEARCH_MULTIPLE; i ++) {
    com = i % 6;
    this->bldc->commutate(com);
    encoderResults[i][0] = com;
    Serial.print("commutate: ");
    Serial.print(com);
    delay(1000);                              // spring restitution
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
  Serial.print("less 1/12 modulo -> centers zones: ");
  Serial.println(offsetGuess - moduloPick / 12);

  // if reverse flip necessary, set reverse, run again
  
  return false;
}


