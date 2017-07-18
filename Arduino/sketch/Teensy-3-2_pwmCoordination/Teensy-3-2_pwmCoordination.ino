/*
 * TODO: 
 * - prototype transforms maths, implement
 * - kernel / modules, what where?
 */
#include "constants.h"
#include "t3pwm.h" // chip-specific implementations
#include "t3adc.h"

#include "svpwm.h"

T3ADC* theADC;
SVPWM* theSVPWM;

const uint8_t tickPin = 14;
const uint8_t ledPin = 13;
const bool on = HIGH;
const bool off = LOW;

void setup () {

  Serial.begin(115200);

  pinMode(tickPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  theSVPWM = new SVPWM();
  theADC = new T3ADC(PIN_SENSA_U, PIN_SENSA_V, PIN_SENSV_U, PIN_SENSV_V, PIN_SENSV_W);
  // initialize the digital pin as an output.


  theSVPWM->init();
  theADC->init();

}

unsigned long ctr = 0;

void loop (){
  digitalWriteFast(tickPin, on);
  ctr = 0;
  for(float p = 0; p < TWO_PI; p += PI/144){
    //theSVPWM->doAngular(p, 3.5);
    #define DEBUG_ADC_VALS
    #ifdef DEBUG_ADC_VALS
    if(!(ctr % 24)){
      Serial.print(theADC->RB_SENSA_U.latest());
      Serial.print("\t");
      Serial.println(theADC->RB_SENSA_V.latest());
    }
    #endif
    ctr ++;
    /*
    unsigned short u = 1024 + 1024*sin(p);
    unsigned short v = 1024 + 1024*sin(p - TWO_PI_OVER_THREE);
    unsigned short w = 1024 + 1024*sin(p + TWO_PI_OVER_THREE);
    Serial.print(u);
    Serial.print("\t");
    Serial.print(v);
    Serial.print("\t");
    Serial.print(w);
    Serial.println("\t");
    theSVPWM->directPhases(u, v, w);
    */
    delay(5);
  }
  digitalWriteFast(tickPin, off);
  delayMicroseconds(1);
}


void adc0_isr(void){
  digitalWriteFast(ledPin, on);
  if(ADC0_SC1A & ADC_SC1_COCO){ // status regitser & conversion & complete flag
    theADC->RB_SENSA_U.push((uint16_t)ADC0_RA); // adc result data register
  }
  digitalWriteFast(ledPin, off);
}

unsigned short result;

void adc1_isr(void){ // not an object f'n bc triggered in some depths of teensy core that I don't understand
  digitalWriteFast(tickPin, on);
  if(ADC1_SC1A & ADC_SC1_COCO){ // status regitser & conversion & complete flag
    theADC->RB_SENSA_V.push((uint16_t)ADC1_RA); // adc result data register
  }
  digitalWriteFast(tickPin, off);
}

