#include "CCD.h"
#include "control.h"
void SamplingDelay(void);

#define SI_SetVal_M() LPLD_GPIO_Output_b(PTE, 10, 1)
#define SI_ClrVal_M() LPLD_GPIO_Output_b(PTE, 10, 0)
#define CLK_SetVal_M() LPLD_GPIO_Output_b(PTE, 11, 1)
#define CLK_ClrVal_M() LPLD_GPIO_Output_b(PTE, 11, 0)

signed char FindMidLine(unsigned char *Arr, signed char *MidPoint, unsigned char LeftPoint, unsigned char RightPoint);
CCD_Status_Struct CCDMain_Status;
signed char CCD_Correction[128];
CCD_SLave_Status_Struct CCDSlave_Status;
extern AngPID_InitTypeDef Ang_PID;
extern float SpeedSet_Variable ;
uint8 u32_trans_uint8(uint16 data); //只有本地用

unsigned char CCDTimeMs = 0;
unsigned char CCDReady = 0;
uint16 send_data = 0;
uint8 IntegrationTime = 4;
unsigned char CCDM_Arr[128] = { 0 };
unsigned char CCDS_Arr[128] = { 0 };
extern short SpeedControlPeriod, DirectionConrtolPeriod;
int TurningRight_Correction[20] = { -3, -3,-1-1 };
int TurningLeft_Correction[20] = { 3, 3, 1,1};
unsigned char Flag_SpeedGot = 0;

