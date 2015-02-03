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
//#include "MPU6050.h"
//#include "L3G4200.h"
//#include "DataScope_DP.h"
/*ѡ�˼����������ж�ʱ���Ƿ�����
PC12-2Ms;
PC13-5Ms;
PC14-20Ms;
PC15-40Ms;
PC16-CCDReady(20Ms);
------------------------*/
#define Scope5Ms 12
#define Scope4Ms 13
#define Scope20Ms 14
#define Scope40Ms 15
#define ScopeCCDReady 16
#define CCD2
#define CAR_STAND_ANG_MAX 70
#define CAR_STAND_ANG_MIN 10
char CarStandFlag = 1;
CarInfo_TypeDef CarInfo_Now;
CarControl_TypeDef MotorControl; //�洢������Ƶ�ֵ
short acc_x, gyro_2;
float tempfloat = 0;//��ʱ����,û������
float Ang_dt = 0.005;//ȫ�ֱ���,������Ҫ���ڵĶ������,һ������20ms
float Speed_Dt = 0.04;//�ٶȵ�����
signed char Beep = 0;
int Beep_TimeMs = 0;
#define ScopeDataNum 4
#define OutDataLen (ScopeDataNum*4+4)
#define PageDateLen (4*9)
uint8 OUTDATA[OutDataLen] =
{ 0x03, 0xfc, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 0xfc, 0x03 }; //ʾ����
extern float GravityAngle; //������
extern short TimeFlag_5Ms, TimeFlag_40Ms, TimeFlag_20Ms, TimeFlag_2Ms;
extern unsigned char CCDReady;
extern unsigned char CCDTimeMs;//ʱ��Ƭ��־
extern uint8 debugerConnected; //�Ƿ����ӵ�������
extern TempOfMotor_TypeDef TempValue; //��ʱ�洢�ǶȺ��ٶȿ��Ƹ�������Ľṹ��
extern float AngleIntegraed;//�Խ��ٶȻ��ֵĽǶ�ֵ
extern uint8 IntegrationTime;
extern float SpeedSet_Variable ;
extern float IntSum;
extern float Dir_AngSpeed ;
//--������
 uint8 Debuger = 1;
 uint8 CCDOn = 1;
 uint8 CCDSendImage = 0;
 uint8 AngleCale = 1;
 uint8 AngDataSend =0;
 signed char CCDtoPC = 1;//Ϊ0���͵�ɽ�������,Ϊ1���͵�����ccd���ߵ�����


 signed char CCDSendToDebuger = 0;//���͵�������
//����������
char WhichCCD=0;
uint8 AngSendCount = 0; //���ƽǶȵķ��ʹ���
unsigned char testbyte[4];
unsigned char TempArr[128];
unsigned char ShanWaiCCD[128 * 2];
char CCDDataSendOK = 0;

char CCDDataSendStart = 0;
char AngData_Ready = 0;
char AngDataSendOK = 1;
char AngSendToDebuger = 0;//�������ݵ�������,Ĭ�ϲ���,��������������Ӻ���
char ControlByteRcvStart = 0;
char ControlByteCnt = 0;
signed char CarDownFlag = 0;
unsigned char AngDataSendTEMP = 0;//���޸�ǰ������ű�־λ����ʱ״̬.������������
int ControlRcvErrorCnt = 0;//�����ֽ��ܴ������
unsigned char ControlByte[7];
unsigned char AngTemp5b[5] = { 0xdd};
#define ANG_TO_DEBUGER_TIME 120000
#define InitData_Delay_Time 3000
unsigned short InitDataDelayCnt = 0;
signed char InitDataTimesUp = 0;
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
short TimeFlag_5Ms, TimeFlag_40Ms, TimeFlag_20Ms, TimeFlag_2Ms;
short TimerMsCnt = 0,AngTimes=0;
extern short SpeedControlPeriod, DirectionConrtolPeriod;
uint8 DebugerByte36[PageDateLen];
extern void Beep_Isr(void);

unsigned char Status_Check(void)
{
	if (((CarInfo_Now.CarAngle < CAR_STAND_ANG_MIN) || (CarInfo_Now.CarAngle > CAR_STAND_ANG_MAX)))
	{
		CarStandFlag = 0;
		Speed_PID.OutValueSum = 0;
		Speed_PID.IntegralSum = 0;
		Speed_PID.OutValueSum_Right = 0;
		Speed_PID.OutValueSum_Left = 0;
		IntSum = 0;
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch4, 0);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch5, 0);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch6, 0);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch7, 0);
		return 1;
	}
	else
	{
		CarStandFlag = 1;
		return 0;
	}
}

