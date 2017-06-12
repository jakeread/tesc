//
// servo stepper
//
// sec 2017
//
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "serial.h"
//#include "spi.h"
//calibration offsets for ADC
#define ADCACAL0_offset 0x20
#define ADCACAL1_offset 0x21
#define DEAD_TIME_CYCLES 2 //Dead time length, given in main system clock cycles.
#define ADC_SAMPLES 10 //adc samples to average for current measurement

static char input_buffer[100] = {0};
char checksum = 0;
USART_data_t USART_data;
int position_in_buffer = 0;
uint16_t angle = 0;

//currents through phases
uint16_t cur_1[ADC_SAMPLES];
uint16_t cur_2[ADC_SAMPLES];
uint16_t cur_1_avg = 0;
uint16_t cur_2_avg = 0;
uint16_t cur_pointer = 0;

//functions to set pwm and direction on motors
void set_pwms(uint16_t cca, uint16_t ccb, uint16_t ccc, uint16_t ccd){
	TCC0.CCABUF = cca;
	TCC0.CCBBUF = ccb;
	TCC0.CCCBUF = ccc;
	TCC0.CCDBUF = ccd;
	do {} while(TCC0.INTFLAGS && TC0_OVFIF_bm == 0 );  //wait
	TCC0.INTFLAGS = TC0_OVFIF_bm;
}


void send_packet(){
	usart_send_byte(&USART_data,200);
	usart_send_byte(&USART_data,200);
	usart_send_byte(&USART_data,200);
	usart_send_byte(&USART_data,200);
	usart_send_byte(&USART_data,200);
	usart_send_uint16(&USART_data,TCC0.CCABUF);
	usart_send_uint16(&USART_data,TCC0.CCBBUF);
	usart_send_uint16(&USART_data,TCC0.CCCBUF);
	usart_send_uint16(&USART_data,TCC0.CCDBUF);
	usart_send_uint16(&USART_data,angle);
	usart_send_uint16(&USART_data,cur_1_avg);
	usart_send_uint16(&USART_data,cur_2_avg);
	usart_send_byte(&USART_data,201);
	usart_send_byte(&USART_data,201);
	usart_send_byte(&USART_data,201);
	usart_send_byte(&USART_data,201);
	usart_send_byte(&USART_data,201);
}
void send_ack(){
	usart_send_byte(&USART_data,200);
	usart_send_byte(&USART_data,200);
	usart_send_byte(&USART_data,200);
	usart_send_byte(&USART_data,200);
	usart_send_byte(&USART_data,200);
	usart_send_byte(&USART_data,6);
	usart_send_byte(&USART_data,201);
	usart_send_byte(&USART_data,201);
	usart_send_byte(&USART_data,201);
	usart_send_byte(&USART_data,201);
	usart_send_byte(&USART_data,201);	
}

void spi_transcieve_uint16(uint16_t data, uint16_t *result){
	//send 16 bit command frame data, recieve 16 data frame, assign to variable with pointer result
	PORTD.OUTCLR = PIN4_bm; //Pull SS line low
	SPID.DATA = (data >> 8); //upper byte first
	while(!(SPID.STATUS & SPI_IF_bm)) {}
	*result = ((uint16_t)SPID.DATA << 8);
	SPID.DATA = (data & 0xFF); //lower byte next
	while(!(SPID.STATUS & SPI_IF_bm)) {}
	*result |= SPID.DATA;
	PORTD.OUTSET = PIN4_bm; //Pull SS line high
}

void parse_packet(char* buffer){
	checksum = 0;
	for( int i=5; i<position_in_buffer-5; ++i) {checksum += buffer[i];}
	if (checksum == buffer[position_in_buffer-5] && buffer[0]==(char)200 && buffer[1]==(char)200 && buffer[2]==(char)200 && buffer[3]==(char)200 && buffer[4]==(char)200){
		send_ack();
		char id = buffer[5];
		if (id==110){
			//_delay_us(100);
			//can we do this only if we've accumulated an appropriate number of samples?
			send_packet();
		} else if (id==100){
			//set pwms directly
			set_pwms(
				parse_uint16(&buffer[6]),
				parse_uint16(&buffer[8]),
				parse_uint16(&buffer[10]),
				parse_uint16(&buffer[12])
				);
		} else if (id==101){
			//read register on as5047d
			//first bit is parity, calculated on lower 15 bits of frame
			//second bit is command type, 0 for write, 1 for read
			//other bits are address
			//spi_transcieve_uint16(( 1<<15 ) | ( 1<<14 ) | ( 0x3FFF ), &angle); //address, ANGLECOM
			spi_transcieve_uint16(( 0<<15 ) | ( 1<<14 ) | ( 0x3FFD ), &angle); //address, MAG, lower 13 bits
			//spi_transcieve_uint16(( 1<<15 ) | ( 1<<14 ) | ( 0x3FFC ), &angle); //address, DIAAGC
			//spi_transcieve_uint16(( 0 << 15 ) | ( 1 << 14 ) | 0x0001, &angle ); //address, ERRFL
			
			_delay_us(1); //must be greater than 350 ns between frames
			spi_transcieve_uint16(0x0000, //no op
							&angle); //read result
			angle &= 0x1FFF; //for mag, just keep lower 13 bits

		}

	}
}

