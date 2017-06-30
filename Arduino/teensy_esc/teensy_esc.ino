#include "motorleg.h"
#include "svm.h"
#include "bldc.h"
#include "encoder_as5047.h"

#include "config.h"

#define RUN_BLDC_DEBUG_LOOP     0
#define RUN_SVM_DEBUG_LOOP      0
#define RUN_ENCODER_LOOP        1
#define RUN_SVM_LOOP            0
#define RUN_BLDC_LOOP           1
#define IS_BBESC_GRINDER        0

#if RUN_BLDC_DEBUG_LOOP || RUN_SVM_DEBUG_LOOP
  IntervalTimer Debug_Timer;
#endif

#if RUN_ENCODER_LOOP
  AS5047 TAS5047;
  IntervalTimer AS5047_Timer;
#endif

#if RUN_SVM_LOOP || RUN_SVM_DEBUG_LOOP
  SVM TSVM(10, 9, 6, 5, 4, 3);
#endif

#if RUN_SVM_LOOP
  IntervalTimer SVM_Timer;
#endif

#if RUN_BLDC_LOOP || RUN_BLDC_DEBUG_LOOP
  BLDC TBLDC(10, 9, 6, 5, 4, 3);
#endif

#if RUN_BLDC_LOOP
  IntervalTimer BLDC_Timer;
#endif


void setup() {
  
  pinMode(13, OUTPUT);

  Serial.begin(115200);

  #if RUN_ENCODER_LOOP
    TAS5047.init();
    AS5047_Timer.begin(AS5047_Loop, 1000000/AS5047_LOOP_HZ); 
  #endif

  #if RUN_SVM_LOOP
    SVM_Timer.begin(SVM_Loop, 1000000/SVM_LOOP_HZ);
  #endif

  #if RUN_BLDC_LOOP
    BLDC_Timer.begin(BLDC_Loop, 1000000/BLDC_LOOP_HZ);       
  #endif

  #if RUN_SVM_DEBUG_LOOP || RUN_BLDC_DEBUG_LOOP
    Debug_Timer.begin(Debug_Loop, 1000000/DEBUG_LOOP_HZ); 
  #endif

  #if IS_BBESC_GRINDER
    pinMode(FWD_PIN, INPUT);
    pinMode(REV_PIN, INPUT);
  #endif
}

// ------------------------------------------------------- AS5047 LOOP
#if RUN_ENCODER_LOOP
void AS5047_Loop(){
  TAS5047.readNow();
}
#endif
// ------------------------------------------------------- SVM Loop
#if RUN_SVM_LOOP
int controlInput = 0;

void SVM_Loop(){
  digitalWrite(13, !digitalRead(13));
  
  controlInput = 824;

  if (controlInput < 512) {
    TSVM.dir(-1);
    TSVM.duty(map(controlInput, 512, 0, 0, 255));
    //Serial.println(map(controlInput, 512, 0, 0, 255));
  } else if (controlInput >= 512) {
    TSVM.dir(1);
    TSVM.duty(map(controlInput, 512, 1023, 0, 255));
    //Serial.println(map(controlInput, 512, 1023, 0, 255));
  }
  TSVM.loop(TAS5047.filteredInt());
}
#endif
// ------------------------------------------------------- SVM Debug Loop
#if RUN_SVM_DEBUG_LOOP
int cycleCounter = 0;
int filteredKeeper = 0;
double thetaKeeper = 0;
int immediateReading = 0;

void Debug_Loop(){
  digitalWrite(13, !digitalRead(13));

  if(cycleCounter % 12 == 0){

    noInterrupts();
    filteredKeeper = TAS5047.filteredInt();
    thetaKeeper = TSVM.getTheta();
    immediateReading = TAS5047  .mostRecent();
    interrupts();
    
    Serial.print("F:\t");
    Serial.print(filteredKeeper);
    Serial.print("\tI:\t");
    Serial.print(immediateReading);
    Serial.print("\tFM:\t");
    Serial.print(filteredKeeper % MOTOR_MODULO);
    
    Serial.print("\tT:\t");
    Serial.print(thetaKeeper);

    /*
    Serial.print("\tsetValA:\t");
    Serial.print(TSVM.getSetVal(0));
    TSVM.MLA->report();
    Serial.print("\tsetValB:\t");
    Serial.print(TSVM.getSetVal(1));
    TSVM.MLB->report();
    Serial.print("\tsetValC:\t");
    Serial.print(TSVM.getSetVal(2));
    TSVM.MLC->report();
    */
    Serial.println("");

  }
 
  TSVM.commutate(PI/48);  // moves theta along
  TSVM.duty(158);         // sets duty
  TSVM.assert();          // flips switches
  
  cycleCounter ++;
  
  
  // u kno the modulo
  // do modulo, print vals
  // avg all 0-com-cycle vals for 0-point
  // this is 'where it is' when encoder,
  // ++ // -- 90deg (pi/2 rads) (or less, for safety, #define a PHASE_ADVANCE) from this for commutate
}
#endif

