#ifndef  _CONTROL_H_
#define  _CONTROL_H_
#include "common.h"
#include "AngGet.h"
#include "common.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "datastructure.h"


extern CarInfo_TypeDef CarInfo_Now; //当前车子的信息
extern CarControl_TypeDef MotorControl; //电机控制的值
extern AngPID_InitTypeDef Ang_PID; //角度控制的PID结构体
extern TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
extern SpeedPID_TypeDef Speed_PID;
extern DirPID_TypeDef Dir_PID;

void MotorControl_Out(void);
void AngleControlValueCalc(void);
void SpeedControlValueCalc(void);
void DirControlValueCale(void);






#endif
