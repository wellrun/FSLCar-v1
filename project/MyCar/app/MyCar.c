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
#include "MMA8451_moni.h"
#include "oled.h"
//#include "MPU6050.h"
#include "L3G4200.h"
//#include "DataScope_DP.h"
/*选了几个引脚来判断时序是否正常
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
#define CAR_STAND_ANG_MAX 10
#define CAR_STAND_ANG_MIN -40

extern float Dir_AngSpeed ;
char CarStandFlag = 1;
CarInfo_TypeDef CarInfo_Now;
CarControl_TypeDef MotorControl; //存储电机控制的值
short acc_x, gyro_2;
float tempfloat = 0;//临时变量,没有意义
float Ang_dt = 0.005;//全局变量,所有需要周期的都是这个,一个周期20ms
float Speed_Dt = 0.04;//速度的周期
signed char Beep = 0;
int Beep_TimeMs = 0;
#define ScopeDataNum 4
#define OutDataLen (ScopeDataNum*4+4)
#define PageDateLen (4*9)
uint8 OUTDATA[OutDataLen] =
{ 0x03, 0xfc, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 0xfc, 0x03 }; //示波器
extern float GravityAngle; //重力角
extern short TimeFlag_5Ms, TimeFlag_40Ms, TimeFlag_20Ms, TimeFlag_2Ms;
extern unsigned char CCDReady;
extern unsigned char CCDTimeMs;//时间片标志
extern uint8 debugerConnected; //是否连接到调试器
extern TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
extern float AngleIntegraed;//对角速度积分的角度值
extern uint8 IntegrationTime;
extern float SpeedSet_Variable ;
extern float IntSum;
extern float Dir_AngSpeed ;
//--控制区
 uint8 Debuger = 1;
 uint8 CCDOn = 1;
 uint8 CCDSendImage = 0;
 uint8 AngleCale = 1;
 uint8 AngDataSend =0;
 signed char CCDtoPC = 1;//为0发送到山外调试器,为1发送到蓝宙ccd或者调试器


 signed char CCDSendToDebuger = 0;//发送到调试器
//控制区结束
char WhichCCD=0;
uint8 AngSendCount = 0; //控制角度的发送次数
unsigned char testbyte[4];
unsigned char TempArr[128];
unsigned char ShanWaiCCD[128 * 2];
char CCDDataSendOK = 0;

char CCDDataSendStart = 0;
char AngData_Ready = 0;
char AngDataSendOK = 1;
char AngSendToDebuger = 0;//发送数据到调试器,默认不发,与调试器建立连接后发送
char ControlByteRcvStart = 0;
char ControlByteCnt = 0;
signed char CarDownFlag = 0;
unsigned char AngDataSendTEMP = 0;//在修改前用来存放标志位的临时状态.擦擦擦擦擦擦
int ControlRcvErrorCnt = 0;//控制字接受错误计数
unsigned char ControlByte[7];
unsigned char AngTemp5b[5] = { 0xdd};
#define ANG_TO_DEBUGER_TIME 120000
#define InitData_Delay_Time 3000
unsigned short InitDataDelayCnt = 0;
signed char InitDataTimesUp = 0;
unsigned int AngToDebugerDelay = 0;//发送角度时间的计数,计数多少个周期才发送数据
char AngToDebugerTimesUp = 0;//发送角度的时间到
/**********************/
char DebugerDataStart = 0;//开始发送数据
char ShakeHandFlag = 0;//是否握手
char WaitFlag = 1;//等待周期结束,置1
unsigned char DTUDataCnt = 0;//透传接受到的数据计数
uint8 Temp1B = 0;
char DataReciveStart = 0;
char CarStop = 0;//强制停车的标志
int DebugerErrorCnt = 0;
short TimeFlag_5Ms, TimeFlag_40Ms, TimeFlag_20Ms, TimeFlag_2Ms;
short TimerMsCnt = 0,AngTimes=0;
extern short SpeedControlPeriod, DirectionConrtolPeriod;
uint8 DebugerByte36[PageDateLen];
extern void Beep_Isr(void);
extern void AngleIntegration(float Anglespeed);
extern float GravityAngle, GyroscopeAngleSpeed;
	float Voltage = 0;
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

void Speed_Change(void)//测试速度pi用..需要配合示波器
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
  uint8 Whoami=0;
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
			LPLD_GPIO_Toggle_b(PTC, Scope4Ms);
			AngleGet();
			AngleControlValueCalc();
			AngData_Ready = 1;
			Status_Check();
			if (CarStandFlag == 1 && CarStop == 0)
			{
				MotorControl_Out(); //输出电机控制的值
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
		if (AngTimes == 4)
		{
			Whoami = MMA8451_ReadByte(MMA8451_REG_WHOAMI);
			if (Whoami!=0x1a)
			{
				LPLD_GPIO_Output_b(PTE, 5, 1);
			}
		}
}


