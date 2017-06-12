
#include "t3spi.h"

//Initialize T3SPI class as SPI_MASTER
T3SPI SPI_MASTER;

//The number of integers per data packet
#define dataLength 8

//Initialize the arrays for outgoing data
//volatile uint8_t  data[dataLength] = {}; 
volatile uint16_t data[dataLength] = {}; 


//Initialize the arrays for incoming data
//volatile uint8_t returnData[dataLength] = {}; 
volatile uint16_t returnData[dataLength] = {};

volatile uint16_t ReadAddress;
volatile uint16_t NoOp;

void setup(){
  
  Serial.begin(115200);
  
  //Begin SPI in MASTER (SCK pin, MOSI pin, MISO pin, CS pin, Active State)
  SPI_MASTER.begin_MASTER(ALT_SCK, ALT_MOSI, ALT_MISO, CS4, CS_ActiveLOW); //CS1->Pin9
  
  //Set the CTAR (CTARn, Frame Size, SPI Mode, Shift Mode, Clock Divider) 
  //SPI_MASTER.setCTAR(CTAR0,8,SPI_MODE0,LSB_FIRST,SPI_CLOCK_DIV2);
  SPI_MASTER.setCTAR(CTAR_0,16,SPI_MODE1,LSB_FIRST,SPI_CLOCK_DIV6); //as5047 max clock 10MHz

  ReadAddress = 0x3FFF | 0x4000 | 0x8000;
  NoOp = 0x0000 | 0x4000 | 0x8000;
  
  //Populate data array 
  data[0] = ReadAddress;
  data[1] = ReadAddress;
  data[2] = ReadAddress;
  data[3] = ReadAddress;
  data[4] = ReadAddress;
  data[5] = ReadAddress;
  data[6] = ReadAddress;
  data[7] = NoOp;

  //Wait for Slave
  delay(1500);
}

void loop(){

  //Capture the time before sending data
  SPI_MASTER.timeStamp1 = micros();

  //Send n number of data packets
  //for (int i=0; i<1; i++) {
    //Send data (data array, data array length, CTARn, CS pin)
    //SPI_MASTER.txrx8(data, returnData, dataLength,CTAR0,CS1);}  
    SPI_MASTER.txrx16(data, returnData, dataLength,CTAR_0,CS4);
  //}

  for (int i=0; i<dataLength; i++){
    returnData[i] = returnData[i] << 2;
    returnData[i] = returnData[i]/4;
  }
  //Capture the time when transfer is done
  SPI_MASTER.timeStamp2 = micros();
  
  //Print data sent & data received
  for (int i=0; i<dataLength; i++){
    Serial.print("data[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.print(data[i]);
    Serial.print("   returnData[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(returnData[i]);
    Serial.flush();
  }

  //Print statistics for the previous transfer
  SPI_MASTER.printStatistics(dataLength);
  
  //Reset the packet count 
  SPI_MASTER.packetCT=0;
  
  
  delay(50);
}

