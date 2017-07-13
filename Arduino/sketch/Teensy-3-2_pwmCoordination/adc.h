#include <mk20dx128.h>

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

unsigned short result0RA;
unsigned short result1RA;

volatile uint8_t led = 0;

void adc0_isr(void){
  if(ADC0_SC1A & ADC_SC1_COCO){ // status regitser & conversion & complete flag
    result0RA = (unsigned short)ADC0_RA; // adc result data register
      led = ~led;
      digitalWrite(13, led);
  }
  led = ~led;
  digitalWrite(13, led);
}

void adc1_isr(void){
  if(ADC1_SC1A & ADC_SC1_COCO){ // status regitser & conversion & complete flag
    result1RA = (unsigned short)ADC1_RA; // adc result data register
      led = ~led;
      digitalWrite(13, led);
  }
  led = ~led;
  digitalWrite(13, led);
}
