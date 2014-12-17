#ifndef _ANGGET_H_
#define _ANGGET_H_

//#include "Kalman.h"
#include "common.H"
extern  float AngleIntegraed;

void AngleGet(void);
void Byte2Float(float *target,unsigned char *buf, unsigned char beg);
//int GyroScopeGetZero(void);
//int GravityGetZero(void);
void Float2Byte(float *target,uint8 *buf,uint8 beg);
void Short2Byte(int16 *target, int8 *buf, int8 beg);
void Byte2Int(int *target, unsigned char *buf, unsigned char beg);
void Int2Byte(int *target, unsigned char *buf, unsigned char beg);
//void AngleControlValueCalc(void);


#endif