extern void CCDCP(void);
extern void Beep_Isr(void);
void ccd_exposure(void)
{
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


void ImageCapture_M(unsigned char * ImageData, unsigned char * ImageData2)
{

	unsigned char i;
	int j = 0;
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
	for (i = 0; i < 200; i++) {                    
		SamplingDelay();  //200ns                  //把该值改大或者改小达到自己满意的结果。
	}

	//Sampling Pixel 1
	*ImageData = LPLD_ADC_Get(ADC0, AD17) + CCD_Correction[j];
	*ImageData2 = LPLD_ADC_Get(ADC0, AD18) + CCD_Correction[j];
	ImageData++;
	ImageData2++;
	j++;
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

		*ImageData = LPLD_ADC_Get(ADC0, AD17) + CCD_Correction[j];
		*ImageData2 = LPLD_ADC_Get(ADC0, AD18) + CCD_Correction[j];
		ImageData++;
		ImageData2++;
		j++;
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



/*#define CCD_Diff_Threshold 100
void CCD_GetLine(void)
{
	short i, j, k;
	static short CCD_Main_Diff[127];
	static short CCD_Slave_Diff[127];
	unsigned char ZhongZhi1[3],ZhongZhi2[3];
	unsigned char Temp1, Temp2;
	static short SumTemp_Left_M = 0, SumTemp_Left_S = 0,SumTemp_Right_M = 0, SumTemp_Right_S = 0;
	static unsigned char MaxPoint_M = 0, MinPoint_M = 0, MaxPoint_S = 0, MinPoint_S = 0;
	static short MaxValue_M = 0, MinValue_M = 0, MaxValue_S = 0, MinValue_S = 0;
	for (i = 1; i < 125; i++)//中值滤波
	{
		ZhongZhi1[0] = CCDM_Arr[i - 1];
		ZhongZhi1[1] = CCDM_Arr[i];
		ZhongZhi1[2] = CCDM_Arr[i + 1];
		
		ZhongZhi2[0] = CCDS_Arr[i - 1];
		ZhongZhi2[1] = CCDS_Arr[i];
		ZhongZhi2[2] = CCDS_Arr[i + 1];
		for (k = 0; k < 3; k++)
		{
			for (j = 0; j < 2; j++)
			{
				if (ZhongZhi1[j] < ZhongZhi1[j + 1])
				{
					Temp1 = ZhongZhi1[j + 1];
					ZhongZhi1[j + 1] = ZhongZhi1[j];
					ZhongZhi1[j] = Temp1;
				}
				if (ZhongZhi2[j] < ZhongZhi2[j + 1])
				{
					Temp2 = ZhongZhi2[j + 1];
					ZhongZhi2[j + 1] = ZhongZhi2[j];
					ZhongZhi2[j] = Temp2;
				}
			}
		}
		CCDM_Arr[i] = ZhongZhi1[1];
		CCDS_Arr[i] = ZhongZhi2[1];
	}


	for (i = 0; i < 126; i++)
	{
		CCD_Main_Diff[i] = CCDM_Arr[i] - CCDM_Arr[i + 2];
		CCD_Slave_Diff[i] = CCDS_Arr[i] - CCDS_Arr[i + 2];
	}

	for (i = 0; i < 126; i++)
	{
		if (MaxValue_M < CCD_Main_Diff[i])
		{
			MaxValue_M= CCD_Main_Diff[i];
			MaxPoint_M = i;
		}
		if (MinValue_M> CCD_Main_Diff[i])
		{
			MinValue_M = CCD_Main_Diff[i];
			MinPoint_M = i;
		}
		if (MaxValue_S < CCD_Slave_Diff[i])
		{
			MaxValue_S = CCD_Slave_Diff[i];
			MaxPoint_S= i;
		}
		if (MinValue_S > CCD_Slave_Diff[i])
		{
			MinValue_S = CCD_Slave_Diff[i];
			MinPoint_S = i;
		}
	}
	SumTemp_Left_M = CCD_Main_Diff[MinPoint_M] + CCD_Main_Diff[MinPoint_M + 1] + CCD_Main_Diff[MinPoint_M - 1];
	SumTemp_Right_M = CCD_Main_Diff[MaxPoint_M] + CCD_Main_Diff[MaxPoint_M + 1] + CCD_Main_Diff[MaxPoint_M - 1];
	SumTemp_Left_S = CCD_Slave_Diff[MinPoint_S] + CCD_Slave_Diff[MinPoint_S + 1] + CCD_Slave_Diff[MinPoint_S - 1];
	SumTemp_Right_S = CCD_Slave_Diff[MaxPoint_S] + CCD_Slave_Diff[MaxPoint_S + 1] + CCD_Slave_Diff[MaxPoint_S - 1];
	if (SumTemp_Left_M < -CCD_Diff_Threshold)
	{
		CCDMain_Status.LeftPoint = MinPoint_M + 1;
		CCDMain_Status.Left_LostFlag = 0;
	}
	else
	{
		CCDMain_Status.Left_LostFlag = 1;
		CCDMain_Status.LeftPoint = 0;
	}
	if (SumTemp_Right_M > CCD_Diff_Threshold)
	{
		CCDMain_Status.RightPoint = MaxPoint_M + 1;
		CCDMain_Status.Right_LostFlag = 0;
	}
	else
	{
		CCDMain_Status.Right_LostFlag = 0;
		CCDMain_Status.RightPoint = 127;
	}
}
*/


void CCD_Median_Filtering(void)
{
	short i, j, k;
	unsigned char ZhongZhi1[3], ZhongZhi2[3];
	unsigned char Temp1, Temp2;
	for (i = 1; i < 125; i++)//中值滤波
	{
		ZhongZhi1[0] = CCDM_Arr[i - 1];
		ZhongZhi1[1] = CCDM_Arr[i];
		ZhongZhi1[2] = CCDM_Arr[i + 1];

		ZhongZhi2[0] = CCDS_Arr[i - 1];
		ZhongZhi2[1] = CCDS_Arr[i];
		ZhongZhi2[2] = CCDS_Arr[i + 1];
		for (k = 0; k < 3; k++)
		{
			for (j = 0; j < 2; j++)
			{
				if (ZhongZhi1[j] < ZhongZhi1[j + 1])
				{
					Temp1 = ZhongZhi1[j + 1];
					ZhongZhi1[j + 1] = ZhongZhi1[j];
					ZhongZhi1[j] = Temp1;
				}
				if (ZhongZhi2[j] < ZhongZhi2[j + 1])
				{
					Temp2 = ZhongZhi2[j + 1];
					ZhongZhi2[j + 1] = ZhongZhi2[j];
					ZhongZhi2[j] = Temp2;
				}
			}
		}
		CCDM_Arr[i] = ZhongZhi1[1];
		CCDS_Arr[i] = ZhongZhi2[1];
	}
}

#define CCD_Threshold_Min 60
void CCD_GetLine_Main(void)
{
	short TempArrLeft[50], TempArrRight[50], i=0, j = 0,k=0;
	short Sum1, Sum2, Cnt_Left = 0, Cnt_Right = 0;
	int ValueSum = 0;
	short DiffMax = 0, DiffMaxPoint = 0;
	short  m=0;
	if (CCDMain_Status.Flag_CenterLine == 0 &&(CCDMain_Status.Left_LostFlag==0 && CCDMain_Status.Right_LostFlag==0)&& (FindMidLine(CCDM_Arr, &CCDMain_Status.MidPoint, CCDMain_Status.LeftPoint , CCDMain_Status.RightPoint) == 1))
	{
		CCDMain_Status.Flag_CenterLine = 1;
		CCDMain_Status.SearchBegin = CCDMain_Status.MidPoint;
CCDMain_Status.LeftPoint =  CCDMain_Status.MidPoint-CCDMain_Status.MidSet +CCDMain_Status.LeftSet ;
		CCDMain_Status.RightPoint = CCDMain_Status.MidPoint+CCDMain_Status.RightSet - CCDMain_Status.MidSet;
	}
	else if (CCDMain_Status.Flag_CenterLine==1 && (FindMidLine(CCDM_Arr, &CCDMain_Status.MidPoint, CCDMain_Status.LeftPoint, CCDMain_Status.RightPoint)==1))
	{
		CCDMain_Status.SearchBegin = CCDMain_Status.MidPoint;
		CCDMain_Status.LeftPoint =  CCDMain_Status.MidPoint-CCDMain_Status.MidSet +CCDMain_Status.LeftSet ;
		CCDMain_Status.RightPoint = CCDMain_Status.MidPoint+CCDMain_Status.RightSet - CCDMain_Status.MidSet;
	}
	else
	{
		CCDMain_Status.Flag_CenterLine = 0;
		k = CCDMain_Status.SearchBegin - 40;
		if (k < 10)
			k = 10;
		m = CCDMain_Status.SearchBegin + 40;
		if (m > 118)
			m = 118;
		for (i = CCDMain_Status.SearchBegin; i > k; i = i - 2, j++)
		{
			ValueSum += CCDM_Arr[i];
		}
		CCDMain_Status.Threshold_Left = ValueSum / j;
		j = 0;
		ValueSum = 0;
		for (i = CCDMain_Status.SearchBegin; i < m; i = i + 2, j++)
		{
			ValueSum += CCDM_Arr[i];
		}
		CCDMain_Status.Threshold_Right = ValueSum / j;
		CCDMain_Status.Threshold_Right /= 2;
		CCDMain_Status.Threshold_Left /= 2;
		if (CCDMain_Status.Threshold_Right < CCD_Threshold_Min)
			CCDMain_Status.Threshold_Right = CCD_Threshold_Min;
		if (CCDMain_Status.Threshold_Left < CCD_Threshold_Min)
			CCDMain_Status.Threshold_Left = CCD_Threshold_Min;
		//1100011
		for (i = CCDMain_Status.SearchBegin; i > 8; i--)
		{
			Sum1 = CCDM_Arr[i] + CCDM_Arr[i - 1];
			Sum2 = CCDM_Arr[i - 5] + CCDM_Arr[i - 6];
			if ((Sum1 - Sum2) > CCDMain_Status.Threshold_Left)
			{
				TempArrLeft[Cnt_Left] = i - 2;
				Cnt_Left++;
			}
		}
		for (i = CCDMain_Status.SearchBegin; i < 120; i++)
		{
			Sum1 = CCDM_Arr[i] + CCDM_Arr[i + 1];
			Sum2 = CCDM_Arr[i + 5] + CCDM_Arr[i + 6];
			if ((Sum1 - Sum2)>CCDMain_Status.Threshold_Right)
			{
				TempArrRight[Cnt_Right] = i + 2;
				Cnt_Right++;
			}
		}
		CCDMain_Status.Left_LostFlag = 1;
		CCDMain_Status.Right_LostFlag = 1;
		CCDMain_Status.LastRightPoint = CCDMain_Status.RightPoint;
		CCDMain_Status.LastLeftPoint = CCDMain_Status.LeftPoint;
		CCDMain_Status.RightPoint = 127;
		CCDMain_Status.LeftPoint = 0;
		for (i = 0; i < Cnt_Left; i++)
		{
			if ((CCDM_Arr[TempArrLeft[i]] - (CCDM_Arr[TempArrLeft[i] + 1] + CCDM_Arr[TempArrRight[i] - 1]) / 2) < CCDMain_Status.Threshold_Left)
			{
				for (j = 0; j < 4; j++)
				{
					Sum1 = CCDM_Arr[TempArrLeft[i] - j] + CCDM_Arr[TempArrLeft[i] - 1 - j];
					Sum2 = CCDM_Arr[TempArrLeft[i] - j - 4] + CCDM_Arr[TempArrLeft[i] - j - 5];
					if (DiffMax < (Sum1 - Sum2))
					{
						DiffMax = Sum1 - Sum2;
						DiffMaxPoint = TempArrLeft[i] - 1 - j;
					}
				}
				if (DiffMax != 0)
				{
					CCDMain_Status.Left_LostFlag = 0;
					CCDMain_Status.LeftPoint = DiffMaxPoint;
					break;
				}
			}
		}
		DiffMax = 0;
		for (i = 0; i < Cnt_Right; i++)
		{
			if ((CCDM_Arr[TempArrRight[i]] - (CCDM_Arr[TempArrRight[i] - 1] + CCDM_Arr[TempArrRight[i] + 1]) / 2) < CCDMain_Status.Threshold_Right)
			{
				for (j = 0; j < 4; j++)
				{
					Sum1 = CCDM_Arr[TempArrRight[i] + j] + CCDM_Arr[TempArrRight[i] + 1 + j];
					Sum2 = CCDM_Arr[TempArrRight[i] + j + 4] + CCDM_Arr[TempArrRight[i] + 5 + j];
					if (DiffMax < (Sum1 - Sum2))
					{
						DiffMax = Sum1 - Sum2;
						DiffMaxPoint = TempArrRight[i] + 1 + j;
					}
				}
				if (DiffMax != 0)
				{
					CCDMain_Status.Right_LostFlag = 0;
					CCDMain_Status.RightPoint = DiffMaxPoint;
					break;
				}
			}
		}
		if ((CCDMain_Status.Left_LostFlag == 1 && CCDMain_Status.Right_LostFlag == 1))
		{
			CCDMain_Status.MidPoint = 64;
		}
		else if (CCDMain_Status.Left_LostFlag == 1)
		{
			CCDMain_Status.MidPoint = (CCDMain_Status.LeftPoint + CCDMain_Status.RightPoint) / 2;
			CCDMain_Status.SearchBegin = CCDMain_Status.RightPoint - 20;
		}
		else if (CCDMain_Status.Right_LostFlag == 1)
		{
			CCDMain_Status.MidPoint = (CCDMain_Status.LeftPoint + CCDMain_Status.RightPoint) / 2;
			CCDMain_Status.SearchBegin = CCDMain_Status.LeftPoint + 20;
		}
		else
		{
			CCDMain_Status.MidPoint = (CCDMain_Status.LeftPoint + CCDMain_Status.RightPoint) / 2;
			CCDMain_Status.SearchBegin = CCDMain_Status.MidPoint;
		}
		if (CCDMain_Status.SearchBegin > 115)
		{
			CCDMain_Status.SearchBegin = 115;
		}
		else if (CCDMain_Status.SearchBegin < 15)
		{
			CCDMain_Status.SearchBegin = 15;
		}
	}
}
void CCD_GetLine_Slave(void)
{
	short TempArrLeft[50], TempArrRight[50], i=0, j = 0,k=0;
	short Sum1, Sum2, Cnt_Left = 0, Cnt_Right = 0;
	int ValueSum = 0;
	short DiffMax = 0, DiffMaxPoint = 0;
	short  m=0;

	/*if (FindMidLine(CCDS_Arr, &CCDSlave_Status.MidPoint, CCDMain_Status.LeftPoint, CCDMain_Status.RightPoint) == 1)
	{
		CCDSlave_Status.Flag_CenterLine = 1;
	}
	else
	{*/
		k = CCDSlave_Status.SearchBegin - 40;
		if (k < 10)
			k = 10;
		m = CCDSlave_Status.SearchBegin + 40;
		if (m > 118)
			m = 118;
		for (i = CCDSlave_Status.SearchBegin; i > k; i = i - 2, j++)
		{
			ValueSum += CCDS_Arr[i];
		}
		CCDSlave_Status.Threshold_Left = ValueSum / j;
		j = 0;
		ValueSum = 0;
		for (i = CCDSlave_Status.SearchBegin; i < m; i = i + 2, j++)
		{
			ValueSum += CCDS_Arr[i];
		}
		CCDSlave_Status.Threshold_Right = ValueSum / j;
		CCDSlave_Status.Threshold_Right /= 2;
		CCDSlave_Status.Threshold_Left /= 2;

		if (CCDSlave_Status.Threshold_Right < CCD_Threshold_Min)
			CCDSlave_Status.Threshold_Right = CCD_Threshold_Min;
		if (CCDSlave_Status.Threshold_Left < CCD_Threshold_Min)
			CCDSlave_Status.Threshold_Left = CCD_Threshold_Min;
		//1100011
		for (i = CCDSlave_Status.SearchBegin; i > 7; i--)
		{
			Sum1 = CCDS_Arr[i] + CCDS_Arr[i - 1];
			Sum2 = CCDS_Arr[i - 5] + CCDS_Arr[i - 6];
			if ((Sum1 - Sum2) > CCDSlave_Status.Threshold_Left)
			{
				TempArrLeft[Cnt_Left] = i - 2;
				Cnt_Left++;
			}
		}
		for (i = CCDSlave_Status.SearchBegin; i < 120; i++)
		{
			Sum1 = CCDS_Arr[i] + CCDS_Arr[i + 1];
			Sum2 = CCDS_Arr[i + 5] + CCDS_Arr[i + 6];
			if ((Sum1 - Sum2)>CCDSlave_Status.Threshold_Right)
			{
				TempArrRight[Cnt_Right] = i + 2;
				Cnt_Right++;
			}
		}
		CCDSlave_Status.Left_LostFlag = 1;
		CCDSlave_Status.Right_LostFlag = 1;
		CCDSlave_Status.LastLeftPoint = CCDSlave_Status.LeftPoint;
		CCDSlave_Status.LastRightPoint = CCDSlave_Status.RightPoint;
		CCDSlave_Status.LeftPoint = 0;
		CCDSlave_Status.RightPoint = 127;
		for (i = 0; i < Cnt_Left; i++)
		{
			if ((CCDS_Arr[TempArrLeft[i]] - (CCDS_Arr[TempArrLeft[i] + 1] + CCDS_Arr[TempArrRight[i] - 1]) / 2) < CCDSlave_Status.Threshold_Left)
			{
				for (j = 0; j < 4; j++)
				{
					Sum1 = CCDS_Arr[TempArrLeft[i] - j] + CCDS_Arr[TempArrLeft[i] - 1 - j];
					Sum2 = CCDS_Arr[TempArrLeft[i] - j - 4] + CCDS_Arr[TempArrLeft[i] - j - 5];
					if (DiffMax < (Sum1 - Sum2))
					{
						DiffMax = Sum1 - Sum2;
						DiffMaxPoint = TempArrLeft[i] - 1 - j;
					}
				}
				if (DiffMax != 0)
				{
					CCDSlave_Status.Left_LostFlag = 0;
					CCDSlave_Status.LeftPoint = DiffMaxPoint;
					break;
				}
			}
		}
		DiffMax = 0;
		for (i = 0; i < Cnt_Right; i++)
		{
			if ((CCDS_Arr[TempArrRight[i]] - (CCDS_Arr[TempArrRight[i] - 1] + CCDS_Arr[TempArrRight[i] + 1]) / 2) < CCDSlave_Status.Threshold_Right)
			{
				for (j = 0; j < 4; j++)
				{
					Sum1 = CCDS_Arr[TempArrRight[i] + j] + CCDS_Arr[TempArrRight[i] + 1 + j];
					Sum2 = CCDS_Arr[TempArrRight[i] + j + 4] + CCDS_Arr[TempArrRight[i] + 5 + j];
					if (DiffMax < (Sum1 - Sum2))
					{
						DiffMax = Sum1 - Sum2;
						DiffMaxPoint = TempArrRight[i] + 1 + j;
					}
				}
				if (DiffMax != 0)
				{
					CCDSlave_Status.Right_LostFlag = 0;
					CCDSlave_Status.RightPoint = DiffMaxPoint;
					break;
				}
			}
		}

		if ((CCDSlave_Status.Left_LostFlag == 1 && CCDSlave_Status.Right_LostFlag == 1))
		{
			CCDSlave_Status.MidPoint = 127;
		}
		else if (CCDSlave_Status.Left_LostFlag == 1)
		{
			CCDSlave_Status.MidPoint = (CCDSlave_Status.LeftPoint + CCDSlave_Status.RightPoint) / 2;
			CCDSlave_Status.SearchBegin = CCDSlave_Status.RightPoint - 20;
		}
		else if (CCDSlave_Status.Right_LostFlag == 1)
		{
			CCDSlave_Status.MidPoint = (CCDSlave_Status.LeftPoint + CCDSlave_Status.RightPoint) / 2;
			CCDSlave_Status.SearchBegin = CCDSlave_Status.LeftPoint + 20;
		}
		else
		{
			CCDSlave_Status.MidPoint = (CCDSlave_Status.LeftPoint + CCDSlave_Status.RightPoint) / 2;
			CCDSlave_Status.SearchBegin = CCDSlave_Status.MidPoint;
		}
		if (CCDSlave_Status.SearchBegin > 115)
		{
			CCDSlave_Status.SearchBegin = 115;
		}
		else if (CCDSlave_Status.SearchBegin < 15)
		{
			CCDSlave_Status.SearchBegin = 15;
		}
	//}
}

signed char FindMidLine(unsigned char *Arr,signed char *MidPoint,unsigned char LeftPoint,unsigned char RightPoint)
{
	int i, j,PointCounter=0, LineLen, Center, Sum1, Sum2, Sum3, Threshold,MinValue=500;
	unsigned char TempArr[20];
	LineLen = RightPoint - LeftPoint;
	Center = (RightPoint + LeftPoint) / 2;
	for (i = 0,j=0; i < 16; i++, j++)
	{
		Sum1 += Arr[Center-8 + i];
	}
	Threshold = Sum1 / j;
	Threshold /= 4;
	if (Threshold < 20)
		Threshold = 20;
	for (i = LeftPoint + 5; i < RightPoint - 5; i++)
	{
		Sum1 = Arr[i];
		Sum2 = Arr[i - 4] ;
		Sum3 = Arr[i + 4];
		if (((Sum2-Sum1)>Threshold)&&((Sum3-Sum1)>Threshold))
		{
			TempArr[PointCounter] = i;
			PointCounter++;
		}
	}
	if (PointCounter==0)
	{
		return 0;
	}
	else
	{
		for (i = 0; i < PointCounter; i++)
		{
			if (MinValue>Arr[i])
			{
				MinValue = Arr[i];
				*MidPoint = TempArr[i];
			}
		}
		return 1;
        }
}
extern signed char FlagToPhone;
void CCD_Deal_Both(void)
{
	static signed char Flag_Cross = 0,Flag_LeftLost_Main=0,Flag_RightLost_Main=0;
	static int Cnt_Cross = 0, Cnt_LeftLost_Main = 0,Cnt_RightLost_Main=0;

	if (CCDMain_Status.LastRightPoint<100 & CCDMain_Status.LastLeftPoint>30)
	{
		if (CCDMain_Status.LeftPoint==0 && CCDMain_Status.RightPoint>100)
		{
			Flag_Cross = 1;
			CCDMain_Status.RightPoint = CCDMain_Status.LastRightPoint;
			CCDMain_Status.LeftPoint = CCDMain_Status.LastLeftPoint;
		}
		if (CCDMain_Status.RightPoint==0 && CCDMain_Status.LeftPoint<30)
		{
			Flag_Cross = 1;
			CCDMain_Status.RightPoint = CCDMain_Status.LastRightPoint;
			CCDMain_Status.LeftPoint = CCDMain_Status.LastLeftPoint;
		}
	}
	if (Flag_Cross==1)
	{
		Cnt_Cross++;
		BeepBeepBeep(500);
		if (Cnt_Cross>2)
		{
			Flag_Cross = 0;
			Cnt_Cross = 0;
		}
	}
	else
	{
		if (Flag_LeftLost_Main == 0 && CCDMain_Status.Left_LostFlag == 1)
		{
			Cnt_LeftLost_Main++;
			CCDMain_Status.Left_LostFlag = 0;
			CCDMain_Status.LeftPoint = CCDMain_Status.LastLeftPoint;
			if (Cnt_LeftLost_Main > 2)
			{
				CCDMain_Status.Left_LostFlag = 1;
			}
		}
		else
		{
			Cnt_LeftLost_Main = 0;
		}
		if (Flag_RightLost_Main == 0 && CCDMain_Status.Right_LostFlag == 1)
		{
			Cnt_RightLost_Main++;
			CCDMain_Status.Right_LostFlag = 0;
			CCDMain_Status.RightPoint = CCDMain_Status.LastRightPoint;
			if (Cnt_RightLost_Main > 2)
			{
				CCDMain_Status.Right_LostFlag = 1;
			}
		}
		else
		{
			Cnt_RightLost_Main = 0;
		}
	}
	if (CCDMain_Status.Flag_CenterLine==1)
	{
		Dir_PID.ControlValue = CCDMain_Status.MidSet - CCDMain_Status.MidPoint;
	}
	else
	{
		if (CCDMain_Status.Left_LostFlag==1 && CCDMain_Status.Right_LostFlag==1)
		{
			Dir_PID.ControlValue =0;
		}
		else
			Dir_PID.ControlValue = CCDMain_Status.MidSet - (CCDMain_Status.LeftPoint + CCDMain_Status.RightPoint) / 2;
	}
	
	Dir_PID.Kd_Temp = Dir_PID.Kd;
	Dir_PID.Kp_Temp = Dir_PID.Kp;
}

void CCD_ControlValueCale(void)
{


}

void ChangeStrategy(unsigned char Strategy)
{

}
void CCDLineInit(void)
{
	//signed short MidTemp = 0;
	float a[128];
	int i;
	
	CCDSlave_Status.LeftSet = 27;
	CCDSlave_Status.RightSet = 104;
	CCDMain_Status.LeftSet = 38;
	CCDMain_Status.RightSet = 91;
        
    CCDMain_Status.MidSet = (CCDMain_Status.LeftSet+CCDMain_Status.RightSet)/2;
	CCDMain_Status.SearchBegin = CCDMain_Status.MidSet;
	CCDSlave_Status.MidSet = (CCDSlave_Status.LeftSet+CCDSlave_Status.RightSet)/2;
	CCDSlave_Status.SearchBegin = CCDSlave_Status.MidSet;

	CCDMain_Status.LineWidth = (short)((CCDMain_Status.RightSet - CCDMain_Status.LeftSet)*0.7);
	CCDSlave_Status.LineWidth = (short)((CCDSlave_Status.RightSet - CCDSlave_Status.LeftSet)*0.7);

	CCDMain_Status.InitOK = 1;

	Dir_PID.Kd_Temp = Dir_PID.Kd;
	Dir_PID.Kp_Temp = Dir_PID.Kp;

	for (i = 0; i < 64; i++)
	{
		a[i] = (20.0 / 64)*(64 - i);
	}
	for (i = 64; i < 128; i++)
	{
		a[i] = (20.0 / 64)*(i - 64);
	}
	for (i = 0; i < 128; i++)
	{
		CCD_Correction[i] = (int)a[i];//CCD畸变的矫正数组//影响阈值的获取
	}


}