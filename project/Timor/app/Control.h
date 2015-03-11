#ifndef  _CONTROL_H_
#define  _CONTROL_H_
#include "common.h"
#include "AngGet.h"
#include "common.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "datastructure.h"
// #define CONTROL_PERIOD	5 //电机的输出周期
// #define SPEED_CONTROL_COUNT 8 //速度控制的分割次数
#define SPEED_CONTROL_PERIOD 83//速度控制的总时间40ms
//#define DIRECTION_CONTROL_COUNT			4  //方向控制是20Ms一次
#define DIRECTION_CONTROL_PERIOD		18//方向控制的总时间


extern CarInfo_TypeDef CarInfo_Now; //当前车子的信息
extern CarControl_TypeDef MotorControl; //电机控制的值
extern AngPID_InitTypeDef Ang_PID; //角度控制的PID结构体
extern TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
//extern SpeedPID_TypeDef Speed_PID;
extern DirPID_TypeDef Dir_PID;
extern SpeedPID_TypeDef Speed_PID;

typedef struct 
{
	int Left;
	int Right;
}Speed_Data_Struct;
Speed_Data_Struct* SpeedGet(int Mode);
void MotorControl_Out(void);
void AngleControlValueCalc(void);
void SpeedControlValueCalc(void);
void DirControlValueCale(void);
void BeepBeepBeep(int Times);






#endif
