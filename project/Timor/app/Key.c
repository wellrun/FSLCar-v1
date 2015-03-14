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
signed char Course = 1;
signed char PageNum = 0;
float ScreenData[2][8];
uint8 ScreenStr[2][8][30];
extern int  DeathValueLeft;//死区电压 2%的占空比S
extern int DeathValueRight ;//右轮的死区电压 
void ClearAllParameters(void)
{
	Speed_PID.SpeedSet = 0;
	Dir_PID.Kp = 0;
	Dir_PID.Kd = 0;
	Speed_PID.Kp = 0;
	Speed_PID.Kd = 0;
	Speed_PID.Ki = 0;
}
void PageInit(signed char sPageNum)
{
	int i;
	PageNum = sPageNum;
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
	sprintf(ScreenStr[1][2], "DeathLeft");
	sprintf(ScreenStr[1][3], "DeathRight");
	sprintf(ScreenStr[1][4], "ClearAllParameters");
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
	ScreenData[1][2] = DeathValueLeft;
	ScreenData[1][3] = DeathValueRight;

	Key_delay();
	LED_Init();
	Key_delay();
	LED_Fill(0);
	Key_delay();
	if (sPageNum == 0)
	{
		for (i = 0; i < 8; i++)
		{
			LED_P6x8Str(8, i, ScreenStr[0][i]);
			LED_PrintValueF(70, i, ScreenData[0][i], 2);
		}
	}
	else if (sPageNum == 1)
	{
		for (i = 0; i < 4; i++)
		{
			LED_P6x8Str(8, i, ScreenStr[1][i]);
			LED_PrintValueF(70, i, ScreenData[1][i], 2);
		}
		LED_P6x8Str(8, 4, ScreenStr[1][4]);
	}
	LED_P6x8Char(0, Course, '*');
}
void Key_Up(void)
{
	LED_P6x8Char(0, (uint8)Course, ' ');
	if (PageNum == 0)
	{
		Course--;
		if (Course <1)
		{
			Course = 7;
		}
	}
	else if (PageNum == 1)
	{
		Course--;
		if (Course < 0)
		{
			Course = 7;
			PageNum = 0;
			PageInit(PageNum);
		}
	}
	LED_P6x8Char(0, (uint8)Course, '*');
}
void Key_Down(void)
{
	LED_P6x8Char(0, (uint8)Course, ' ');
	if (PageNum == 0)
	{
		Course++;
		if (Course > 7)
		{
			Course = 0;
			PageNum=1;
			PageInit(PageNum);
		}
	}
	else if (PageNum == 1)
	{
		Course++;
		if (Course > 4)
		{
			Course = 0;
		}
	}
	LED_P6x8Char(0, (uint8)Course, '*');
}
void Key_DataUp(void)
{
	if (PageNum==0)
	{
		switch (Course)
		{
		case 1: ScreenData[PageNum][Course]	+= 5 ; break;
		case 2: ScreenData[PageNum][Course] += 0.1; break;
		case 3: ScreenData[PageNum][Course] += 2; break;
		case 4: ScreenData[PageNum][Course] += 0.1; break;
		case 5: ScreenData[PageNum][Course] += 2; break;
		case 6: ScreenData[PageNum][Course] += 1; break;
		case 7: ScreenData[PageNum][Course] += 1; break;
		}
		Speed_PID.SpeedSet = ScreenData[0][1];
		Ang_PID.AngSet = ScreenData[0][2];
		Dir_PID.Kp = ScreenData[0][3];
		Dir_PID.Kd = ScreenData[0][4];
		Speed_PID.Kp = ScreenData[0][5];
		Speed_PID.Kd = ScreenData[0][6];
		Speed_PID.Ki = ScreenData[0][7];
	}
	else if (PageNum==1)
	{
		switch (Course)
		{
		case 0:ScreenData[PageNum][Course] += 5; break;
		case 1:ScreenData[PageNum][Course] += 1; break;
		case 2:ScreenData[PageNum][Course] += 10; break;
		case 3:ScreenData[PageNum][Course] += 10; break;
		case 4:ClearAllParameters(); PageInit(0); break;
		}
		Ang_PID.Kp = ScreenData[1][0];
		Ang_PID.Kd = ScreenData[1][1];
		DeathValueLeft=(int)ScreenData[1][2];
		DeathValueRight=(int)ScreenData[1][3] ;
	}
	LED_P6x8Str(70, Course, "      ");
	LED_PrintValueF(70, Course, ScreenData[PageNum][Course], 2);
}
void Key_DataDown(void)
{
	if (PageNum == 0)
	{
		switch (Course)
		{
		case 1: ScreenData[PageNum][Course] -= 5; break;
		case 2: ScreenData[PageNum][Course] -= 0.1; break;
		case 3: ScreenData[PageNum][Course] -= 2; break;
		case 4: ScreenData[PageNum][Course] -= 0.1; break;
		case 5: ScreenData[PageNum][Course] -= 2; break;
		case 6: ScreenData[PageNum][Course] -= 1; break;
		case 7: ScreenData[PageNum][Course] -= 1; break;
		}
		Speed_PID.SpeedSet = ScreenData[0][1];
		Ang_PID.AngSet = ScreenData[0][2];
		Dir_PID.Kp = ScreenData[0][3];
		Dir_PID.Kd = ScreenData[0][4];
		Speed_PID.Kp = ScreenData[0][5];
		Speed_PID.Kd = ScreenData[0][6];
		Speed_PID.Ki = ScreenData[0][7];
	}
	else if (PageNum == 1)
	{
		switch (Course)
		{
		case 0:ScreenData[PageNum][Course] -= 5; break;
		case 1:ScreenData[PageNum][Course] -= 1; break;
		case 2:ScreenData[PageNum][Course] -= 10; break;
		case 3:ScreenData[PageNum][Course] -= 10; break;
		}
		Ang_PID.Kp = ScreenData[1][0];
		Ang_PID.Kd = ScreenData[1][1];
		DeathValueLeft =(int) ScreenData[1][2];
		DeathValueRight = (int)ScreenData[1][3];
	}
	LED_P6x8Str(70, Course, "      ");
	LED_PrintValueF(70, Course, ScreenData[PageNum][Course], 2);
}
extern int Anginitok ;
void Key_Process(void)
{
//	static unsigned char Debug = 0;
	unsigned char temp = 10;
	PageInit(PageNum);
	DisableInterrupts;
	LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch0, 0);
	LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch1, 0);
	LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch2, 0);
	LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch3, 0);
	Anginitok = 0;
	while (1)
	{
		temp = KeyScan();
		if (temp==0)
		{
			Key_delay();
			Key_delay();
			while (PTD1_I==0)
			{
			}
			LED_Init();
			Key_delay();Key_delay();
			LED_Fill(0);
			Key_delay();Key_delay();
			EnableInterrupts;
			break;
		}
		else
		{
			switch (temp)
			{
			case 2:Key_Up(); Key_delay();  break;
			case 3:Key_DataUp(); Key_delay();  break;
			case 4:Key_Down(); Key_delay();  break;
			case 5:Key_DataDown(); Key_delay();  break;
			}
		}
	}
}
extern uint8 Voltage_Display;
void Key_Isr(void)
{
	if (LPLD_GPIO_IsPinxExt(PORTD, GPIO_Pin1))
	{
		//去抖
		Key_delay();
		if (PTD1_I == 0)
		{
			while (PTD1_I == 0)
			{
			}
			Key_Process();
		}
	}
	else if (LPLD_GPIO_IsPinxExt(PORTD,GPIO_Pin0))
	{
		Key_delay();
		if (PTD0_I == 0)
		{
                  while (PTD0_I == 0)
			{
			}
			if (Screen_WhichCCDImg == 1)
			{
				Screen_WhichCCDImg++;
				LED_Init();
				Key_delay();
				Key_delay();
				LED_Fill(0);
				Key_delay();
				Key_delay();
			}
			else if (Screen_WhichCCDImg == 2)
			{
				Screen_WhichCCDImg = 0;
				LED_Init();
				Key_delay();
				Key_delay();
				LED_Fill(0);
				Key_delay();
				Key_delay();
			}
			else if (Screen_WhichCCDImg == 0)
			{
				Screen_WhichCCDImg++;
				LED_Init();
				Key_delay();
				Key_delay();
				LED_Fill(0);
				Key_delay();
				Key_delay();
			}
			
		}
	}
	else if (LPLD_GPIO_IsPinxExt(PORTD, GPIO_Pin7))
	{
		if (Voltage_Display == 1)
		{
			Voltage_Display = 0;
		}
		else
		{
			Voltage_Display = 1;
		}
		while (LPLD_GPIO_Input_b(PTD, 7) == 0)
		{
		}
	}
	else if (LPLD_GPIO_IsPinxExt(PORTD, GPIO_Pin4))
	{
		while (LPLD_GPIO_Input_b(PTD, 4) == 0)
		{
		}
		LED_Init();
		Key_delay();
		Key_delay();
		LED_Fill(0);
		Key_delay();
		Key_delay();
	}
}
