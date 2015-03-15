#include "debug.h"
#include "datastructure.h"
#include "Communicate.h"
#include "angget.h"
#include "control.h"
/*
 需要调试的值,放到这里准备发送到调试器调试
 */
//extern float AngToMotorRatio;
extern CarInfo_TypeDef CarInfo_Now;
uint8 debugerConnected=0;
uint8 Temp1b = 0;
uint8 Temp2b[2] = { 0, 0 };
uint8 Temp3b[3] = { 0, 0, 0 };
uint8 Temp8b[8];
uint8 Temp4b[4] = { 0xf1, 0x04, K60SendDataComplete, 0xf2 };
uint8 SendTemp8b[8] = { 0xf1, 0, 1, 2, 3, 4, 5, 0xf2 };
uint8 Temp35b[35]={0};
AngDataStruct page1;

PIT_InitTypeDef Code_Timer_Struct;

extern AngPID_InitTypeDef Ang_PID; 
//extern SpeedPID_TypeDef Speed_PID;
extern TempOfMotor_TypeDef TempValue; 
unsigned int Code_Count = 0;
extern float AngVar_Arr_Positive[101];
extern float AngVar_Arr_Negative[101];
void Struct_Init(void) //在这里调试初始参数,把准备好的参数发往调试器
{
	int i;
	float  a = 0,b=0;
	Ang_PID.Kp = 420; 
	Ang_PID.Kd =16; 
	Ang_PID.AngSet = 49.5; 
	Speed_PID.SpeedSet = 45;
	Speed_PID.Kp = -70;
	Speed_PID.Ki =-40;
	//Speed_PID.Kd = 0;
	Dir_PID.Kp = 76;
	Dir_PID.Kd =5.2;
	
	a = (3.5 / 20);
	b = (2.0 / 20);
	for (i = 0; i < 100;i++)
	{
		AngVar_Arr_Positive[i] = a*((i+1));
		AngVar_Arr_Negative[i] = b*(i + 1);
	}
}

