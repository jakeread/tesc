/*
 * TODO: 
 * - Speedy SVPWM Algorithm
 * - hotwire existing board, setup DAC channels etc, test svm on open-loop, read dac & display
 * - controller loop sketch w/ interrupts... detail, buffers, filtering, 
 * - prototype transforms maths, implement
 * - kernel / modules, what where?
 */

#include "t3pwm.h" // chip-specific implementations
#include "t3adc.h"

#include "svpwm.h"

T3ADC* theADC;
SVPWM* theSVPWM;

// Sin table normalized to amplitude of 600, 180 deg zero values, 2 degree angle resolution (to re-write when doing proper svpwm code)
const uint16_t sintable[] = {
  0, 21, 42, 63, 84, 104, 125, 145, 165, 185, 205, 225, 244, 263, 282, 300, 318, 336, 353, 369, 386, 401, 417, 432, 446, 460, 473, 485, 497, 509, 520,
  530, 539, 548, 556, 564, 571, 577, 582, 587, 591, 594, 597, 599, 600, 600, 600, 599, 597, 594, 591, 587, 582, 577, 571, 564, 556, 548, 539, 530, 520,
  509, 497, 485, 473, 460, 446, 432, 417, 401, 386, 369, 353, 336, 318, 300, 282, 263, 244, 225, 205, 185, 165, 145, 125, 104, 84, 63, 42, 21, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Phasenlage von u,v,w
byte u = 0;       // Phase U ->   0 Grad Referenz
byte v = 180 - 60; // Phase V -> 120 Grad später
byte w = 180 - 120; // Phase W -> 240 Grad später
int multiple = 3; // Spannungsabsenkung bei kleinen Drehzahlen

const uint8_t tickPin = 14;
const uint8_t ledPin = 13;
const bool on = HIGH;
const bool off = LOW;


IntervalTimer timer_Sin;    // 1 Sekunden Timer

void setup () {

  Serial.begin(115200);

  pinMode(tickPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  theSVPWM = new SVPWM();
  theADC = new T3ADC(PIN_SENSA_A, PIN_SENSA_B, PIN_SENSA_C, PIN_SENSV_A, PIN_SENSV_B, PIN_SENSV_C);
  // initialize the digital pin as an output.


  theSVPWM->init();
  theADC->init();

  timer_Sin.begin(TimerSin, 1000); //   Timer
}

void loop ()
{
  delay(50);
  uint16_t theLatestA;
  uint16_t theLatestB;
  noInterrupts();
  theLatestA = theADC->RB_SENSA_A.latest();
  theLatestB = theADC->RB_SENSA_B.latest();
  interrupts();
  Serial.print(theLatestA);
  Serial.print("\t");
  Serial.println(theLatestB);
}

void TimerSin()
{
  theSVPWM->directPhases(sintable[u]*multiple, sintable[v]*multiple, sintable[w]*multiple); // PWM-Tastverhältnis für Phase u setzen, Aplitude skalieren

  u++;
  if (u == 180) u = 0;
  v++;
  if (v == 180) v = 0;
  w++;
  if (w == 180) w = 0;

  //led = ~led;
  //digitalWrite(Trigger_01, led);
}

void adc0_isr(void){
  digitalWriteFast(ledPin, on);
  if(ADC0_SC1A & ADC_SC1_COCO){ // status regitser & conversion & complete flag
    theADC->RB_SENSA_A.push((uint16_t)ADC0_RA); // adc result data register
  }
  digitalWriteFast(ledPin, off);
}

unsigned short result;

void adc1_isr(void){ // not an object f'n bc triggered in some depths of teensy core that I don't understand
  digitalWriteFast(tickPin, on);
  if(ADC1_SC1A & ADC_SC1_COCO){ // status regitser & conversion & complete flag
    theADC->RB_SENSA_B.push((uint16_t)ADC1_RA); // adc result data register
  }
  digitalWriteFast(tickPin, off);
}

