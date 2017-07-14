#include "t3pwm.h"
#include "config.h"
#include <mk20dx128.h>

/* from PRJC's cores / teensy3 / pins_teensy.c -> FTM0 channels are stuck to these pins.
  #elif defined(__MK20DX256__)
  #define FTM0_CH0_PIN 22
  #define FTM0_CH1_PIN 23
  #define FTM0_CH2_PIN  9
  #define FTM0_CH3_PIN 10
  #define FTM0_CH4_PIN  6
  #define FTM0_CH5_PIN 20
  #define FTM0_CH6_PIN 21
  #define FTM0_CH7_PIN  5
  #define FTM1_CH0_PIN  3
  #define FTM1_CH1_PIN  4
  #define FTM2_CH0_PIN 32
  #define FTM2_CH1_PIN 25
*/

uint32_t PERIPHERAL_BUS_CLOCK = 48000000;
uint8_t FTM0_CLK_PRESCALE = 0;  // FTM0 Prescaler
uint32_t FTM0_OVERFLOW_FREQUENCY = 11718; // PWM frequency in Hz
uint8_t FTM0_DEADTIME_DTVAL = 2; // DeadTime, in clk cycles (unconfirmed)

// don't need dis?
const uint16_t MAX_DUTY_CYCLE = PERIPHERAL_BUS_CLOCK / FTM0_OVERFLOW_FREQUENCY;   // Maximum duty cycle value.
const uint16_t MIN_DUTY_CYCLE = 0;      // Minimum duty cycle value.

T3PWM::T3PWM() {
  // save it for init
}

void T3PWM::init() {
  //Enable the Clock to the FTM0 Module
  SIM_SCGC6 |= SIM_SCGC6_FTM0;

  // Pin Settings
  PORTC_PCR1 = PORT_PCR_MUX(0x4)  | PORT_PCR_DSE; // FTM0_CHN0 Pin 22
  PORTC_PCR2 = PORT_PCR_MUX(0x4)  | PORT_PCR_DSE; // FTM0_CHN1 Pin 23
  PORTC_PCR3 = PORT_PCR_MUX(0x4)  | PORT_PCR_DSE; // FTM0_CHN2 Pin 9
  PORTC_PCR4 = PORT_PCR_MUX(0x4)  | PORT_PCR_DSE; // FTM0_CHN3 Pin 10
  PORTD_PCR4 = PORT_PCR_MUX(0x4)  | PORT_PCR_DSE; // FTM0_CHN4 Pin 6
  PORTD_PCR5 = PORT_PCR_MUX(0x4)  | PORT_PCR_DSE; // FTM0_CHN5 Pin 20

  /* 36.4.9 Complementary mode
    The Complementary mode is selected when:
    FTMEN = 1
    QUADEN = 0
    DECAPEN = 0
    COMBINE = 1
    CPWMS = 0, and
    COMP = 1
  */

  //FTM0_MODE[WPDIS] = 1; //Disable Write Protection - enables changes to QUADEN, DECAPEN, etc.
  FTM0_MODE |= FTM_MODE_WPDIS;

  //FTMEN is bit 0 set to 1
  FTM0_MODE |= FTM_MODE_FTMEN;

  //Set Edge Aligned PWM set to 0
  FTM0_QDCTRL &= ~FTM_QDCTRL_QUADEN;
  //QUADEN is Bit 1, Set Quadrature Decoder Mode (QUADEN) Enable to 0,   (disabled)

  // Also need to setup the FTM0C0SC channel control register
  FTM0_CNT = 0x0; //FTM Counter Value - reset counter to zero
  FTM0_MOD = (PERIPHERAL_BUS_CLOCK / (1 << FTM0_CLK_PRESCALE)) / FTM0_OVERFLOW_FREQUENCY ; //Set the overflow rate
  FTM0_CNTIN = 0; //Set the Counter Initial Value to 0

  // FTMx_CnSC - contains the channel-interrupt status flag control bits
  FTM0_C0SC |= FTM_CSC_ELSB; //Edge or level select
  FTM0_C0SC &= ~FTM_CSC_ELSA; //Edge or level Select
  FTM0_C0SC |= FTM_CSC_MSB; //Channel Mode select

  // FTMx_CnSC - contains the channel-interrupt status flag control bits
  FTM0_C1SC |= FTM_CSC_ELSB; //Edge or level select
  FTM0_C1SC &= ~FTM_CSC_ELSA; //Edge or level Select
  FTM0_C1SC |= FTM_CSC_MSB; //Channel Mode select
  // want to enable channel interrupts here, run something on isr ?

  // FTMx_CnSC - contains the channel-interrupt status flag control bits
  FTM0_C2SC |= FTM_CSC_ELSB; //Edge or level select
  FTM0_C2SC &= ~FTM_CSC_ELSA; //Edge or level Select
  FTM0_C2SC |= FTM_CSC_MSB; //Channel Mode select

  // FTMx_CnSC - contains the channel-interrupt status flag control bits
  FTM0_C3SC |= FTM_CSC_ELSB; //Edge or level select
  FTM0_C3SC &= ~FTM_CSC_ELSA; //Edge or level Select
  FTM0_C3SC |= FTM_CSC_MSB; //Channel Mode select

  // FTMx_CnSC - contains the channel-interrupt status flag control bits
  FTM0_C4SC |= FTM_CSC_ELSB; //Edge or level select
  FTM0_C4SC &= ~FTM_CSC_ELSA; //Edge or level Select
  FTM0_C4SC |= FTM_CSC_MSB; //Channel Mode select

  // FTMx_CnSC - contains the channel-interrupt status flag control bits
  FTM0_C5SC |= FTM_CSC_ELSB; //Edge or level select
  FTM0_C5SC &= ~FTM_CSC_ELSA; //Edge or level Select
  FTM0_C5SC |= FTM_CSC_MSB; //Channel Mode select

  //Edit registers when no clock is fed to timer so the MOD value, gets pushed in immediately
  FTM0_SC = 0; //Make sure its Off!

  //FTMx_CnV contains the captured FTM counter value, this value determines the pulse width
  FTM0_C0V = FTM0_MOD / 2;
  FTM0_C1V = FTM0_MOD / 2;
  FTM0_C2V = FTM0_MOD / 2;
  FTM0_C3V = FTM0_MOD / 2;
  FTM0_C4V = FTM0_MOD / 2;
  FTM0_C5V = FTM0_MOD / 2;
  FTM0_COMBINE = FTM_COMBINE_DTEN0 | FTM_COMBINE_SYNCEN0 | FTM_COMBINE_COMBINE0 | FTM_COMBINE_COMP0 |
                 FTM_COMBINE_DTEN1 | FTM_COMBINE_SYNCEN1 | FTM_COMBINE_COMBINE1 | FTM_COMBINE_COMP1 |
                 FTM_COMBINE_DTEN2 | FTM_COMBINE_SYNCEN2 | FTM_COMBINE_COMBINE2 | FTM_COMBINE_COMP2;

  FTM0_DEADTIME = FTM0_DEADTIME_DTVAL; //About 5usec, value is in terms of system clock cycles

  FTM0_MODE |= FTM_MODE_INIT;

  //Status and Control bits
  FTM0_SC = FTM_SC_CLKS(1); // Selects Clock source to be "system clock"
  FTM0_SC |= FTM_SC_PS(FTM0_CLK_PRESCALE); // Sets clock prescale value. Not sure about this.

  FTM0_EXTTRIG |= 0x08; // external trigger channel to fire (so, here is where we set multiple adc fire triggers?
  //FTM0_EXTTRIG = FTM_EXTTRIG_CH5TRIG; // q on this

  // Interrupts
  // FTM0_SC |= FTM_SC_TOIE; //Enable the interrupt mask.  timer overflow interrupt.. enables interrupt signal to come out of the module itself...  (have to enable 2x, one in the peripheral and once in the NVIC
  // NVIC_SET_PRIORITY(IRQ_FTM0, 64);
  // NVIC_ENABLE_IRQ(IRQ_FTM0);
}

