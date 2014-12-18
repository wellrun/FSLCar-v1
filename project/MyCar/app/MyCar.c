#include "common.h"
#include "Init.h"
#include "angget.h"
#include "math.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "debug.h"
#include "datastructure.h"
#include "Control.h"
#include "CCD.h"
#include "Communicate.h"
//#include "DataScope_DP.h"
/*ѡ�˼����������ж�ʱ���Ƿ�����
PC12-2Ms;
PC13-4Ms;
PC14-20Ms;
PC15-80Ms;
PC16-CCDReady(20Ms);
------------------------*/
#define Scope2Ms 12
#define Scope4Ms 13
#define Scope20Ms 14
#define Scope80Ms 15
#define ScopeCCDReady 16

CarInfo_TypeDef CarInfo_Now;
CarControl_TypeDef MotorControl; //�洢������Ƶ�ֵ
short acc_x, gyro_2;
float tempfloat = 0;//��ʱ����,û������
float dt = 0.019;//ȫ�ֱ���,������Ҫ���ڵĶ������,һ������20ms
#define ScopeDataNum 4
#define OutDataLen (ScopeDataNum*4+4)
#define PageDateLen (4*9)
uint8 OUTDATA[OutDataLen] =
{ 0x03, 0xfc, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 0xfc, 0x03 }; //ʾ����
extern float GravityAngle; //������
extern char TimeFlag_4Ms, TimeFlag_80Ms, TimeFlag_20Ms, TimeFlag_2Ms;
extern unsigned char CCDReady;
extern unsigned char CCDTimeMs;//ʱ��Ƭ��־
extern uint8 debugerConnected; //�Ƿ����ӵ�������
extern TempOfMotor_TypeDef TempValue; //��ʱ�洢�ǶȺ��ٶȿ��Ƹ�������Ľṹ��
extern float AngleIntegraed;//�Խ��ٶȻ��ֵĽǶ�ֵ
//--������
uint8 Debuger = 1;
uint8 CCDOn = 1;
uint8 CCDSendImage = 0;
uint8 AngleCale = 1;
uint8 AngDataSend = 0;
//����������
uint8 AngSendCount = 0; //���ƽǶȵķ��ʹ���
unsigned char testbyte[4];
unsigned char TempArr[128];
char CCDDataSendOK = 0;
char CCDSendToDebuger = 0;//���͵�������
char CCDDataSendStart = 0;
char AngDataStart = 0;
char AngDataSendOK = 1;
char AngSendToDebuger = 0;//�������ݵ�������,Ĭ�ϲ���,��������������Ӻ���
char ControlByteRcvStart = 0;
char ControlByteCnt = 0;
int ControlRcvErrorCnt = 0;//�����ֽ��ܴ������
unsigned char ControlByte[7];
unsigned char AngTemp5b[5] = { 0xdd};
#define ANG_TO_DEBUGER_TIME 65000
unsigned int AngToDebugerDelay = 0;//���ͽǶ�ʱ��ļ���,�������ٸ����ڲŷ�������
char AngToDebugerTimesUp = 0;//���ͽǶȵ�ʱ�䵽
/**********************/
char DebugerDataStart = 0;//��ʼ��������
char ShakeHandFlag = 0;//�Ƿ�����
char WaitFlag = 1;//�ȴ����ڽ���,��1
unsigned char DTUDataCnt = 0;//͸�����ܵ������ݼ���
uint8 Temp1B = 0;
char DataReciveStart = 0;
char CarStop = 0;//ǿ��ͣ���ı�־
int DebugerErrorCnt = 0;

