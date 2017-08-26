#include "fastrig.h"
#include "constants.h"
// s/o to Ben Katz

const float FASTRIG_multiplier = NUM_TABLE_ENTRIES / TWO_PI;
float FASTRIG_lookUp[2048];

void FASTRIG::ftTableInit(){
  for(int i = 0; i <= NUM_TABLE_ENTRIES; i++){
    FASTRIG_lookUp[i] = sinf(i*(TWO_PI/NUM_TABLE_ENTRIES));
  }
}

float FASTRIG::ftSin(float theta){
    while(theta < 0.0f) theta += TWO_PI;
    while(theta >= TWO_PI) theta -= TWO_PI;
    return FASTRIG_lookUp[(int)(FASTRIG_multiplier*theta)];
}

float FASTRIG::ftCos(float theta){
  return ftSin(theta - PI_OVER_TWO);
}

