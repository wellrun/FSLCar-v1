#include "CCD.h"
void SamplingDelay(void);
#define TSL1401_SI(x)   LPLD_GPIO_Output_b(PTA, 28, x)
#define TSL1401_CLK(x)  LPLD_GPIO_Output_b(PTA, 29, x)

#define SI_SetVal() LPLD_GPIO_Output_b(PTA, 28, 1)
#define SI_ClrVal() LPLD_GPIO_Output_b(PTA, 28, 0)
#define CLK_SetVal() LPLD_GPIO_Output_b(PTA, 29, 1)
#define CLK_ClrVal() LPLD_GPIO_Output_b(PTA, 29, 0)



//SI--PTA28
//CLK-PTA29
//AD--PTB2


#include "ccd.h"


CCD_Status_Struct CCDMain_Status;

uint8 u32_trans_uint8(uint16 data); //只有本地用

unsigned char CCDTimeMs = 0;
unsigned char CCDReady = 0;
uint16 send_data = 0;
uint8 IntegrationTime = 10;
unsigned char ccd_array[128] = { 0 };
extern short SpeedControlPeriod, DirectionConrtolPeriod;
char TimeFlag_4Ms, TimeFlag_80Ms, TimeFlag_20Ms,TimeFlag_2Ms;
char TimerMsCnt = 0;
#define CONTROL_PERIOD	4
#define SPEED_CONTROL_COUNT 20
#define SPEED_CONTROL_PERIOD (SPEED_CONTROL_COUNT * CONTROL_PERIOD)
#define DIRECTION_CONTROL_COUNT			5
#define DIRECTION_CONTROL_PERIOD		(DIRECTION_CONTROL_COUNT * CONTROL_PERIOD)

void ccd_exposure(void)
{
	unsigned char integration_piont;
	CCDTimeMs++;
	TimerMsCnt++;
	SpeedControlPeriod++;
	DirectionConrtolPeriod++;
	integration_piont = 20 - IntegrationTime;
	if (integration_piont >= 2)
	{
		if (integration_piont == CCDTimeMs)
			StartIntegration();
	}
	if (CCDTimeMs >= 20)
	{
	 	CCDTimeMs = 0;
	 	CCDReady = 1;
	}
	if (TimerMsCnt % 2 == 0)
	{
		TimeFlag_2Ms = 1;
	}
	if (TimerMsCnt % 4 == 0)
	{
		TimeFlag_4Ms = 1;
	}
	if (TimerMsCnt % 20 == 0)
	{
		TimeFlag_20Ms = 1;
	}
	if (TimerMsCnt >= 80)
	{
		TimeFlag_80Ms = 1;
		TimerMsCnt = 0;
		SpeedControlPeriod = 0;
		DirectionConrtolPeriod = 0;
	}
}



void StartIntegration(void)
{

	unsigned char i;

	SI_SetVal();            /* SI  = 1 */
	SamplingDelay();
	CLK_SetVal();           /* CLK = 1 */
	SamplingDelay();
	SI_ClrVal();            /* SI  = 0 */
	SamplingDelay();
	CLK_ClrVal();           /* CLK = 0 */

	for (i = 0; i < 127; i++) {
		SamplingDelay();
		SamplingDelay();
		CLK_SetVal();       /* CLK = 1 */
		SamplingDelay();
		SamplingDelay();
		CLK_ClrVal();       /* CLK = 0 */
	}
	SamplingDelay();
	SamplingDelay();
	CLK_SetVal();           /* CLK = 1 */
	SamplingDelay();
	SamplingDelay();
	CLK_ClrVal();           /* CLK = 0 */
}


void ImageCapture(unsigned char * ImageData)
{

	unsigned char i;
	extern uint8 AtemP;

	SI_SetVal();            /* SI  = 1 */
	SamplingDelay();
	CLK_SetVal();           /* CLK = 1 */
	SamplingDelay();
	SI_ClrVal();            /* SI  = 0 */
	SamplingDelay();

	//Delay 10us for sample the first pixel
	/**/
	for (i = 0; i < 250; i++) {                    //更改250，让CCD的图像看上去比较平滑，
		SamplingDelay();  //200ns                  //把该值改大或者改小达到自己满意的结果。
	}

	//Sampling Pixel 1

	*ImageData = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD12));
	ImageData++;
	CLK_ClrVal();           /* CLK = 0 */

	for (i = 0; i < 127; i++) {
		SamplingDelay();
		SamplingDelay();
		CLK_SetVal();       /* CLK = 1 */
		SamplingDelay();
		SamplingDelay();
		//Sampling Pixel 2~128

		*ImageData = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD12));
		ImageData++;
		CLK_ClrVal();       /* CLK = 0 */
	}
	SamplingDelay();
	SamplingDelay();
	CLK_SetVal();           /* CLK = 1 */
	SamplingDelay();
	SamplingDelay();
	CLK_ClrVal();           /* CLK = 0 */
}



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
	PixelAverageValue = PixelAverage(128, ccd_array);
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