// -------------------------------------------------------- BLDC LOOP
#if RUN_BLDC_LOOP

uint16_t dutyUser = 0; // what is threshold duty?
uint16_t dutyDished = 0;
uint16_t encoderFiltered = 0;
uint8_t currentCom = 0;
bool dir = 0;
uint32_t tick = 0;
uint32_t tickTimer = BLDC_LOOP_HZ / BLDC_REPORT_HZ;

void BLDC_Loop(){

  digitalWrite(13, !digitalRead(13));

  dutyUser = analogRead(DEBUG_POT_PIN);
  
  if (dutyUser < 512) {
    dir = 0;
    dutyDished = map(dutyUser, 512, 0, 110, 255); 
  } else if (dutyUser >= 512) {
    dir = 1;
    dutyDished = map(dutyUser, 512, 1024, 110, 255);
  } else {
    dutyDished = 0;
  }
  
  encoderFiltered = TAS5047.filteredInt();

  TBLDC.dir(dir);
  TBLDC.duty(dutyDished);

  if(tick % tickTimer == 0){
    Serial.println(""); // print 1st: i.e. at 'end' of last loop... so that spring has come out of motor
    Serial.print("\tIN:\t");
    Serial.print(dutyUser);
    Serial.print("\tDIR:\t");
    Serial.print(dir);
    Serial.print("\tDUTY:\t");
    Serial.print(dutyDished);
    Serial.print("\tENC:\t");
    Serial.print(encoderFiltered);
  }

  TBLDC.loop(encoderFiltered);       // NEEDS REWRITE

  tick ++;
}

#endif
// --------------------------------------------------------- FIN BLDC LOOP

// --------------------------------------------------------- BLDC DEBUG LOOP
#if RUN_BLDC_DEBUG_LOOP

// LOOP VARS

uint16_t dutyDirUser = 0; // what is threshold duty?
uint16_t dutyDirDished = 0;
uint16_t encoderFiltered = 0;
uint16_t encoderModulod = 0;
uint8_t currentCom = 0;
int dir = 0;

int dblCounter = 0; // COM cycles should not have to be counted, should be determined directly from encoder modulo location (with phase advance?)
uint16_t comCounter = 5;
int comDivisor = 20;

void Debug_Loop() {

  digitalWrite(13, !digitalRead(13));

  dutyDirUser = analogRead(DEBUG_POT_PIN);
  dutyDirDished = map(dutyDirUser, 0, 1024, 110, 255);

  if (dblCounter % comDivisor == 0) {

    TBLDC.duty(dutyDirDished);

    comCounter ++;
    TBLDC.commutate(comCounter % 6);

    delay(2000); // motor moves to comloc position, settles

    for(int i = 0; i < AS5047_AVERAGING; i ++){
      TAS5047.readNow();
      delay(1);
    }

    encoderFiltered = TAS5047.filteredInt();
    encoderModulod = encoderFiltered % MOTOR_MODULO;
  
    Serial.println(""); // print 1st: i.e. at 'end' of last loop... so that spring has come out of motor
    Serial.print("DU:\t");
    Serial.print(dutyDirUser);
    Serial.print("\tDD:\t");
    Serial.print(dutyDirDished);
    /*
    Serial.print("\tPAU:\t");
    Serial.print(phaseAdvanceUser);
    Serial.print("\tPAD:\t");
    Serial.print(phaseAdvanceDished);
    */
    Serial.print("\tDIR:\t");
    Serial.print(dir);
    Serial.print("\tEF:\t");
    Serial.print(encoderFiltered);
    Serial.print("\tEM:\t");
    Serial.print(encoderModulod);
    Serial.print("\tCOM:\t");
    Serial.print(comCounter % 6);
  }

  dblCounter ++;

}
#endif

// --------------------------------------------------------- FIN BLDC DEBUG LOOP

void loop() { 
}

