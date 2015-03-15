#include "Control.h"
#include "CCD.h"
//#include "Fuzzy.h"
//#include "mpu6050.h"
//float AngToMotorRatio=300;//角度转换成电机控制的比例因子..我也不知道取多少合适..以后再调试
#define MOTOR_OUT_MAX       9500
#define MOTOR_OUT_MIN       -9500
#define ANGLE_CONTROL_OUT_MAX			12000
#define ANGLE_CONTROL_OUT_MIN			-12000
#define SPEED_CONTROL_OUT_MAX			8000
#define SPPED_CONTROL_OUT_MIN			-5000
//轮子转一圈..编码器增加5200
int  DeathValueLeft = 290;//死区电压 2%的占空比S
int DeathValueRight = 90;//右轮的死区电压 

extern float Ang_dt;//控制周期,在主函数定义,20ms
extern float Speed_Dt;//速度的周期,0.08ms

extern CarInfo_TypeDef CarInfo_Now; //当前车子的信息
extern CarControl_TypeDef MotorControl; //电机控制的值
extern AngPID_InitTypeDef Ang_PID; //角度控制的PID结构体
extern TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
SpeedPID_TypeDef Speed_PID;
DirPID_TypeDef Dir_PID;
short SpeedControlPeriod, DirectionConrtolPeriod;
extern signed char Beep;
extern int Beep_TimeMs;
extern char CarStandFlag;
void Beep_Isr(void)
{
	static int Cnt_Times = 0;
	if (CarStart_Mask==1 && Beep == 1)
	{
		LPLD_GPIO_Output_b(PTC, 10, 1);
		Cnt_Times +=2;
		if (Cnt_Times > Beep_TimeMs)
		{
			LPLD_GPIO_Output_b(PTC, 10, 0);
			Cnt_Times = 0;
			Beep_TimeMs = 0;
			Beep = 0;
		}
	}
	else
	{
		Cnt_Times = 0;
		Beep_TimeMs = 0;
		LPLD_GPIO_Output_b(PTC, 10, 0);
	}
}
void BeepBeepBeep(int Times)
{
	if (Beep == 0)
	{
		Beep = 1;
		Beep_TimeMs = Times;
	}
}

void DeathTime_Delay(void)
{
	int t = 100;
	while (t > 0)
	{
		t--;
	}
}
float AngSet_Var = 0;
void AngleControlValueCalc(void)
{
	//static float lastControlValue = 0;
	float ControlValue = 0;
	//Ang_PID.Delta = (AngSet_Var)-CarInfo_Now.CarAngle; //当前误差//这里全是角度,值很小
        Ang_PID.Delta = (Ang_PID.AngSet)-CarInfo_Now.CarAngle;
	//微分项应该是负的
	ControlValue = Ang_PID.Delta * Ang_PID.Kp - CarInfo_Now.CarAngSpeed* Ang_PID.Kd; //微分项,如果角速度大于0.说明角度趋势变大,把控制量增大
	//ControlValue *= AngToMotorRatio; //乘上比例因子将角度转换成PWM的占空比
	if (ControlValue > ANGLE_CONTROL_OUT_MAX)
		ControlValue = ANGLE_CONTROL_OUT_MAX;
	else if (ControlValue < ANGLE_CONTROL_OUT_MIN)
		ControlValue = ANGLE_CONTROL_OUT_MIN; //限幅
	TempValue.AngControl_OutValue = ControlValue; //更新控制临时变量的值
}
extern unsigned char Flag_SpeedGot;
Speed_Data_Struct SpeedCnt;
Speed_Data_Struct* CounterGet(int Mode)
{//FTM1是左电机,FTM2是右电机
	Speed_Data_Struct *p = &SpeedCnt;
	static int LeftSum1, RightSum1, LeftSum2, RightSum2;
	static int LastCntLeft = 0, LastCntRight = 0;
	int LeftCnt, RightCnt;
	LeftCnt = LPLD_FTM_GetCounter(FTM2);
	RightCnt = LPLD_FTM_GetCounter(FTM1);
	LPLD_FTM_ClearCounter(FTM1);
	LPLD_FTM_ClearCounter(FTM2);

	if (LeftCnt >= 30000)
	{
		LeftCnt -= 65535;
	}
	if (RightCnt >= 30000)
	{
		RightCnt -= 65535;
	}
        LeftCnt = (int)((float)LeftCnt*0.6 +(float) LastCntLeft*0.4);
	RightCnt = (int)((float)RightCnt*0.6 + (float)LastCntRight*0.4);
	LastCntRight = RightCnt;
	LastCntLeft = LeftCnt;
        
        
	RightCnt = -RightCnt;
	LeftSum1 += LeftCnt;
	RightSum1 += RightCnt;
	LeftSum2 += LeftCnt;
	RightSum2 += RightCnt;
	if (Mode==1)
	{
		p->Left = LeftSum1;
		p->Right = RightSum1;
		LeftSum1 = 0, RightSum1 = 0;
		return p;
	}
	else if (Mode==2)
	{
		p->Left = LeftSum2;
		p->Right = RightSum2;
		LeftSum2 = 0, RightSum2 = 0;
		return p;
	}
	p->Left = 0;
	p->Right = 0;
	return p;
}
float SpeedSet_Variable = 0;

