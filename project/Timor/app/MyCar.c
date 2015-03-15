
#include "MyCar.h"

#define CAR_STAND_ANG_MAX 73
#define CAR_STAND_ANG_MIN 20

extern float Dir_AngSpeed ;
char CarStandFlag = 1;
CarInfo_TypeDef CarInfo_Now;
CarControl_TypeDef MotorControl; //存储电机控制的值
short acc_x, gyro_2;
float tempfloat = 0;//临时变量,没有意义
//float Ang_dt = 0.002;//全局变量,所有需要周期的都是这个,一个周期20ms
//float Speed_Dt = 0.04;//速度的周期
signed char Beep = 0;
int Beep_TimeMs = 0;
#define ScopeDataNum 8
#define OutDataLen (ScopeDataNum*4+4)
#define PageDateLen (4*9)
uint8 OUTDATA[OutDataLen] =
{ 0x03, 0xfc, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 0xfc, 0x03 }; //示波器
//--控制区
 uint8 Debuger = 1;
 uint8 CCDOn = 1;
 uint8 CCDSendImage = 0;
 uint8 AngleCale = 1;
 uint8 AngDataSend =0;
 signed char CCDtoPC = 1;//为0发送到山外调试器,为1发送到蓝宙ccd或者调试器
 signed char CCDSendToDebuger = 0;//发送到调试器
//控制区结束
char CCDDataSendStart = 0;
char AngToDebugerTimesUp = 0;//发送角度的时间到
/**********************/
char AngData_Ready = 0;
char DebugerDataStart = 0;//开始发送数据
char CarStop = 0;//强制停车的标志
int DebugerErrorCnt = 0;
short Screen_WhichCCDImg = 0;
short TimeFlag_5Ms, TimeFlag_40Ms, TimeFlag_20Ms, TimeFlag_2Ms;
short TimerMsCnt = 0,AngTimes=0;
signed char FlagToPhone = 0;
uint8 Voltage_Display = 0;
float Voltage = 0;
signed char CarStart = 0;//为0未发车.为1已发车
signed char CarStart_Mask = 0;//为1允许发车,为0不允许发车
signed char CarStand_Mask = 0;//为0不允许站立,
unsigned char Status_Check(void)
{
	if (((CarInfo_Now.CarAngle < CAR_STAND_ANG_MIN) || (CarInfo_Now.CarAngle > CAR_STAND_ANG_MAX)))
	{
		CarStandFlag = 0;
		Speed_PID.OutValueSum = 0;
		Speed_PID.IntegralSum = 0;
		Speed_PID.OutValueSum_Right = 0;
		Speed_PID.OutValueSum_Left = 0;
        TempValue.New_SpeedOutValue=0;
		SpeedSet_Variable = 0;
		CarStart = 0;
		CarStart_Mask = 0;
		CarStand_Mask = 0;
		IntSum = 0;
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch0, 0);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch1, 0);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch2, 0);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch3, 0);
		return 1;
	}
	else
	{
		if (CarStandFlag==0)
		{
			if (((CarInfo_Now.CarAngle - Ang_PID.AngSet) <5) || ((CarInfo_Now.CarAngle - Ang_PID.AngSet) >-5))
			{
				if (CarStand_Mask==1)
				{
					CarStandFlag = 1;
				}	
			}
			else
			{
				Speed_PID.OutValueSum = 0;
				Speed_PID.IntegralSum = 0;
				Speed_PID.OutValueSum_Right = 0;
				Speed_PID.OutValueSum_Left = 0;
				TempValue.New_SpeedOutValue = 0;
			}
		}
		if (CarStandFlag==1)
		{
			if (CarStart_Mask==1 && CarStart==0)
			{
				CarStart = 1;
                                SpeedSet_Variable=20;
			}
			else if (CarStart_Mask==1 && CarStart==1)
			{
				SpeedSet_Variable += 0.03;
				if (SpeedSet_Variable >Speed_PID.SpeedSet)
				{
					SpeedSet_Variable = Speed_PID.SpeedSet;
				}
			}
		}
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
void CCDCP(void)
{
	
	if (CCDOn == 1)
	{
		CCD_Median_Filtering();
		CCD_GetLine_Main();
		CCD_GetLine_Slave();
		CCD_Deal_Both();
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
		CCDM_Arr[CCDMain_Status.LeftPoint] = 200;
		CCDM_Arr[CCDMain_Status.RightPoint] = 200;
		CCDS_Arr[CCDSlave_Status.LeftPoint] = 200;
		CCDS_Arr[CCDSlave_Status.RightPoint] = 200;
	}
}
void AngleCon_Isr(void)
{
	unsigned char integration_piont;
	
		CCDTimeMs+=2;
		TimerMsCnt += 2;
		AngTimes += 2;
		SpeedControlPeriod += 2;
		DirectionConrtolPeriod += 2;
		integration_piont = DIRECTION_CONTROL_PERIOD - IntegrationTime;
		if (integration_piont == CCDTimeMs)
		{
			LPLD_GPIO_Output_b(PTC, 0,1);
			StartIntegration_M();
		}
		if (CCDTimeMs >= DIRECTION_CONTROL_PERIOD)
		{
			LPLD_GPIO_Output_b(PTC, 0, 0);
			ImageCapture_M(CCDM_Arr, CCDS_Arr);
			CCDCP();
			CCDTimeMs = 0;
			CCDReady = 1;
			DirectionConrtolPeriod = 0;
		}
		if (TimerMsCnt >= SPEED_CONTROL_PERIOD)
		{
			TimeFlag_40Ms = 1;
			TimerMsCnt = 0;
			SpeedControlPeriod = 0;
			SpeedControlValueCalc();
			LPLD_GPIO_Toggle_b(PTA, 17);//一闪一闪亮晶晶
		}
		Beep_Isr();
		if (AngTimes == 2)
		{
			AngTimes = 0;
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
				LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch0, 0);
				LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch1, 0);
				LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch2, 0);
				LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch3, 0);
                                if (CarStandFlag != 0)
				{
					CarStandFlag = 0;
					//LPLD_LPTMR_DelayMs(3000);
				}
			}
		}
}



	/*测试死区参数*/
	uint32 death1 = 0, death2 = 0, death3= 0, death4 = 0;
