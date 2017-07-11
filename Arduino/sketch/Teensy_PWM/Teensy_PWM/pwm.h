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
  FTM0_QDCTRL &=~FTM_QDCTRL_QUADEN;  
  //QUADEN is Bit 1, Set Quadrature Decoder Mode (QUADEN) Enable to 0,   (disabled)

  // Also need to setup the FTM0C0SC channel control register
  FTM0_CNT = 0x0; //FTM Counter Value - reset counter to zero
  FTM0_MOD = (PERIPHERAL_BUS_CLOCK/(1<<FTM0_CLK_PRESCALE))/FTM0_OVERFLOW_FREQUENCY ;  //Set the overflow rate
  FTM0_CNTIN = 0; //Set the Counter Initial Value to 0

  // FTMx_CnSC - contains the channel-interrupt status flag control bits
  FTM0_C0SC |= FTM_CSC_ELSB; //Edge or level select
  FTM0_C0SC &= ~FTM_CSC_ELSA; //Edge or level Select
  FTM0_C0SC |= FTM_CSC_MSB; //Channel Mode select

  // FTMx_CnSC - contains the channel-interrupt status flag control bits
  FTM0_C1SC |= FTM_CSC_ELSB; //Edge or level select
  FTM0_C1SC &= ~FTM_CSC_ELSA; //Edge or level Select
  FTM0_C1SC |= FTM_CSC_MSB; //Channel Mode select

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
  FTM0_C0V = FTM0_MOD/2;
  FTM0_C1V = FTM0_MOD/2;
  FTM0_C2V = FTM0_MOD/2;
  FTM0_C3V = FTM0_MOD/2;
  FTM0_C4V = FTM0_MOD/2;
  FTM0_C5V = FTM0_MOD/2;
  FTM0_COMBINE = FTM_COMBINE_DTEN0|FTM_COMBINE_SYNCEN0|FTM_COMBINE_COMBINE0|FTM_COMBINE_COMP0|
                 FTM_COMBINE_DTEN1|FTM_COMBINE_SYNCEN1|FTM_COMBINE_COMBINE1|FTM_COMBINE_COMP1|
                 FTM_COMBINE_DTEN2|FTM_COMBINE_SYNCEN2|FTM_COMBINE_COMBINE2|FTM_COMBINE_COMP2;

  FTM0_DEADTIME = FTM0_DEADTIME_DTVAL; //About 5usec

  //Status and Control bits
  FTM0_SC = FTM_SC_CLKS(1);  // Selects Clock source to be "system clock"
  //sets pre-scale value see details below
  FTM0_SC |= FTM_SC_PS(FTM0_CLK_PRESCALE);
  
  // Interrupts
  //FTM0_SC |= FTM_SC_TOIE; //Enable the interrupt mask.  timer overflow interrupt.. enables interrupt signal to come out of the module itself...  (have to enable 2x, one in the peripheral and once in the NVIC

}

void PWM_SetDutyCycle(unsigned short pwm1,unsigned short pwm2,unsigned short pwm3)
{
  unsigned mod = FTM0_MOD/2;
  FTM0_C0V = mod-pwm1;
  FTM0_C1V = mod+pwm1;
  FTM0_C2V = mod-pwm2;
  FTM0_C3V = mod+pwm2;
  FTM0_C4V = mod-pwm3;
  FTM0_C5V = mod+pwm3;
  FTM0_PWMLOAD |= FTM_PWMLOAD_LDOK;
}

void PWM_BreakeModeEnd()
{
  // SWOCTRL Bit Fields
  // u,v,w durch Software steuern und mit '0' ueberschreiben, damit x,y,z mittels PWM bremsen koennen
  FTM0_SWOCTRL &= ~FTM_SWOCTRL_CH1OC&~FTM_SWOCTRL_CH3OC&~FTM_SWOCTRL_CH5OC;
}

void PWM_BreakeMode()
{
  // SWOCTRL Bit Fields
  // u,v,w durch Software steuern und mit LOW ueberschreiben, damit x,y,z mittels PWM bremsen koennen
  FTM0_SWOCTRL |= FTM_SWOCTRL_CH1OC|FTM_SWOCTRL_CH3OC|FTM_SWOCTRL_CH5OC;
  FTM0_SWOCTRL &= ~FTM_SWOCTRL_CH1OCV&~FTM_SWOCTRL_CH3OCV&~FTM_SWOCTRL_CH5OCV;
}

void PWM_Force0(){
  // SWOCTRL Bit Fields
  // Force all outoputs to LOW
  FTM0_SWOCTRL |= FTM_SWOCTRL_CH0OC|FTM_SWOCTRL_CH1OC|FTM_SWOCTRL_CH2OC
                 |FTM_SWOCTRL_CH3OC|FTM_SWOCTRL_CH4OC|FTM_SWOCTRL_CH5OC;
  FTM0_SWOCTRL &= ~FTM_SWOCTRL_CH0OCV&~FTM_SWOCTRL_CH1OCV&~FTM_SWOCTRL_CH2OCV&
                  ~FTM_SWOCTRL_CH3OCV&~FTM_SWOCTRL_CH4OCV&~FTM_SWOCTRL_CH5OCV;
}

void PWM_Enable(){
  // SWOCTRL Bit Fields
  // Force all outoputs to LOW
  FTM0_SWOCTRL &= ~FTM_SWOCTRL_CH0OC&~FTM_SWOCTRL_CH1OC&~FTM_SWOCTRL_CH2OC&
                  ~FTM_SWOCTRL_CH3OC&~FTM_SWOCTRL_CH4OC&~FTM_SWOCTRL_CH5OC;
}

void PWM_Break(unsigned short strength)  // PWM abschalten
{
  FTM0_C0V = 0;
  FTM0_C1V = strength*2;
  FTM0_C2V = 0;
  FTM0_C3V = strength*2;
  FTM0_C4V = 0;
  FTM0_C5V = strength*2;
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