void T3PWM::setPhases(unsigned short phase_a, unsigned short phase_b, unsigned short phase_c) { // * were unsigned short 's
  unsigned _mod = FTM0_MOD/2;
  FTM0_C0V = _mod - phase_a;
  FTM0_C1V = _mod + phase_a;
  FTM0_C2V = _mod - phase_b;
  FTM0_C3V = _mod + phase_b;
  FTM0_C4V = _mod - phase_c;
  FTM0_C5V = _mod + phase_c;
  FTM0_PWMLOAD |= FTM_PWMLOAD_LDOK; // enables the loading of MOD, CTIN and CV registers
}

void T3PWM::setBrake(uint16_t strength)  // PWM abschalten
{
  FTM0_C0V = 0;
  FTM0_C1V = strength * 2;
  FTM0_C2V = 0;
  FTM0_C3V = strength * 2;
  FTM0_C4V = 0;
  FTM0_C5V = strength * 2;
  FTM0_PWMLOAD |= FTM_PWMLOAD_LDOK;
}

void T3PWM::setPhasesLow()  // PWM abschalten
{
  FTM0_C0V = 0;
  FTM0_C1V = 0;
  FTM0_C2V = 0;
  FTM0_C3V = 0;
  FTM0_C4V = 0;
  FTM0_C5V = 0;
  FTM0_PWMLOAD |= FTM_PWMLOAD_LDOK;
}

/*
   as-of-now unused interrupt, leaving this here in case this ever becomes useful...
  volatile uint8_t fourt = 0;

  void ftm0_isr(void) {
  if (FTM0_SC & FTM_SC_TOF) { // clearing FTM interrupt flag
    FTM0_SC &= ~FTM_SC_TOF;
    fourt = ~ fourt;
    digitalWrite(14, fourt);
    fourt = ~ fourt;
    digitalWrite(14, fourt);
  }
  }
*/