void main(void)
{
	int i = 0;
	signed char KeyChanged = 0;
	short CCDSendPointCnt = 0;
	short ScopeSendPointCnt = 0;
	short DebugDataPointCnt = 0;
	char WhichCCD = 0;
	unsigned char TempArr[128];
	unsigned char ShanWaiCCD[128 * 2];
	char CCDDataSendOK = 0;
	char AngDataSendOK = 1;
	char AngSendToDebuger = 0;//发送数据到调试器,默认不发,与调试器建立连接后发送
	char ControlByteRcvStart = 0;
	char ControlByteCnt = 0;
	unsigned char AngDataSendTEMP = 0;//在修改前用来存放标志位的临时状态.擦擦擦擦擦擦
	unsigned char ControlByte[7];
	unsigned char AngTemp5b[5] = { 0xdd };
#define ANG_TO_DEBUGER_TIME 120000
#define InitData_Delay_Time 3000
	unsigned short InitDataDelayCnt = 0;
	signed char InitDataTimesUp = 0;
	unsigned int AngToDebugerDelay = 0;//发送角度时间的计数,计数多少个周期才发送数据
	unsigned char DTUDataCnt = 0;//透传接受到的数据计数
	uint8 Temp1B = 0;
	char DataReciveStart = 0;
	int Voltage_Cnt = 0;
	uint8 DebugerByte36[PageDateLen];
	
	uint8 VoltageTooLowCnt = 0;
	Struct_Init(); //初始各种结构体的值
	//LPLD_Flash_Init();
   // Flash_ReadAllData();
	CarInit();
	LED_Init();
	Voltage = LPLD_ADC_Get(ADC0, AD11)*3.3 * 4 / 256;
	LED_PrintValueF(70, 0, Voltage, 2);
	
	/*测试死区*/
	/*DisableInterrupts;
	while (1)
	{
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch0, death1);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch1, death3);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch2, death4);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch3, death2);
	}//右反300,右正300,左翻650,左正430*/
	while (1)
	{
		if (CCDReady==1)
		{
			CCDReady = 0;
			Voltage_Cnt++;
			if (Voltage_Cnt > 10)
			{
				Voltage_Cnt = 0;
				Voltage = LPLD_ADC_Get(ADC0, AD11)*3.3 * 4 / 256;
				if (Voltage < 7.3)
				{
					VoltageTooLowCnt++;
					if (VoltageTooLowCnt > 30)
					{
						BeepBeepBeep(10000);
					}
				}
				else
				{
					VoltageTooLowCnt = 0;
				}
			}
			if (Screen_WhichCCDImg == 1)
			{
				showimage(CCDM_Arr);
				LED_PrintValueF(0, 0, Dir_PID.ControlValue, 2);
				LED_PrintValueC(40, 0, CCDMain_Status.LeftPoint);
				LED_PrintValueC(80, 0, CCDMain_Status.RightPoint);
			}
			else if (Screen_WhichCCDImg == 2)
			{
				showimage(CCDS_Arr);
				LED_PrintValueF(0, 0, Dir_PID.ControlValue, 2);
				LED_PrintValueC(40, 0, CCDSlave_Status.LeftPoint);
				LED_PrintValueC(80, 0, CCDSlave_Status.RightPoint);
			}
			else if (Voltage_Display==1)
			{
				LED_PrintValueF(0, 0, Dir_PID.ControlValue, 2);
				LED_PrintValueF(70, 0, Voltage, 2);
			}
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
					LPLD_UART_PutChar(Debug_Usart_Port, '*');
					if (CCDSendToDebuger == 0)
					{
						LPLD_UART_PutChar(Debug_Usart_Port, 'L');
						LPLD_UART_PutChar(Debug_Usart_Port, 'D');

						SendHex(0);
						SendHex(0);
						SendHex(0);
						SendHex(0);
					}
				}
				if (CCDSendToDebuger == 1)
				{
					LPLD_UART_PutChar(Debug_Usart_Port, TempArr[CCDSendPointCnt]);
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
					LPLD_UART_PutChar(Debug_Usart_Port, '#');
					CCDDataSendOK = 1;
					CCDDataSendStart = 0;
					if (CCDSendToDebuger == 1)
					{
						LPLD_UART_PutChar(Debug_Usart_Port, CCDMain_Status.LeftPoint);
						LPLD_UART_PutChar(Debug_Usart_Port, CCDMain_Status.RightPoint);
						LPLD_UART_PutChar(Debug_Usart_Port, (uint8)CCDMain_Status.ControlValue);
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
					LPLD_UART_PutChar(Debug_Usart_Port, 0x02);
					LPLD_UART_PutChar(Debug_Usart_Port, 0xfd);
					
				}
				LPLD_UART_PutChar(Debug_Usart_Port, ShanWaiCCD[CCDSendPointCnt]);
				CCDSendPointCnt++;
				if (CCDSendPointCnt >= 256)
				{
					CCDSendPointCnt = 0;
					LPLD_UART_PutChar(Debug_Usart_Port, 0xfd);
					LPLD_UART_PutChar(Debug_Usart_Port, 0x02);
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


					/*Float2Byte(&CarInfo_Now.CarAngle, OUTDATA, 2);
					tempfloat = CarInfo_Now.CarAngSpeed;
					Float2Byte(&tempfloat, OUTDATA, 10);
					Float2Byte(&GravityAngle, OUTDATA, 6);
					tempfloat = Dir_AngSpeed;
					Float2Byte(&tempfloat, OUTDATA, 14);*/
					//通用
//   					Float2Byte(&CarInfo_Now.CarAngSpeed, OUTDATA, 2);
//   					tempfloat = CCDMain_Status.RightPoint*10;
//   					Float2Byte(&tempfloat, OUTDATA, 10);
// 					Float2Byte(&Dir_PID.OutValue, OUTDATA, 6);
// 					tempfloat = Dir_AngSpeed;
//   					Float2Byte(&tempfloat, OUTDATA, 14);

					//调速度PI

					tempfloat = (float)SpeedSet_Variable;
 					Float2Byte(&tempfloat, OUTDATA, 2);
					//tempfloat = CarInfo_Now.MotorCounterRight/ 10;
					tempfloat = TempValue.AngControl_OutValue/100;
					Float2Byte(&tempfloat, OUTDATA, 6);
 					tempfloat = (float)CarInfo_Now.CarSpeed;
 					Float2Byte(&tempfloat, OUTDATA, 10);
					//tempfloat = (float)CarInfo_Now.MotorCounterLeft/ 10;
					tempfloat = TempValue.SpeedOutValue/100;
 					Float2Byte(&tempfloat, OUTDATA, 14);

					tempfloat = CarInfo_Now.CarAngle;
					Float2Byte(&tempfloat, OUTDATA, 18);
					tempfloat = CarInfo_Now.CarAngSpeed;
					Float2Byte(&tempfloat, OUTDATA, 22);
					tempfloat = Ang_PID.AngSet;
					Float2Byte(&tempfloat, OUTDATA, 26);
					tempfloat = (float)CarInfo_Now.MotorCounterRight / 10;
					Float2Byte(&tempfloat, OUTDATA, 30);



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
				LPLD_UART_PutChar(Debug_Usart_Port, OUTDATA[ScopeSendPointCnt]);
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
					LPLD_UART_PutChar(Debug_Usart_Port, AngTemp5b[ScopeSendPointCnt]);
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
			if (LPLD_UART_GetChar_Present(Debug_Usart_Port))
			{
				Temp1B = LPLD_UART_GetChar(Debug_Usart_Port);
				if (Temp1B == 0xFE)
				{
					//ShakeHandFlag = 1;
					CCDSendImage = 0;
					AngDataSend = 0;
					LPLD_UART_PutChar(Debug_Usart_Port, 0xEF);
				}
				else if (Temp1B == 0xFD)
				{
					DebugerDataStart = 1;
					DebugDataPointCnt = 0;
					LPLD_UART_PutChar(Debug_Usart_Port, 0xDF);
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
					LPLD_UART_PutChar(Debug_Usart_Port, 0x9f);
					Debuger = 0;
				}
				else if (Temp1B == 0xf8)
				{
					LPLD_UART_PutChar(Debug_Usart_Port, 0x8f);
					CCDSendImage = 1;
					CCDSendToDebuger = 1;
				}
				else if (Temp1B == 0xf7)
				{
					LPLD_UART_PutChar(Debug_Usart_Port, 0x7f);
					CCDSendImage = 0;
					CCDSendToDebuger = 0;
				}
				else if (Temp1B == 0xf6)//开始接受控制字,接收控制字的时候取消和调试器的连接
				{
					ControlByteRcvStart = 1;
					Debuger = 0;
					ControlByteCnt = 0;
					LPLD_UART_PutChar(Debug_Usart_Port, 0x6f);
				}
				else if (Temp1B == 0xf5)
				{
					if (CarStop == 1)
					{
						CarStop = 0;
						LPLD_UART_PutChar(Debug_Usart_Port, 0x5f);
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
						LPLD_UART_PutChar(Debug_Usart_Port, 0x4f);
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
                    AngSendToDebuger=0;//不在向调试器发送数据了
				}
			}
		}
		if (DebugerDataStart == 1 )
		{
			if (InitDataTimesUp == 1)
			{
				InitDataTimesUp = 0;
				LPLD_UART_PutChar(Debug_Usart_Port, DebugerByte36[DebugDataPointCnt]);
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
			if (LPLD_UART_GetChar_Present(Debug_Usart_Port))
			{
				DebugerByte36[DTUDataCnt] = LPLD_UART_GetChar(Debug_Usart_Port);
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
					LPLD_UART_PutChar(Debug_Usart_Port, 0xaf);
					
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
			if (LPLD_UART_GetChar_Present(Debug_Usart_Port))
			{
				ControlByte[ControlByteCnt] = LPLD_UART_GetChar(Debug_Usart_Port);
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
					LPLD_UART_PutChar(Debug_Usart_Port, 0x6f);
				}
			}
		}
		if (LPLD_GPIO_Input_b(PTC, 16) == 0)//发送数据到上位机切换模式
		{
			Debuger = 0;
			KeyChanged = 0;
			if (LPLD_GPIO_Input_b(PTC, 17) == 0)
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
		}
		else
		{
			FlagToPhone = 1;
			if (KeyChanged == 0)
			{
				AngDataSend = 0;
				CCDSendImage = 0;
				KeyChanged = 1;
				CCDtoPC = 1;
				Debuger = 1;
			}
		}
		if (LPLD_GPIO_Input_b(PTC, 18) == 1 && LPLD_GPIO_Input_b(PTC, 19) == 1)//这段判定用于修改积分时间
		{
			IntegrationTime = 2;
		}
		else if (LPLD_GPIO_Input_b(PTC, 18) == 0 && LPLD_GPIO_Input_b(PTC, 19) == 0)
		{
			IntegrationTime = 8;
		}
		else if (LPLD_GPIO_Input_b(PTC, 18) == 1 && LPLD_GPIO_Input_b(PTC, 19) == 0)
		{
			IntegrationTime = 4;
		}
		else if (LPLD_GPIO_Input_b(PTC, 18) == 0 && LPLD_GPIO_Input_b(PTC, 19) == 1)
		{
			IntegrationTime = 6;
		}
	}
}


