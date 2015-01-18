#include "init.h"
#include "datastructure.h"
#include "DEV_MMA8451.h"
#include "mpu6050.h"
#include "ccd.h"
ADC_InitTypeDef Init_ADC_Struct;
ADC_InitTypeDef Init_ADC_CCD_Struct;
GPIO_InitTypeDef Init_GPIO_Struct;
FTM_InitTypeDef Init_FTM_Struct;
PIT_InitTypeDef Init_PIT_Struct;
UART_InitTypeDef Init_UART_Struct;
//I2C_InitTypeDef Init_I2C_Struct;
DMA_InitTypeDef Init_DMA_Struct;

extern void ccd_exposure(void);
extern void UART5_RxIsr(void);

// void PIT2_ISR(void)
// {
// 	Count_100us++;
// 	if (Count_100us == 10)
// 	{
// 		Count_100us = 0;
// 		Count_1Ms++;
// 		Flag_1Ms = 1;
// 	}
// 	if (Count_1Ms == 10)
// 	{
// 		Count_1Ms = 0;
// 		Flag_10Ms = 1;
// 	}
// }

extern void AngleCon_Isr(void);
void Init_PIT(void)
{
// 	Init_PIT_Struct.PIT_Pitx = PIT2;
// 	Init_PIT_Struct.PIT_PeriodUs = 100;
// 	Init_PIT_Struct.PIT_Isr = PIT2_ISR;
// 	LPLD_PIT_Init(Init_PIT_Struct); //��PIT0����1MS���ж�
// 	LPLD_PIT_EnableIrq(Init_PIT_Struct); //����PIT0���ж�
	Init_PIT_Struct.PIT_Pitx=PIT0;
	Init_PIT_Struct.PIT_PeriodMs=1;
	Init_PIT_Struct.PIT_Isr=ccd_exposure;
	LPLD_PIT_Init(Init_PIT_Struct);
	LPLD_PIT_EnableIrq(Init_PIT_Struct); //CCD�ĺ��붨ʱ��


	Init_PIT_Struct.PIT_Pitx = PIT1;
	Init_PIT_Struct.PIT_PeriodMs = 5;
	Init_PIT_Struct.PIT_Isr = AngleCon_Isr;
	LPLD_PIT_Init(Init_PIT_Struct);
	LPLD_PIT_EnableIrq(Init_PIT_Struct); //CCD�ĺ��붨ʱ��


}

//void Init_I2C(void)
//{
//	Init_I2C_Struct.I2C_I2Cx=I2C0;
//	Init_I2C_Struct.I2C_ICR=0x2b;
//	Init_I2C_Struct.I2C_IntEnable=FALSE;
//	Init_I2C_Struct.I2C_OpenDrainEnable=TRUE;
////	Init_I2C_Struct.I2C_SclPin=PTB0;
////	Init_I2C_Struct.I2C_SdaPin=PTB1;
//	LPLD_I2C_Init(Init_I2C_Struct);
//}

void Init_ADC(void)
{
	Init_ADC_Struct.ADC_Adcx = ADC1;
	Init_ADC_Struct.ADC_BitMode = SE_12BIT;
	Init_ADC_Struct.ADC_SampleTimeCfg = SAMTIME_SHORT;
	Init_ADC_Struct.ADC_HwAvgSel = HW_4AVG;
	Init_ADC_Struct.ADC_CalEnable = TRUE;
	LPLD_ADC_Init(Init_ADC_Struct);
	LPLD_ADC_Chn_Enable(ADC1, AD14);
	LPLD_ADC_Chn_Enable(ADC1, AD15);   
	//������ҪAD��������
	//�����ĸ�ͨ����ADC,

//	 *      |__AD14          --����(ADC1_SE14--PTB10)  //Gyro_1
//	 *      |__AD15          --����(ADC1_SE15--PTB11) //Gyro_2
	//LPLD_ADC_Chn_Enable(ADC1,AD12);//����CCD��AD�˿�
	Init_ADC_CCD_Struct.ADC_Adcx = ADC0;
	Init_ADC_CCD_Struct.ADC_BitMode = SE_12BIT;
	LPLD_ADC_Init(Init_ADC_CCD_Struct);
	LPLD_ADC_Chn_Enable(ADC0, AD14); //CCD1��AD�˿� PTc0
	LPLD_ADC_Chn_Enable(ADC0, AD15);//CCD2��AD�˿� PTC1
}
void Init_GPIO(void)
{
	Init_GPIO_Struct.GPIO_PTx = PTA;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin17;
	LPLD_GPIO_Init(Init_GPIO_Struct); //��ʼ��LED��;

	Init_GPIO_Struct.GPIO_PTx = PTA;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin28;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	LPLD_GPIO_Init(Init_GPIO_Struct); //�������˿���CCD��SI��CLK

	Init_GPIO_Struct.GPIO_PTx = PTA;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin29;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	LPLD_GPIO_Init(Init_GPIO_Struct);

	Init_GPIO_Struct.GPIO_PTx = PTA;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin26;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	LPLD_GPIO_Init(Init_GPIO_Struct);

	Init_GPIO_Struct.GPIO_PTx = PTA;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin27;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	LPLD_GPIO_Init(Init_GPIO_Struct);

	Init_GPIO_Struct.GPIO_PTx = PTC;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin12 | GPIO_Pin13 | GPIO_Pin14 | GPIO_Pin15 | GPIO_Pin16;
	LPLD_GPIO_Init(Init_GPIO_Struct);//��ʾ�������濴ʱ���Ƿ�����
}