void Speed_Change(void)//�����ٶ�pi��..��Ҫ���ʾ����
{
	static int t = 0;
	t++;
	if (t > 10000)
	{
		if (SpeedSet_Variable!=40)
		{
			SpeedSet_Variable = 40;
			t = 0;
		}
		else
		{
			SpeedSet_Variable = 80;
			t = 0;
		}
	}
	
}

void AngleCon_Isr(void)
{
	unsigned char integration_piont;
		CCDTimeMs++;
		TimerMsCnt++;
		AngTimes++;
		SpeedControlPeriod++;
		DirectionConrtolPeriod++;
		integration_piont = DIRECTION_CONTROL_PERIOD - IntegrationTime;
		if (integration_piont == CCDTimeMs)
		{
			StartIntegration_M();
		}
		if (CCDTimeMs >= DIRECTION_CONTROL_PERIOD)
		{
			ImageCapture_M(CCDM_Arr, CCDS_Arr);
			CCDTimeMs = 0;
			CCDReady = 1;
			DirectionConrtolPeriod = 0;
		}
		if (TimerMsCnt >= SPEED_CONTROL_PERIOD)
		{
			TimeFlag_40Ms = 1;
			TimerMsCnt = 0;
			SpeedControlPeriod = 0;
		}
		Beep_Isr();
		if (AngTimes == 5)
		{
			AngTimes = 0;
			if (AngleCale == 1)
			{
				LPLD_GPIO_Toggle_b(PTC, Scope4Ms);
				AngleGet();
				AngleControlValueCalc();
			}
			AngData_Ready = 1;

			if (CarStandFlag == 1 && CarStop == 0)
			{
				MotorControl_Out(); //���������Ƶ�ֵ
			}
			else
			{
				if (CarStandFlag != 0)
				{
					CarStandFlag = 0;
					LPLD_SYSTICK_DelayMs(1000);
				}
				LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch4, 0);
				LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch5, 0);
				LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch6, 0);
				LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch7, 0);
			}
		}
		Status_Check();
		//Speed_Change();
}


void CCDCP(void)
{
	LPLD_GPIO_Toggle_b(PTC, ScopeCCDReady);
	if (CCDOn == 1)
	{
		CCD_Median_Filtering();
		CCD_Deal_Main(CCDM_Arr);
		CCD_Deal_Slave(CCDS_Arr);//��������������ֻ�����ҵ���
		CCD_Deal_Both();
		//CCD_GetLine();
		if (CCDMain_Status.InitOK == 0)
		{
			CCDLineInit();
		}
		else
		{
			CCD_ControlValueCale();
			DirControlValueCale();//�������
			if (CarStandFlag == 1 && CarStop == 0)
			{
				MotorControl_Out();
			}
		}
		CCDDataSendStart = 1;
	}
}

