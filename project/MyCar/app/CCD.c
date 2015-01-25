#include "CCD.h"
#include "control.h"
void SamplingDelay(void);
// #define TSL1401_SI(x)   LPLD_GPIO_Output_b(PTA, 28, x)
// #define TSL1401_CLK(x)  LPLD_GPIO_Output_b(PTA, 29, x)

#define SI_SetVal_M() LPLD_GPIO_Output_b(PTA, 28, 1)
#define SI_ClrVal_M() LPLD_GPIO_Output_b(PTA, 28, 0)
#define CLK_SetVal_M() LPLD_GPIO_Output_b(PTA, 29, 1)
#define CLK_ClrVal_M() LPLD_GPIO_Output_b(PTA, 29, 0)

/*#define SI_SetVal_S() LPLD_GPIO_Output_b(PTA, 26, 1)
#define SI_ClrVal_S() LPLD_GPIO_Output_b(PTA, 26, 0)
#define CLK_SetVal_S() LPLD_GPIO_Output_b(PTA, 27, 1)
#define CLK_ClrVal_S() LPLD_GPIO_Output_b(PTA, 27, 0)*/



//SI--PTA28
//CLK-PTA29
//AD--PTB2


#include "ccd.h"


CCD_Status_Struct CCDMain_Status;

CCD_SLave_Status_Struct CCDSlave_Status;
extern AngPID_InitTypeDef Ang_PID;

uint8 u32_trans_uint8(uint16 data); //只有本地用

unsigned char CCDTimeMs = 0;
unsigned char CCDReady = 0;
uint16 send_data = 0;
uint8 IntegrationTime = 5;
unsigned char CCDM_Arr[128] = { 0 };
unsigned char CCDS_Arr[128] = { 0 };
extern short SpeedControlPeriod, DirectionConrtolPeriod;
char TimeFlag_5Ms, TimeFlag_40Ms, TimeFlag_20Ms, TimeFlag_2Ms;
char TimerMsCnt = 0;
// #define CONTROL_PERIOD	5
// #define SPEED_CONTROL_COUNT 20
// #define SPEED_CONTROL_PERIOD (SPEED_CONTROL_COUNT * CONTROL_PERIOD)
// #define DIRECTION_CONTROL_COUNT			5
// #define DIRECTION_CONTROL_PERIOD		(DIRECTION_CONTROL_COUNT * CONTROL_PERIOD)
extern void CCDCP(void);
void ccd_exposure(void)
{
	unsigned char integration_piont;
	CCDTimeMs++;
	TimerMsCnt++;
	SpeedControlPeriod++;
	DirectionConrtolPeriod++;
	integration_piont = 17 - IntegrationTime;
	if (integration_piont >= 2)
	{
		if (integration_piont == CCDTimeMs)
		{
			//StartIntegration_S();
			StartIntegration_M();
		}
	}
	if (CCDTimeMs >= 17)
	{
		CCDTimeMs = 0;
		CCDCP();
	}
	if (TimerMsCnt % 2 == 0)
	{
		TimeFlag_2Ms = 1;
	}
	/*if (TimerMsCnt % 5 == 0)
	{
	TimeFlag_5Ms = 1;
	}*/
	if (TimerMsCnt % 20 == 0)
	{
		TimeFlag_20Ms = 1;
		DirectionConrtolPeriod = 0;
	}
	if (TimerMsCnt >= SPEED_CONTROL_PERIOD)
	{
		TimeFlag_40Ms = 1;
		TimerMsCnt = 0;
		SpeedControlPeriod = 0;
	}
}



void StartIntegration_M(void)
{

	unsigned char i;

	SI_SetVal_M();            /* SI  = 1 */
	//SI_SetVal_S();            /* SI  = 1 */
	SamplingDelay();
	CLK_SetVal_M();           /* CLK = 1 */
	//CLK_SetVal_S();           /* CLK = 1 */
	SamplingDelay();
	SI_ClrVal_M();
	//SI_ClrVal_S(); /* SI  = 0 */
	SamplingDelay();
	CLK_ClrVal_M();
	//CLK_ClrVal_S(); /* CLK = 0 */
	SamplingDelay();
	SamplingDelay();
	for (i = 0; i < 127; i++)
	{
		SamplingDelay();
		SamplingDelay();
		CLK_SetVal_M();
		//CLK_SetVal_S();/* CLK = 1 */
		SamplingDelay();
		SamplingDelay();
		CLK_ClrVal_M();
		//CLK_ClrVal_S();  /* CLK = 0 */
	}
	SamplingDelay();
	SamplingDelay();
	CLK_SetVal_M();
	//CLK_SetVal_S();/* CLK = 1 */
	SamplingDelay();
	SamplingDelay();
	CLK_ClrVal_M();
	//CLK_ClrVal_S();  /* CLK = 0 */
}


