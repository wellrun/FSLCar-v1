#include "Control.h"
#include "CCD.h"

//float AngToMotorRatio=300;//角度转换成电机控制的比例因子..我也不知道取多少合适..以后再调试
#define MOTOR_OUT_MAX       8000
#define MOTOR_OUT_MIN       -8000
#define ANGLE_CONTROL_OUT_MAX			MOTOR_OUT_MAX
#define ANGLE_CONTROL_OUT_MIN			MOTOR_OUT_MIN
#define CoderResolution 500 //编码器的线数
#define TyreCircumference 41//轮胎周长CM
#define DeathValue 200//死区电压 2%的占空比

#define CONTROL_PERIOD	5 //电机的输出周期
#define SPEED_CONTROL_COUNT 8 //速度控制的分割次数
#define SPEED_CONTROL_PERIOD (SPEED_CONTROL_COUNT * CONTROL_PERIOD) //速度控制的总时间
#define DIRECTION_CONTROL_COUNT			4  //方向控制是20Ms一次
#define DIRECTION_CONTROL_PERIOD		(DIRECTION_CONTROL_COUNT * CONTROL_PERIOD)//方向控制的总时间

extern float Ang_dt;//控制周期,在主函数定义,20ms
extern float Speed_Dt;//速度的周期,0.08ms

extern CarInfo_TypeDef CarInfo_Now; //当前车子的信息
extern CarControl_TypeDef MotorControl; //电机控制的值
extern AngPID_InitTypeDef Ang_PID; //角度控制的PID结构体
extern TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
SpeedPID_TypeDef Speed_PID;
DirPID_TypeDef Dir_PID;
short SpeedControlPeriod, DirectionConrtolPeriod;

void AngleControlValueCalc(void)
{
	float ControlValue=0;
	Ang_PID.Delta = (Ang_PID.AngSet) - CarInfo_Now.CarAngle; //当前误差//这里全是角度,值很小
	//微分项应该是负的
        ControlValue = Ang_PID.Delta * Ang_PID.Kp - CarInfo_Now.CarAngSpeed* Ang_PID.Kd; //微分项,如果角速度大于0.说明角度趋势变大,把控制量增大
	//ControlValue *= AngToMotorRatio; //乘上比例因子将角度转换成PWM的占空比
	if (ControlValue > ANGLE_CONTROL_OUT_MAX)
		ControlValue = ANGLE_CONTROL_OUT_MAX;
	if (ControlValue < ANGLE_CONTROL_OUT_MIN)
		ControlValue = ANGLE_CONTROL_OUT_MIN; //限幅
	TempValue.AngControl_OutValue = ControlValue; //更新控制临时变量的值
}

void SpeedGet(void)
{//FTM1是左电机,FTM2是右电机
	CarInfo_Now.MotorCounterLeft = LPLD_FTM_GetCounter(FTM1);
	CarInfo_Now.MotorCounterRight = LPLD_FTM_GetCounter(FTM2);
	LPLD_FTM_ClearCounter(FTM1);
	LPLD_FTM_ClearCounter(FTM2);

	if (CarInfo_Now.MotorCounterLeft >= 30000)
	{
		CarInfo_Now.MotorCounterLeft -= 65535;
	}
	if (CarInfo_Now.MotorCounterRight >= 30000)
	{
		CarInfo_Now.MotorCounterRight -= 65535;
	}
	CarInfo_Now.MotorCounterRight = -CarInfo_Now.MotorCounterRight;
	CarInfo_Now.LeftSpeed = (CarInfo_Now.MotorCounterLeft / CoderResolution*TyreCircumference) / Speed_Dt;//计算出速度,是准确的cm/s
	CarInfo_Now.RightSpeed = (CarInfo_Now.MotorCounterRight / CoderResolution*TyreCircumference) / Speed_Dt;//计算出速度,是准确的cm/s
	//CarInfo_Now.CarSpeed = (CarInfo_Now.LeftSpeed + CarInfo_Now.RightSpeed) / 2;//车子的速度用左右轮速度平均值,不准确
	CarInfo_Now.CarSpeed = (CarInfo_Now.MotorCounterLeft + CarInfo_Now.MotorCounterRight) / 2;

	/*printf("QD Counter1 = %d\r\n", CarInfo_Now.MotorCounterLeft);
	printf("QD Counter2 = %d\r\n", CarInfo_Now.MotorCounterRight);*/
}

