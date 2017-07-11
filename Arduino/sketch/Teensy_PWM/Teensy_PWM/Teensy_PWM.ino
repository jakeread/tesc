/*
BLDC Test Programm
Only Signal generation
Sinwave 6Phase Signal
PWM-Brake

signals pins:
pin 22 channel 00 phase U-high
pin 23 channel 01 phase U-low
pin 09 channel 02 phase V-high
pin 10 channel 03 phase V-low
pin 06 channel 04 phase W-high
pin 20 channel 05 phase W-low

pin 13 Trigger out01 if high PWM is switch off
pin 14 Trigger out02 if high PWM break starts

11.07.2014 Barney
*/

#include "pwm.h"

#define PERIPHERAL_BUS_CLOCK 48000000   // Bus Clock 48MHz
#define FTM0_CLK_PRESCALE 0             // FTM0 Prescaler
#define FTM0_OVERFLOW_FREQUENCY 80000   // PWM frequency in Hz
#define FTM0_DEADTIME_DTVAL 5           // DeadTimeHigh, zum Schutz der MOSFETs

// Berechnen von max und min Tastverhaeltnis 
const uint16_t MAX_DUTY_CYCLE = PERIPHERAL_BUS_CLOCK/FTM0_OVERFLOW_FREQUENCY;     // Maximum duty cycle value.
const uint16_t MIN_DUTY_CYCLE = 0;      // Minimum duty cycle value.

// Sinustabelle normiert auf Amplitudenwert 300, positive Halbwelle und 180 Grad Nullwerte, 2 Grad Winkelaufloesung 
const uint16_t sintable[]={
  0,10,21,31,42,52,62,73,83,93,103,112,122,132,141,150,159,168,176,185,193,201,208,216,223,230,236,243,249,254,
  260,265,270,274,278,282,285,288,291,293,295,297,298,299,300,300,300,299,298,297,295,293,291,288,285,282,278,274,
  270,265,260,254,249,243,236,230,223,216,208,201,193,185,176,168,159,150,141,132,122,112,103,93,83,73,62,52,42,31,
  21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Phasenlage von u,v,w
byte u = 0;       // Phase U ->   0 degree reference
byte v = 180-60;  // Phase V -> 120 degrees later
byte w = 180-120; // Phase W -> 240 degrees later

uint16_t BreakPower = 0;
byte Ramp = 1;    // Spannungsabsenkung bei kleinen Drehzahlen

int test = 0;     // for some tests
int Trigger_01 = 13;
int Trigger_02 = 14;


void setup () {
  // initialize the digital pin as an output.
  pinMode(Trigger_01, OUTPUT);    
  digitalWrite(Trigger_01, LOW);

  pinMode(Trigger_02, OUTPUT);    
  digitalWrite(Trigger_02, LOW);
  PWMInit(PERIPHERAL_BUS_CLOCK, FTM0_CLK_PRESCALE, FTM0_OVERFLOW_FREQUENCY, FTM0_DEADTIME_DTVAL);
}

void loop (){
  // Diverse Tests
  test ++;

  if (test < 1400 || test > 1850 & test < 2200) {
    //delayMicroseconds(1); 
    PWM_SetDutyCycle(sintable[u]/Ramp,sintable[v]/Ramp,sintable[w]/Ramp);  // PWM-Tastverhaeltnis fuer Phase u setzen, Aplitude skalieren
  }


  if (test == 1400) {
    digitalWrite(Trigger_02, HIGH);     // Trigger fuer LA
    // PWM Bremse
    // Fuer die PWM-Bremse muessen die oberen MOSFETS abgeschaltet werden
    // Achtung Registerdokumentaion in pwmc.c ist falsch. Das Datenblatt des Microkontrollers ist zu nehmen
    BreakPower = 0;                 // PWM switch off
    PWM_BreakeMode();
    PWM_Break(BreakPower);          // PWM-Tastverhaeltnis fuer Phase u setzen
  }

  if (test > 1450 && test < 1700) {
      // Bremsen
      BreakPower++;
      PWM_Break(BreakPower);        // PWM-Tastverhaeltnis fuer Phase u setzen
    }

  if (test == 1850) {

    BreakPower = 0;                 // Bremse loesen und alle Ausgaenge auf Normalbetrieb
    PWM_Break(BreakPower);          // PWM-Tastverhaeltnis fuer Phase u setzen
    PWM_BreakeModeEnd();            // Ende PWM Bremse
  }

  if (test == 2200) {
    digitalWrite(Trigger_01, HIGH); // Trigger for logic analyser
    PWM_Force0();                   // PWM switch off
   }

 
  u++;
  if (u == 180) u=0;
  v++;
  if (v == 180) v=0;
  w++;
  if (w == 180) w=0;
}