float AngVar_Arr_Positive[101];
float AngVar_Arr_Negative[101];
void SpeedControlValueCalc(void)
{
	//15 10
	//-50*10-(20*-5)
	/*static SpeedPID_TypeDef *p = &Speed_PID;
	static Speed_Data_Struct *ps;
	float SpeedSet_Variable_ORG = 0;
	int Index = 1;
#define ErrorMax 400
#define IntegralSumMAX 50000
	ps = CounterGet(1);
	CarInfo_Now.MotorCounterLeft = ps->Left;
	CarInfo_Now.MotorCounterRight = ps->Right;
	CarInfo_Now.CarSpeed = (ps->Left + ps->Right) / 20;
	
	p->ThisError = Speed_PID.SpeedSet - CarInfo_Now.CarSpeed;
	if (p->ThisError > ErrorMax)
		p->ThisError = ErrorMax;
	else if (p->ThisError < -ErrorMax)
		p->ThisError = -ErrorMax;
	if (p->OutValue >SPEED_CONTROL_OUT_MAX)
	{
		if (p->ThisError>0)
		{
			Index = 0;
		}
		else
		{
			Index = 1;
		}
	}
	else if (p->OutValue < SPPED_CONTROL_OUT_MIN)
	{
		if (p->ThisError>0)
		{
			Index = 1;
		}
		else
		{
			Index = 0;
		}
	}
	p->IntegralSum += ((p->Ki / 10.0)*p->ThisError*Index);
	p->OutValue = p->Kp*(p->ThisError) \
		+ p->IntegralSum \
		+ p->Kd*(p->ThisError -  p->LastError);
	p->LastError = p->ThisError;
	
	TempValue.Old_AngSet = TempValue.New_AngSet;
	TempValue.New_AngSet = SpeedSet_Variable_ORG;
	TempValue.Old_SpeedOutValue = TempValue.New_SpeedOutValue;
	TempValue.New_SpeedOutValue = p->OutValue;
	if (TempValue.New_SpeedOutValue  > SPEED_CONTROL_OUT_MAX)
		TempValue.New_SpeedOutValue = SPEED_CONTROL_OUT_MAX;
	else if (TempValue.New_SpeedOutValue  < SPPED_CONTROL_OUT_MIN)
		TempValue.New_SpeedOutValue = SPPED_CONTROL_OUT_MIN;*/
	//左右分开
	/*CarInfo_Now.MotorCounterLeft /= 10;
	CarInfo_Now.MotorCounterRight /= 10;
	p->ThisError_Left = p->SpeedSet - CarInfo_Now.MotorCounterLeft;
	if (p->ThisError_Left > 40)
	p->ThisError_Left = 40;
	else if (p->ThisError_Left < -40)
	p->ThisError_Left = -40;
	p->OutValue_Left = p->Kp*(p->ThisError_Left - p->LastError_Left) \
	+ (p->Ki/10.0)*p->ThisError_Left \
	+ p->Kd*(p->ThisError_Left - 2 * p->LastError_Left + p->PreError_Left);
	p->PreError_Left = p->LastError_Left;
	p->LastError_Left = p->ThisError_Left;
	p->OutValueSum_Left += p->OutValue_Left;
	if (p->OutValueSum_Left > SPEED_CONTROL_OUT_MAX)
	p->OutValueSum_Left = SPEED_CONTROL_OUT_MAX;
	else if (p->OutValueSum_Left < SPPED_CONTROL_OUT_MIN)
	p->OutValueSum_Left = SPPED_CONTROL_OUT_MIN;
	TempValue.Old_SpeedOutValueLeft = TempValue.New_SpeedOutValueLeft;
	TempValue.New_SpeedOutValueLeft = p->OutValueSum_Left;


	p->ThisError_Right = p->SpeedSet - CarInfo_Now.MotorCounterRight;
	if (p->ThisError_Right > 40)
	p->ThisError_Right = 40;
	else if (p->ThisError_Right < -40)
	p->ThisError_Right = -40;
	p->OutValue_Right = p->Kp*(p->ThisError_Right - p->LastError_Right) \
	+ (p->Ki/10.0)*p->ThisError_Right \
	+ p->Kd*(p->ThisError_Right - 2 * p->LastError_Right + p->PreError_Right);
	p->PreError_Right = p->LastError_Right;
	p->LastError_Right = p->ThisError_Right;
	p->OutValueSum_Right += p->OutValueSum_Right;
	if (p->OutValueSum_Right > SPEED_CONTROL_OUT_MAX)
	p->OutValueSum_Right = SPEED_CONTROL_OUT_MAX;
	else if (p->OutValueSum_Right < SPPED_CONTROL_OUT_MIN)
	p->OutValueSum_Right= SPPED_CONTROL_OUT_MIN;
	TempValue.Old_SpeedOutValueRight = TempValue.New_SpeedOutValueRight;
	TempValue.New_SpeedOutValueRight = p->OutValueSum_Right;*/

	static SpeedPID_TypeDef *p = &Speed_PID;
	static Speed_Data_Struct *ps;
#define ErrorMax 100
	ps=CounterGet(1);
	CarInfo_Now.MotorCounterLeft = ps->Left;
	CarInfo_Now.MotorCounterRight = ps->Right;
	CarInfo_Now.CarSpeed = (ps->Left + ps->Right) / 20;
        p->ThisError = SpeedSet_Variable - CarInfo_Now.CarSpeed;
	if (p->ThisError > ErrorMax)
		p->ThisError = ErrorMax;
	else if (p->ThisError < -ErrorMax)
		p->ThisError = -ErrorMax;
	if (p->ThisError>=0)
	{
		p->OutValue = p->Kp*(p->ThisError - p->LastError) \
			+ (p->Ki / 10.0)*p->ThisError \
			+ p->Kd*(p->ThisError - 2 * p->LastError + p->PreError);
	}
	else
	{
		p->OutValue = (p->Kp*1.5)*(p->ThisError - p->LastError) \
			+ (p->Ki / 4)*p->ThisError \
			+ p->Kd*(p->ThisError - 2 * p->LastError + p->PreError);
	}
	p->PreError = p->LastError;
	p->LastError = p->ThisError;
	p->OutValueSum += p->OutValue;
	if (p->OutValueSum  > SPEED_CONTROL_OUT_MAX)
		p->OutValueSum = SPEED_CONTROL_OUT_MAX;
	else if (p->OutValueSum  < SPPED_CONTROL_OUT_MIN)
		p->OutValueSum = SPPED_CONTROL_OUT_MIN;
	TempValue.Old_SpeedOutValue = TempValue.New_SpeedOutValue;
	TempValue.New_SpeedOutValue = p->OutValueSum;

}
float IntSum = 0;
void DirControlValueCale(void)
{
	static float Ki = 0;
	static float Last_DirAngSpeed;
	extern float Dir_AngSpeed;
	
	Dir_PID.LastError = Dir_PID.ThisError;
	Dir_PID.ThisError = Dir_PID.ControlValue;
	//Dir_Diff = Dir_PID.LastError - Dir_PID.ThisError;
	IntSum += Ki*Dir_PID.ThisError;
	//Dir_PID.OutValue = -(Dir_PID.ThisError*0.7 + Dir_PID.LastError*0.3)* (Dir_PID.Kp_Temp) + (Dir_Diff)*Dir_PID.Kd_Temp + IntSum;
	Dir_PID.OutValue = -(Dir_PID.ThisError)* (Dir_PID.Kp_Temp) + (Dir_AngSpeed)*Dir_PID.Kd_Temp + IntSum;
	TempValue.DirOutValue_Old = TempValue.DirOutValue_New;
	TempValue.DirOutValue_New = Dir_PID.OutValue;
	Last_DirAngSpeed = Dir_AngSpeed;
}

