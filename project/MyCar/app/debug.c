#include "debug.h"
#include "datastructure.h"
#include "Communicate.h"
#include "angget.h"
#include "control.h"
/*
 ��Ҫ���Ե�ֵ,�ŵ�����׼�����͵�����������
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

extern AngPID_InitTypeDef Ang_PID; //��������PID����
extern SpeedPID_TypeDef Speed_PID;
extern TempOfMotor_TypeDef TempValue; //��ʱ�洢�ǶȺ��ٶȿ��Ƹ�������Ľṹ��
unsigned int Code_Count = 0;

void SaveAllDateToFlash(void);

void Timer_Init(void)
{
	Code_Timer_Struct.PIT_Pitx = PIT1;
	Code_Timer_Struct.PIT_PeriodUs = 100;
	Code_Timer_Struct.PIT_Isr = PIT3_ISR;
	LPLD_PIT_Init(Code_Timer_Struct);
	LPLD_PIT_EnableIrq(Code_Timer_Struct);
}

void PIT3_ISR(void)
{
	Code_Count++;
}

void Timer_ReSet(void)
{
	Code_Count = 0;
}

void Struct_Init(void) //��������Գ�ʼ����,��׼���õĲ�������������
{
	Ang_PID.Kp = 190; //����
	Ang_PID.Kd = 95; //΢��
	Ang_PID.AngSet = 76.5; //����ʱ������һ��
	Ang_PID.AngSpeedSet = 0.00;

	Speed_PID.OutMax = 4000;
	Speed_PID.OutMin = -4000;
	Speed_PID.Kp = 0.0;
	Speed_PID.Ki = 0.000;
	Speed_PID.SpeedSet = 0;
	Speed_PID.IntegralSum = 0;

	Dir_PID.Kp = 0.0;
	Dir_PID.Kd = 0.000;

	TempValue.AngControl_OutValue = 0;
	TempValue.Dir_RightOutValue = 0;
	TempValue.Dir_LeftOutValue = 0;
	TempValue.SpeedOutValue = 0;
}

void PagePrepare(void)
{

}

void UART5_RxIsr(void)
{
}

void SaveAllDateToFlash(void)
{

}

void ReciveArr(uint8 *ch, uint8 len)
{
	while (len--)
	{
		*ch = LPLD_UART_GetChar(UART5);
		ch++;
                if(len>=0xa0)
                  break;
	}
}
void SendFlashData(uint8 sendcount)
{

}
