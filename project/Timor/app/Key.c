#include "Key.h"

void Key_delay()
{
	uint16 i, n;
	for (i = 0; i < 30000; i++)
	{
		for (n = 0; n < 50; n++)
		{
			asm("nop");
		}
	}
}
/*
D1	D0
D3	D2
D7	D4
*/
unsigned char KeyScan(void)
{
	if (PTD1_I == 0)
	{
		Key_delay();
		if (PTD1_I==0)
		{
			return 0;
		}
	}
	else if (PTD0_I == 0)
	{
		Key_delay();
		if (PTD0_I == 0)
		{
			return 1;
		}
	}
	else if (PTD3_I == 0)
	{
		Key_delay();
		if (PTD3_I == 0)
		{
			return 2;
		}
	}
	else if (PTD2_I == 0)
	{
		Key_delay();
		if (PTD2_I == 0)
		{
			return 3;
		}
	}
	else if (PTD7_I == 0)
	{
		Key_delay();
		if (PTD7_I == 0)
		{
			return 4;
		}
	}
	else if (PTD4_I == 0)
	{
		Key_delay();
		if (PTD4_I == 0)
		{
			return 5;
		}
	}
	return 10;
}
unsigned char Course = 1;
void PageInit(unsigned char PageNum)
{
	float ScreenData[2][8];
	int i;
	uint8 ScreenStr[2][8][30];
	sprintf(ScreenStr[0][0], "Voltage");
	sprintf(ScreenStr[0][1], "SpeedSet");
	sprintf(ScreenStr[0][2], "AngSet");
	sprintf(ScreenStr[0][3], "Dir_Kp");
	sprintf(ScreenStr[0][4], "Dir_kd");
	sprintf(ScreenStr[0][5], "Speed_Kp");
	sprintf(ScreenStr[0][6], "Speed_Kd");
	sprintf(ScreenStr[0][7], "Speed_Ki");
	sprintf(ScreenStr[1][0], "Ang_Kp");
	sprintf(ScreenStr[1][1], "Ang_Kd");
	disable_irq((IRQn_Type)(PORTD_IRQn));
	ScreenData[0][0] = LPLD_ADC_Get(ADC0, AD11)*3.3 * 4 / 256;
	ScreenData[0][1] = Speed_PID.SpeedSet;
	ScreenData[0][2] = Ang_PID.AngSet;
	ScreenData[0][3] = Dir_PID.Kp;
	ScreenData[0][4] = Dir_PID.Kd;
	ScreenData[0][5] = Speed_PID.Kp;
	ScreenData[0][6] = Speed_PID.Kd;
	ScreenData[0][7] = Speed_PID.Ki;
	ScreenData[1][0] = Ang_PID.Kp;
	ScreenData[1][1] = Ang_PID.Kd;
	Key_delay();
	LED_Init();
	Key_delay();
	LED_Fill(0);
	Key_delay();
	if (PageNum == 0)
	{
		for (i = 0; i < 8; i++)
		{
			LED_P6x8Str(8, i, ScreenStr[0][i]);
			LED_PrintValueF(70, i, ScreenData[0][i], 2);
		}
	}
	else if (PageNum == 1)
	{
		for (i = 0; i < 2; i++)
		{
			LED_P6x8Str(8, i, ScreenStr[1][i]);
			LED_PrintValueF(70, i, ScreenData[1][i], 2);
		}
	}
}

void Key_Process(void)
{
	static unsigned char Debug = 0;
	unsigned char temp = 10;
	PageInit(0);

	while (1)
	{
		temp = KeyScan();
		if (temp==1)
		{
			Key_delay();
			Key_delay();
			Key_delay();
			while (PTD0_I==0)
			{
			}
            enable_irq((IRQn_Type)( PORTD_IRQn));
			LED_Init();
			Key_delay();Key_delay();
			LED_Fill(0);
			Key_delay();Key_delay();
			break;
		}
	}
}

void Key_Isr(void)
{
	if (LPLD_GPIO_IsPinxExt(PORTD, GPIO_Pin0))
	{
		//È¥¶¶
		Key_delay();
		if (PTD0_I == 0)
		{
			while (PTD0_I == 1)
			{
			}
			Key_Process();
		}
	}
	else if (LPLD_GPIO_IsPinxExt(PORTD,GPIO_Pin1))
	{
		Key_delay();
		if (PTD1_I == 0)
		{
			while (PTD1_I == 1)
			{
			}
			if (Screen_WhichCCDImg == 1)
				Screen_WhichCCDImg++;
			else if (Screen_WhichCCDImg==2)
			{
				Screen_WhichCCDImg = 0;
				LED_Init();
				Key_delay(); Key_delay();
				LED_Fill(0);
				Key_delay(); Key_delay();
			}
			else if (Screen_WhichCCDImg==0)
			{
				Screen_WhichCCDImg++;
			}
		}
	}
}
