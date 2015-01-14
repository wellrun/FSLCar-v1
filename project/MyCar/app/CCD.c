#include "CCD.h"
void SamplingDelay(void);
// #define TSL1401_SI(x)   LPLD_GPIO_Output_b(PTA, 28, x)
// #define TSL1401_CLK(x)  LPLD_GPIO_Output_b(PTA, 29, x)

#define SI_SetVal_M() LPLD_GPIO_Output_b(PTA, 28, 1)
#define SI_ClrVal_M() LPLD_GPIO_Output_b(PTA, 28, 0)
#define CLK_SetVal_M() LPLD_GPIO_Output_b(PTA, 29, 1)
#define CLK_ClrVal_M() LPLD_GPIO_Output_b(PTA, 29, 0)

#define SI_SetVal_S() LPLD_GPIO_Output_b(PTA, 26, 1)
#define SI_ClrVal_S() LPLD_GPIO_Output_b(PTA, 26, 0)
#define CLK_SetVal_S() LPLD_GPIO_Output_b(PTA, 27, 1)
#define CLK_ClrVal_S() LPLD_GPIO_Output_b(PTA, 27, 0)



//SI--PTA28
//CLK-PTA29
//AD--PTB2


#include "ccd.h"


CCD_Status_Struct CCDMain_Status;

CCD_SLave_Status_Struct CCDSlave_Status;

uint8 u32_trans_uint8(uint16 data); //ֻ�б�����

unsigned char CCDTimeMs = 0;
unsigned char CCDReady = 0;
uint16 send_data = 0;
uint8 IntegrationTime = 10;
unsigned char CCDM_Arr[128] = { 0 };
unsigned char CCDS_Arr[128] = { 0 };
extern short SpeedControlPeriod, DirectionConrtolPeriod;
char TimeFlag_5Ms, TimeFlag_40Ms, TimeFlag_20Ms,TimeFlag_2Ms;
char TimerMsCnt = 0;
// #define CONTROL_PERIOD	5
// #define SPEED_CONTROL_COUNT 20
// #define SPEED_CONTROL_PERIOD (SPEED_CONTROL_COUNT * CONTROL_PERIOD)
// #define DIRECTION_CONTROL_COUNT			5
// #define DIRECTION_CONTROL_PERIOD		(DIRECTION_CONTROL_COUNT * CONTROL_PERIOD)

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
		{
			//StartIntegration_S();
			StartIntegration_M();
		}
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
	if (TimerMsCnt % 5 == 0)
	{
		TimeFlag_5Ms = 1;
	}
	if (TimerMsCnt % 20 == 0)
	{
		TimeFlag_20Ms = 1;
		DirectionConrtolPeriod = 0;
	}
	if (TimerMsCnt >= 40)
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
	SI_SetVal_S();            /* SI  = 1 */
	SamplingDelay();
	CLK_SetVal_M();           /* CLK = 1 */
	CLK_SetVal_S();           /* CLK = 1 */
	SamplingDelay();
	SI_ClrVal_M(); 
	SI_ClrVal_S(); /* SI  = 0 */
	SamplingDelay();
	CLK_ClrVal_M();  
	CLK_ClrVal_S(); /* CLK = 0 */
	SamplingDelay(); 
	SamplingDelay();
	for (i = 0; i < 127; i++) 
	{
		SamplingDelay();
		SamplingDelay();
		CLK_SetVal_M();  
		CLK_SetVal_S();/* CLK = 1 */
		SamplingDelay();
		SamplingDelay();
		CLK_ClrVal_M(); 
		CLK_ClrVal_S();  /* CLK = 0 */
	}
	SamplingDelay();
	SamplingDelay();
	CLK_SetVal_M(); 
	CLK_SetVal_S();/* CLK = 1 */
	SamplingDelay();
	SamplingDelay();
	CLK_ClrVal_M();  
	CLK_ClrVal_S();  /* CLK = 0 */
}


