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
	Ang_PID.Kp = 700; //����
	/*Ang_PID.Integral = 0; //����*/
	Ang_PID.Kd = 0.01; //΢��
	Ang_PID.AngSet = 37.3; //����ʱ������һ��
	Ang_PID.AngSpeedSet = 0.00;
	//AngToMotorRatio = 90;
//	Ang_PID.SetPoint=0;//�趨��ֵ
//	Ang_PID.LastError=0;//��һ�ε����
//	Ang_PID.PrevError=0;//���ϴε����

	Speed_PID.OutMax = 8000;
	Speed_PID.OutMin = -8000;
	Speed_PID.Kp = 0.7;
	Speed_PID.Ki = 0.5;
	Speed_PID.SpeedSet = 0;
	Speed_PID.IntegralSum = 0;

	Dir_PID.Kp = 2.5;
	Dir_PID.Kd = 0.5;

	TempValue.AngControl_OutValue = 0;
	TempValue.Dir_RightOutValue = 0;
	TempValue.Dir_LeftOutValue = 0;
	TempValue.SpeedOutValue = 0;
}

void PagePrepare(void)
{
	page1.AngSet = Ang_PID.AngSet;
	page1.AngSpeedSet = Ang_PID.AngSpeedSet;
	//page1.AngToMotorRatio = AngToMotorRatio;
	page1.Kp = Ang_PID.Kp;
	page1.Kd = Ang_PID.Kd;
}

void UART5_RxIsr(void)
{
}

void SaveAllDateToFlash(void)
{
	FlashFloatBuffer[0]=Ang_PID.AngSet;
	FlashFloatBuffer[1]=Ang_PID.Kp;
	FlashFloatBuffer[2]=Ang_PID.Kd;
	//FlashFloatBuffer[3]=AngToMotorRatio;
	Flash_WriteAllData();
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
	PagePrepare();
	switch (sendcount)
	{
	case Data_AngSet:
		Float2Byte(&page1.AngSet, SendTemp8b, 3);
		SendTemp8b[1] = 0x08;
		SendTemp8b[2] = sendcount;
		LPLD_UART_PutCharArr(UART5, SendTemp8b, SendTemp8b[1]);
		break;
	case Data_AngPID_D:
		Float2Byte(&page1.Kd, SendTemp8b, 3);
		SendTemp8b[1] = 0x08;
		SendTemp8b[2] = sendcount;
		LPLD_UART_PutCharArr(UART5, SendTemp8b, SendTemp8b[1]);
		break;
	case Data_AngPID_P:
		Float2Byte(&page1.Kp, SendTemp8b, 3);
		SendTemp8b[1] = 0x08;
		SendTemp8b[2] = sendcount;
		LPLD_UART_PutCharArr(UART5, SendTemp8b, SendTemp8b[1]);
		break;
	case Data_AngToMotor_Ratio:
		//Float2Byte(&page1.AngToMotorRatio, SendTemp8b, 3);
		SendTemp8b[1] = 0x08;
		SendTemp8b[2] = sendcount;
		LPLD_UART_PutCharArr(UART5, SendTemp8b, SendTemp8b[1]);
		break;
	case Data_AngNow:
		Float2Byte(&CarInfo_Now.CarAngle, SendTemp8b, 3);
		SendTemp8b[1] = 0x08;
		SendTemp8b[2] = sendcount;
		LPLD_UART_PutCharArr(UART5, SendTemp8b, SendTemp8b[1]);
		break;
	}

	if (sendcount >= DATANumMAX) //ֻ�����һҳ��5��ֵ
	{
		Temp4b[0] = 0xf1;
		Temp4b[1] = 0x04;
		Temp4b[2] = K60SendDataComplete;
		Temp4b[3] = 0xf2;
		LPLD_UART_PutCharArr(UART5, Temp4b, Temp4b[1]);
		//break;
	}

}
