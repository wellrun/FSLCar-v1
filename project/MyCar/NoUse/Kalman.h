#ifndef _KALMAN_H
#define _KALMAN_H

#include <math.h>
//#include "UartScope.h"

extern float angle, angle_dot; 
void Kalman_Filter(float angle_m,float gyro_m);


#endif