// void StartIntegration_S(void)
// {
// 
// 	unsigned char i;
// 
// 	SI_SetVal_S();            /* SI  = 1 */
// 	SamplingDelay();
// 	CLK_SetVal_S();           /* CLK = 1 */
// 	SamplingDelay();
// 	SI_ClrVal_S();            /* SI  = 0 */
// 	SamplingDelay();
// 	CLK_ClrVal_S();           /* CLK = 0 */
// 	SamplingDelay(); SamplingDelay();
// 	for (i = 0; i < 127; i++) {
// 		SamplingDelay();
// 		SamplingDelay();
// 		CLK_SetVal_S();       /* CLK = 1 */
// 		SamplingDelay();
// 		SamplingDelay();
// 		CLK_ClrVal_S();       /* CLK = 0 */
// 	}
// 	SamplingDelay();
// 	SamplingDelay();
// 	CLK_SetVal_S();           /* CLK = 1 */
// 	SamplingDelay();
// 	SamplingDelay();
// 	CLK_ClrVal_S();           /* CLK = 0 */
// }
// 


void ImageCapture_M(unsigned char * ImageData, unsigned char * ImageData2)
{

	unsigned char i;
	extern uint8 AtemP;

	SI_SetVal_M(); 
	SI_SetVal_S();  /* SI  = 1 */
	SamplingDelay();
	CLK_SetVal_M();
	CLK_SetVal_S();     /* CLK = 1 */
	SamplingDelay();
	SI_ClrVal_M(); 
	SI_ClrVal_S();  /* SI  = 0 */
	SamplingDelay();

	//Delay 10us for sample the first pixel
	/**/
	for (i = 0; i < 200; i++) {                    //����250����CCD��ͼ����ȥ�Ƚ�ƽ����
		SamplingDelay();  //200ns                  //�Ѹ�ֵ�Ĵ���߸�С�ﵽ�Լ�����Ľ����
	}

	//Sampling Pixel 1

	*ImageData = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD14));
	*ImageData2 = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD15));
	ImageData++;
	ImageData2++;
	CLK_ClrVal_M();   
	CLK_ClrVal_S();/* CLK = 0 */
	SamplingDelay();
	SamplingDelay();
	SamplingDelay();
	for (i = 0; i < 127; i++) 
	{
		SamplingDelay();
		SamplingDelay();
		CLK_SetVal_M();  
		CLK_SetVal_S(); /* CLK = 1 */
		SamplingDelay();
		SamplingDelay();
		//Sampling Pixel 2~128

		*ImageData = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD14));
		*ImageData2 = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD15));
		ImageData++;
		ImageData2++;
		CLK_ClrVal_M();  
		CLK_ClrVal_S(); /* CLK = 0 */
	}
	SamplingDelay();
	SamplingDelay();
	CLK_SetVal_M(); 
	CLK_SetVal_S(); /* CLK = 1 */
	SamplingDelay();
	SamplingDelay();
	CLK_ClrVal_M();
	CLK_ClrVal_S();/* CLK = 0 */
}


// void ImageCapture_S(unsigned char * ImageData)
// {
// 
// 	unsigned char i;
// 	extern uint8 AtemP;
// 
// 	SI_SetVal_S();            /* SI  = 1 */
// 	SamplingDelay();
// 	CLK_SetVal_S();           /* CLK = 1 */
// 	SamplingDelay();
// 	SI_ClrVal_S();            /* SI  = 0 */
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
// 	CLK_ClrVal_S();           /* CLK = 0 */
// 	SamplingDelay();
// 	SamplingDelay();
// 	SamplingDelay();
// 	for (i = 0; i < 127; i++) {
// 		SamplingDelay();
// 		SamplingDelay();
// 		CLK_SetVal_S();       /* CLK = 1 */
// 		SamplingDelay();
// 		SamplingDelay();
// 		//Sampling Pixel 2~128
// 
// 		*ImageData = u32_trans_uint8(LPLD_ADC_Get(ADC0, AD15));
// 		ImageData++;
// 		CLK_ClrVal_S();       /* CLK = 0 */
// 	}
// 	SamplingDelay();
// 	SamplingDelay();
// 	CLK_SetVal_S();           /* CLK = 1 */
// 	SamplingDelay();
// 	SamplingDelay();
// 	CLK_ClrVal_S();           /* CLK = 0 */
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