// void StartIntegration_S(void)
// {
// 
// 	unsigned char i;
// 
// 	//SI_SetVal_S();            /* SI  = 1 */
// 	SamplingDelay();
// 	//CLK_SetVal_S();           /* CLK = 1 */
// 	SamplingDelay();
// 	//SI_ClrVal_S();            /* SI  = 0 */
// 	SamplingDelay();
// 	//CLK_ClrVal_S();           /* CLK = 0 */
// 	SamplingDelay(); SamplingDelay();
// 	for (i = 0; i < 127; i++) {
// 		SamplingDelay();
// 		SamplingDelay();
// 		//CLK_SetVal_S();       /* CLK = 1 */
// 		SamplingDelay();
// 		SamplingDelay();
// 		//CLK_ClrVal_S();       /* CLK = 0 */
// 	}
// 	SamplingDelay();
// 	SamplingDelay();
// 	//CLK_SetVal_S();           /* CLK = 1 */
// 	SamplingDelay();
// 	SamplingDelay();
// 	//CLK_ClrVal_S();           /* CLK = 0 */
// }
// 


void ImageCapture_M(unsigned char * ImageData, unsigned char * ImageData2)
{

	unsigned char i;
	extern uint8 AtemP;

	SI_SetVal_M();
	//SI_SetVal_S();  /* SI  = 1 */
	SamplingDelay();
	CLK_SetVal_M();
	//CLK_SetVal_S();     /* CLK = 1 */
	SamplingDelay();
	SI_ClrVal_M();
	//SI_ClrVal_S();  /* SI  = 0 */
	SamplingDelay();

	//Delay 10us for sample the first pixel
	/**/
	for (i = 0; i < 200; i++) {                    //更改250，让CCD的图像看上去比较平滑，
		SamplingDelay();  //200ns                  //把该值改大或者改小达到自己满意的结果。
	}

	//Sampling Pixel 1

	*ImageData = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD14));
	*ImageData2 = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD15));
	ImageData++;
	ImageData2++;
	CLK_ClrVal_M();
	//CLK_ClrVal_S();/* CLK = 0 */
	SamplingDelay();
	SamplingDelay();
	SamplingDelay();
	for (i = 0; i < 127; i++)
	{
		SamplingDelay();
		SamplingDelay();
		CLK_SetVal_M();
		//CLK_SetVal_S(); /* CLK = 1 */
		SamplingDelay();
		SamplingDelay();
		//Sampling Pixel 2~128

		*ImageData = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD14));
		*ImageData2 = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD15));
		ImageData++;
		ImageData2++;
		CLK_ClrVal_M();
		//CLK_ClrVal_S(); /* CLK = 0 */
	}
	SamplingDelay();
	SamplingDelay();
	CLK_SetVal_M();
	//CLK_SetVal_S(); /* CLK = 1 */
	SamplingDelay();
	SamplingDelay();
	CLK_ClrVal_M();
	//CLK_ClrVal_S();/* CLK = 0 */
}


// void ImageCapture_S(unsigned char * ImageData)
// {
// 
// 	unsigned char i;
// 	extern uint8 AtemP;
// 
// 	//SI_SetVal_S();            /* SI  = 1 */
// 	SamplingDelay();
// 	//CLK_SetVal_S();           /* CLK = 1 */
// 	SamplingDelay();
// 	//SI_ClrVal_S();            /* SI  = 0 */
// 	SamplingDelay();
// 
// 	//Delay 10us for sample the first pixel
// 	for (i = 0; i < 200; i++) {                    //更改250，让CCD的图像看上去比较平滑，
// 		SamplingDelay();  //200ns                  //把该值改大或者改小达到自己满意的结果。
// 	}
// 
// 	//Sampling Pixel 1
// 
// 	*ImageData = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD15));
// 	ImageData++;
// 	//CLK_ClrVal_S();           /* CLK = 0 */
// 	SamplingDelay();
// 	SamplingDelay();
// 	SamplingDelay();
// 	for (i = 0; i < 127; i++) {
// 		SamplingDelay();
// 		SamplingDelay();
// 		//CLK_SetVal_S();       /* CLK = 1 */
// 		SamplingDelay();
// 		SamplingDelay();
// 		//Sampling Pixel 2~128
// 
// 		*ImageData = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD15));
// 		ImageData++;
// 		//CLK_ClrVal_S();       /* CLK = 0 */
// 	}
// 	SamplingDelay();
// 	SamplingDelay();
// 	//CLK_SetVal_S();           /* CLK = 1 */
// 	SamplingDelay();
// 	SamplingDelay();
// 	//CLK_ClrVal_S();           /* CLK = 0 */
// }



/* 曝光时间，单位ms */

