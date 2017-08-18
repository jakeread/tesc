#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "config.h"

const float SQRT_THREE = 1.732050808;
const float SQRT_TWO_OVER_THREE = sqrt(2/3);
// const float TWO_PI = PI * 2; already, from arduino core
const float THREE_PI = PI * 3;

// mostly for sector-checking

const float PI_OVER_THREE  =   PI / 3;
const float TWO_PI_OVER_THREE = 2*PI / 3;
const float FOUR_PI_OVER_THREE = 4*PI / 3;
const float FIVE_PI_OVER_THREE = 5*PI / 3;

const float ONE_OVER_SQRT_THREE = 1 / sqrt(3);
const float TWO_OVER_SQRT_THREE = 2 / sqrt(3);

const double TZ = 2048;
const float TWO_OVER_THREE_VDC = 26/3;
const float SIN_PI_OVER_THREE = sin(PI_OVER_THREE);

const double MOTOR_MODULO_OVER_TWO_PI = MOTOR_MODULO/TWO_PI; // for encoder ticks -> theta for DQ transform, etc

#endif
