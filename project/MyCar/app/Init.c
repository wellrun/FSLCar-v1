#include "init.h"
#include "datastructure.h"
//#include "DEV_MMA8451.h"
#include "MMA8451_Moni.h"
//#include "mpu6050.h"
#include "ccd.h"
//#include "MPU6050_Moni.h"
//#include "My_IIC.h"
#include "l3g4200.h"
ADC_InitTypeDef Init_ADC_Struct;
ADC_InitTypeDef Init_ADC_CCD_Struct;
GPIO_InitTypeDef Init_GPIO_Struct;
FTM_InitTypeDef Init_FTM_Struct;
PIT_InitTypeDef Init_PIT_Struct;
UART_InitTypeDef Init_UART_Struct;
//I2C_InitTypeDef Init_I2C_Struct;
DMA_InitTypeDef Init_DMA_Struct;
NVIC_InitTypeDef Init_NVIC_Struct;
SYSTICK_InitType Init_Systick_Struct;

extern void ccd_exposure(void);
extern void UART5_RxIsr(void);
extern void Beep_Isr(void);

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

void Init_Systick(void)
{
	Init_Systick_Struct.SYSTICK_PeriodUs = 1;
	LPLD_SYSTICK_Init(Init_Systick_Struct);
}

void Init_NVIC(void)
{
	Init_NVIC_Struct.NVIC_IRQChannel = PIT0_IRQn;
	Init_NVIC_Struct.NVIC_IRQChannelGroupPriority = NVIC_PriorityGroup_1;
	Init_NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
	LPLD_NVIC_Init(Init_NVIC_Struct);
	//配置PIT1的NVIC分组
	Init_NVIC_Struct.NVIC_IRQChannel = PIT1_IRQn;
	Init_NVIC_Struct.NVIC_IRQChannelGroupPriority = NVIC_PriorityGroup_1;
	Init_NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
	LPLD_NVIC_Init(Init_NVIC_Struct);
}
extern void Status_Check(void);
void Init_PIT(void)
{
// 	Init_PIT_Struct.PIT_Pitx = PIT2;
// 	Init_PIT_Struct.PIT_PeriodUs = 100;
// 	Init_PIT_Struct.PIT_Isr = PIT2_ISR;
// 	LPLD_PIT_Init(Init_PIT_Struct); //用PIT0来做1MS的中断
// 	LPLD_PIT_EnableIrq(Init_PIT_Struct); //开启PIT0的中断
	/*Init_PIT_Struct.PIT_Pitx=PIT1;
	Init_PIT_Struct.PIT_PeriodMs=1;
	Init_PIT_Struct.PIT_Isr=ccd_exposure;
	LPLD_PIT_Init(Init_PIT_Struct);
	LPLD_PIT_EnableIrq(Init_PIT_Struct); //CCD的毫秒定时器*/


	Init_PIT_Struct.PIT_Pitx = PIT0;
	Init_PIT_Struct.PIT_PeriodMs = 1;
	Init_PIT_Struct.PIT_Isr = AngleCon_Isr;
	LPLD_PIT_Init(Init_PIT_Struct);
	LPLD_PIT_EnableIrq(Init_PIT_Struct); //CCD的毫秒定时器

/*
	Init_PIT_Struct.PIT_Pitx = PIT2;
	Init_PIT_Struct.PIT_PeriodMs = 11;
	Init_PIT_Struct.PIT_Isr = Beep_Isr;
	LPLD_PIT_Init(Init_PIT_Struct);
	LPLD_PIT_EnableIrq(Init_PIT_Struct); //蜂鸣器的毫秒定时器*/

/*
	Init_PIT_Struct.PIT_Pitx = PIT3;
	Init_PIT_Struct.PIT_PeriodMs = 26;
	Init_PIT_Struct.PIT_Isr = Status_Check;
	LPLD_PIT_Init(Init_PIT_Struct);
	LPLD_PIT_EnableIrq(Init_PIT_Struct); //蜂鸣器的毫秒定时器*/
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
	Init_ADC_Struct.ADC_HwAvgSel = HW_DISAVG;
	Init_ADC_Struct.ADC_CalEnable = TRUE;
	Init_ADC_Struct.ADC_MuxSel = MUX_ADXXB;
	LPLD_ADC_Init(Init_ADC_Struct);
	LPLD_ADC_Chn_Enable(ADC1, AD6);   
	LPLD_ADC_Chn_Enable(ADC1, AD7);
	LPLD_ADC_Chn_Enable(ADC1, AD14);
	//不再需要AD来读数据
	//开启四个通道的ADC,

//	 *      |__AD14          --单端(ADC1_SE14--PTB10)  //Gyro_1
//	 *      |__AD15          --单端(ADC1_SE15--PTB11) //Gyro_2
	//LPLD_ADC_Chn_Enable(ADC1,AD12);//线性CCD的AD端口
	Init_ADC_CCD_Struct.ADC_Adcx = ADC0;
	Init_ADC_CCD_Struct.ADC_BitMode = SE_8BIT;
	LPLD_ADC_Init(Init_ADC_CCD_Struct);
	LPLD_ADC_Chn_Enable(ADC0, AD14); //CCD1的AD端口 PTc0
	LPLD_ADC_Chn_Enable(ADC0, AD15);//CCD2的AD端口 PTC1
	
}
void Init_GPIO(void)
{
	Init_GPIO_Struct.GPIO_PTx = PTA;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin17;
	LPLD_GPIO_Init(Init_GPIO_Struct); //初始化LED灯;

	Init_GPIO_Struct.GPIO_PTx = PTA;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin28;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	LPLD_GPIO_Init(Init_GPIO_Struct); //这两个端口是CCD的SI和CLK

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
        Init_GPIO_Struct.GPIO_Output = OUTPUT_L;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin17 | GPIO_Pin13 | GPIO_Pin14 | GPIO_Pin15 | GPIO_Pin16;
	LPLD_GPIO_Init(Init_GPIO_Struct);//在示波器上面看时序是否正常

	Init_GPIO_Struct.GPIO_PTx = PTE;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin4 | GPIO_Pin5;
	LPLD_GPIO_Init(Init_GPIO_Struct);//两个led

	Init_GPIO_Struct.GPIO_PTx = PTB;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin20 | GPIO_Pin21 | GPIO_Pin22 | GPIO_Pin23;
	Init_GPIO_Struct.GPIO_Dir = DIR_INPUT;
	LPLD_GPIO_Init(Init_GPIO_Struct);//4个拨码开关

	Init_GPIO_Struct.GPIO_PTx = PTC;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin4 | GPIO_Pin5 | GPIO_Pin6 | GPIO_Pin7;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	LPLD_GPIO_Init(Init_GPIO_Struct);//oled
}

