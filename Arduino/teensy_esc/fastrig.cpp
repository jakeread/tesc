#include "fastrig.h"
#include "constants.h"
// s/o to Ben Katz

const float FASTRIG_multiplier = NUM_TABLE_ENTRIES / TWO_PI;
float FASTRIG_lookUpSin[NUM_TABLE_ENTRIES];
float FASTRIG_lookUpCos[NUM_TABLE_ENTRIES];

void FASTRIG::ftTableInit(){
  for(int i = 0; i <= NUM_TABLE_ENTRIES; i++){
    FASTRIG_lookUpSin[i] = sinf(i*(TWO_PI/NUM_TABLE_ENTRIES));
    FASTRIG_lookUpCos[i] = cosf(i*(TWO_PI/NUM_TABLE_ENTRIES));
  }
}

float FASTRIG::ftSin(float theta){
    while(theta < 0.0f) theta += TWO_PI;
    while(theta >= TWO_PI) theta -= TWO_PI;
    return FASTRIG_lookUpSin[(int)(FASTRIG_multiplier*theta)];
}

float FASTRIG::ftCos(float theta){
    while(theta < 0.0f) theta += TWO_PI;
    while(theta >= TWO_PI) theta -= TWO_PI;
    return FASTRIG_lookUpCos[(int)(FASTRIG_multiplier*theta)];
}

