#include "init.h"
#include "datastructure.h"
#include "DEV_MMA8451.h"
#include "ccd.h"
#include "l3g4200.h"
#include "Key.h"
ADC_InitTypeDef Init_ADC_Struct;
ADC_InitTypeDef Init_ADC_CCD_Struct;
GPIO_InitTypeDef Init_GPIO_Struct;
FTM_InitTypeDef Init_FTM_Struct;
PIT_InitTypeDef Init_PIT_Struct;
UART_InitTypeDef Init_UART_Struct;
DMA_InitTypeDef Init_DMA_Struct;
NVIC_InitTypeDef Init_NVIC_Struct;
SYSTICK_InitType Init_Systick_Struct;

extern void ccd_exposure(void);
extern void UART5_RxIsr(void);
extern void Beep_Isr(void);


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
	Init_PIT_Struct.PIT_Pitx = PIT0;
	Init_PIT_Struct.PIT_PeriodMs = 2;
	Init_PIT_Struct.PIT_Isr = AngleCon_Isr;
	LPLD_PIT_Init(Init_PIT_Struct);
	LPLD_PIT_EnableIrq(Init_PIT_Struct); //CCD的毫秒定时器
}


void Init_ADC(void)
{
	Init_ADC_CCD_Struct.ADC_Adcx = ADC0;
	Init_ADC_CCD_Struct.ADC_BitMode = SE_8BIT;
	Init_ADC_CCD_Struct.ADC_CalEnable = TRUE;
	LPLD_ADC_Init(Init_ADC_CCD_Struct);
	LPLD_ADC_Chn_Enable(ADC0, AD17); //CCD1的AD端口 PTe24
	LPLD_ADC_Chn_Enable(ADC0, AD18);//CCD2的AD端口 PTe25
	LPLD_ADC_Chn_Enable(ADC0, AD11);//电池电压
	
}
void Init_GPIO(void)
{
	Init_GPIO_Struct.GPIO_PTx = PTA;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin17;
	LPLD_GPIO_Init(Init_GPIO_Struct); //初始化LED灯;

	Init_GPIO_Struct.GPIO_PTx = PTC;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_L;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin0;
	LPLD_GPIO_Init(Init_GPIO_Struct); //时序测试

	Init_GPIO_Struct.GPIO_PTx = PTB;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin23 | GPIO_Pin22 | GPIO_Pin21 | GPIO_Pin20 ;
	LPLD_GPIO_Init(Init_GPIO_Struct); //屏幕

	Init_GPIO_Struct.GPIO_PTx = PTC;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin16 | GPIO_Pin17 | GPIO_Pin18 | GPIO_Pin19;
	Init_GPIO_Struct.GPIO_Dir = DIR_INPUT;
	LPLD_GPIO_Init(Init_GPIO_Struct);//4个拨码开关

	Init_GPIO_Struct.GPIO_PTx = PTC;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;	
	Init_GPIO_Struct.GPIO_Output = OUTPUT_L;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin13 | GPIO_Pin12;
	LPLD_GPIO_Init(Init_GPIO_Struct); //指示灯

	Init_GPIO_Struct.GPIO_PTx = PTE;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_H;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin11 | GPIO_Pin10;
	LPLD_GPIO_Init(Init_GPIO_Struct); //SI和CLK

	Init_GPIO_Struct.GPIO_PTx = PTC;
	Init_GPIO_Struct.GPIO_Dir = DIR_OUTPUT;
	Init_GPIO_Struct.GPIO_Output = OUTPUT_L;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin10;
	LPLD_GPIO_Init(Init_GPIO_Struct); //蜂鸣器

	Init_GPIO_Struct.GPIO_PTx = PTD;
	Init_GPIO_Struct.GPIO_Pins = GPIO_Pin0 | GPIO_Pin1 | GPIO_Pin2 | GPIO_Pin3 | GPIO_Pin4 | GPIO_Pin7;
	Init_GPIO_Struct.GPIO_Dir = DIR_INPUT;
	Init_GPIO_Struct.GPIO_Isr = Key_Isr;
	Init_GPIO_Struct.GPIO_PinControl = IRQC_FA;
	LPLD_GPIO_Init(Init_GPIO_Struct);//6个按键
	LPLD_GPIO_EnableIrq(Init_GPIO_Struct);

	/*
	D1	D0
	D3	D2
	D7	D4
	*/


}

void CarInit(void)
{
	unsigned char whoami1 = 1, whoami2 = 1,initok=1; //用來判斷寄存器正常不正常
	Init_NVIC();
	Init_Systick();
	Init_GPIO();
	Init_ADC();
	Init_FTM(); 
    whoami1= L3G4200_Init();
	if (whoami1 != 0xd3)
	{
		initok = 0;
		LPLD_GPIO_Output_b(PTC, 12, 1);
	}
	whoami2 = LPLD_MMA8451_Init();
	if (whoami2 != 0x1a)
	{
		initok = 0;
		LPLD_GPIO_Output_b(PTC, 13, 1);
	}
	if (initok==0)
	{
		while (1)
		{
		}
	}
        whoami2=whoami2;
        whoami1=whoami1;
      Init_PIT();
}
void Init_FTM(void)
{
  	Init_FTM_Struct.FTM_Ftmx = FTM0;
	Init_FTM_Struct.FTM_Mode = FTM_MODE_PWM;
	Init_FTM_Struct.FTM_PwmFreq = 20000;
	LPLD_FTM_Init(Init_FTM_Struct);
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch0, 0, PTC1, ALIGN_LEFT); //左边电机正转
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch3, 0, PTC4, ALIGN_LEFT); //左边电机反转
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch1, 0, PTC2, ALIGN_LEFT); //右边电机正转
	LPLD_FTM_PWM_Enable(FTM0, FTM_Ch2, 0, PTC3, ALIGN_LEFT); //右边电机反转


	Init_FTM_Struct.FTM_Ftmx=FTM1;
	Init_FTM_Struct.FTM_Mode= FTM_MODE_QD;
	Init_FTM_Struct.FTM_QdMode=QD_MODE_CNTDIR;
	LPLD_FTM_Init(Init_FTM_Struct);
	LPLD_FTM_QD_Enable(FTM1, PTB0, PTB1); //正交解码FTM1是左电机,FTM2是右电机

	Init_FTM_Struct.FTM_Ftmx=FTM2;
	Init_FTM_Struct.FTM_Mode= FTM_MODE_QD;
	Init_FTM_Struct.FTM_QdMode = QD_MODE_CNTDIR;
	LPLD_FTM_Init(Init_FTM_Struct);
	LPLD_FTM_QD_Enable(FTM2, PTA10, PTA11);//开启正交解码模式
}