void CalculateIntegrationTime(void)
{
	//extern uint8 Pixel[128];
	/* 128个像素点的平均AD值 */
	uint8 PixelAverageValue;
	/* 128个像素点的平均电压值的10倍 */
	uint8 PixelAverageVoltage;
	/* 设定目标平均电压值，实际电压的10倍 */
	uint16 TargetPixelAverageVoltage = 25;
	/* 设定目标平均电压值与实际值的偏差，实际电压的10倍 */
	char PixelAverageVoltageError = 0;
	/* 设定目标平均电压值允许的偏差，实际电压的10倍 */
	uint16 TargetPixelAverageVoltageAllowError = 2;

	/* 计算128个像素点的平均AD值 */
	PixelAverageValue = PixelAverage(128, CCDM_Arr);
	/* 计算128个像素点的平均电压值,实际值的10倍 */
	PixelAverageVoltage = (unsigned char)((int)PixelAverageValue * 25 / 194);//把0-194平均分成了25份

	PixelAverageVoltageError = TargetPixelAverageVoltage - PixelAverageVoltage;
	if (PixelAverageVoltageError < -TargetPixelAverageVoltageAllowError)
	{
		PixelAverageVoltageError = 0 - PixelAverageVoltageError;
		PixelAverageVoltageError /= 2;
		if (PixelAverageVoltageError > 10)
			PixelAverageVoltageError = 10;
		IntegrationTime -= PixelAverageVoltageError;
	}
	if (PixelAverageVoltageError > TargetPixelAverageVoltageAllowError)
	{
		PixelAverageVoltageError /= 2;
		if (PixelAverageVoltageError > 10)
			PixelAverageVoltageError = 10;
		IntegrationTime += PixelAverageVoltageError;
	}


	//  LPLD_UART_PutChar(UART5,0XAA) ;

	//   LPLD_UART_PutChar(UART5,PixelAverageVoltage) ;
	//   LPLD_UART_PutChar(UART5,PixelAverageVoltageError) ;
	//    LPLD_UART_PutChar(UART5,IntegrationTime) ;
	if (IntegrationTime <= 1)
		IntegrationTime = 1;
	if (IntegrationTime >= 100)
		IntegrationTime = 100;
}


uint8 PixelAverage(uint8 len, uint8 *data) {
	unsigned char i;
	unsigned int sum = 0;
	for (i = 0; i < len; i++) {
		sum = sum + *data++;
	}
	return ((unsigned char)(sum / len));
}

void SendHex(unsigned char hex)
{
	unsigned char temp;
	temp = hex >> 4;
	if (temp < 10) {
		LPLD_UART_PutChar(UART5, temp + '0');
	}
	else {
		LPLD_UART_PutChar(UART5, temp - 10 + 'A');
	}
	temp = hex & 0x0F;
	if (temp < 10) {
		LPLD_UART_PutChar(UART5, temp + '0');
	}
	else {
		LPLD_UART_PutChar(UART5, temp - 10 + 'A');
	}
}

void SendImageData(unsigned char * ImageData)
{

	unsigned char i;
	unsigned char crc = 0;

	/* Send Data */
	LPLD_UART_PutChar(UART5, '*');
	LPLD_UART_PutChar(UART5, 'L');
	LPLD_UART_PutChar(UART5, 'D');

	SendHex(0);
	SendHex(0);
	SendHex(0);
	SendHex(0);

	for (i = 0; i < 128; i++)
	{
		SendHex(*ImageData++);
	}

	SendHex(crc);
	LPLD_UART_PutChar(UART5, '#');
}


void SamplingDelay(void)
{
	volatile uint8 i;
	for (i = 0; i < 1; i++) {
		asm("nop");
		asm("nop");
	}

}
uint8 u32_trans_uint8(uint16 data)
{
	return (uint8)((uint32)data * 255 / 4095);
}


#define LeftBoundary 6 //去掉开始的10个点
#define RightBoundary 122//去掉后面的10个点
#define CCD_Threshold 45//40  //这是两组两组差的阈值//实际使用的时候取
#define LeftLostPrepare 10
#define RightLostPrepare 118 //左右准备丢线的阈值
#define LeftMode -1
#define RightMode 1
#define MidMode 0
#define SearchMode_Left -1
#define SearchMode_Right 1 //往右边寻线
#define SearchMode_Double 0 //双边寻线
#define CCDSlaveModeChangeModeValueLeft 64//左线大于30的时候切换模式
#define CCDSlaveModeChangeModeValueRight 64//右线小于90的时候切换模式
#define Msg_UseMainCCD 0
#define Msg_UseSlaveCCD 1
#define Msg_Lost 2
#define Road_TurnRight 0
#define Road_TurnLeft 1
#define Road_Cross 2
#define Road_Straight 3
void CCDLineInit(void);
unsigned short CCDInitCnt = 0;//用来初始化左右线的计数,,每次启动重新初始化左右线位置
signed char CCDMsg = 0;
short CCDInitLine_Left = 0;
short CCDInitLine_Right = 0;
signed char CCD_Correction[128];//ccd畸变的矫正值
/*#define LostLineDiffValue 20 //准备丢线的差分值*/