/**********************/
void main(void)
{
	int i = 0;
	signed char KeyChanged = 0;
	short CCDSendPointCnt = 0;
	short ScopeSendPointCnt = 0;
	short DebugDataPointCnt = 0;
	Struct_Init(); //��ʼ���ֽṹ���ֵ
	CarInit();

	/*while (1)
	{
		LPLD_GPIO_Toggle_b(PTC, 14);
		MPU6050_GetData(ACCEL_YOUT_H);
		MPU6050_GetData(GYRO_XOUT_H);
	}*/
	//LPLD_Flash_Init(); //��ʼ��EEPROM,���еĳ�ʼ�����ݱ�����EEPROM�ĵ�60������
	//Flash_WriteTest(); ����flash��

	//Timer_Init(); //��ʼ������ʱ�������
	while (1)
	{
		//������ʱ��Ƭ��ģʽ
		if (TimeFlag_40Ms == 1)
		{
			TimeFlag_40Ms = 0;
			LPLD_GPIO_Toggle_b(PTC, Scope40Ms);
			LPLD_GPIO_Toggle_b(PTA, 17);//һ��һ��������
            //LPLD_GPIO_Output_b(PTA,17,0);
			SpeedControlValueCalc();
		}
		if (CCDReady==1)
		{
			CCDReady = 0;
			CCDCP();
		}

		if (CCDSendImage == 1 &&CCDtoPC==1)//���͵�������
		{
			if (CCDDataSendStart == 1)
			{
				if (CCDDataSendOK == 1)
				{
					CCDDataSendOK = 0;
					for (i = 0; i < 128; i++)
					{
						if (WhichCCD == 0)
						{
							TempArr[i] = CCDM_Arr[i];
						}
						else if (WhichCCD == 1)
						{
							TempArr[i] = CCDS_Arr[i];
						}
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
						LPLD_UART_PutChar(UART5, (uint8)CCDMain_Status.ControlValue);
					}
				}
			}
		}

		if (CCDSendImage == 1 && CCDtoPC==0)//���͵�ɽ�������
		{
			if (CCDDataSendStart == 1)
			{
				if (CCDDataSendOK == 1)
				{
					CCDDataSendOK = 0;
					for (i = 0; i < 128; i++)
					{
						ShanWaiCCD[i] = CCDM_Arr[i];
					}
					for (i = 0; i < 128; i++)
					{
						ShanWaiCCD[i+128] = CCDS_Arr[i];
					}
					LPLD_UART_PutChar(UART5, 0x02);
					LPLD_UART_PutChar(UART5, 0xfd);
					
				}
				LPLD_UART_PutChar(UART5, ShanWaiCCD[CCDSendPointCnt]);
				CCDSendPointCnt++;
				if (CCDSendPointCnt >= 256)
				{
					CCDSendPointCnt = 0;
					LPLD_UART_PutChar(UART5, 0xfd);
					LPLD_UART_PutChar(UART5, 0x02);
					CCDDataSendOK = 1;
					CCDDataSendStart = 0;
				}
			}
		}
		if (AngDataSend == 1 && AngSendToDebuger==0)
		{
			if (AngData_Ready == 1)
			{
				if (AngDataSendOK == 1)
				{
					AngDataSendOK = 0;
					//��ֱ����
// 					Float2Byte(&CarInfo_Now.CarAngle, OUTDATA, 2);
// 					tempfloat = CarInfo_Now.CarAngSpeed;
// 					Float2Byte(&tempfloat, OUTDATA, 10);
// 					Float2Byte(&GravityAngle, OUTDATA, 6);
// 					tempfloat = Dir_AngSpeed;
// 					Float2Byte(&tempfloat, OUTDATA, 14);

					//���ٶ�PI
					tempfloat = (float)SpeedSet_Variable;
					Float2Byte(&tempfloat, OUTDATA, 2);
					tempfloat = (float)TempValue.AngControl_OutValue;
					Float2Byte(&tempfloat, OUTDATA, 6);
					tempfloat = (float)CarInfo_Now.CarSpeed;
					Float2Byte(&tempfloat, OUTDATA, 10);
					tempfloat = (float)TempValue.SpeedOutValue;
					Float2Byte(&tempfloat, OUTDATA, 14);


					//��������ٶ�
// 					tempfloat = (float)TempValue.SpeedOutValue;
// 					Float2Byte(&tempfloat, OUTDATA, 10);
// 					tempfloat = (float)CarInfo_Now.CarSpeed;
// 					Float2Byte(&tempfloat, OUTDATA, 2);
// 					tempfloat = (float)SpeedSet_Variable;
// 					Float2Byte(&tempfloat, OUTDATA, 6);
// 					tempfloat = (float)TempValue.AngControl_OutValue;
// 					Float2Byte(&tempfloat, OUTDATA, 14);

// 					tempfloat = (float)CCDMain_Status.MidPoint;
// 					Float2Byte(&tempfloat, OUTDATA, 2);
// 					tempfloat = (float)CCDSlave_Status.MidPoint;
// 					Float2Byte(&tempfloat, OUTDATA, 6);
// 					tempfloat = (float)SpeedSet_Variable;
// 					Float2Byte(&tempfloat, OUTDATA, 14);
// 					tempfloat = (float)CarInfo_Now.CarSpeed;
// 					Float2Byte(&tempfloat, OUTDATA, 10);
				}
				LPLD_UART_PutChar(UART5, OUTDATA[ScopeSendPointCnt]);
				ScopeSendPointCnt++;
				if (ScopeSendPointCnt >= OutDataLen)
				{
					ScopeSendPointCnt = 0;
					AngDataSendOK = 1;
					AngData_Ready = 0;
				}
			}
		}
		if (AngDataSend == 1 && AngSendToDebuger == 1)
		{
			if (AngToDebugerTimesUp == 1)
			{
				if (AngData_Ready == 1)
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
						AngData_Ready = 0;
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
					AngDataSendTEMP = AngDataSend;
					AngDataSend = 0;
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
                      //  Speed_PID.IntegralSum_Left=0;
						//Speed_PID.IntegralSum_Right = 0;
						Speed_PID.OutValueSum = 0;
						Speed_PID.IntegralSum = 0;
						Speed_PID.OutValueSum_Right = 0;
						Speed_PID.OutValueSum_Left = 0;
						//Dir_PID.OutValueSum = 0;
					}
					else
					{
						CarStop = 1;
						LPLD_UART_PutChar(UART5, 0x4f);
						//Speed_PID.IntegralSum_Left = 0;
						//Speed_PID.IntegralSum_Right = 0;
						Speed_PID.OutValueSum = 0;
						Speed_PID.IntegralSum = 0;
						Speed_PID.OutValueSum_Right = 0;
						Speed_PID.OutValueSum_Left = 0;
						//Dir_PID.OutValueSum = 0;
					}
				}
				else if (Temp1B == 0xdf)
				{
					AngDataSend = 1;
                    AngSendToDebuger=1;
				}
			}
		}
		if (DebugerDataStart == 1 )
		{
			if (InitDataTimesUp == 1)
			{
				InitDataTimesUp = 0;
				LPLD_UART_PutChar(UART5, DebugerByte36[DebugDataPointCnt]);
				DebugDataPointCnt++;
				if (DebugDataPointCnt >= (PageDateLen))
				{
					DebugerDataStart = 0;
					DebugDataPointCnt = 0;
				}
			}
			else
			{
				InitDataDelayCnt++;
				if (InitDataDelayCnt >= InitData_Delay_Time)
				{
					InitDataDelayCnt = 0;
					InitDataTimesUp = 1;
				}
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
					AngDataSend = AngDataSendTEMP;
					Byte2Float(&CarInfo_Now.CarAngle, DebugerByte36, 0);
					Byte2Float(&Ang_PID.Kp, DebugerByte36, 1 * 4);
					Byte2Float(&Ang_PID.Kd, DebugerByte36, 2 * 4);
					Byte2Float(&Ang_PID.AngSet, DebugerByte36, 3 * 4);
					Byte2Float(&Speed_PID.SpeedSet, DebugerByte36, 4 * 4);
					Byte2Float(&Speed_PID.Kp, DebugerByte36, 5 * 4);
					Byte2Float(&Speed_PID.Ki, DebugerByte36, 6 * 4);
					Byte2Float(&Dir_PID.Kp, DebugerByte36, 7 * 4);
					Byte2Float(&Dir_PID.Kd, DebugerByte36, 8 * 4);
					LPLD_SYSTICK_DelayUs(500);
					LPLD_UART_PutChar(UART5, 0xaf);
					
				}
                DebugerErrorCnt=0;
			}
			else
			{
				DebugerErrorCnt++;
				LPLD_SYSTICK_DelayUs(20);
				if (DebugerErrorCnt >= 400000)
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
					WhichCCD = ControlByte[2];
					/*CCDSendToDebuger = ControlByte[3];*/
					LPLD_UART_PutChar(UART5, 0x6f);
				}
			}
		}
		if (LPLD_GPIO_Input_b(PTB, 20) == 0)//�������ݵ���λ���л�ģʽ
		{
			Debuger = 0;
			KeyChanged = 0;
			if (LPLD_GPIO_Input_b(PTB, 21) == 0)
			{
				CCDSendImage = 1;
				AngDataSend = 0;//����CCDͼ��
				CCDtoPC = 0;
			}
			else
			{
				CCDSendImage = 0;
				AngDataSend = 1;//���ͽǶ�ͼ��
				CCDtoPC = 1;
			}
			LPLD_GPIO_Output_b(PTE, 4, 1);
		}
		else
		{

			if (KeyChanged == 0)
			{
				AngDataSend = 0;
				CCDSendImage = 0;
				KeyChanged = 1;
				CCDtoPC = 1;
				Debuger = 1;
			}
			LPLD_GPIO_Output_b(PTE, 4, 0);
		}
		if (LPLD_GPIO_Input_b(PTB, 22) == 1 && LPLD_GPIO_Input_b(PTB, 23) == 1)//����ж������޸Ļ���ʱ��
		{
			IntegrationTime = 3;
		}
		else if (LPLD_GPIO_Input_b(PTB, 22) == 0 && LPLD_GPIO_Input_b(PTB, 23) == 0)
		{
			IntegrationTime = 6;
		}
		else if (LPLD_GPIO_Input_b(PTB, 22) == 1 && LPLD_GPIO_Input_b(PTB, 23) == 0)
		{
			IntegrationTime = 4;
		}
		else if (LPLD_GPIO_Input_b(PTB, 22) == 0 && LPLD_GPIO_Input_b(PTB, 23) == 1)
		{
			IntegrationTime = 5;
		}

	}
}



/*char WatiPeriod(int t)
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
}*/