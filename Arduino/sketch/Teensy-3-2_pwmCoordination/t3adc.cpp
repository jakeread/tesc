#include "t3adc.h"
  /*
void ADCInit(){

  // turning on modules
  SIM_SCGC6 |= SIM_SCGC6_ADC0;
  SIM_SCGC3 |= SIM_SCGC3_ADC1;

  // setup IRS's
  NVIC_SET_PRIORITY(IRQ_ADC0, 48);
  NVIC_ENABLE_IRQ(IRQ_ADC0);

  NVIC_SET_PRIORITY(IRQ_ADC1, 32);
  NVIC_ENABLE_IRQ(IRQ_ADC1);
  
  // MODE = 0x3 = single-ended 16-bit conversion
  ADC0_CFG1 |= ADC_CFG1_MODE(0x3);
  ADC1_CFG1 |= ADC_CFG1_MODE(0x3);

  // ADHSC = 1 = very high speed operation
  ADC0_CFG2 |= ADC_CFG2_ADHSC;
  ADC1_CFG2 |= ADC_CFG2_ADHSC;

  // ADTRG = 1 = hardware trigger
  ADC0_SC2 |= ADC_SC2_ADTRG;
  ADC1_SC2 |= ADC_SC2_ADTRG;

  // trigger selection
  SIM_SOPT7 |= SIM_SOPT7_ADC0TRGSEL(0x8) | SIM_SOPT7_ADC0ALTTRGEN;
  SIM_SOPT7 |= SIM_SOPT7_ADC1TRGSEL(0x8) | SIM_SOPT7_ADC1ALTTRGEN;

  // AIEN = 1 = conversion complete interrupt enabled, 
  // ADCH = 0x9 = channel 9 selected as SE input
  ADC0_SC1A |= ADC_SC1_AIEN | ADC_SC1_ADCH(0x9);
  ADC1_SC1A |= ADC_SC1_AIEN | ADC_SC1_ADCH(0x1);
}
  */

/*
 * pevide's SC1A -> Pin # Chart -> for channel select
 * translate SC1A to pin number
 * channel2sc1aADCx converts a pin number to their value for the SC1A register, for the ADC0 and ADC1

///////// ADC0
#if defined(ADC_TEENSY_3_0) || defined(ADC_TEENSY_3_1)
const uint8_t ADC::sc1a2channelADC0[]= { // new version, gives directly the pin number
    34, 0, 0, 36, 23, 14, 20, 21, 16, 17, 0, 0, 19, 18, // 0-13
    15, 22, 23, 0, 0, 35, 0, 37, // 14-21
    39, 40, 0, 0, 38, 41, 42, 43, // VREF_OUT, A14, temp. sensor, bandgap, VREFH, VREFL.
    0 // 31 means disabled, but just in case
};

///////// ADC1
#if defined(ADC_TEENSY_3_1)
const uint8_t ADC::sc1a2channelADC1[]= { // new version, gives directly the pin number
    36, 0, 0, 34, 28, 26, 29, 30, 16, 17, 0, 0, 0, 0, // 0-13. 5a=26, 5b=27, 4b=28, 4a=31
    0, 0, 0, 0, 39, 37, 0, 0, // 14-21
    0, 0, 0, 0, 38, 41, 0, 42, // 22-29. VREF_OUT, A14, temp. sensor, bandgap, VREFH, VREFL.
    43
};

#elif defined(__MK20DX256__) // from PRJC core
static const uint8_t pin2sc1a[] = {
  5, 14, 8, 9, 13, 12, 6, 7, 15, 4, 0, 19, 3, 19+128, // 0-13 -> A0-A13
  5, 14, 8, 9, 13, 12, 6, 7, 15, 4, // 14-23 are A0-A9
  255, 255, // 24-25 are digital only
  5+192, 5+128, 4+128, 6+128, 7+128, 4+192, // 26-31 are A15-A20
  255, 255, // 32-33 are digital only
  0, 19, 3, 19+128, // 34-37 are A10-A13
  26,     // 38 is temp sensor,
  18+128, // 39 is vref
  23      // 40 is A14
};

*/

T3ADC::T3ADC(uint8_t pin_sensa_u, uint8_t pin_sensa_v, uint8_t pin_sensv_u, uint8_t pin_sensv_v, uint8_t pin_sensv_w){
  _pin_sensa_u = pin_sensa_u;
  _pin_sensa_v = pin_sensa_v;
  _pin_sensv_u = pin_sensv_u;
  _pin_sensv_v = pin_sensv_v;
  _pin_sensv_w = pin_sensv_w;
}

void T3ADC::init(){
  // ADC1 Init
  SIM_SCGC5 |= SIM_SCGC5_PORTE | SIM_SCGC5_PORTD | SIM_SCGC5_PORTC | SIM_SCGC5_PORTB | SIM_SCGC5_PORTA; // enable port a/b/c/d/e clock
  SIM_SCGC6 |= SIM_SCGC6_ADC0;  // ADC0 turnt on
  SIM_SCGC3 |= SIM_SCGC3_ADC1;  // ADC1 turnt on
  ADC0_CFG1 |= ADC_CFG1_ADIV(1);
  ADC1_CFG1 |= ADC_CFG1_ADIV(1); // adc clock is bus clock / 2
  ADC0_CFG2 = 0x00; 
  ADC1_CFG2 = 0x00; // nothing to setup here
  ADC0_SC2 |= ADC_SC2_ADTRG | ADC_SC2_REFSEL(0);
  ADC1_SC2 |= ADC_SC2_ADTRG | ADC_SC2_REFSEL(0); // hardware triggered & voltage reference external (3.3v)
  ADC0_SC3 |= ADC_SC3_AVGE | ADC_SC3_AVGS(0);
  ADC1_SC3 |= ADC_SC3_AVGE | ADC_SC3_AVGS(0); // not continuous conversion, hardware average enabled, 4 samples
  // ADC1 Interrupt Setup / Triggers
  // interrupt enable and select ADC1_DM1 channel // channel -> pin in chart above (thx pevide)
  ADC0_SC1A = ADC_SC1_AIEN | ADC_SC1_ADCH(12); // A5
  // ADC0_CFG2 |= ADC_CFG2_MUXSEL;
  ADC1_SC1A = ADC_SC1_AIEN | ADC_SC1_ADCH(9); // A3
  // enable ADC1, ADC0 alternate trigger select, and select trigger 8 (ftm0)
  SIM_SOPT7 = SIM_SOPT7_ADC1ALTTRGEN | SIM_SOPT7_ADC1TRGSEL(8) | SIM_SOPT7_ADC0ALTTRGEN | SIM_SOPT7_ADC0TRGSEL(8); 
  // Setup ADC1, ADC0 Interrupt
  NVIC_SET_PRIORITY(IRQ_ADC0, 16);
  NVIC_ENABLE_IRQ(IRQ_ADC0);
  NVIC_SET_PRIORITY(IRQ_ADC1, 8);
  NVIC_ENABLE_IRQ(IRQ_ADC1);

  
}