void CarInit(void)
{
	char whoami = 1; //�Á��Д�Ĵ�������������
	Init_ADC();
	Init_PIT();
	Init_GPIO();
	Init_FTM();
	whoami = LPLD_MMA8451_Init();
	if (whoami != 0x1a)
	{
		LPLD_GPIO_Output_b(PTA, 17, 0);
		while (1);
	}
	if (MPU6050_Init() != 0x68)
	{
		LPLD_GPIO_Output_b(PTA, 17, 0);
		while (1);
	}
	//Init_I2C();
}
void Init_FTM(void)
{
	Init_FTM_Struct.FTM_Ftmx = FTM0;
	Init_FTM_Struct.FTM_Mode = FTM_MODE_PWM;
	Init_FTM_Struct.FTM_PwmFreq = 10000;
	LPLD_FTM_Init(Init_FTM_Struct);
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch4, 0, PTD4, ALIGN_LEFT); //��ߵ����ת
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch5, 0, PTD5, ALIGN_LEFT); //��ߵ����ת
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch6, 0, PTD6, ALIGN_LEFT); //�ұߵ����ת
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch7, 0, PTD7, ALIGN_LEFT); //�ұߵ����ת

	Init_FTM_Struct.FTM_Ftmx=FTM1;
	Init_FTM_Struct.FTM_Mode= FTM_MODE_QD;
	Init_FTM_Struct.FTM_QdMode=QD_MODE_PHAB;
	LPLD_FTM_Init(Init_FTM_Struct);
	LPLD_FTM_QD_Enable(FTM1, PTB0, PTB1); //��������FTM1������,FTM2���ҵ��

	Init_FTM_Struct.FTM_Ftmx=FTM2;
	Init_FTM_Struct.FTM_Mode= FTM_MODE_QD;
	Init_FTM_Struct.FTM_QdMode=QD_MODE_PHAB;
	LPLD_FTM_Init(Init_FTM_Struct);
	LPLD_FTM_QD_Enable(FTM2, PTB18, PTB19);//������������ģʽ
}


//void Init_DMA_UART5(unsigned char num,int8* datap)
//{
//	Init_DMA_Struct.DMA_CHx=DMA_CH0;
//	Init_DMA_Struct.DMA_Req=UART5_TRAN_DMAREQ;
//	Init_DMA_Struct.DMA_MajorLoopCnt=1;//����һ��
//	Init_DMA_Struct.DMA_MinorByteCnt=num;//һ�η��Ͷ��ٸ��ֽ�
//	Init_DMA_Struct.DMA_SourceAddr=(uint32)datap;//����Դ�ĵ�ַ
//	Init_DMA_Struct.DMA_SourceDataSize=DMA_SRC_8BIT;
//	Init_DMA_Struct.DMA_SourceAddrOffset=1;//ÿ�δ���ƫ��һ���ֽ�
//	Init_DMA_Struct.DMA_LastSourceAddrAdj= -num;
//	Init_DMA_Struct.DMA_DestAddr=(uint32)&UART5->D;
//	Init_DMA_Struct.DMA_DestAddrOffset=0;  //����һ�������Ժ��ƫ��
//	Init_DMA_Struct.DMA_DestDataSize=DMA_DST_8BIT;
//	Init_DMA_Struct.DMA_LastDestAddrAdj=0; //�������ͽ������ƫ��
//	Init_DMA_Struct.DMA_AutoDisableReq=TRUE;
//	LPLD_DMA_Init(Init_DMA_Struct);
//	LPLD_DMA_EnableIrq(Init_DMA_Struct);
//
//}