//24 102
char CCD_Deal_Main(unsigned char *CCDArr)
{
	int i = 0, j = 0;
	static
		short SumTemp1_short = 0;
	short SumTemp2_short = 0;
	//short SearchBeginTemp = 0;
	signed char LeftTempArr[50];
	signed char RightTempArr[50];
	unsigned char Counter_R = 0;
	unsigned char Counter_L = 0;
	unsigned char LineLenCnt = 0;
	//防止溢出
	if (CCDMain_Status.SearchBegin < 0)
		CCDMain_Status.SearchBegin = 0;
	if (CCDMain_Status.SearchBegin > 126)
		CCDMain_Status.SearchBegin = 127;

	if (CCDMain_Status.InitOK == 0)
	{
		CCDMain_Status.SearchBegin = 64;//初始化起始中点,其他的可以不初始化
		//CCDMain_Status.Mode = 0;//初始状态用中线引导
		CCDMain_Status.PointCnt = 0;
		CCDMain_Status.LastLeftPoint = -1;
		CCDMain_Status.LastRightPoint = -1;
		CCDMain_Status.SearchMode = SearchMode_Double;
	}
	if (CCDMain_Status.SearchMode != SearchMode_Left)
	{
		for (i = CCDMain_Status.SearchBegin; i < RightBoundary; i++)
		{
			SumTemp1_short = CCDArr[i] + CCDArr[i + 1];
			SumTemp2_short = CCDArr[i + 4] + CCDArr[i + 5];//110011来检测
			if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//表示下降的特别快
			{
				RightTempArr[Counter_R] = i + 2;
				Counter_R++;
			}
		}
	}
	if (CCDMain_Status.SearchMode != SearchMode_Right)
	{
		for (i = CCDMain_Status.SearchBegin; i > LeftBoundary; i--)
		{
			SumTemp1_short = CCDArr[i] + CCDArr[i - 1];
			SumTemp2_short = CCDArr[i - 4] + CCDArr[i - 5];
			if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//表示下降的特别快
			{
				LeftTempArr[Counter_L] = i - 2; //为了新想法,需要判断这个点是否为坏点
				Counter_L++;
			}  //找到一个跳变点...两个两个一对得比较..从中间向两边比较得出下降沿,将位置放出临时数组里面,以后再做 处理;
		}
	}
	CCDMain_Status.Left_LostFlag = 1;
	CCDMain_Status.Right_LostFlag = 1;
	//CCDMain_Status.LeftLineArr[CCDMain_Status.PointCnt] = 0;
	CCDMain_Status.RightPoint = 127;
	CCDMain_Status.LeftPoint = 0;
	CCDMain_Status.MidPoint = -1;
	//CCDMain_Status.RightLineArr[CCDMain_Status.PointCnt] = 127;
	LineLenCnt = 0;
	for (i = 0; i < Counter_L; i++)
	{
		if ((CCDArr[LeftTempArr[i]]) - ((CCDArr[LeftTempArr[i] - 1] + CCDArr[LeftTempArr[i] + 1]) / 2) < (CCD_Threshold))
		{
			/*if (CCDMain_Status.LastLeftPoint >= 0)
			{
			if (LeftTempArr[i] - CCDMain_Status.LastLeftPoint > 20 || LeftTempArr[i] - CCDMain_Status.LastLeftPoint < -20)
			continue;
			}
			else*/
			{
				SumTemp1_short = CCDArr[LeftTempArr[i]] + CCDArr[LeftTempArr[i] + 1];
				for (j = 1; j < 8; j++)
				{
					SumTemp2_short = CCDArr[LeftTempArr[i] - j] + CCDArr[LeftTempArr[i] - j - 1];
					if (SumTemp1_short - SumTemp2_short >(CCD_Threshold - CCD_Correction[LeftTempArr[i] - j]))
						LineLenCnt++;
				}
				if (LineLenCnt > 3)
				{
					//CCDMain_Status.LeftLineArr[CCDMain_Status.PointCnt] = LeftTempArr[i];
					CCDMain_Status.Left_LostFlag = 0;
					CCDMain_Status.LastLeftPoint = CCDMain_Status.LeftPoint;
					CCDMain_Status.LeftPoint = LeftTempArr[i];
					break;
				}
			}
		}
	}
	LineLenCnt = 0;
	for (i = 0; i < Counter_R; i++)
	{
		if ((CCDArr[RightTempArr[i]]) - ((CCDArr[RightTempArr[i] - 1] + CCDArr[RightTempArr[i] + 1]) / 2) < (CCD_Threshold))
		{
			/*if (CCDMain_Status.LastRightPoint >= 0)
			{
			if (RightTempArr[i] - CCDMain_Status.LastRightPoint > 20 || RightTempArr[i] - CCDMain_Status.LastRightPoint < -20)
			continue;
			}
			else*/
			{
				SumTemp1_short = CCDArr[RightTempArr[i]] + CCDArr[RightTempArr[i] + 1];
				for (j = 1; j < 8; j++)
				{
					SumTemp2_short = CCDArr[RightTempArr[i] + j] + CCDArr[RightTempArr[i] + j + 1];
					if (SumTemp1_short - SumTemp2_short >(CCD_Threshold - CCD_Correction[RightTempArr[i] + j]))
						LineLenCnt++;
				}
				if (LineLenCnt > 3)
				{
					//CCDMain_Status.RightLineArr[CCDMain_Status.PointCnt] = RightTempArr[i];
					CCDMain_Status.Right_LostFlag = 0;
					CCDMain_Status.LastRightPoint = CCDMain_Status.RightPoint;
					CCDMain_Status.RightPoint = RightTempArr[i];
					break;
				}
			}
		}
	}
	/*if (CCDMain_Status.Left_LostFlag == 1 && CCDMain_Status.Right_LostFlag == 1)
			CCDMain_Status.LostAllLine = 1;
			else
			CCDMain_Status.LostAllLine = 0;
			if (CCDMain_Status.LostAllLine == 0)
			{
			if (((CCDMain_Status.LeftPoint < LeftLostPrepare) && (CCDMain_Status.RightPoint<RightLostPrepare)) || CCDMain_Status.Left_LostFlag == 1)
			{
			if (CCDMain_Status.Mode != RightMode)
			CCDMain_Status.Mode = RightMode;
			}
			if (((CCDMain_Status.LeftPoint>LeftLostPrepare) && (CCDMain_Status.RightPoint > RightLostPrepare)) || CCDMain_Status.Right_LostFlag == 1)
			{
			if (CCDMain_Status.Mode != LeftMode)
			CCDMain_Status.Mode = LeftMode;
			}
			if ((CCDMain_Status.LeftPoint > (LeftLostPrepare + 5)) && (CCDMain_Status.RightPoint < (RightLostPrepare - 5)))
			{
			if (CCDMain_Status.Mode != MidMode)
			{
			CCDMain_Status.Mode = MidMode;
			}
			}

			if (CCDMain_Status.Mode == MidMode)
			{
			SearchBeginTemp = CCDMain_Status.LeftPoint + CCDMain_Status.RightPoint;
			CCDMain_Status.SearchBegin = SearchBeginTemp / 2;
			CCDMain_Status.ControlValue = CCDMain_Status.MidSet - CCDMain_Status.SearchBegin;
			CCDMain_Status.MidPoint = CCDMain_Status.SearchBegin;
			}
			else if (CCDMain_Status.Mode == LeftMode)
			{
			SearchBeginTemp = CCDMain_Status.LeftPoint + 12;
			CCDMain_Status.SearchBegin = SearchBeginTemp;
			CCDMain_Status.ControlValue = CCDMain_Status.LeftSet - CCDMain_Status.LeftPoint;
			//CCDMain_Status.MidPoint = CCDMain_Status.LeftPoint - CCDMain_Status.ControlValue;
			CCDMain_Status.RightPoint = 127;
			}
			else if (CCDMain_Status.Mode == RightMode)
			{
			CCDMain_Status.SearchBegin = CCDMain_Status.RightPoint - 12;
			CCDMain_Status.ControlValue = CCDMain_Status.RightSet - CCDMain_Status.RightPoint;
			//CCDMain_Status.MidPoint = CCDMain_Status.RightPoint - CCDMain_Status.ControlValue;
			CCDMain_Status.LeftPoint = 0;
			}
			else
			{
			//	CCDMain_Status.ErrorCnt++;
			}
			}
			else
			{
			CCDMain_Status.ControlValue /= 2;
			}*/
	/*	if (CCDMain_Status.Left_LostFlag == 0 && CCDMain_Status.LeftPoint > 20)
		{

		}
		else if (CCDMain_Status.Right_LostFlag == 0 && CCDMain_Status.RightPoint <108)
		{

		}
		else if (CCDSlave_Status.Road == Road_TurnLeft)
		{
		if (CCDMain_Status.RightPoint > 126)
		{
		CCDMain_Status.SearchBegin = 64;
		}
		else
		{
		if (CCDMain_Status.RightPoint < 30)
		{
		CCDMain_Status.SearchBegin = 20;
		}
		else
		CCDMain_Status.SearchBegin = CCDMain_Status.RightPoint - 20;
		}
		CCDMain_Status.ControlValue = CCDMain_Status.RightSet - CCDMain_Status.RightPoint;
		}
		else if (CCDSlave_Status.Road == Road_TurnRight)
		{
		CCDMain_Status.ControlValue = CCDMain_Status.LeftSet - CCDMain_Status.LeftPoint;
		if (CCDMain_Status.LeftPoint < 1)
		{
		CCDMain_Status.SearchBegin = 64;
		}
		else
		{
		if (CCDMain_Status.LeftPoint > 100)
		{
		CCDMain_Status.SearchBegin = 110;
		}
		else
		CCDMain_Status.SearchBegin = CCDMain_Status.LeftPoint + 20;
		}
		CCDMain_Status.ControlValue = CCDMain_Status.LeftSet - CCDMain_Status.LeftPoint;
		}*/
	/*CCDMain_Status.SearchBegin = 20;
	CCDMain_Status.ControlValue = 70 - CCDMain_Status.RightPoint;*/

	if (CCDMain_Status.InitOK == 1)
	{
		if ((CCDMain_Status.Left_LostFlag == 1 && CCDMain_Status.Right_LostFlag == 1))
		{
			CCDMain_Status.ControlValue *= 0.7;
		}
		else
		{
			CCDMain_Status.MidPoint = (CCDMain_Status.LeftPoint + CCDMain_Status.RightPoint) / 2;
			CCDMain_Status.SearchBegin = CCDMain_Status.MidPoint;
			CCDMain_Status.ControlValue = CCDMain_Status.MidSet - CCDMain_Status.MidPoint;
		}
	}
	return 0;
}



