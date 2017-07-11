#include "pwm.h"

#define PERIPHERAL_BUS_CLOCK 48000000   // Bus Clock 48MHz
#define FTM0_CLK_PRESCALE 0             // FTM0 Prescaler
#define FTM0_OVERFLOW_FREQUENCY 40000   // PWM frequency in Hz
#define FTM0_DEADTIME_DTVAL 0           // DeadTimeHigh, zum Schutz der MOSFETs

#define BAUDRATE 115200   // Festsetzen der Datenrate für die Arduino Debug Schnittstelle und oder Bluetooth Modul

// Berechnen von max und min Tastverhältnis 
const uint16_t MAX_DUTY_CYCLE = PERIPHERAL_BUS_CLOCK/FTM0_OVERFLOW_FREQUENCY;     // Maximum duty cycle value.
const uint16_t MIN_DUTY_CYCLE = 0;      // Minimum duty cycle value.

// Sinustabelle normiert auf Amplitudenwert 600, positive Halbwelle und 180 Grad Nullwerte, 2 Grad Winkelauflösung 
const uint16_t sintable[]={
  0,21,42,63,84,104,125,145,165,185,205,225,244,263,282,300,318,336,353,369,386,401,417,432,446,460,473,485,497,509,520,
  530,539,548,556,564,571,577,582,587,591,594,597,599,600,600,600,599,597,594,591,587,582,577,571,564,556,548,539,530,520,
  509,497,485,473,460,446,432,417,401,386,369,353,336,318,300,282,263,244,225,205,185,165,145,125,104,84,63,42,21,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Phasenlage von u,v,w
byte u = 0;       // Phase U ->   0 Grad Referenz
byte v = 180-60;  // Phase V -> 120 Grad später
byte w = 180-120; // Phase W -> 240 Grad später

uint16_t BreakPower = 0;
byte Ramp = 1; // Spannungsabsenkung bei kleinen Drehzahlen

int test = 0;     // für tests
int Trigger_01 = 13;
int Trigger_02 = 14;

IntervalTimer timer_Sin;    // 1 Sekunden Timer

void setup () {
  // initialize the digital pin as an output.
  pinMode(Trigger_01, OUTPUT);    
  digitalWrite(Trigger_01, LOW);

  pinMode(Trigger_02, OUTPUT);    
  digitalWrite(Trigger_02, LOW);

  PWMInit(PERIPHERAL_BUS_CLOCK, FTM0_CLK_PRESCALE, FTM0_OVERFLOW_FREQUENCY, FTM0_DEADTIME_DTVAL);

  timer_Sin.begin(TimerSin, 130); //   Timer


}

void loop ()
{
  delay(1000);
}

void TimerSin() 
{
  PWM_SetDutyCycle(sintable[u]/Ramp,sintable[v]/Ramp,sintable[w]/Ramp);  // PWM-Tastverhältnis für Phase u setzen, Aplitude skalieren

  u++;
  if (u == 180) u=0;
  v++;
  if (v == 180) v=0;
  w++;
  if (w == 180) w=0;
}
