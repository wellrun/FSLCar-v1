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
extern float SpeedSet_Variable ;
uint8 u32_trans_uint8(uint16 data); //ֻ�б�����

unsigned char CCDTimeMs = 0;
unsigned char CCDReady = 0;
uint16 send_data = 0;
uint8 IntegrationTime = 4;
unsigned char CCDM_Arr[128] = { 0 };
unsigned char CCDS_Arr[128] = { 0 };
extern short SpeedControlPeriod, DirectionConrtolPeriod;
int TurningRight_Correction[20] = { -3, -3,-1-1 };
int TurningLeft_Correction[20] = { 3, 3, 1,1};


// #define CONTROL_PERIOD	5
// #define SPEED_CONTROL_COUNT 20
// #define SPEED_CONTROL_PERIOD (SPEED_CONTROL_COUNT * CONTROL_PERIOD)
// #define DIRECTION_CONTROL_COUNT			5
// #define DIRECTION_CONTROL_PERIOD		(DIRECTION_CONTROL_COUNT * CONTROL_PERIOD)
extern void CCDCP(void);
extern void Beep_Isr(void);
void ccd_exposure(void)
{
	/*unsigned char integration_piont;
	CCDTimeMs++;
	TimerMsCnt++;
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
	Beep_Isr();*/
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
	for (i = 0; i < 100; i++) {                    
		SamplingDelay();  //200ns                  //�Ѹ�ֵ�Ĵ���߸�С�ﵽ�Լ�����Ľ����
	}

	//Sampling Pixel 1
	*ImageData = LPLD_ADC_Get(ADC0, AD14);
	*ImageData2 = LPLD_ADC_Get(ADC0, AD15);
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

		*ImageData = LPLD_ADC_Get(ADC0, AD14);
		*ImageData2 = LPLD_ADC_Get(ADC0, AD15);
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
// 	for (i = 0; i < 200; i++) {                    //����250����CCD��ͼ����ȥ�Ƚ�ƽ����
// 		SamplingDelay();  //200ns                  //�Ѹ�ֵ�Ĵ���߸�С�ﵽ�Լ�����Ľ����
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



/* �ع�ʱ�䣬��λms */

void CalculateIntegrationTime(void)
{
	//extern uint8 Pixel[128];
	/* 128�����ص��ƽ��ADֵ */
	uint8 PixelAverageValue;
	/* 128�����ص��ƽ����ѹֵ��10�� */
	uint8 PixelAverageVoltage;
	/* �趨Ŀ��ƽ����ѹֵ��ʵ�ʵ�ѹ��10�� */
	uint16 TargetPixelAverageVoltage = 25;
	/* �趨Ŀ��ƽ����ѹֵ��ʵ��ֵ��ƫ�ʵ�ʵ�ѹ��10�� */
	char PixelAverageVoltageError = 0;
	/* �趨Ŀ��ƽ����ѹֵ�����ƫ�ʵ�ʵ�ѹ��10�� */
	uint16 TargetPixelAverageVoltageAllowError = 2;

	/* ����128�����ص��ƽ��ADֵ */
	PixelAverageValue = PixelAverage(128, CCDM_Arr);
	/* ����128�����ص��ƽ����ѹֵ,ʵ��ֵ��10�� */
	PixelAverageVoltage = (unsigned char)((int)PixelAverageValue * 25 / 194);//��0-194ƽ���ֳ���25��

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



#define CCD_Diff_Threshold 100
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
	for (i = 1; i < 125; i++)//��ֵ�˲�
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









#define LeftBoundary 8 //ȥ����ʼ��10����
#define RightBoundary 120//ȥ�������10����
#define CCD_Threshold 50//40  //����������������ֵ//ʵ��ʹ�õ�ʱ��ȡ
#define LeftLostPrepare 10
#define RightLostPrepare 118 //����׼�����ߵ���ֵ
#define LeftMode -1
#define RightMode 1
#define MidMode 0
#define SearchMode_Left -1
#define SearchMode_Right 1 //���ұ�Ѱ��
#define SearchMode_Double 0 //˫��Ѱ��
#define CCDSlaveModeChangeModeValueLeft 64//���ߴ���30��ʱ���л�ģʽ
#define CCDSlaveModeChangeModeValueRight 64//����С��90��ʱ���л�ģʽ
#define Msg_UseMainCCD 0
#define Msg_UseSlaveCCD 1
#define Msg_Lost 2
#define Road_TurnRight 0
#define Road_TurnLeft 1
#define Road_Cross 2
#define Road_Straight 3
void CCDLineInit(void);
unsigned short CCDInitCnt = 0;//������ʼ�������ߵļ���,,ÿ���������³�ʼ��������λ��
signed char CCDMsg = 0;
short CCDInitLine_Left = 0;
short CCDInitLine_Right = 0;
signed char CCD_Correction[128];//ccd����Ľ���ֵ
unsigned char Flag_SpeedGot = 0;
/*#define LostLineDiffValue 20 //׼�����ߵĲ��ֵ*/


//24 102

void CCD_Median_Filtering(void)
{
	short i, j, k;
	unsigned char ZhongZhi1[3], ZhongZhi2[3];
	unsigned char Temp1, Temp2;
	for (i = 1; i < 125; i++)//��ֵ�˲�
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
	//��ֹ���
	if (CCDMain_Status.SearchBegin < 0)
		CCDMain_Status.SearchBegin = 0;
	if (CCDMain_Status.SearchBegin > 126)
		CCDMain_Status.SearchBegin = 127;

	if (CCDMain_Status.InitOK == 0)
	{
		CCDMain_Status.SearchBegin = 64;//��ʼ����ʼ�е�,�����Ŀ��Բ���ʼ��
		//CCDMain_Status.Mode = 0;//��ʼ״̬����������
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
			SumTemp2_short = CCDArr[i + 4] + CCDArr[i + 5];//110011�����
			if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//��ʾ�½����ر��
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
			if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//��ʾ�½����ر��
			{
				LeftTempArr[Counter_L] = i - 2; //Ϊ�����뷨,��Ҫ�ж�������Ƿ�Ϊ����
				Counter_L++;
			}  //�ҵ�һ�������...��������һ�ԵñȽ�..���м������߱Ƚϵó��½���,��λ�÷ų���ʱ��������,�Ժ����� ����;
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
	CCDMain_Status.MidPoint = (CCDMain_Status.LeftPoint + CCDMain_Status.RightPoint) / 2;
	CCDMain_Status.SearchBegin = CCDMain_Status.MidPoint;
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
		SumTemp2_short = CCDArr[i + 4] + CCDArr[i + 5];//110011�����
		if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//��ʾ�½����ر��
		{
			RightTempArr[Counter_R] = i + 2;
			Counter_R++;
		}
	}


	for (i = CCDSlave_Status.SearchBegin; i > LeftBoundary; i--)
	{
		SumTemp1_short = CCDArr[i] + CCDArr[i - 1];
		SumTemp2_short = CCDArr[i - 4] + CCDArr[i - 5];
		if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//��ʾ�½����ر��
		{
			LeftTempArr[Counter_L] = i - 2; //Ϊ�����뷨,��Ҫ�ж�������Ƿ�Ϊ����
			Counter_L++;
		}  //�ҵ�һ�������...��������һ�ԵñȽ�..���м������߱Ƚϵó��½���,��λ�÷ų���ʱ��������,�Ժ����� ����;
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
			//������һ�б�ʾȷ������һ�����ߵ��ǲ�ȷ���ǲ�����һ���غ������ĺ���
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
	CCDSlave_Status.MidPoint = (CCDSlave_Status.LeftPoint + CCDSlave_Status.RightPoint) / 2;
	CCDSlave_Status.SearchBegin = CCDSlave_Status.MidPoint;
}

void CCD_Deal_Both(void)
{
	static int Cnt_TurnLeft = 0, Cnt_TurnRight = 0, Cnt_Straight=0;
	static int Cnt_SpeedChange = 0, Cnt_Lost_Main_L = 0, Cnt_Lost_Main_R = 0, Cnt_Lost_Slave_L = 0, Cnt_Lost_Slave_R = 0;
	static signed char Flag_Lost_Main_L = 0, Flag_Lost_Main_R = 0, Flag_Lost_Slave_R = 0, Flag_Lost_Slave_L= 0, Flag_Cross = 0;
	static int Cnt_Cross = 0;
	static signed char Flag_Ohm = 0;
	static signed char Flag_TurnRight = 0, Flag_TurnLeft = 0, Flag_Straight = 0;
	static int Cnt_Assist = 0;
	static int DistanceOfCar = 0;
	static signed char Flag_DistanceCnt = 0;
	static float k = 0.8;//��ccd��Ȩ��
	if (CCDSlave_Status.Left_LostFlag==1)
	{

		if (CCDSlave_Status.RightPoint < (CCDSlave_Status.MidSet+10))
		{
			Flag_Lost_Slave_L = 1;
		}
		if (Flag_Lost_Slave_L == 0)
		{
			Cnt_Lost_Slave_L++;
			if (Cnt_Lost_Slave_L > 4)
			{
				Flag_Lost_Slave_L = 1;
			}
			else
			{
				CCDSlave_Status.Left_LostFlag = 0;
				CCDSlave_Status.LeftPoint = CCDSlave_Status.LastLeftPoint;
			}
		}
	
	}
	else
	{
		Flag_Lost_Slave_L = 0;
		Cnt_Lost_Slave_L = 0;
	}
	if (CCDSlave_Status.Right_LostFlag == 1)
	{
		if (CCDSlave_Status.LeftPoint > (CCDSlave_Status.MidSet - 10))
		{
			Flag_Lost_Slave_R = 1;
		}
		if (Flag_Lost_Slave_R == 0)
		{
			Cnt_Lost_Slave_R++;
			
			if (Cnt_Lost_Slave_R>4)
			{
				Flag_Lost_Slave_R = 1;
			}
			else
			{
				CCDSlave_Status.Right_LostFlag = 0;
				CCDSlave_Status.RightPoint = CCDSlave_Status.LastRightPoint;
			}
			
		}
	}
	else
	{
		Flag_Lost_Slave_R = 0;
		Cnt_Lost_Slave_R = 0;
	}
	if (Flag_Lost_Slave_R==1 && Flag_Lost_Slave_L==1 &&(CCDMain_Status.MidPoint-CCDMain_Status.MidSet<6 || CCDMain_Status.MidPoint-CCDMain_Status.MidSet>-6))
	{
		if (Flag_Cross == 0)
		{
			Cnt_Cross++;
			if (Cnt_Cross > 2)
			{
				Flag_Cross = 1;
			}
		}
	}
	else
	{
		Flag_Cross = 0;
		Cnt_Cross = 0;
	}
	if (CCDMain_Status.Left_LostFlag == 1)
	{
		if (Flag_Lost_Main_L == 0)
		{
			Cnt_Lost_Main_L++;
			if (Cnt_Lost_Main_L > 2)
			{
				Flag_Lost_Main_L = 1;
			}
			else
			{
				CCDMain_Status.Left_LostFlag = 0;
				CCDMain_Status.LeftPoint = CCDMain_Status.LastLeftPoint;
			}
		}
	}
	else
	{
		Cnt_Lost_Main_L = 0;
		Flag_Lost_Main_L = 0;
	}
	if (CCDMain_Status.Right_LostFlag == 1)
	{
		if (Flag_Lost_Main_R == 0)
		{
			Cnt_Lost_Main_R++;
			if (Cnt_Lost_Main_R > 2)
			{
				Flag_Lost_Main_R = 1;
			}
			else
			{
				CCDMain_Status.Right_LostFlag = 0;
				CCDMain_Status.RightPoint = CCDMain_Status.LastRightPoint;
			}
		}
	}
	else
	{
		Cnt_Lost_Main_R = 0;
		Flag_Lost_Main_R = 0;
	}
	if ((CCDMain_Status.Left_LostFlag == 1 && CCDMain_Status.Right_LostFlag == 1))
	{
		CCDMain_Status.ControlValue = 0;
		CCDMain_Status.MidPoint = 127;
	}
	else
	{
		CCDMain_Status.MidPoint = (CCDMain_Status.LeftPoint + CCDMain_Status.RightPoint) / 2;
		CCDMain_Status.SearchBegin = CCDMain_Status.MidPoint;
		CCDMain_Status.ControlValue = CCDMain_Status.MidSet - CCDMain_Status.MidPoint;
	}
	if (!(CCDSlave_Status.Left_LostFlag == 1 && CCDSlave_Status.Right_LostFlag == 1))
	{

		CCDSlave_Status.MidPoint = (CCDSlave_Status.LeftPoint + CCDSlave_Status.RightPoint) / 2;
		if (CCDSlave_Status.Left_LostFlag==1)
		{
			CCDSlave_Status.SearchBegin = CCDSlave_Status.RightPoint-10;
			if (CCDSlave_Status.SearchBegin < 15)
				CCDSlave_Status.SearchBegin = 15;
			CCDSlave_Status.ControlValue = CCDSlave_Status.RightSet - CCDSlave_Status.RightPoint;
		}
		else if (CCDSlave_Status.Right_LostFlag)
		{
			CCDSlave_Status.SearchBegin = CCDSlave_Status.LeftPoint + 10;
			if (CCDSlave_Status.SearchBegin > 115)
				CCDSlave_Status.SearchBegin = 115;
			CCDSlave_Status.ControlValue = CCDSlave_Status.LeftSet - CCDSlave_Status.LeftPoint;
		}
		else
		{
			CCDSlave_Status.SearchBegin = CCDSlave_Status.MidPoint;
			CCDSlave_Status.ControlValue = CCDSlave_Status.MidSet - CCDSlave_Status.MidPoint;
		}
			
	}
	else
	{
		CCDSlave_Status.MidPoint = 1;
		CCDSlave_Status.ControlValue = 0;
	}
	/*if (((CCDSlave_Status.ControlValue - CCDMain_Status.ControlValue) > 25 || (CCDSlave_Status.ControlValue - CCDMain_Status.ControlValue) < -25)\
		|| CCDMain_Status.ControlValue>10 || CCDMain_Status.ControlValue <-10)
	{
		Cnt_Straight = 0;
		Dir_PID.Kp_Temp = Dir_PID.Kp;
		Dir_PID.Kd_Temp = Dir_PID.Kd;
		SpeedSet_Variable = Speed_PID.SpeedSet;
	}
	else
	{
		Cnt_Straight++;
		if (Cnt_Straight > 12)
		{
			Dir_PID.Kp_Temp = Dir_PID.Kp*0.4;
			Dir_PID.Kd_Temp = Dir_PID.Kd;
			SpeedSet_Variable = Speed_PID.SpeedSet;
		}
	}*/
	if (CCDMain_Status.Left_LostFlag==0 && CCDMain_Status.Right_LostFlag==0 &&CCDSlave_Status.Right_LostFlag==0\
		&& CCDSlave_Status.Left_LostFlag==0)
	{
		Cnt_Straight++;
		if (Cnt_Straight > 5)
		{
			Dir_PID.Kp_Temp = Dir_PID.Kp*0.55;
			Dir_PID.Kd_Temp = Dir_PID.Kd;
			SpeedSet_Variable = Speed_PID.SpeedSet;
		}
		
	}
	else
	{
		Cnt_Straight = 0;
		Dir_PID.Kp_Temp = Dir_PID.Kp;
		Dir_PID.Kd_Temp = Dir_PID.Kd;
		SpeedSet_Variable = Speed_PID.SpeedSet;
	}
	if ((CCDSlave_Status.Left_LostFlag == 0 && CCDSlave_Status.Right_LostFlag == 0))
	{
		AngleIntegraed = 0;
		Flag_Ohm = 0;
		Cnt_Assist = 0;
		k = 0.7;
	}
	if ((AngleIntegraed > 80 || AngleIntegraed < -80) && (CCDMain_Status.Left_LostFlag == 1 || CCDMain_Status.Right_LostFlag == 1))
	{
		Cnt_Assist++;
		if (Cnt_Assist>3)
		{
			if (Flag_Ohm == 0)
			{
				Flag_Ohm = 1;
				AngleIntegraed = 0;
				k = 0.3;
			}
		}//û��վ������ֱ�����ԣ����߼�ת������ŷķ��
	}
	/*if (CCDSlave_Status.Left_LostFlag == 0 && CCDSlave_Status.Right_LostFlag == 0 && CCDMain_Status.Left_LostFlag == 0 \
		&& CCDMain_Status.Right_LostFlag == 0)
	{//ֱ����Сs
		Dir_PID.Kp_Temp = Dir_PID.Kp*0.4;
		Dir_PID.Kd_Temp = Dir_PID.Kd;
		SpeedSet_Variable = Speed_PID.SpeedSet;
		AngleIntegraed = 0;
		Flag_Ohm = 0;
		Cnt_Assist = 0;
		k = 0.9;
		Cnt_TurnLeft = 0;
		Cnt_TurnRight = 0;
	}
	else if (CCDSlave_Status.Left_LostFlag==1 && CCDSlave_Status.Right_LostFlag==0)
	{
		Cnt_TurnLeft++;
		if (Cnt_TurnLeft>3)
		{
			Dir_PID.Kp_Temp = Dir_PID.Kp;
			Dir_PID.Kd_Temp = Dir_PID.Kd;
			SpeedSet_Variable = Speed_PID.SpeedSet;
		}
		else
		{
			Dir_PID.Kp_Temp = Dir_PID.Kp*0.8;
			Dir_PID.Kd_Temp = Dir_PID.Kd;
			SpeedSet_Variable = Speed_PID.SpeedSet;
		}
	}
	else if (CCDSlave_Status.Left_LostFlag==0 && CCDSlave_Status.Right_LostFlag==1)
	{
		Cnt_TurnRight++;
		if (Cnt_TurnRight>3)
		{
			Dir_PID.Kp_Temp = Dir_PID.Kp;
			Dir_PID.Kd_Temp = Dir_PID.Kd;
			SpeedSet_Variable = Speed_PID.SpeedSet;
		}
		else
		{
			Dir_PID.Kp_Temp = Dir_PID.Kp*0.8;
			Dir_PID.Kd_Temp = Dir_PID.Kd;
			SpeedSet_Variable = Speed_PID.SpeedSet;
		}
	}*/

	if (CCDMain_Status.Left_LostFlag == 1 && CCDMain_Status.Right_LostFlag == 1)
	{//��CCD�����߶�����,����Ϊʮ��
		if (CCDSlave_Status.Left_LostFlag == 1 && CCDSlave_Status.Right_LostFlag == 1)
		{
			Dir_PID.ControlValue = 0;
		}
		else if (CCDSlave_Status.Left_LostFlag == 1 || CCDSlave_Status.Right_LostFlag == 1)
		{//�����CCD���ߵ��Ǵ�CCDֻ����һ����,��ô����ֵ�ɴ�CCD��һ��Ȩ��ȷ��
			Dir_PID.ControlValue = CCDSlave_Status.ControlValue*0.4;
		}
		else if (CCDSlave_Status.Left_LostFlag == 0 && CCDSlave_Status.Right_LostFlag == 0)
		{
			Dir_PID.ControlValue = CCDSlave_Status.ControlValue*0.7;
		}
		else
		{
			Dir_PID.ControlValue = 0;
		}
	}
	else if (CCDSlave_Status.Left_LostFlag == 1 && CCDSlave_Status.Right_LostFlag == 1)
	{
		//�����CCD�����߶�����,��ôȨ��Ϊ0
		Dir_PID.ControlValue = CCDMain_Status.ControlValue;
	}
	else if (CCDSlave_Status.Left_LostFlag == 1 || CCDSlave_Status.Right_LostFlag == 1)
	{
		//�������֮һ����,,Ȩ������
		Dir_PID.ControlValue = CCDMain_Status.ControlValue*k + CCDSlave_Status.ControlValue*(1-k);
	}
	else
	{
		Dir_PID.ControlValue = CCDMain_Status.ControlValue;
	}

}

void Xielv(void)
{
	typedef struct 
	{
		unsigned char CCD0_LeftPoint[3000];
		unsigned char CCD0_RightPoint[3000];
		unsigned char CCD1_LeftPoint[3000];
		unsigned char CCD1_RightPoint[3000];
		int Distance;
	}Lujin_Struct;
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
	
	CCDMain_Status.LeftSet = 40;
	CCDMain_Status.RightSet = 88;


	CCDSlave_Status.LeftSet = 50;
	CCDSlave_Status.RightSet = 77;
        
        CCDMain_Status.MidSet = (CCDMain_Status.LeftSet+CCDMain_Status.RightSet)/2;
	CCDSlave_Status.MidSet = (CCDSlave_Status.LeftSet+CCDSlave_Status.RightSet)/2;

	CCDMain_Status.InitOK = 1;

	Dir_PID.Kd_Temp = Dir_PID.Kd;
	Dir_PID.Kp_Temp = Dir_PID.Kp;

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
		CCD_Correction[i] = (int)a[i];//CCD����Ľ�������//Ӱ����ֵ�Ļ�ȡ
	}


}