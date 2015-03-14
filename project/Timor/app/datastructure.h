#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H
#include "common.h"


typedef struct
{
	float AngSet; //设定的角度
	float AngSpeedSet; //设定的角速度
	float Delta; //误差

	float Kp; //比例常数
	/*	float Integral; //积分常数;*/
	float Kd; //微分常数;

	//float LastError;
	//float PrevError;
} AngPID_InitTypeDef; //角度PID的数据结构

typedef struct
{
	float SpeedSet; //设定的速度]
	float SpeedSetTemp;
	float Kp;
	float Ki;
	float Kd;
	float iError;
	//float IntegralSum_Left;//积分的误差
	//float OutValue_Left;//本次的输出控制量
	//float IntegralSum_Right;//积分的误差
	//float OutValue_Right;//本次的输出控制量
	//float OutMax;//积分饱和值
	//float OutMin;//积分饱和值

	float ThisError_Left;
	float LastError_Left;
	float PreError_Left;
	float OutValueSum_Left;//增量和


	float ThisError_Right;
	float LastError_Right;
	float PreError_Right;
	float OutValueSum_Right;//增量和



	float ThisError;
	float LastError;
	float PreError;
	float OutValueSum;//增量和
	float IntegralSum;//积分的误差
	float OutValue;//本次的输出控制量

	

}SpeedPID_TypeDef;

typedef struct 
{
	float Kp;
	float Kd;

	float Kp_Temp;
	float Kd_Temp;
	float ThisError;
	float LastError;
	float PreError;
	float OutValue;
	float OutValueSum;
	float ControlValue;
	//普通
}DirPID_TypeDef;

typedef struct
{
	int RightMotorOutValue;
	int LeftMotorOutValue;
} CarControl_TypeDef; //用来控制电机

typedef struct
{
	int MotorCounterLeft; //通过编码器读出来
	int MotorCounterRight; //通过编码器读出来
	float LeftSpeed;
	float RightSpeed;
	float CarSpeed;//车子的速度
	float CarAngle;
	float CarAngSpeed;
	float SpeedSet;
	float SpeedSetTemp;
} CarInfo_TypeDef; //存当前车的信息

typedef struct
{
	float AngControl_OutValue;
	float Dir_RightOutValue;
	float Dir_LeftOutValue;
	float DirOutValue;
	float DirOutValue_New;
	float DirOutValue_Old;
	float SpeedOutValueRight;//
	float SpeedOutValueLeft;
	float New_SpeedOutValueRight;//
	float New_SpeedOutValueLeft;
	float Old_SpeedOutValueRight;//
	float Old_SpeedOutValueLeft;

	float SpeedOutValue;//
	float New_SpeedOutValue;
	float Old_SpeedOutValue;

	float Old_AngSet;
	float New_AngSet;
} TempOfMotor_TypeDef;

typedef struct
{
	//float AngToMotorRatio;
	float AngSet; //设定的角度
	float AngSpeedSet; //设定的角速度;
	float Kp; //比例常数
	float Kd; //微分常数;
}AngDataStruct; //第一页需要发送的数据

void Flash_WriteTest(void);
void Flash_DataToBuffer(float data, uint8 num);
void Flash_WriteAllData(void);
uint8 Flash_ReadAllData(void);

#endif