void CarInit(void)
{
	static char whoami = 1; //用來判斷寄存器正常不正常

      

	Init_NVIC();
	Init_Systick();
	Init_GPIO();
	Init_ADC();
	
	Init_FTM();

     
        
   /*    whoami= L3G4200_Init();
       if (whoami != 0xd3)
	{
		LPLD_GPIO_Output_b(PTA, 17, 0);
		while (1);
	}*/
        
        
	//whoami = LPLD_MMA8451_Init();
/*
	if (whoami != 0x1a)
	{
		LPLD_GPIO_Output_b(PTE, 5, 1);
		while (1)
		{
			LPLD_SYSTICK_DelayMs(500);
			LPLD_GPIO_Toggle_b(PTE, 5);
		}
	}
	else
		LPLD_GPIO_Output_b(PTE, 5, 0);*/
	/*PORTD_PCR8 = PORT_PCR_MUX(1);    //IO模拟IIC SCL
	PORTD_PCR9 = PORT_PCR_MUX(1);    //IO模拟IIC SDA
	MPU6050_Inital();
	whoami=MPU6050_ReadByte(0x75);
	if (whoami != 0x68)
	{
		LPLD_GPIO_Output_b(PTE, 5, 1);
		while (1)
		{
			LPLD_SYSTICK_DelayMs(500);
			LPLD_GPIO_Toggle_b(PTE, 5);
		}
	}
	else
		LPLD_GPIO_Output_b(PTE, 5, 0);*/
	PORTD_PCR8 = PORT_PCR_MUX(1);    //IO模拟IIC SCL
	PORTD_PCR9 = PORT_PCR_MUX(1);    //IO模拟IIC SDA
	if (MMA8451_Inital() != 0x1a)
	{
		LPLD_GPIO_Output_b(PTA, 17, 0);
		while (1);
	}

      Init_PIT();
}
void Init_FTM(void)
{
  	Init_FTM_Struct.FTM_Ftmx = FTM0;
	Init_FTM_Struct.FTM_Mode = FTM_MODE_PWM;
	Init_FTM_Struct.FTM_PwmFreq = 10000;
	LPLD_FTM_Init(Init_FTM_Struct);
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch4, 0, PTD4, ALIGN_LEFT); //左边电机正转
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch5, 0, PTD5, ALIGN_LEFT); //左边电机反转
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch6, 0, PTD6, ALIGN_LEFT); //右边电机正转
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch7, 0, PTD7, ALIGN_LEFT); //右边电机反转


	Init_FTM_Struct.FTM_Ftmx=FTM1;
	Init_FTM_Struct.FTM_Mode= FTM_MODE_QD;
	Init_FTM_Struct.FTM_QdMode=QD_MODE_CNTDIR;
	LPLD_FTM_Init(Init_FTM_Struct);
	LPLD_FTM_QD_Enable(FTM1, PTB0, PTB1); //正交解码FTM1是左电机,FTM2是右电机

	Init_FTM_Struct.FTM_Ftmx=FTM2;
	Init_FTM_Struct.FTM_Mode= FTM_MODE_QD;
	Init_FTM_Struct.FTM_QdMode = QD_MODE_CNTDIR;
	LPLD_FTM_Init(Init_FTM_Struct);
	LPLD_FTM_QD_Enable(FTM2, PTB18, PTB19);//开启正交解码模式
}


//void Init_DMA_UART5(unsigned char num,int8* datap)
//{
//	Init_DMA_Struct.DMA_CHx=DMA_CH0;
//	Init_DMA_Struct.DMA_Req=UART5_TRAN_DMAREQ;
//	Init_DMA_Struct.DMA_MajorLoopCnt=1;//传送一次
//	Init_DMA_Struct.DMA_MinorByteCnt=num;//一次发送多少个字节
//	Init_DMA_Struct.DMA_SourceAddr=(uint32)datap;//数据源的地址
//	Init_DMA_Struct.DMA_SourceDataSize=DMA_SRC_8BIT;
//	Init_DMA_Struct.DMA_SourceAddrOffset=1;//每次传输偏移一个字节
//	Init_DMA_Struct.DMA_LastSourceAddrAdj= -num;
//	Init_DMA_Struct.DMA_DestAddr=(uint32)&UART5->D;
//	Init_DMA_Struct.DMA_DestAddrOffset=0;  //发送一个数据以后的偏移
//	Init_DMA_Struct.DMA_DestDataSize=DMA_DST_8BIT;
//	Init_DMA_Struct.DMA_LastDestAddrAdj=0; //调整发送结束后的偏移
//	Init_DMA_Struct.DMA_AutoDisableReq=TRUE;
//	LPLD_DMA_Init(Init_DMA_Struct);
//	LPLD_DMA_EnableIrq(Init_DMA_Struct);
//
//}