uint8 DebugerByte36[PageDateLen];
void WatiPeriod(int t);
/**********************/
void main(void)
{
	int i = 0;
	short CCDSendPointCnt = 0;
	short ScopeSendPointCnt = 0;
	short DebugDataPointCnt = 0;
	Struct_Init(); //��ʼ���ֽṹ���ֵ
	CarInit();
	LPLD_Flash_Init(); //��ʼ��EEPROM,���еĳ�ʼ�����ݱ�����EEPROM�ĵ�60������
	//	Flash_WriteTest(); ����flash��

	//Timer_Init(); //��ʼ������ʱ�������
	Flash_ReadAllData(); //��Flash�ж�ȡ���е�����
	Struct_Init();
	while (1)
	{
		//������ʱ��Ƭ��ģʽ
		if (CCDReady == 1)
		{
			CCDReady = 0;
			LPLD_GPIO_Toggle_b(PTC, ScopeCCDReady);
			//Timer_ReSet();  //���ó���ʱ�������
			if (CCDOn == 1)
			{
				ImageCapture(ccd_array);
				//CalculateIntegrationTime();
				CCD_Deal_Main(ccd_array);
				DirControlValueCale();//�������
				CCDDataSendStart = 1;
			}
		}
		if (TimeFlag_4Ms == 1)
		{
			TimeFlag_4Ms = 0;
			LPLD_GPIO_Toggle_b(PTC, Scope4Ms);
			if (AngleCale == 1)
			{
				AngleGet();
				AngleControlValueCalc();
				AngDataStart = 1;
			}
		}
		if (TimeFlag_80Ms == 1)
		{
			TimeFlag_80Ms = 0;
			LPLD_GPIO_Toggle_b(PTC, Scope80Ms);
			LPLD_GPIO_Toggle_b(PTA, 17);//һ��һ��������
			SpeedControlValueCalc();//�ٶȱջ�,�ȵ�ֱ��,�ٵ��ٶȱջ�
		}
		if (TimeFlag_2Ms == 1)
		{
			TimeFlag_2Ms = 0;
			LPLD_GPIO_Toggle_b(PTC, Scope2Ms);
			if (CarStop == 0)//���ͣ����־λΪ1,��ֹͣ������ֵ
			{
				MotorControl_Out(); //���������Ƶ�ֵ
			}
			else
			{
				TempValue.AngControl_OutValue = 0;
				TempValue.Dir_LeftOutValue = 0;
				TempValue.Dir_RightOutValue = 0;
				TempValue.SpeedOutValue = 0;
				MotorControl_Out();
			}
		}
		if (CCDSendImage == 1)
		{
			if (CCDDataSendStart == 1)
			{
				if (CCDDataSendOK == 1)
				{
					CCDDataSendOK = 0;
					for (i = 0; i < 128; i++)
					{
						TempArr[i] = ccd_array[i];
					}
					/*TempArr[CCDMain_Status.LeftPoint] = 250;
					TempArr[CCDMain_Status.RightPoint] = 250;*/
					LPLD_UART_PutChar(UART5, '*');
					if (CCDSendToDebuger == 0)
					{
						LPLD_UART_PutChar(UART5, 'L');
						LPLD_UART_PutChar(UART5, 'D');

						SendHex(0);
						SendHex(0);
						SendHex(0);
						SendHex(0);
					}
				}
				if (CCDSendToDebuger == 1)
				{
					LPLD_UART_PutChar(UART5, TempArr[CCDSendPointCnt]);
				}
				else
				{
					SendHex(TempArr[CCDSendPointCnt]);
				}
				CCDSendPointCnt++;
				if (CCDSendPointCnt >= 128)
				{
					CCDSendPointCnt = 0;
					if (CCDSendToDebuger == 0)
					{
						SendHex(0);
					}
					LPLD_UART_PutChar(UART5, '#');
					CCDDataSendOK = 1;
					CCDDataSendStart = 0;
					if (CCDSendToDebuger == 1)
					{
						LPLD_UART_PutChar(UART5, CCDMain_Status.LeftPoint);
						LPLD_UART_PutChar(UART5, CCDMain_Status.RightPoint);
						LPLD_UART_PutChar(UART5, CCDMain_Status.ControlValue);
					}
				}
			}
		}
		if (AngDataSend == 1 && AngSendToDebuger==0)
		{
			if (AngDataStart == 1)
			{
				if (AngDataSendOK == 1)
				{
					AngDataSendOK = 0;
					//tempfloat = TempValue.AngControl_OutValue / 10.0;
					// 				tempfloat = AngleIntegraed;
					// 				Float2Byte(&tempfloat, OUTDATA, 2);
					Float2Byte(&CarInfo_Now.CarAngle, OUTDATA, 2);
					//Float2Byte(&CarInfo_Now.CarAngSpeed, OUTDATA, 10);
					tempfloat = CarInfo_Now.CarAngSpeed;
					Float2Byte(&tempfloat, OUTDATA, 10);
					Float2Byte(&GravityAngle, OUTDATA, 6);
					tempfloat = (float)gyro_2;
					Float2Byte(&tempfloat, OUTDATA, 14);
					/*LPLD_UART_PutCharArr(UART5, OUTDATA, 20);*/
				}
				LPLD_UART_PutChar(UART5, OUTDATA[ScopeSendPointCnt]);
				ScopeSendPointCnt++;
				if (ScopeSendPointCnt >= OutDataLen)
				{
					ScopeSendPointCnt = 0;
					AngDataSendOK = 1;
					AngDataStart = 0;
				}
			}
		}
		if (AngDataSend == 1 && AngSendToDebuger == 1)
		{
			if (AngToDebugerTimesUp == 1)
			{
				if (AngDataStart == 1)
				{
					if (AngDataSendOK == 1)
					{
						AngDataSendOK = 0;
						Float2Byte(&CarInfo_Now.CarAngle, AngTemp5b, 1);
						AngTemp5b[0] = 0xdd;
					}
					LPLD_UART_PutChar(UART5, AngTemp5b[ScopeSendPointCnt]);
					ScopeSendPointCnt++;
					if (ScopeSendPointCnt >= 5)
					{
						ScopeSendPointCnt = 0;
						AngDataSendOK = 1;
						AngDataStart = 0;
						AngToDebugerTimesUp = 0;
					}
				}
			}
			else
			{
				AngToDebugerDelay++;
				if (AngToDebugerDelay >= ANG_TO_DEBUGER_TIME)
				{
					AngToDebugerDelay = 0;
					AngToDebugerTimesUp = 1;
				}
			}
		}
		if (Debuger == 1)
		{
			if (LPLD_UART_GetChar_Present(UART5))
			{
				Temp1B = LPLD_UART_GetChar(UART5);
				if (Temp1B == 0xFE)
				{
					ShakeHandFlag = 1;
					CCDSendImage = 0;
					AngDataSend = 0;
					AngSendToDebuger = 1;
					LPLD_UART_PutChar(UART5, 0xEF);
				}
				else if (Temp1B == 0xFD)
				{
					DebugerDataStart = 1;
					DebugDataPointCnt = 0;
					LPLD_UART_PutChar(UART5, 0xDF);
					Float2Byte(&CarInfo_Now.CarAngle, DebugerByte36, 0);
					Float2Byte(&Ang_PID.Kp, DebugerByte36, 1 * 4);
					Float2Byte(&Ang_PID.Kd, DebugerByte36, 2 * 4);
					Float2Byte(&Ang_PID.AngSet, DebugerByte36, 3 * 4);
					Float2Byte(&Speed_PID.SpeedSet, DebugerByte36, 4 * 4);
					Float2Byte(&Speed_PID.Kp, DebugerByte36, 5 * 4);
					Float2Byte(&Speed_PID.Ki, DebugerByte36, 6 * 4);
					Float2Byte(&Dir_PID.Kp, DebugerByte36, 7 * 4);
					Float2Byte(&Dir_PID.Kd, DebugerByte36, 8 * 4);
					LPLD_SYSTICK_DelayUs(1000);
				}
				else if (Temp1B == 0xf9)//
				{
					DataReciveStart = 1;
					DTUDataCnt = 0;
					LPLD_UART_PutChar(UART5, 0x9f);
					Debuger = 0;
				}
				else if (Temp1B == 0xf8)
				{
					LPLD_UART_PutChar(UART5, 0x8f);
					CCDSendImage = 1;
					CCDSendToDebuger = 1;
				}
				else if (Temp1B == 0xf7)
				{
					LPLD_UART_PutChar(UART5, 0x7f);
					CCDSendImage = 0;
					CCDSendToDebuger = 0;
				}
				else if (Temp1B == 0xf6)//��ʼ���ܿ�����,���տ����ֵ�ʱ��ȡ���͵�����������
				{
					ControlByteRcvStart = 1;
					Debuger = 0;
					ControlByteCnt = 0;
					LPLD_UART_PutChar(UART5, 0x6f);
				}
				else if (Temp1B == 0xf5)
				{
					if (CarStop == 1)
					{
						CarStop = 0;
						LPLD_UART_PutChar(UART5, 0x5f);
					}
					else
					{
						CarStop = 1;
						LPLD_UART_PutChar(UART5, 0x4f);
					}
				}
			}
		}
		if (DebugerDataStart == 1)
		{
			LPLD_UART_PutChar(UART5, DebugerByte36[DebugDataPointCnt]);
			DebugDataPointCnt++;
			if (DebugDataPointCnt >= (PageDateLen))
			{
				DebugerDataStart = 0;
				DebugDataPointCnt = 0;
				AngDataSend = 1;
			}
		}
		if (DataReciveStart == 1 && Debuger == 0)//���ݽ���Ҫ����
		{
			if (LPLD_UART_GetChar_Present(UART5))
			{
				DebugerByte36[DTUDataCnt] = LPLD_UART_GetChar(UART5);
				DTUDataCnt++;
				if (DTUDataCnt >= (PageDateLen))
				{
					DataReciveStart = 0;
					DTUDataCnt = 0;
					Debuger = 1;
					Byte2Float(&CarInfo_Now.CarAngle, DebugerByte36, 0);
					Byte2Float(&Ang_PID.Kp, DebugerByte36, 1 * 4);
					Byte2Float(&Ang_PID.Kd, DebugerByte36, 2 * 4);
					Byte2Float(&Ang_PID.AngSet, DebugerByte36, 3 * 4);
					Byte2Float(&Speed_PID.SpeedSet, DebugerByte36, 4 * 4);
					Byte2Float(&Speed_PID.Kp, DebugerByte36, 5 * 4);
					Byte2Float(&Speed_PID.Ki, DebugerByte36, 6 * 4);
					Byte2Float(&Dir_PID.Kp, DebugerByte36, 7 * 4);
					Byte2Float(&Dir_PID.Kd, DebugerByte36, 8 * 4);
					LPLD_UART_PutChar(UART5, 0xaf);
				}
			}
			else
			{
				DebugerErrorCnt++;
				LPLD_SYSTICK_DelayUs(20);
				if (DebugerErrorCnt >= 40000)
				{
					DebugerDataStart = 0;
					DTUDataCnt = 0;
					Debuger = 1;
					DebugerErrorCnt = 0;
				}
			}
		}
		if (ControlByteRcvStart == 1 && Debuger == 0)
		{
			if (LPLD_UART_GetChar_Present(UART5))
			{
				ControlByte[ControlByteCnt] = LPLD_UART_GetChar(UART5);
				ControlByteCnt++;
				if (ControlByteCnt>=7)
				{
					ControlByteRcvStart = 0;
					Debuger = 1;
					//���ϵ�������7������
					CCDSendImage = ControlByte[0];//���������Ӧ�ø�,�����������Ҫccdͼ���ʱ�����һ,���ϣ��ͼ�񷢻ص���,
													//��˲���Ϊ1,��CCDSendToDebugerΪ0
					if (CCDSendImage == 1)
						CCDSendToDebuger = 0;
					else
						CCDSendToDebuger = 1;
					AngDataSend = ControlByte[1];
					AngSendToDebuger = ControlByte[2];
					/*CCDSendToDebuger = ControlByte[3];*/
					LPLD_UART_PutChar(UART5, 0x6f);
				}
			}
		}

	}
}

void WatiPeriod(int t)
{
	static int WaitCnt;
	if (WaitFlag == 0)
	{
		WaitCnt--;
		if (WaitCnt <= 0)
			WaitFlag = 1;
	}
	else
	{
		WaitCnt = t;
		WaitFlag = 0;
	}
}