#define LeftBoundary 8 //去掉开始的10个点
#define RightBoundary 119//去掉后面的10个点
#define CCD_Threshold 40//40  //这是两组两组差的阈值//实际使用的时候取
#define LeftLostPrepare 23
#define RightLostPrepare 105 //左右准备丢线的阈值
#define LeftMode -1
#define RightMode 1
#define MidMode 0

/*#define LostLineDiffValue 20 //准备丢线的差分值*/



char CCD_Deal_Main(unsigned char *CCDArr)
{
	int i = 0, j = 0;
	short SumTemp1_short = 0;
	short SumTemp2_short = 0;
	short SearchBeginTemp = 0;
	char LeftTempArr[50];
	char RightTempArr[50];
	unsigned char Counter_R = 0;
	unsigned char Counter_L = 0;
	unsigned char LineLenCnt = 0;
	if (CCDMain_Status.InitOK == 0)
	{
		CCDMain_Status.SearchBegin = 64;//初始化起始中点,其他的可以不初始化
		CCDMain_Status.Mode = 0;//初始状态用中线引导
		CCDMain_Status.PointCnt = 0;
	}
	if (CCDMain_Status.PointCnt > (CCD_DataLen - 1))
		CCDMain_Status.PointCnt = 0;

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
	CCDMain_Status.Left_LostFlag = 1;
	CCDMain_Status.Right_LostFlag = 1;
	CCDMain_Status.LeftLineArr[CCDMain_Status.PointCnt] = 0;
	CCDMain_Status.RightLineArr[CCDMain_Status.PointCnt] = 127;
	LineLenCnt = 0;
	for (i = 0; i < Counter_L; i++)
	{
		if ((CCDArr[LeftTempArr[i]]) - ((CCDArr[LeftTempArr[i] - 1] + CCDArr[LeftTempArr[i] + 1]) / 2) < CCD_Threshold/2)
		{
			SumTemp1_short = CCDArr[LeftTempArr[i]] + CCDArr[LeftTempArr[i] + 1];
			for (j = 1; j < 8; j++)
			{
				SumTemp2_short = CCDArr[LeftTempArr[i] - j] + CCDArr[LeftTempArr[i] - j - 1];
				if (SumTemp1_short - SumTemp2_short >CCD_Threshold)
					LineLenCnt++;
			}
			if (LineLenCnt > 3)
			{
				CCDMain_Status.LeftLineArr[CCDMain_Status.PointCnt] = LeftTempArr[i];
				CCDMain_Status.Left_LostFlag = 0;
				CCDMain_Status.LeftPoint = LeftTempArr[i];
				break;
			}
		}
	}
	LineLenCnt = 0;
	for (i = 0; i < Counter_R; i++)
	{
		if ((CCDArr[RightTempArr[i]]) - ((CCDArr[RightTempArr[i] - 1] + CCDArr[RightTempArr[i] + 1]) / 2) < CCD_Threshold)
		{
			SumTemp1_short = CCDArr[RightTempArr[i]] + CCDArr[RightTempArr[i] + 1];
			for (j = 1; j < 8; j++)
			{
				SumTemp2_short = CCDArr[RightTempArr[i] + j] + CCDArr[RightTempArr[i] + j + 1];
				if (SumTemp1_short - SumTemp2_short >CCD_Threshold)
					LineLenCnt++;
			}
			if (LineLenCnt > 3)
			{
				CCDMain_Status.RightLineArr[CCDMain_Status.PointCnt] = RightTempArr[i];
				CCDMain_Status.Right_LostFlag = 0;
				CCDMain_Status.RightPoint = RightTempArr[i];
				break;
			}
		}
	}
	if (CCDMain_Status.Left_LostFlag == 1 && CCDMain_Status.Right_LostFlag == 1)
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
			SearchBeginTemp = CCDMain_Status.LeftPoint + 8;
			CCDMain_Status.SearchBegin = SearchBeginTemp;
			CCDMain_Status.ControlValue = CCDMain_Status.LeftSet - CCDMain_Status.LeftPoint;
			//CCDMain_Status.MidPoint = CCDMain_Status.LeftPoint - CCDMain_Status.ControlValue;
			CCDMain_Status.RightPoint = 127;
		}
		else if (CCDMain_Status.Mode == RightMode)
		{
			CCDMain_Status.SearchBegin = CCDMain_Status.RightPoint - 8;
			CCDMain_Status.ControlValue = CCDMain_Status.RightSet - CCDMain_Status.RightPoint;
			//CCDMain_Status.MidPoint = CCDMain_Status.RightPoint - CCDMain_Status.ControlValue;
			CCDMain_Status.LeftPoint = 0;
		}
		else
		{
			CCDMain_Status.ErrorCnt++;
		}
	}
	else
	{
		CCDMain_Status.ControlValue /= 2;
	}
	if (CCDMain_Status.InitOK == 0)
	{
		i = (CCDMain_Status.LeftPoint + CCDMain_Status.RightPoint) / 2;
		CCDMain_Status.RightSet = 64 + i;
		CCDMain_Status.LeftSet = 64 - i;
		CCDMain_Status.MidSet = 64;
		CCDMain_Status.InitOK = 1;
		CCDMain_Status.ControlValue = 0;
	}

	return 0;
}
