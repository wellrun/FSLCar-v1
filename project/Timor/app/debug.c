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

extern AngPID_InitTypeDef Ang_PID; //用来调试PID参数
//extern SpeedPID_TypeDef Speed_PID;
extern TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
unsigned int Code_Count = 0;

extern float Dir_Kp_Correction[100];
void Struct_Init(void) //在这里调试初始参数,把准备好的参数发往调试器
{
	int i;

	Ang_PID.Kp = 750; //比例
	Ang_PID.Kd =8; //微分
	Ang_PID.AngSet = 48.8; 

	Speed_PID.SpeedSet = 95;
	Speed_PID.Kp = -184;
	Speed_PID.Ki =-57;
	Speed_PID.Kd = 138;
	Dir_PID.Kp = 70;
	Dir_PID.Kd =5.3;
        
	TempValue.AngControl_OutValue = 0;
	TempValue.Dir_RightOutValue = 0;
	TempValue.Dir_LeftOutValue = 0;

	for (i = 0; i < 50; i++)
	{
		Dir_Kp_Correction[i] = -(((0.007*i) + 0.65)*(i-50));
	}
	for (i = 50; i < 100; i++)
	{
		Dir_Kp_Correction[i] = -((1 - (0.007*(i - 50)))*(i-50));
	}

}

