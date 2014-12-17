#ifndef  _CONTROL_H_
#define  _CONTROL_H_
#include "common.h"
#include "AngGet.h"
#include "common.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "datastructure.h"


extern CarInfo_TypeDef CarInfo_Now; //��ǰ���ӵ���Ϣ
extern CarControl_TypeDef MotorControl; //������Ƶ�ֵ
extern AngPID_InitTypeDef Ang_PID; //�Ƕȿ��Ƶ�PID�ṹ��
extern TempOfMotor_TypeDef TempValue; //��ʱ�洢�ǶȺ��ٶȿ��Ƹ�������Ľṹ��
extern SpeedPID_TypeDef Speed_PID;
extern DirPID_TypeDef Dir_PID;

void MotorControl_Out(void);
void AngleControlValueCalc(void);
void SpeedControlValueCalc(void);
void DirControlValueCale(void);






#endif