void CCD_Deal_Slave(unsigned char *CCDArr)
{
	int i, j;
	short SumTemp1_short = 0;
	short SumTemp2_short = 0;
	char LeftTempArr[50];
	char RightTempArr[50];
	unsigned char Counter_R = 0;
	unsigned char Counter_L = 0;
	unsigned char LineLenCnt = 0;
	if (CCDSlave_Status.SearchBegin < 0)
	{
		CCDSlave_Status.SearchBegin = 0;
	}
	if (CCDSlave_Status.SearchBegin > 126)
	{
		CCDSlave_Status.SearchBegin = 127;
	}
	//if (CCDMain_Status.PointCnt >= (CCD_DataLen - 1))
	//	CCDMain_Status.PointCnt = 0;
	if (CCDMain_Status.InitOK == 0)
	{
		CCDSlave_Status.SearchBegin = 64;
		CCDSlave_Status.LastLeftPoint = -1;
		CCDSlave_Status.LastRightPoint = -1;
		CCDSlave_Status.PointCnt = 0;
	}
	for (i = CCDSlave_Status.SearchBegin; i < RightBoundary; i++)
	{
		SumTemp1_short = CCDArr[i] + CCDArr[i + 1];
		SumTemp2_short = CCDArr[i + 4] + CCDArr[i + 5];//110011来检测
		if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//表示下降的特别快
		{
			RightTempArr[Counter_R] = i + 2;
			Counter_R++;
		}
	}


	for (i = CCDSlave_Status.SearchBegin; i > LeftBoundary; i--)
	{
		SumTemp1_short = CCDArr[i] + CCDArr[i - 1];
		SumTemp2_short = CCDArr[i - 4] + CCDArr[i - 5];
		if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//表示下降的特别快
		{
			LeftTempArr[Counter_L] = i - 2; //为了新想法,需要判断这个点是否为坏点
			Counter_L++;
		}  //找到一个跳变点...两个两个一对得比较..从中间向两边比较得出下降沿,将位置放出临时数组里面,以后再做 处理;
	}

	CCDSlave_Status.Left_LostFlag = 1;
	CCDSlave_Status.Right_LostFlag = 1;
	//CCDSlave_Status.LeftLineArr[CCDSlave_Status.PointCnt] = 0;
	//CCDSlave_Status.RightLineArr[CCDSlave_Status.PointCnt] = 127;
	CCDSlave_Status.LeftPoint = 0;
	CCDSlave_Status.RightPoint = 127;
	LineLenCnt = 0;

	for (i = 0; i < Counter_L; i++)
	{
		if ((CCDArr[LeftTempArr[i]]) - ((CCDArr[LeftTempArr[i] - 1] + CCDArr[LeftTempArr[i] + 1]) / 2) < CCD_Threshold / 2)
		{
			//到了这一行表示确定这是一条黑线但是不确定是不是上一个回合连续的黑线
			/*if (CCDSlave_Status.LastLeftPoint >= 0)
			{
			if (LeftTempArr[i] - CCDSlave_Status.LastLeftPoint > 20 || LeftTempArr[i] - CCDSlave_Status.LastLeftPoint < -20)
			{
			CCDSlave_Status.ErrorCntLeft++;
			if (CCDSlave_Status.ErrorCntLeft > 10)
			{
			CCDSlave_Status.ErrorCntLeft = 0;
			CCDSlave_Status.LastLeftPoint = -1;
			}
			}
			else
			continue;
			}
			else*/
			{
				SumTemp1_short = CCDArr[LeftTempArr[i]] + CCDArr[LeftTempArr[i] + 1];
				for (j = 1; j < 8; j++)
				{
					SumTemp2_short = CCDArr[LeftTempArr[i] - j] + CCDArr[LeftTempArr[i] - j - 1];
					if (SumTemp1_short - SumTemp2_short >(CCD_Threshold - CCD_Correction[LeftTempArr[i] - j]))
						LineLenCnt++;
				}
				if (LineLenCnt > 3)
				{
					//CCDSlave_Status.LeftLineArr[CCDSlave_Status.PointCnt] = LeftTempArr[i];
					CCDSlave_Status.Left_LostFlag = 0;
					CCDSlave_Status.LastLeftPoint = CCDSlave_Status.LeftPoint;
					CCDSlave_Status.LeftPoint = LeftTempArr[i];
					break;
				}
			}
		}

	}
	LineLenCnt = 0;

	for (i = 0; i < Counter_R; i++)
	{
		if ((CCDArr[RightTempArr[i]]) - ((CCDArr[RightTempArr[i] - 1] + CCDArr[RightTempArr[i] + 1]) / 2) < CCD_Threshold)
		{
			/*if (CCDSlave_Status.LastRightPoint >= 0)
			{
			if (RightTempArr[i] - CCDSlave_Status.LastRightPoint > 20 || RightTempArr[i] - CCDSlave_Status.LastRightPoint < -20)
			{
			CCDSlave_Status.ErrorCntRight++;
			if (CCDSlave_Status.ErrorCntRight > 10)
			{
			CCDSlave_Status.ErrorCntRight = 0;
			CCDSlave_Status.LastRightPoint = -1;
			}
			}
			else
			continue;
			}
			else*/
			{
				SumTemp1_short = CCDArr[RightTempArr[i]] + CCDArr[RightTempArr[i] + 1];
				for (j = 1; j < 8; j++)
				{
					SumTemp2_short = CCDArr[RightTempArr[i] + j] + CCDArr[RightTempArr[i] + j + 1];
					if (SumTemp1_short - SumTemp2_short >(CCD_Threshold - CCD_Correction[RightTempArr[i] + j]))
						LineLenCnt++;
				}
				if (LineLenCnt > 3)
				{
					//CCDSlave_Status.RightLineArr[CCDSlave_Status.PointCnt] = RightTempArr[i];
					CCDSlave_Status.Right_LostFlag = 0;
					CCDSlave_Status.LastRightPoint = CCDSlave_Status.RightPoint;
					CCDSlave_Status.RightPoint = RightTempArr[i];
					break;
				}
			}
		}
	}

	if (CCDSlave_Status.Left_LostFlag == 1 && CCDSlave_Status.Right_LostFlag == 1)
	{
		CCDSlave_Status.ControlValue *= 0.7;
	}
	else
	{
		CCDSlave_Status.MidPoint = (CCDSlave_Status.LeftPoint + CCDSlave_Status.RightPoint) / 2;
		CCDSlave_Status.SearchBegin = CCDSlave_Status.MidPoint;
		CCDSlave_Status.ControlValue = CCDSlave_Status.MidSet - CCDSlave_Status.MidPoint;
	}

	/*	if (CCDSlave_Status.Right_LostFlag == 1)
			CCDSlave_Status.LastRightPoint = -1;
			if (CCDSlave_Status.Left_LostFlag == 1)
			CCDSlave_Status.LastLeftPoint = -1;
			if (CCDSlave_Status.Right_LostFlag == 1 && CCDSlave_Status.Left_LostFlag == 1)
			{
			//需要判断是否为直角弯?
			//CCDSlave_Status.ControlValue /= CCDSlave_Status.ControlValue;
			CCDSlave_Status.SearchMode = SearchMode_Double;
			}
			else
			{//根据找到的线更新搜寻起点和判断道路类型和是否初始化完成
			if (CCDSlave_Status.Right_LostFlag == 0 && CCDSlave_Status.Left_LostFlag == 0)//如果两条线都在
			{
			CCDSlave_Status.SearchBegin = (CCDSlave_Status.RightPoint + CCDSlave_Status.LeftPoint) / 2;
			CCDSlave_Status.MidPoint = CCDSlave_Status.SearchBegin;
			CCDSlave_Status.Mode = MidMode;
			CCDSlave_Status.ControlValue = CCDSlave_Status.MidSet - CCDSlave_Status.MidPoint;
			CCDSlave_Status.Road = Road_Straight;
			}
			else if (CCDSlave_Status.Right_LostFlag == 0 && CCDSlave_Status.Left_LostFlag == 1)//如果右边丢线
			{
			CCDSlave_Status.LastRightPoint = -1;
			if (CCDSlave_Status.RightPoint > 50)
			{
			CCDSlave_Status.SearchBegin = CCDSlave_Status.RightPoint - 15;
			CCDSlave_Status.SearchMode = SearchMode_Double;
			}
			else
			{
			CCDSlave_Status.SearchBegin = 30;
			CCDSlave_Status.SearchMode = SearchMode_Right;
			}
			CCDSlave_Status.MidPoint = 0;
			CCDSlave_Status.Mode = LeftMode;
			CCDSlave_Status.ControlValue = CCDSlave_Status.RightSet - CCDSlave_Status.RightPoint;
			}
			else if (CCDSlave_Status.Left_LostFlag == 0 && CCDSlave_Status.Right_LostFlag == 1)
			{
			CCDSlave_Status.LastLeftPoint = -1;
			if (CCDSlave_Status.LeftPoint <= 80)
			{
			CCDSlave_Status.SearchBegin = CCDSlave_Status.LeftPoint + 15;
			CCDSlave_Status.SearchMode = SearchMode_Double;
			}
			else
			{
			CCDSlave_Status.SearchBegin = 90;
			CCDSlave_Status.SearchMode = SearchMode_Left;
			}
			CCDSlave_Status.MidPoint = 0;
			CCDSlave_Status.Mode = RightMode;
			CCDSlave_Status.ControlValue = CCDSlave_Status.LeftSet - CCDSlave_Status.LeftPoint;
			}
			if (CCDSlave_Status.LeftPoint > CCDSlaveModeChangeModeValueLeft-23)
			{
			CCDSlave_Status.Road = Road_TurnRight;
			if (CCDMsg != Msg_UseMainCCD)
			{
			CCDMsg = Msg_UseMainCCD;
			CCDMain_Status.SearchBegin = 80;
			}
			}
			else if (CCDSlave_Status.RightPoint < CCDSlaveModeChangeModeValueRight+23)
			{
			CCDSlave_Status.Road = Road_TurnLeft;
			if (CCDMsg != Msg_UseMainCCD)
			{
			CCDMsg = Msg_UseMainCCD;
			CCDMain_Status.SearchBegin = 40;
			}
			}
			if (CCDSlave_Status.LeftPoint < 35 && CCDSlave_Status.RightPoint > 90 && CCDSlave_Status.LeftPoint != 0 && CCDSlave_Status.RightPoint != 127)
			{
			if (CCDMsg != Msg_UseSlaveCCD)
			{
			CCDMsg = Msg_UseSlaveCCD;
			}
			}
			}
			//CCDSlave_Status.PointCnt++;
			}*/


	/*if (CCDMsg == Msg_UseSlaveCCD)
	{
		if (CCDSlave_Status.Left_LostFlag == 0 && CCDSlave_Status.Right_LostFlag == 0)
		{
			CCDSlave_Status.MidPoint = (CCDSlave_Status.LeftPoint + CCDSlave_Status.RightPoint) / 2;
			CCDSlave_Status.ControlValue = CCDSlave_Status.MidSet - CCDSlave_Status.MidPoint;
		}
		else if (CCDSlave_Status.Left_LostFlag == 1 && CCDSlave_Status.Right_LostFlag == 0)
		{
			CCDSlave_Status.ControlValue = CCDSlave_Status.RightSet - CCDSlave_Status.RightPoint;
		}
		else if (CCDSlave_Status.Left_LostFlag == 0 && CCDSlave_Status.Right_LostFlag == 1)
		{
			CCDSlave_Status.ControlValue = CCDSlave_Status.LeftSet - CCDSlave_Status.LeftPoint;
		}
		else
		{
			CCDMsg = Msg_UseMainCCD;
		}
	}*/

	

}