#define LeftBoundary 8 //ȥ����ʼ��10����
#define RightBoundary 119//ȥ�������10����
#define CCD_Threshold 40//40  //����������������ֵ//ʵ��ʹ�õ�ʱ��ȡ
#define LeftLostPrepare 12
#define RightLostPrepare 116 //����׼�����ߵ���ֵ
#define LeftMode -1
#define RightMode 1
#define MidMode 0
#define SearchMode_Left -1
#define SearchMode_Right 1 //���ұ�Ѱ��
#define SearchMode_Double 0 //˫��Ѱ��
#define CCDSlaveModeChangeModeValueLeft 45//���ߴ���30��ʱ���л�ģʽ
#define CCDSlaveModeChangeModeValueRight 85//����С��90��ʱ���л�ģʽ
#define Msg_UseMainCCD 0
#define Msg_UseSlaveCCD 1
#define Road_TurnRight 0
#define Road_TurnLeft 1
#define Road_Cross 2
#define Road_Straight 3
#define CCDMain_LeftSet 30
#define CCDMain_RightSet 100
void CCDLineInit(void);
unsigned short CCDInitCnt = 0;//������ʼ�������ߵļ���,,ÿ���������³�ʼ��������λ��
signed char CCDMsg = 0;
short CCDInitLine_Left = 0;
short CCDInitLine_Right = 0;
/*#define LostLineDiffValue 20 //׼�����ߵĲ��ֵ*/



char CCD_Deal_Main(unsigned char *CCDArr)
{
	int i = 0, j = 0;
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
		CCDMain_Status.Mode = 0;//��ʼ״̬����������
		CCDMain_Status.PointCnt = 0;
		CCDMain_Status.LastLeftPoint = -1;
		CCDMain_Status.LastRightPoint = -1;
	}
	if (CCDMain_Status.PointCnt > (CCD_DataLen - 1))
		CCDMain_Status.PointCnt = 0;

	if (CCDSlave_Status.Road == Road_TurnLeft)
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
	if (CCDSlave_Status.Road == Road_TurnRight)
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
	CCDMain_Status.LeftLineArr[CCDMain_Status.PointCnt] = 0;
	CCDMain_Status.RightPoint = 127;
	CCDMain_Status.LeftPoint = 0;
	CCDMain_Status.RightLineArr[CCDMain_Status.PointCnt] = 127;
	LineLenCnt = 0;
	if (CCDSlave_Status.Road == Road_TurnRight)
	{
		for (i = 0; i < Counter_L; i++)
		{
			if ((CCDArr[LeftTempArr[i]]) - ((CCDArr[LeftTempArr[i] - 1] + CCDArr[LeftTempArr[i] + 1]) / 2) < CCD_Threshold / 2)
			{
				if (CCDMain_Status.LastLeftPoint >= 0)
				{
					if (LeftTempArr[i] - CCDMain_Status.LastLeftPoint > 20 || LeftTempArr[i] - CCDMain_Status.LastLeftPoint < -20)
						continue;
				}
				else
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
						CCDMain_Status.LastLeftPoint = CCDMain_Status.LeftPoint;
						CCDMain_Status.LeftPoint = LeftTempArr[i];
						break;
					}
				}
			}
		}
	}
	LineLenCnt = 0;
	if (CCDSlave_Status.Road == Road_TurnLeft)
	{
		for (i = 0; i < Counter_R; i++)
		{
			if ((CCDArr[RightTempArr[i]]) - ((CCDArr[RightTempArr[i] - 1] + CCDArr[RightTempArr[i] + 1]) / 2) < CCD_Threshold)
			{
				if (CCDMain_Status.LastRightPoint >= 0)
				{
					if (RightTempArr[i] - CCDMain_Status.LastRightPoint > 20 || RightTempArr[i] - CCDMain_Status.LastRightPoint < -20)
						continue;
				}
				else
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
						CCDMain_Status.LastRightPoint = CCDMain_Status.RightPoint;
						CCDMain_Status.RightPoint = RightTempArr[i];
						break;
					}
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
	}*/
	if (CCDSlave_Status.Road == Road_TurnLeft)
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
	}
// 	if (CCDMain_Status.InitOK == 0)
// 	{
// 		CCDLineInit();
// 		CCDMain_Status.ControlValue = 0;
// 	}

	CCDMain_Status.PointCnt++;
	//2015��1��8�� 01:46:35  �����ϴε�������жϺ��ߵ�λ���Ƿ����..�����һ�߷���������..��ô����Ҫ�ж��Ƿ���û����
	return 0;
}

