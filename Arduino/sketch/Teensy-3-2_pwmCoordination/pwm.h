#include <mk20dx128.h>

void PWMInit(uint32_t PERIPHERAL_BUS_CLOCK, uint8_t FTM0_CLK_PRESCALE, uint32_t FTM0_OVERFLOW_FREQUENCY, uint8_t FTM0_DEADTIME_DTVAL)
{
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

  // FTM0_POL = FTM_POL_POL0 | FTM_POL_POL1 | FTM_POL_POL2 | FTM_POL_POL3 | FTM_POL_POL4 | FTM_POL_POL5; // flipping all channel pols, so that deadtime is inserted on high-side

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
  //sets pre-scale value see details below
  FTM0_SC |= FTM_SC_PS(FTM0_CLK_PRESCALE);

  FTM0_EXTTRIG |= 0x08;

  //FTM0_EXTTRIG = FTM_EXTTRIG_CH5TRIG; // q on this

  // Interrupts
  // FTM0_SC |= FTM_SC_TOIE; //Enable the interrupt mask.  timer overflow interrupt.. enables interrupt signal to come out of the module itself...  (have to enable 2x, one in the peripheral and once in the NVIC

  // NVIC_SET_PRIORITY(IRQ_FTM0, 64);
  // NVIC_ENABLE_IRQ(IRQ_FTM0);
}

void ADCInit(void) {
  SIM_SCGC5 = SIM_SCGC5 | 0x3E00;
  SIM_SCGC3 |= 0x08000000;
  ADC1_CFG1 = 0x44; // adc clock is bus clock / 2
  ADC1_CFG2 = 0x00;
  ADC1_SC2 = 0x40; // hardware triggered
  ADC1_SC3 = 0x00; // not continuous conversion
  ADC1_SC1A = 0x54; // interrupt enable and select ADC1_DM1 channel
  // set such that FTM0 to trigger ADC1
  SIM_SOPT7 = 0x8800; // FTM0 triggers ADC1, alternative triggering, only adc1_sc1a // conversion is valid ?

  NVIC_SET_PRIORITY(IRQ_ADC1, 8);
  NVIC_ENABLE_IRQ(IRQ_ADC1);
  /*
    NVICIP58 = 0x30;  // setting interrupt of ADC1
    NVICICPR1 |= 1 << 26; // clear the pending register of interrupt source 58
    NVICISER1 |= 1 << 26; // set the interrupt source of ADC1
  */
}

unsigned short result0RA;
unsigned short result1RA;

volatile uint8_t led = 0;

const uint8_t ledPin = 13;
const bool on = HIGH;
const bool off = LOW;

/*
void adc0_isr(void){
  if(ADC0_SC1A & ADC_SC1_COCO){ // status regitser & conversion & complete flag
    result0RA = (unsigned short)ADC0_RA; // adc result data register
      led = ~led;
      digitalWrite(13, led);
  }
  led = ~led;
  digitalWrite(13, led);
}
*/
void adc1_isr(void){
  digitalWriteFast(ledPin, on);
  if(ADC1_SC1A & ADC_SC1_COCO){ // status regitser & conversion & complete flag
    result1RA = (unsigned short)ADC1_RA; // adc result data register
  }
  digitalWriteFast(ledPin, off);
}

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

void PWM_SetDutyCycle(unsigned short pwm1, unsigned short pwm2, unsigned short pwm3)
{
  unsigned mod = FTM0_MOD / 2;
  FTM0_C0V = mod - pwm1;
  FTM0_C1V = mod + pwm1;
  FTM0_C2V = mod - pwm2;
  FTM0_C3V = mod + pwm2;
  FTM0_C4V = mod - pwm3;
  FTM0_C5V = mod + pwm3;
  FTM0_PWMLOAD |= FTM_PWMLOAD_LDOK;
}

void PWM_BreakeModeEnd()
{
  // SWOCTRL Bit Fields
  // u,v,w durch Software steuern und mit '0' �berschreiben, damit x,y,z mittels PWM bremsen koennen
  FTM0_SWOCTRL &= ~FTM_SWOCTRL_CH1OC & ~FTM_SWOCTRL_CH3OC & ~FTM_SWOCTRL_CH5OC;
}

void PWM_BreakeMode()
{
  // SWOCTRL Bit Fields
  // u,v,w set low in software so that x,y,z pwms can be used for braking
  FTM0_SWOCTRL |= FTM_SWOCTRL_CH1OC | FTM_SWOCTRL_CH3OC | FTM_SWOCTRL_CH5OC;
  FTM0_SWOCTRL &= ~FTM_SWOCTRL_CH1OCV & ~FTM_SWOCTRL_CH3OCV & ~FTM_SWOCTRL_CH5OCV;
}

void PWM_Force0() {
  // SWOCTRL Bit Fields
  // Force all outoputs to LOW
  FTM0_SWOCTRL |= FTM_SWOCTRL_CH0OC | FTM_SWOCTRL_CH1OC | FTM_SWOCTRL_CH2OC
                  | FTM_SWOCTRL_CH3OC | FTM_SWOCTRL_CH4OC | FTM_SWOCTRL_CH5OC;
  FTM0_SWOCTRL &= ~FTM_SWOCTRL_CH0OCV & ~FTM_SWOCTRL_CH1OCV & ~FTM_SWOCTRL_CH2OCV &
                  ~FTM_SWOCTRL_CH3OCV & ~FTM_SWOCTRL_CH4OCV & ~FTM_SWOCTRL_CH5OCV;
}

void PWM_Enable() {
  // SWOCTRL Bit Fields
  // Force all outoputs to LOW
  FTM0_SWOCTRL &= ~FTM_SWOCTRL_CH0OC & ~FTM_SWOCTRL_CH1OC & ~FTM_SWOCTRL_CH2OC &
                  ~FTM_SWOCTRL_CH3OC & ~FTM_SWOCTRL_CH4OC & ~FTM_SWOCTRL_CH5OC;
}

void PWM_Break(unsigned short strength)  // PWM abschalten
{
  FTM0_C0V = 0;
  FTM0_C1V = strength * 2;
  FTM0_C2V = 0;
  FTM0_C3V = strength * 2;
  FTM0_C4V = 0;
  FTM0_C5V = strength * 2;
  FTM0_PWMLOAD |= FTM_PWMLOAD_LDOK;
}

void PWM_DisableChannel()  // PWM abschalten
{
  FTM0_C0V = 0;
  FTM0_C1V = 0;
  FTM0_C2V = 0;
  FTM0_C3V = 0;
  FTM0_C4V = 0;
  FTM0_C5V = 0;
  FTM0_PWMLOAD |= FTM_PWMLOAD_LDOK;
}