void CCD_ControlValueCale(void)
{
	/*if (CCDMsg == Msg_UseMainCCD)
	{
		Dir_PID.ControlValue = CCDMain_Status.ControlValue;
	}
	else if (CCDMsg == Msg_UseSlaveCCD)
	{
		Dir_PID.ControlValue = CCDSlave_Status.ControlValue;
	}
	else if (CCDMsg == Msg_Lost)
	{
		Dir_PID.ControlValue *= 0.7;
	}*/
	if (CCDMain_Status.Left_LostFlag == 1 && CCDMain_Status.Right_LostFlag == 1)
	{//主CCD两条线都丢了,可能为十字
		if (CCDSlave_Status.Left_LostFlag == 1 && CCDSlave_Status.Right_LostFlag == 1)
		{
			Dir_PID.ControlValue *= 0.3;
		}
		else if (CCDSlave_Status.Left_LostFlag == 1 || CCDSlave_Status.Right_LostFlag == 1)
		{//如果主CCD丢线但是从CCD只丢了一条线,那么控制值由从CCD的一个权重确定
			Dir_PID.ControlValue = CCDSlave_Status.ControlValue*0.2;
		}
		else
		{
			Dir_PID.ControlValue = CCDSlave_Status.ControlValue*0.6;
		}
	}
	else if (CCDSlave_Status.Left_LostFlag == 1 && CCDSlave_Status.Right_LostFlag == 1)
	{
		//如果从CCD两条线都丢了,那么权重为0
		Dir_PID.ControlValue = CCDMain_Status.ControlValue;
	}
	else if (CCDSlave_Status.Left_LostFlag == 1 || CCDSlave_Status.Right_LostFlag==1)
	{
		//如果其中之一丢线,,权重降低
		Dir_PID.ControlValue = CCDMain_Status.ControlValue * 0.8 + CCDSlave_Status.ControlValue*0.2;
	}
	else
	{
		Dir_PID.ControlValue = CCDMain_Status.ControlValue * 0.6 + CCDSlave_Status.ControlValue*0.4;
	}

	//Dir_PID.ControlValue = CCDMain_Status.ControlValue;
}