void ControlSmooth(void)
{
	static float TempF = 0;
	//TempF = TempValue.New_SpeedOutValueRight - TempValue.Old_SpeedOutValueRight;
	//TempValue.SpeedOutValueRight = TempF*(SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + TempValue.Old_SpeedOutValueRight;

	//TempF = TempValue.New_SpeedOutValueLeft - TempValue.Old_SpeedOutValueLeft;
	//TempValue.SpeedOutValueLeft = TempF*(SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + TempValue.Old_SpeedOutValueLeft;

	TempF = TempValue.New_SpeedOutValue - TempValue.Old_SpeedOutValue;
	TempValue.SpeedOutValue = TempF*((SpeedControlPeriod + 1) / (float)SPEED_CONTROL_PERIOD) + TempValue.Old_SpeedOutValue;


	TempF = TempValue.DirOutValue_New - TempValue.DirOutValue_Old;
	TempValue.DirOutValue = TempF*(DirectionConrtolPeriod + 1) /(float)DIRECTION_CONTROL_PERIOD + TempValue.DirOutValue_Old;


	TempF = TempValue.New_AngSet - TempValue.Old_AngSet;
	AngSet_Var = TempF*(SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + TempValue.Old_AngSet;
	if (TempValue.DirOutValue > 0)
	{
	TempValue.Dir_LeftOutValue = TempValue.DirOutValue;
	TempValue.Dir_RightOutValue = -TempValue.DirOutValue;
	}
	else
	{
	TempValue.Dir_LeftOutValue = TempValue.DirOutValue;
	TempValue.Dir_RightOutValue = -TempValue.DirOutValue;
	}
	//方向控制去掉平滑处理
}
void MotorControl_Out(void)
{
	ControlSmooth();
	MotorControl.LeftMotorOutValue = (int)TempValue.AngControl_OutValue
		+ (int)TempValue.Dir_LeftOutValue - (int)TempValue.SpeedOutValue; //取整
	MotorControl.RightMotorOutValue = (int)TempValue.AngControl_OutValue
		+ (int)TempValue.Dir_RightOutValue - (int)TempValue.SpeedOutValue;
	//调速度PI参数
	/*MotorControl.LeftMotorOutValue = (int)TempValue.SpeedOutValue;
	MotorControl.RightMotorOutValue = (int)TempValue.SpeedOutValue;*/ //

	if (MotorControl.LeftMotorOutValue > MOTOR_OUT_MAX)
	{
		MotorControl.LeftMotorOutValue = MOTOR_OUT_MAX;
	}
	else if (MotorControl.LeftMotorOutValue < MOTOR_OUT_MIN)
	{
		MotorControl.LeftMotorOutValue = MOTOR_OUT_MIN;
	}
	if (MotorControl.RightMotorOutValue > MOTOR_OUT_MAX)
	{
		MotorControl.RightMotorOutValue = MOTOR_OUT_MAX;
	}
	else if (MotorControl.RightMotorOutValue < MOTOR_OUT_MIN)
	{
		MotorControl.RightMotorOutValue = MOTOR_OUT_MIN;
	} //限幅不能超过10000

	if (MotorControl.LeftMotorOutValue >= 0)
	{
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch3, 0);
		DeathTime_Delay();//手动插入死区时间
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch0, MotorControl.LeftMotorOutValue + DeathValueRight);
	}
	else
	{
		MotorControl.LeftMotorOutValue = -MotorControl.LeftMotorOutValue; //为负值取反
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch0, 0);
		DeathTime_Delay();
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch3, MotorControl.LeftMotorOutValue + DeathValueRight);
	}
	if (MotorControl.RightMotorOutValue >= 0)
	{
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch2, 0);
		DeathTime_Delay();
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch1, MotorControl.RightMotorOutValue + DeathValueLeft);
	}
	else
	{
		MotorControl.RightMotorOutValue = -MotorControl.RightMotorOutValue;
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch1, 0);
		DeathTime_Delay();//
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch2, MotorControl.RightMotorOutValue + DeathValueLeft);
	}
}

//int IncPIDCalc(int NextPoint)
//{
//	int iIncPID;
//	iError=Ang_PID.SetPoint-NextPoint; //需要将角度换算成电机的PWM
//	iIncPID=(int)((Ang_PID.Proportion*iError-Ang_PID.Integral*Ang_PID.LastError+Ang_PID.Derivative*Ang_PID.PrevError));
//	Ang_PID.PrevError=Ang_PID.LastError;
//	Ang_PID.LastError=iError;
//	return(iIncPID);
//}
