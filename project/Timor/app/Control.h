#ifndef  _CONTROL_H_
#define  _CONTROL_H_
#include "common.h"
#include "AngGet.h"
#include "common.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "datastructure.h"
// #define CONTROL_PERIOD	5 //������������
// #define SPEED_CONTROL_COUNT 8 //�ٶȿ��Ƶķָ����
#define SPEED_CONTROL_PERIOD 83//�ٶȿ��Ƶ���ʱ��40ms
//#define DIRECTION_CONTROL_COUNT			4  //���������20Msһ��
#define DIRECTION_CONTROL_PERIOD		18//������Ƶ���ʱ��


extern CarInfo_TypeDef CarInfo_Now; //��ǰ���ӵ���Ϣ
extern CarControl_TypeDef MotorControl; //������Ƶ�ֵ
extern AngPID_InitTypeDef Ang_PID; //�Ƕȿ��Ƶ�PID�ṹ��
extern TempOfMotor_TypeDef TempValue; //��ʱ�洢�ǶȺ��ٶȿ��Ƹ�������Ľṹ��
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