void SpeedControlValueCalc(void)
{
/*	float ControlValue = 0;*/
	short Index = 0;
	SpeedGet();
	Speed_PID.ThisError = Speed_PID.SpeedSet - CarInfo_Now.CarSpeed;
	if (Speed_PID.OutValue >= Speed_PID.OutMax)
	{
		if (Speed_PID.ThisError >= 0)
			Index = 0;//如果上一次的输出值大于最大值,且本次的误差继续增大,则取消积分
		else
			Index = 1;
	}
	else if (Speed_PID.OutValue < Speed_PID.OutMin)
	{
		if (Speed_PID.ThisError <= 0)
			Index = 0;
		else
			Index = 1;
	}
	else
		Index = 1;
	Speed_PID.IntegralSum += Speed_PID.ThisError;//;*Index;//抗饱和//先取消抗饱和..因为Index的判断可能有问题
	Speed_PID.OutValue = Speed_PID.Kp*Speed_PID.ThisError + Speed_PID.Ki*Speed_PID.IntegralSum;
	Speed_PID.OutValue /= 100;//比例因子,转换为PWM占空比
	TempValue.Old_SpeedOutValue = TempValue.New_SpeedOutValue;
	TempValue.New_SpeedOutValue = Speed_PID.OutValue;
}

void DirControlValueCale(void)
{
	float Dir_Diff;	
	Dir_PID.LastError = Dir_PID.iError;
	Dir_PID.iError = CCDMain_Status.ControlValue;
	Dir_Diff = Dir_PID.LastError-Dir_PID.iError;//为了迎合微分项的负号
	Dir_PID.OutValue = Dir_PID.iError*Dir_PID.Kp - Dir_PID.Kd*Dir_Diff;
	TempValue.DirOutValue_Old = TempValue.DirOutValue_New;
	TempValue.DirOutValue_New = Dir_PID.OutValue;
}

void ControlSmooth(void)
{
	static float TempF = 0;
	TempF = TempValue.New_SpeedOutValue - TempValue.Old_SpeedOutValue;
	TempValue.SpeedOutValue = TempF*(SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + TempValue.Old_SpeedOutValue;

	TempF = TempValue.DirOutValue_New - TempValue.DirOutValue_Old;
	TempValue.DirOutValue = TempF*(DirectionConrtolPeriod + 1) / DIRECTION_CONTROL_PERIOD + TempValue.DirOutValue_Old;
	TempValue.Dir_LeftOutValue = TempValue.DirOutValue;
	TempValue.Dir_RightOutValue = -TempValue.DirOutValue;
}
void MotorControl_Out(void)
{
	ControlSmooth();
	MotorControl.LeftMotorOutValue = (int) TempValue.AngControl_OutValue
			+ (int) TempValue.Dir_LeftOutValue-(int)TempValue.SpeedOutValue; //取整
	MotorControl.RightMotorOutValue = (int) TempValue.AngControl_OutValue
			+ (int) TempValue.Dir_RightOutValue-(int)TempValue.SpeedOutValue; 
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
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch4, MotorControl.LeftMotorOutValue+DeathValue);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch5, 0);
	}
	else
	{
		MotorControl.LeftMotorOutValue = -MotorControl.LeftMotorOutValue; //为负值取反
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch4, 0);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch5, MotorControl.LeftMotorOutValue + DeathValue);
	}
	if (MotorControl.RightMotorOutValue >= 0)
	{
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch6, MotorControl.RightMotorOutValue + DeathValue);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch7, 0);
	}
	else
	{
		MotorControl.RightMotorOutValue = -MotorControl.RightMotorOutValue;
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch6, 0);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch7, MotorControl.RightMotorOutValue + DeathValue);
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
