#include "Control.h"
#include "CCD.h"
#include "Fuzzy.h"
//#include "mpu6050.h"
//float AngToMotorRatio=300;//角度转换成电机控制的比例因子..我也不知道取多少合适..以后再调试
#define MOTOR_OUT_MAX       8500
#define MOTOR_OUT_MIN       -8500
#define ANGLE_CONTROL_OUT_MAX			16000
#define ANGLE_CONTROL_OUT_MIN			-16000
#define SPEED_CONTROL_OUT_MAX			13000
#define SPPED_CONTROL_OUT_MIN			-13000
//轮子转一圈..编码器增加5200
int  DeathValueLeft = 280;//死区电压 2%的占空比S
int DeathValueRight = 220;//右轮的死区电压 

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
	if (CarStandFlag == 1 && CarInfo_Now.CarSpeed > 20 && Beep == 1 && CarStandFlag == 1)
	{
		LPLD_GPIO_Output_b(PTC, 10, 1);
		Cnt_Times ++;
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

void AngleControlValueCalc(void)
{
	//static float lastControlValue = 0;
	float ControlValue = 0;
	Ang_PID.Delta = (Ang_PID.AngSet) - CarInfo_Now.CarAngle; //当前误差//这里全是角度,值很小
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
Speed_Data_Struct* SpeedGet(int Mode)
{//FTM1是左电机,FTM2是右电机
	Speed_Data_Struct *p = &SpeedCnt;
	static int LeftSum1, RightSum1, LeftSum2, RightSum2;
	int LeftCnt, RightCnt;
	LeftCnt = LPLD_FTM_GetCounter(FTM1);
	RightCnt = LPLD_FTM_GetCounter(FTM2);
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
void SpeedControlValueCalc(void)
{
	
	//static SpeedPID_TypeDef *Pr = &Speed_PID_Right;
	//PI
	// #define INTEGRAL_MAX 200000
	// 	short Index = 0;
	// 	static float TempIntegral = 0;
	// 	SpeedGet();
	// 	Speed_PID.ThisError = Speed_PID.SpeedSet - CarInfo_Now.CarSpeed;
	// 	if (Speed_PID.ThisError > 100)
	// 		Speed_PID.ThisError = 100;
	// 	if (Speed_PID.ThisError < -100)
	// 		Speed_PID.ThisError = -100;
	// 	Speed_PID.IntegralSum += (Speed_PID.Ki/10)*Speed_PID.ThisError;
	// 	Speed_PID.OutValue = Speed_PID.Kp*Speed_PID.ThisError + Speed_PID.IntegralSum;
	// 
	// 	if (Speed_PID.OutValue > SPEED_CONTROL_OUT_MAX)
	// 		Speed_PID.OutValue = SPEED_CONTROL_OUT_MAX;
	// 	else if (Speed_PID.OutValue < SPPED_CONTROL_OUT_MIN)
	// 		Speed_PID.OutValue = SPPED_CONTROL_OUT_MIN;
	// 	TempValue.Old_SpeedOutValue = TempValue.New_SpeedOutValue;
	// 	TempValue.New_SpeedOutValue = Speed_PID.OutValue;
	//PD控制
	/*static float LastSpeed;
	SpeedGet();
	Speed_PID.ThisError = Speed_PID.SpeedSet - CarInfo_Now.CarSpeed;
	Speed_PID.OutValue = Speed_PID.Kp*Speed_PID.ThisError - Speed_PID.Ki*(CarInfo_Now.CarSpeed-LastSpeed);
	LastSpeed = CarInfo_Now.CarAngSpeed;
	Speed_PID.OutValue /= 100;//比例因子,转换为PWM占空比
	TempValue.Old_SpeedOutValue = TempValue.New_SpeedOutValue;
	TempValue.New_SpeedOutValue = Speed_PID.OutValue; */
	//增量PID

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
	//static int Index = 1;
	//static float Speed_IntSum = 0;
	ps=SpeedGet(1);
	CarInfo_Now.MotorCounterLeft = ps->Left;
	CarInfo_Now.MotorCounterRight = ps->Right;
	CarInfo_Now.CarSpeed = (ps->Left + ps->Right) / 20;
#define IntMax 3500
	//p->ThisError = SpeedSet_Variable - CarInfo_Now.CarSpeed;
        p->ThisError = Speed_PID.SpeedSet - CarInfo_Now.CarSpeed;
	if (p->ThisError > ErrorMax)
		p->ThisError = ErrorMax;
	else if (p->ThisError < -ErrorMax)
		p->ThisError = -ErrorMax;
	p->OutValue = p->Kp*(p->ThisError - p->LastError) \
		+ (p->Ki / 10.0)*p->ThisError \
		+ p->Kd*(p->ThisError - 2 * p->LastError + p->PreError);
	p->PreError = p->LastError;
	p->LastError = p->ThisError;
	p->OutValueSum += p->OutValue;
	if (p->OutValueSum > SPEED_CONTROL_OUT_MAX)
		p->OutValueSum = SPEED_CONTROL_OUT_MAX;
	else if (p->OutValueSum < SPPED_CONTROL_OUT_MIN)
		p->OutValueSum = SPPED_CONTROL_OUT_MIN;
	TempValue.Old_SpeedOutValue = TempValue.New_SpeedOutValue;
	TempValue.New_SpeedOutValue = p->OutValueSum;

	/*#define ErrorMax 100
		p->ThisError_Left = SpeedSet_Variable - CarInfo_Now.MotorCounterLeft / 10;
		p->ThisError_Right = SpeedSet_Variable - CarInfo_Now.MotorCounterRight / 10;
		if (p->ThisError_Right > ErrorMax)
		p->ThisError_Right = ErrorMax;
		else if (p->ThisError_Right < -ErrorMax)
		p->ThisError_Right = -ErrorMax;
		if (p->ThisError_Left > ErrorMax)
		p->ThisError_Left = ErrorMax;
		else if (p->ThisError_Left < -ErrorMax)
		p->ThisError_Left = -ErrorMax;
		p->OutValueSum_Left += p->Kp*(p->ThisError_Left - p->LastError_Left) \
		+ (p->Ki / 10.0)*p->ThisError_Left \
		+ p->Kd*(p->ThisError_Left - 2 * p->LastError_Left + p->PreError_Left);
		p->OutValueSum_Right += p->Kp*(p->ThisError_Right - p->LastError_Right) \
		+ (p->Ki / 10.0)*p->ThisError_Right \
		+ p->Kd*(p->ThisError_Right - 2 * p->LastError_Right + p->PreError_Right);
		if (p->OutValueSum_Left > SPEED_CONTROL_OUT_MAX)
		p->OutValueSum_Left = SPEED_CONTROL_OUT_MAX;
		else if (p->OutValueSum_Left < SPPED_CONTROL_OUT_MIN)
		p->OutValueSum_Left = SPPED_CONTROL_OUT_MIN;
		if (p->OutValueSum_Right > SPEED_CONTROL_OUT_MAX)
		p->OutValueSum_Right = SPEED_CONTROL_OUT_MAX;
		else if (p->OutValueSum_Right < SPPED_CONTROL_OUT_MIN)
		p->OutValueSum_Right = SPPED_CONTROL_OUT_MIN;

		TempValue.Old_SpeedOutValueLeft = TempValue.New_SpeedOutValueLeft;
		TempValue.New_SpeedOutValueLeft = p->OutValueSum_Left;
		TempValue.Old_SpeedOutValueRight = TempValue.New_SpeedOutValueRight;
		TempValue.New_SpeedOutValueRight = p->OutValueSum_Right;*/
}
int PulseCnt1 = 0;
int PulseCnt2 = 1;
void ic_isr0(void)
{
	if (LPLD_FTM_IsCHnF(FTM1, FTM_Ch0))
	{
		PulseCnt1 += 1;
		//清空FTM0 COUNTER
		LPLD_FTM_ClearCounter(FTM1);
		//清除输入中断标志
		LPLD_FTM_ClearCHnF(FTM1, FTM_Ch0);
	}
}
void ic_isr1(void)
{
	if (LPLD_FTM_IsCHnF(FTM2, FTM_Ch0))
	{
		PulseCnt2 += 1;
		//清空FTM0 COUNTER
		LPLD_FTM_ClearCounter(FTM2);
		//清除输入中断标志
		LPLD_FTM_ClearCHnF(FTM2, FTM_Ch0);
	}
}


void SpeedInit(void)
{
	FTM_InitTypeDef ftm1_init_struct;
	ftm1_init_struct.FTM_Ftmx = FTM1;      //使能FTM1通道
	ftm1_init_struct.FTM_Mode = FTM_MODE_IC;       //使能输入捕获模式
	ftm1_init_struct.FTM_ClkDiv = FTM_CLK_DIV128;  //计数器频率为总线时钟的128分频
	ftm1_init_struct.FTM_Isr = ic_isr0;     //设置中断函数
	//初始化FTM0
	LPLD_FTM_Init(ftm1_init_struct);
	//使能输入捕获对应通道,上升沿捕获进入中断
	LPLD_FTM_IC_Enable(FTM1, FTM_Ch0, PTB0, CAPTURE_RI);

	//使能FTM0中断
	LPLD_FTM_EnableIrq(ftm1_init_struct);

	ftm1_init_struct.FTM_Ftmx = FTM2;      //使能FTM1通道
	ftm1_init_struct.FTM_Mode = FTM_MODE_IC;       //使能输入捕获模式
	ftm1_init_struct.FTM_ClkDiv = FTM_CLK_DIV128;  //计数器频率为总线时钟的128分频
	ftm1_init_struct.FTM_Isr = ic_isr1;     //设置中断函数
	//初始化FTM0
	LPLD_FTM_Init(ftm1_init_struct);
	//使能输入捕获对应通道,上升沿捕获进入中断
	LPLD_FTM_IC_Enable(FTM1, FTM_Ch0, PTB0, CAPTURE_RI);

	//使能FTM0中断
	LPLD_FTM_EnableIrq(ftm1_init_struct);
}
float Dir_Kp_Correction[100];
float IntSum = 0;
void DirControlValueCale(void)
{
	// 2  1
	//50   40  30  20  10 

	/*static float PreError=0;
	static float temp = 0;
	Dir_PID.LastError = Dir_PID.ThisError;
	Dir_PID.ThisError = Dir_PID.ControlValue;
	Dir_Diff = Dir_PID.LastError-Dir_PID.ThisError;//为了迎合微分项的负号
	Dir_PID.OutValue = -Dir_PID.ThisError*Dir_PID.Kp + Dir_PID.Kd*Dir_Diff;
	TempValue.DirOutValue_Old = TempValue.DirOutValue_New;
	TempValue.DirOutValue_New = Dir_PID.OutValue;*/
	/*static DirPID_TypeDef *p = &Dir_PID;

	p->ThisError = p->ControlValue;
	p->OutValue = -p->Kp*(p->ThisError - p->LastError)\
	+ p->Kd*(p->ThisError - 2 * p->LastError + p->PreError);
	p->PreError = p->LastError;
	p->LastError = p->ThisError;
	p->OutValueSum += p->OutValue;
	TempValue.DirOutValue_Old = TempValue.DirOutValue_New;
	TempValue.DirOutValue_New = p->OutValueSum;*/


	//float Dir_Diff;
	static float Ki = 0;
	static float Last_DirAngSpeed;
	extern float Dir_AngSpeed;
	
	Dir_PID.LastError = Dir_PID.ThisError;
	Dir_PID.ThisError = Dir_PID.ControlValue;
	//Dir_Diff = Dir_PID.LastError - Dir_PID.ThisError;
	IntSum += Ki*Dir_PID.ThisError;
	//Dir_PID.OutValue = -(Dir_PID.ThisError*0.7 + Dir_PID.LastError*0.3)* (Dir_PID.Kp_Temp) + (Dir_Diff)*Dir_PID.Kd_Temp + IntSum;
	Dir_PID.OutValue = -(Dir_PID.ThisError*0.7 + Dir_PID.LastError*0.3)* (Dir_PID.Kp_Temp) + (Dir_AngSpeed*0.6+Last_DirAngSpeed*0.4)*Dir_PID.Kd_Temp + IntSum;
	TempValue.DirOutValue_Old = TempValue.DirOutValue_New;
	TempValue.DirOutValue_New = Dir_PID.OutValue;
	Last_DirAngSpeed = Dir_AngSpeed;
	/*TempValue.DirOutValue = Dir_PID.OutValue;
	if (TempValue.DirOutValue > 0)
	{
		TempValue.Dir_LeftOutValue = TempValue.DirOutValue;
		TempValue.Dir_RightOutValue = -TempValue.DirOutValue;
	}
	else
	{
		TempValue.Dir_LeftOutValue = TempValue.DirOutValue;
		TempValue.Dir_RightOutValue = -TempValue.DirOutValue;
	}*/
	/*static float LeftError=0;
	static float RightError=0;
	static float LastLeftError = 0;
	static float LastRightError = 0;
	LastLeftError = LeftError;
	LastRightError = RightError;
	LeftError = CCDMain_Status.LeftSet - CCDMain_Status.LeftPoint;
	RightError = CCDMain_Status.RightSet - CCDMain_Status.RightPoint;
	if (CCDMain_Status.Left_LostFlag == 1)
	{
	TempValue.Dir_RightOutValue = -(LeftError+RightError/4)*Dir_PID.Kp + Dir_PID.Kd*(LastLeftError - LeftError);
	}
	else
	TempValue.Dir_RightOutValue = -LeftError*Dir_PID.Kp + Dir_PID.Kd*(LastLeftError - LeftError);
	if (CCDMain_Status.Right_LostFlag == 1)
	{
	TempValue.Dir_LeftOutValue = -(RightError+LeftError/4)*Dir_PID.Kp + Dir_PID.Kd*(LastRightError - RightError);
	}
	else
	TempValue.Dir_LeftOutValue = -RightError*Dir_PID.Kp + Dir_PID.Kd*(LastRightError - RightError);*/
}

void ControlSmooth(void)
{
	static float TempF = 0;
	//TempF = TempValue.New_SpeedOutValueRight - TempValue.Old_SpeedOutValueRight;
	//TempValue.SpeedOutValueRight = TempF*(SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + TempValue.Old_SpeedOutValueRight;

	//TempF = TempValue.New_SpeedOutValueLeft - TempValue.Old_SpeedOutValueLeft;
	//TempValue.SpeedOutValueLeft = TempF*(SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + TempValue.Old_SpeedOutValueLeft;

	TempF = TempValue.New_SpeedOutValue - TempValue.Old_SpeedOutValue;
	TempValue.SpeedOutValue = TempF*(SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + TempValue.Old_SpeedOutValue;


	TempF = TempValue.DirOutValue_New - TempValue.DirOutValue_Old;
	TempValue.DirOutValue = TempF*(DirectionConrtolPeriod + 1) / DIRECTION_CONTROL_PERIOD + TempValue.DirOutValue_Old;
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
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch0, MotorControl.LeftMotorOutValue + DeathValueLeft);
	}
	else
	{
		MotorControl.LeftMotorOutValue = -MotorControl.LeftMotorOutValue; //为负值取反
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch0, 0);
		DeathTime_Delay();
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch3, MotorControl.LeftMotorOutValue + DeathValueLeft);
	}
	if (MotorControl.RightMotorOutValue >= 0)
	{
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch2, 0);
		DeathTime_Delay();
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch1, MotorControl.RightMotorOutValue + DeathValueRight);
	}
	else
	{
		MotorControl.RightMotorOutValue = -MotorControl.RightMotorOutValue;
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch1, 0);
		DeathTime_Delay();//
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch2, MotorControl.RightMotorOutValue + DeathValueRight);
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
