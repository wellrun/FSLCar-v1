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
	float AngSet; //�趨�ĽǶ�
	float AngSpeedSet; //�趨�Ľ��ٶ�
	float Delta; //���

	float Kp; //��������
	/*	float Integral; //���ֳ���;*/
	float Kd; //΢�ֳ���;

	//float LastError;
	//float PrevError;
} AngPID_InitTypeDef; //�Ƕ�PID�����ݽṹ

typedef struct
{
	float SpeedSet; //�趨���ٶ�

	float Kp;
	float Ki;
	float Kd;

	float IntegralSum;//���ֵ����
	float ThisError;
	/*float LastError;*/
	float OutValue;//���ε����������
//	float LastOutValue;//�ϴε����������

	float OutMax;//���ֱ���ֵ
	float OutMin;//���ֱ���ֵ

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
} CarControl_TypeDef; //�������Ƶ��

typedef struct
{
	int MotorCounterLeft; //ͨ��������������
	int MotorCounterRight; //ͨ��������������
	float LeftSpeed;
	float RightSpeed;
	float CarSpeed;//���ӵ��ٶ�
	float CarAngle;
	float CarAngSpeed;
} CarInfo_TypeDef; //�浱ǰ������Ϣ

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
	float AngSet; //�趨�ĽǶ�
	float AngSpeedSet; //�趨�Ľ��ٶ�;
	float Kp; //��������
	float Kd; //΢�ֳ���;
}AngDataStruct; //��һҳ��Ҫ���͵�����

void Flash_WriteTest(void);
void Flash_DataToBuffer(float data, uint8 num);
void Flash_WriteAllData(void);
uint8 Flash_ReadAllData(void);

#endif
