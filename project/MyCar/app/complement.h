#ifndef _COMPLEMENT_H_
#define _COMPLEMENT_H_
#include "math.h"
#include "common.h"

void complement_filter(float angle_m_cf,float gyro_m_cf);
extern float angle_com,angle_dot_com;
void complement2(float angle_m, float gyro_m);

#endif