void CCDLineInit(void)
{
	signed short MidTemp = 0;
	//if (CCDMain_Status.Right_LostFlag ==0)
		//if (CCDMain_Status.Left_LostFlag ==0)
			if (CCDSlave_Status.Left_LostFlag==0)
				if (CCDSlave_Status.Right_LostFlag == 0)
				{
					MidTemp = CCDSlave_Status.MidPoint - CCDMain_Status.MidPoint;//����е�Ĳ�ֵ
					if (MidTemp<3 && MidTemp>-3)
					{
						CCDMain_Status.InitOK = 1;
					}
				}
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
	if (CCDMain_Status.PointCnt > (CCD_DataLen - 1))
		CCDMain_Status.PointCnt = 0;
	if (CCDMain_Status.InitOK == 0)
	{
		CCDSlave_Status.SearchBegin = 64;
		CCDSlave_Status.LastLeftPoint = -1;
		CCDSlave_Status.LastRightPoint = -1;
		CCDSlave_Status.PointCnt = 0;
	}
	if (CCDSlave_Status.Mode == SearchMode_Right || CCDSlave_Status.Mode == SearchMode_Double)
	{
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
	}
	if (CCDSlave_Status.Mode == SearchMode_Left || CCDSlave_Status.Mode == SearchMode_Double)
	{
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
	}

	CCDSlave_Status.Left_LostFlag = 1;
	CCDSlave_Status.Right_LostFlag = 1;
	CCDSlave_Status.LeftLineArr[CCDSlave_Status.PointCnt] = 0;
	CCDSlave_Status.RightLineArr[CCDSlave_Status.PointCnt] = 127;
 	CCDSlave_Status.LeftPoint = 0;
 	CCDSlave_Status.RightPoint = 127;
	LineLenCnt = 0;
	if (CCDSlave_Status.Mode == SearchMode_Left || CCDSlave_Status.Mode == SearchMode_Double)
	{
		for (i = 0; i < Counter_L; i++)
		{
			if ((CCDArr[LeftTempArr[i]]) - ((CCDArr[LeftTempArr[i] - 1] + CCDArr[LeftTempArr[i] + 1]) / 2) < CCD_Threshold / 2)
			{
				//������һ�б�ʾȷ������һ�����ߵ��ǲ�ȷ���ǲ�����һ���غ������ĺ���
				if (CCDSlave_Status.LastLeftPoint >= 0)
				{
					if (LeftTempArr[i] - CCDSlave_Status.LastLeftPoint > 20 || LeftTempArr[i] - CCDSlave_Status.LastLeftPoint < -20)
					{
						CCDSlave_Status.ErrorCntLeft++;
						if (CCDSlave_Status.ErrorCntLeft > 10)
						{
							CCDSlave_Status.ErrorCntLeft = 0;
							CCDSlave_Status.LastLeftPoint = -1;
						}
						else
							continue;
					}
				}
				else
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
						CCDSlave_Status.LeftLineArr[CCDSlave_Status.PointCnt] = LeftTempArr[i];
						CCDSlave_Status.Left_LostFlag = 0;
						CCDSlave_Status.LastLeftPoint = CCDSlave_Status.LeftPoint;
						CCDSlave_Status.LeftPoint = LeftTempArr[i];
						break;
					}
				}
			}
		}
	}
	LineLenCnt = 0;
	if (CCDSlave_Status.Mode == SearchMode_Right || CCDSlave_Status.Mode == SearchMode_Double)
	{
		for (i = 0; i < Counter_R; i++)
		{
			if ((CCDArr[RightTempArr[i]]) - ((CCDArr[RightTempArr[i] - 1] + CCDArr[RightTempArr[i] + 1]) / 2) < CCD_Threshold)
			{
				if (CCDSlave_Status.LastRightPoint >= 0)
				{
					if (RightTempArr[i] - CCDSlave_Status.LastRightPoint > 20 || RightTempArr[i] - CCDSlave_Status.LastRightPoint < -20)
					{
						CCDSlave_Status.ErrorCntRight++;
						if (CCDSlave_Status.ErrorCntRight > 10)
						{
							CCDSlave_Status.ErrorCntRight = 0;
							CCDSlave_Status.LastRightPoint = -1;
						}
						else
							continue;
					}
				}
				else
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
						CCDSlave_Status.RightLineArr[CCDSlave_Status.PointCnt] = RightTempArr[i];
						CCDSlave_Status.Right_LostFlag = 0;
						CCDSlave_Status.RightPoint = RightTempArr[i];
						break;
					}
				}
			}
		}
	}
	if (CCDSlave_Status.Right_LostFlag == 1 && CCDSlave_Status.Left_LostFlag == 1)
	{
		//��Ҫ�ж��Ƿ�Ϊֱ����?
		//CCDSlave_Status.ControlValue /= CCDSlave_Status.ControlValue;
		CCDSlave_Status.SearchMode = SearchMode_Double;
	}
	else
	{//�����ҵ����߸�����Ѱ�����жϵ�·���ͺ��Ƿ��ʼ�����
		if (CCDSlave_Status.Right_LostFlag == 0 && CCDSlave_Status.Left_LostFlag == 0)//��������߶���
		{
			CCDSlave_Status.SearchBegin = (CCDSlave_Status.RightPoint + CCDSlave_Status.LeftPoint) / 2;
			CCDSlave_Status.MidPoint = CCDSlave_Status.SearchBegin;
			CCDSlave_Status.Mode = MidMode;
			CCDSlave_Status.ControlValue = CCDSlave_Status.MidSet - CCDSlave_Status.MidPoint;
			CCDSlave_Status.Road = Road_Straight;
		}
		else if (CCDSlave_Status.Right_LostFlag == 0 && CCDSlave_Status.Right_LostFlag==1)//����ұ߶���
		{
			CCDSlave_Status.LastRightPoint = -1;
			if (CCDSlave_Status.RightPoint >= 30)
			{
				CCDSlave_Status.SearchBegin = CCDSlave_Status.RightPoint - 15;
				CCDSlave_Status.SearchMode = SearchMode_Double;
			}
			else
			{
				CCDSlave_Status.SearchBegin = 20;
				CCDSlave_Status.SearchMode = SearchMode_Right;
			}
			CCDSlave_Status.MidPoint = 0;
			CCDSlave_Status.Mode = LeftMode;
			CCDSlave_Status.ControlValue = CCDSlave_Status.LeftSet - CCDSlave_Status.LeftPoint;
		}
		else if (CCDSlave_Status.Left_LostFlag == 0 && CCDSlave_Status.Right_LostFlag==1)
		{
			CCDSlave_Status.LastLeftPoint = -1;
			if (CCDSlave_Status.LeftPoint <= 100)
			{
				CCDSlave_Status.SearchBegin = CCDSlave_Status.LeftPoint + 15;
				CCDSlave_Status.SearchMode = SearchMode_Double;
			}
			else
			{
				CCDSlave_Status.SearchBegin = 110;
				CCDSlave_Status.SearchMode = SearchMode_Left;
			}
			CCDSlave_Status.MidPoint = 0;
			CCDSlave_Status.Mode = RightMode;
			CCDSlave_Status.ControlValue = CCDSlave_Status.RightSet - CCDSlave_Status.RightPoint;
		}
		if (CCDSlave_Status.LeftPoint > CCDSlaveModeChangeModeValueLeft )
		{
			CCDSlave_Status.Road = Road_TurnRight;
			if (CCDMsg != Msg_UseMainCCD)
			{
				CCDMsg = Msg_UseMainCCD;
				CCDMain_Status.SearchBegin = 80;
			}
		}
		else if (CCDSlave_Status.RightPoint < CCDSlaveModeChangeModeValueRight)
		{
			CCDSlave_Status.Road = Road_TurnLeft;
			if (CCDMsg != Msg_UseMainCCD)
			{
				CCDMsg = Msg_UseMainCCD;
				CCDMain_Status.SearchBegin = 40;
			}
		}
		if (CCDMain_Status.LeftPoint<CCDSlaveModeChangeModeValueLeft)
		{
			if (CCDMsg != Msg_UseSlaveCCD)
			{
				CCDMsg = Msg_UseSlaveCCD;
			}
		}
		if (CCDMain_Status.RightPoint > CCDSlaveModeChangeModeValueRight)
		{
			if (CCDMsg != Msg_UseSlaveCCD)
			{
				CCDMsg = Msg_UseSlaveCCD;
			}
		}
	}
	CCDSlave_Status.PointCnt++;
}

void CCD_ControlValueCale(void)
{
	
}