void CCDCP(void)
{
	
	LPLD_GPIO_Toggle_b(PTC, ScopeCCDReady);
	if (CCDOn == 1)
	{
		//Dir_AngSpeed = L3G4200_GetResult(OUT_Z_L)*Dir_SpeedRatio;
        
		CCD_Median_Filtering();
		CCD_GetLine_Main();
		CCD_GetLine_Slave();
		CCD_Deal_Both();
		//CCD_GetLine();
		if (CCDMain_Status.InitOK == 0)
		{
			CCDLineInit();
		}
		else
		{
			CCD_ControlValueCale();
			DirControlValueCale();//方向控制
			if (CarStandFlag == 1 && CarStop == 0)
			{
				MotorControl_Out();
			}
		}
		CCDDataSendStart = 1;

	/*	showimage(CCDM_Arr);
        LED_P6x8Str(0,0,"Voltage  ");
		LED_PrintValueF(70, 0, Voltage, 4);*/
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

	int Voltage_Cnt = 0;
	Struct_Init(); //初始各种结构体的值
	CarInit();
	LED_Init();
	/*while (1)
	{
		LPLD_GPIO_Toggle_b(PTC, 14);
		MPU6050_GetData(ACCEL_YOUT_H);
		MPU6050_GetData(GYRO_XOUT_H);
	}*/
	//LPLD_Flash_Init(); //初始化EEPROM,所有的初始化数据保存在EEPROM的第60个扇区
	//Flash_WriteTest(); 测试flash区

	//Timer_Init(); //初始化程序时间计数器
	while (1)
	{
		//更改了时间片的模式
		if (TimeFlag_40Ms == 1)
		{
			TimeFlag_40Ms = 0;
			LPLD_GPIO_Toggle_b(PTC, Scope40Ms);
			LPLD_GPIO_Toggle_b(PTA, 17);//一闪一闪亮晶晶
            //LPLD_GPIO_Output_b(PTA,17,0);
			SpeedControlValueCalc();
			Voltage_Cnt++;
			if (Voltage_Cnt>10)
			{
				Voltage_Cnt = 0;
			}
		}
		if (CCDReady==1)
		{
			CCDReady = 0;
			CCDCP();
		}

		if (CCDSendImage == 1 &&CCDtoPC==1)//发送到调试器
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

		if (CCDSendImage == 1 && CCDtoPC==0)//发送到山外调试器
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
					ShanWaiCCD[CCDMain_Status.LeftPoint] = 250;
					ShanWaiCCD[CCDMain_Status.RightPoint] = 250;
					ShanWaiCCD[CCDSlave_Status.LeftPoint + 128] = 250;
					ShanWaiCCD[CCDSlave_Status.RightPoint + 128] = 250;
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
					//调直立用
  					/*Float2Byte(&CarInfo_Now.CarAngle, OUTDATA, 2);
  					tempfloat = CarInfo_Now.CarAngSpeed;
  					Float2Byte(&tempfloat, OUTDATA, 10);
  					Float2Byte(&AngleIntegraed, OUTDATA, 6);
					tempfloat = GravityAngle;
  					Float2Byte(&tempfloat, OUTDATA, 14);*/

					//调速度PI
					tempfloat = (float)Speed_PID.SpeedSet;
 					Float2Byte(&tempfloat, OUTDATA, 2);
 					tempfloat = Dir_AngSpeed;
					Float2Byte(&tempfloat, OUTDATA, 6);
 					tempfloat = (float)CarInfo_Now.CarSpeed;
 					Float2Byte(&tempfloat, OUTDATA, 10);
 					tempfloat = (float)TempValue.SpeedOutValue;
 					Float2Byte(&tempfloat, OUTDATA, 14);


					//调方向和速度
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

					//模糊控制
// 					tempfloat = (float)Dir_PID.ThisError;
// 					Float2Byte(&tempfloat, OUTDATA, 2);
// 					tempfloat = (float)CarInfo_Now.CarSpeed;
// 					Float2Byte(&tempfloat, OUTDATA, 10);
// 					tempfloat = (float)Dir_AngSpeed;
// 					Float2Byte(&tempfloat, OUTDATA, 6);
// 					tempfloat = (float)Speed_PID.SpeedSet;
// 					Float2Byte(&tempfloat, OUTDATA, 14);
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
				else if (Temp1B == 0xf6)//开始接受控制字,接收控制字的时候取消和调试器的连接
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
		if (DataReciveStart == 1 && Debuger == 0)//数据接受要互斥
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
					//从上到下依次7个参数
					CCDSendImage = ControlByte[0];//这个参数不应该改,如果调试器需要ccd图像的时候会置一,如果希望图像发回电脑,
													//则此参数为1,且CCDSendToDebuger为0
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
		if (LPLD_GPIO_Input_b(PTB, 20) == 0)//发送数据到上位机切换模式
		{
			Debuger = 0;
			KeyChanged = 0;
			if (LPLD_GPIO_Input_b(PTB, 21) == 0)
			{
				CCDSendImage = 1;
				AngDataSend = 0;//发送CCD图像
				CCDtoPC = 0;
			}
			else
			{
				CCDSendImage = 0;
				AngDataSend = 1;//发送角度图像
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
		if (LPLD_GPIO_Input_b(PTB, 22) == 1 && LPLD_GPIO_Input_b(PTB, 23) == 1)//这段判定用于修改积分时间
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