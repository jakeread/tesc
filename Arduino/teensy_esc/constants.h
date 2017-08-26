#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "config.h"

const float SQRT_THREE = 1.732050808;
const float SQRT_TWO_OVER_THREE = sqrt(2/3);
// const float TWO_PI = PI * 2; already, from arduino core
const float THREE_PI = PI * 3;
const float PI_OVER_TWO = PI / 2;

// mostly for sector-checking

const float PI_OVER_THREE  =   PI / 3.0;
const float TWO_PI_OVER_THREE = 2.0*PI / 3.0;
const float FOUR_PI_OVER_THREE = 4.0*PI / 3.0;
const float FIVE_PI_OVER_THREE = 5.0*PI / 3.0;

const float ONE_OVER_SQRT_THREE = 1.0 / sqrt(3.0);
const float TWO_OVER_SQRT_THREE = 2.0 / sqrt(3.0);

const float SQRT_THREE_OVER_TWO = sqrt(3.0) / 2.0;
const float SQRT_THREE_OVER_THREE = sqrt(3.0) / 3.0;

const float ONE_OVER_THREE = 1.0 / 3.0;
const float TWO_OVER_THREE = 2.0 / 3.0;
const float THREE_OVER_TWO = 3.0 / 2.0;

const double TZ = 2048;
const float TWO_OVER_THREE_VDC = 26/3;
const float SIN_PI_OVER_THREE = sinf(PI_OVER_THREE);

const double MOTOR_MODULO_OVER_TWO_PI = MOTOR_MODULO/TWO_PI; // for encoder ticks -> theta for DQ transform, etc

#endif
