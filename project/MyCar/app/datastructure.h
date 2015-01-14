#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H
#include "common.h"

#define NumOfFloat	8
#define DATA_SIZE	NumOfFloat*4
#define FLASH_SECTOR   (60)
#define FLASH_ADDR     (FLASH_SECTOR*2048)
#define NumOfInt	2
extern uint8 FlashReadBuffer[DATA_SIZE];
extern uint8 FlashWriteBuffer[DATA_SIZE];
extern float FlashFloatBuffer[NumOfFloat];
extern int FlashIntBuffer[NumOfInt];

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
	float SpeedSet; //设定的速度

	float Kp;
	float Ki;
	float Kd;

	float IntegralSum;//积分的误差
	float ThisError;
	/*float LastError;*/
	float OutValue;//本次的输出控制量
//	float LastOutValue;//上次的输出控制量

	float OutMax;//积分饱和值
	float OutMin;//积分饱和值

}SpeedPID_TypeDef;

typedef struct 
{
	float Kp;
	float Kd;

	float iError;
	float LastError;
	float OutValue;
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
} CarInfo_TypeDef; //存当前车的信息

typedef struct
{
	float AngControl_OutValue;
	float Dir_RightOutValue;
	float Dir_LeftOutValue;
	float DirOutValue;
	float DirOutValue_New;
	float DirOutValue_Old;
	float SpeedOutValue;//
	float New_SpeedOutValue;//
	float Old_SpeedOutValue;//
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