void read_usart(){
	if (USART_RXBufferData_Available(&USART_data)) {
		input_buffer[position_in_buffer] = USART_RXBuffer_GetByte(&USART_data);
		if (input_buffer[position_in_buffer] == (char)201 && 
			input_buffer[position_in_buffer-1] == (char)201 &&
			input_buffer[position_in_buffer-2] == (char)201 &&
			input_buffer[position_in_buffer-3] == (char)201 &&
			input_buffer[position_in_buffer-4] == (char)201 &&
			((position_in_buffer == 19) || (position_in_buffer == 11)) //check against expected lengths
			){
			parse_packet(input_buffer);
			position_in_buffer = 0;
		}
		else{
			position_in_buffer++;
		}
	}
}

//TODO: set up ADC for current measurement
//TODO: set up DMA to transfer encoder value to variable.

int main(void) {
	// set up clock
	OSC.CTRL = OSC_RC32MEN_bm; // enable 32MHz clock
	while (!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for clock to be ready
	CCP = CCP_IOREG_gc; // enable protected register change
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock

	//set up usart
	PORTE.DIRSET = PIN3_bm; //TXE0
	PORTE.DIRCLR = PIN2_bm; //RXD0
	USART_InterruptDriver_Initialize(&USART_data, &USARTE0, USART_DREINTLVL_LO_gc);
	USART_Format_Set(USART_data.usart, USART_CHSIZE_8BIT_gc,
                     USART_PMODE_DISABLED_gc, 0);
	//USARTE0.CTRLC = (USARTE0.CTRLC & ~USART_CMODE_gm) | USART_CMODE_ASYNCHRONOUS_gc;

	USART_RxdInterruptLevel_Set(USART_data.usart, USART_RXCINTLVL_LO_gc);
	//take f_sysclk/(BSEL+1) ~= f_baud*16 with zero scale.  See manual or spreadsheet for scale defs
	USART_Baudrate_Set(&USARTE0, 123 , -4); //230400 baud with .08% error
	USART_Rx_Enable(USART_data.usart);
	USART_Tx_Enable(USART_data.usart);

	//
	//TCC0 AWEX and DTI for driving hbridge
	//
	AWEXC.CTRL |= AWEX_DTICCAEN_bm | AWEX_DTICCBEN_bm | AWEX_DTICCCEN_bm | AWEX_DTICCDEN_bm;
	AWEXC.OUTOVEN = 0xFF; //override all pins on port c for awex
	AWEXC.DTLS = DEAD_TIME_CYCLES;
	AWEXC.DTHS = DEAD_TIME_CYCLES;
	PORTC.DIR = 0xFF; //Enable output on PORTC.
	TCC0.PER = 0x0FFF;
	TCC0.CTRLB = TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm | TC_WGMODE_SS_gc;
	//TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc; //overflow interrupt
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV1_gc; //start TCC0


	//SPI
	PORTD.REMAP |= PORT_SPI_bm; //swap SCK to PD5 and MOSI to PD7
	PORTD.DIRSET = PIN4_bm;		//Init SS pin as output
	PORTD.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc; 	// with wired AND and pull-up.
	PORTD.OUTSET = PIN4_bm;  // Set SS output to high. (No slave addressed).
	SPID.CTRL = SPI_PRESCALER_DIV4_gc | //SPI prescalar (32 MHz / 4 = 8 MHz)
				SPI_CLK2X_bm | 			//double clock speed (32 MHz / 4 = 8 MHz x 2 = 16 MHz)
				SPI_ENABLE_bm | 		//enable spi module
				SPI_MASTER_bm | 		//SPI Master mode
				SPI_MODE_1_gc; 			//SPI mode, see app note 1309 and AS5047D datasheet
	PORTD.DIRSET = PIN5_bm | PIN7_bm; //set CLK and MOSI to outputs

	//ADC for current measurement
	//calibrate to reduce nonlinearity
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	ADCA.CALL = pgm_read_byte(PROD_SIGNATURES_START + ADCACAL0_offset);
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	ADCA.CALL = pgm_read_byte(PROD_SIGNATURES_START + ADCACAL1_offset);
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;

	ADCA.CTRLB = (ADCA.CTRLB & ~ADC_RESOLUTION_gm) |  ADC_RESOLUTION_8BIT_gc; //set resolution
	ADCA.REFCTRL = (ADCA.REFCTRL & ~ADC_REFSEL_gm) | ADC_REFSEL_INT1V_gc; //set reference
	ADCA.PRESCALER = (ADCA.PRESCALER & ~ADC_PRESCALER_gm) | ADC_PRESCALER_DIV64_gc; //set prescalar
	
	//ADCA, channel zero, cur_1
	ADCA.CH0.CTRL = (ADCA.CH0.CTRL & ~ADC_CH_INPUTMODE_gm) | ADC_CH_INPUTMODE_DIFFWGAIN_gc; //set differential with gain
	ADCA.CH0.CTRL = (ADCA.CH0.CTRL & ~ADC_CH_GAIN_gm) | ADC_CH_GAIN_4X_gc; //set gain
	ADCA.CH0.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN6_gc | ADC_CH_MUXNEG_GND_MODE4_gc;
	ADCA.CH0.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_HI_gc;
	
	//ADCA, channel one, cur_2
	ADCA.CH1.CTRL = (ADCA.CH1.CTRL & ~ADC_CH_INPUTMODE_gm) | ADC_CH_INPUTMODE_DIFFWGAIN_gc; //set differential with gain
	ADCA.CH1.CTRL = (ADCA.CH1.CTRL & ~ADC_CH_GAIN_gm) | ADC_CH_GAIN_4X_gc; //set gain
	ADCA.CH1.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN7_gc | ADC_CH_MUXNEG_GND_MODE4_gc;
	ADCA.CH1.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_HI_gc;	
	
	ADCA.EVCTRL = (ADCA.EVCTRL & ~ADC_SWEEP_gm) | ADC_SWEEP_01_gc; //set sweep over channels 0,1
	ADCA.EVCTRL = (ADCA.EVCTRL & ~ADC_EVSEL_gm) | ADC_EVSEL_67_gc; //event channels 6,7 as inputs
	ADCA.EVCTRL = (ADCA.EVCTRL & ~ADC_EVACT_gm) | ADC_EVACT_SWEEP_gc; //event channel 6 triggers adc sweep
	ADCA.CTRLA |= ADC_ENABLE_bm; //enable ADCA

	ADCA.CH1.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_LO_gc; //set up interrupts on ADC CH1
	ADCA.INTFLAGS = ADC_CH1IF_bm; //clear interrupt flag

	//use TCC0 overflow to start adc sweep
	EVSYS.CH6MUX = EVSYS_CHMUX_TCC0_OVF_gc;


	//DAC for resistor network reference
	DACB.CTRLB = ( DACB.CTRLB & ~DAC_CHSEL_gm ) | DAC_CHSEL_DUAL_gc;
	DACB.CTRLC = ( DACB.CTRLC & ~( DAC_REFSEL_gm ) ) | DAC_REFSEL_INT1V_gc;
    DACB.CTRLA |= DAC_CH1EN_bm | DAC_CH0EN_bm | DAC_ENABLE_bm;
	DACB.CH0DATA = 4095; //set to 1V
	DACB.CH1DATA = 4095; //set to 1V


	PMIC.CTRL |= PMIC_LOLVLEX_bm;//enable low level interrupts
	sei();

	while (1) {
		read_usart();
	}
	return 0;
}

ISR(USARTE0_RXC_vect){USART_RXComplete(&USART_data);}
ISR(USARTE0_DRE_vect){USART_DataRegEmpty(&USART_data);}

ISR(ADCA_CH1_vect){
	//cur_1_avg -= cur_1[cur_pointer]; //subtract old value
	//cur_2_avg -= cur_2[cur_pointer];
	//cur_1[cur_pointer] = ADCA.CH0.RESL; //replace old value
	//cur_2[cur_pointer] = ADCA.CH1.RESL; 
	//cur_1_avg += cur_1[cur_pointer]; //add back new value
	//cur_2_avg += cur_2[cur_pointer];
	cur_1_avg = ADCA.CH0.RESL & ~(1<<7); //get rid of sign bit
	cur_2_avg = ADCA.CH1.RESL & ~(1<<7); //get rid of sign bit
	if( cur_pointer < ADC_SAMPLES-1){EVSYS.STROBE = 1<<6;} //trigger event channel 6 to start another sweep
	cur_pointer = (cur_pointer+1)%ADC_SAMPLES;
}