void CCDLineInit(void)
{
	signed short MidTemp = 0;
	float a[128];
	int i;
	//i//f (CCDMain_Status.Left_LostFlag == 0)
	//if (CCDMain_Status.Right_LostFlag == 0)
	//if (CCDSlave_Status.Left_LostFlag == 0)
	//if (CCDSlave_Status.Right_LostFlag == 0)
	{
		//MidTemp = CCDMain_Status.MidPoint - CCDSlave_Status.MidPoint;//双CCD
		//MidTemp = CCDMain_Status.MidPoint - 64;
		//if (MidTemp<3 && MidTemp>-3)
		//{
		CCDMain_Status.MidSet = 64;
		CCDSlave_Status.MidSet = 64;
		CCDMain_Status.LeftSet = 36;
		CCDMain_Status.RightSet = 94;
		CCDSlave_Status.LeftSet = 44;
		CCDSlave_Status.RightSet = 83;
		CCDMain_Status.InitOK = 1;

		for (i = 0; i < 64; i++)
		{
			a[i] = (10.0 / 64)*(64 - i);
		}
		for (i = 64; i < 128; i++)
		{
			a[i] = (10.0 / 64)*(i - 64);
		}
		for (i = 0; i < 128; i++)
		{
			CCD_Correction[i] = (int)a[i];//CCD畸变的矫正数组
		}
	}
}