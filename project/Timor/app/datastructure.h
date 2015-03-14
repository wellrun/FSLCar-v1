#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H
#include "common.h"


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
	float SpeedSet; //�趨���ٶ�]
	float SpeedSetTemp;
	float Kp;
	float Ki;
	float Kd;
	float iError;
	//float IntegralSum_Left;//���ֵ����
	//float OutValue_Left;//���ε����������
	//float IntegralSum_Right;//���ֵ����
	//float OutValue_Right;//���ε����������
	//float OutMax;//���ֱ���ֵ
	//float OutMin;//���ֱ���ֵ

	float ThisError_Left;
	float LastError_Left;
	float PreError_Left;
	float OutValueSum_Left;//������


	float ThisError_Right;
	float LastError_Right;
	float PreError_Right;
	float OutValueSum_Right;//������



	float ThisError;
	float LastError;
	float PreError;
	float OutValueSum;//������
	float IntegralSum;//���ֵ����
	float OutValue;//���ε����������

	

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
	//��ͨ
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
	float SpeedSet;
	float SpeedSetTemp;
} CarInfo_TypeDef; //�浱ǰ������Ϣ

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
