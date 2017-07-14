#include "kernel.h"
#include "config.h"

#include "svm.h"
#include "bldc.h"
#include "encoder_as5047.h"

/*
 * timers, should perhaps belong to kernel, as we will want to be able to command-line change their hz rates?
 */

void setup() {
  Kernel* kernel = new Kernel();
  KERNEL->init();
}

void loop() { 
/*
 * most importantly, checks the Serial Buffer. 
 * this will run as often as possible ..
 * whenever other timers' functions and follow-throughs are finished
 */ 
  KERNEL->onMainLoop(); 
  // TODO: on teensy, large amounts of data become available very fast -> to re-write serial such that other interrupts don't get blocked when, say, 256 bytes show up at once!
}


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


