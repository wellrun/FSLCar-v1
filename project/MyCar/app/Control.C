#include "Control.h"
#include "CCD.h"

//float AngToMotorRatio=300;//角度转换成电机控制的比例因子..我也不知道取多少合适..以后再调试
#define MOTOR_OUT_MAX       9000
#define MOTOR_OUT_MIN       -9000
#define ANGLE_CONTROL_OUT_MAX			8000
#define ANGLE_CONTROL_OUT_MIN			-8000
#define SPEED_CONTROL_OUT_MAX			2000
#define SPPED_CONTROL_OUT_MIN			-2000
#define CoderResolution 500 //编码器的线数
#define TyreCircumference 20//轮胎周长CM
//轮子转一圈..编码器增加5200
int  DeathValueLeft=00;//死区电压 2%的占空比S
int DeathValueRight=0;//右轮的死区电压 


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
	static int LeftSum = 0;
	static int RightSum = 0;
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
	//CarInfo_Now.LeftSpeed = (CarInfo_Now.MotorCounterLeft / CoderResolution*TyreCircumference) / Speed_Dt;//计算出速度,是准确的cm/s
	//CarInfo_Now.RightSpeed = (CarInfo_Now.MotorCounterRight / CoderResolution*TyreCircumference) / Speed_Dt;//计算出速度,是准确的cm/s
	//CarInfo_Now.CarSpeed = (CarInfo_Now.LeftSpeed + CarInfo_Now.RightSpeed) / 2;//车子的速度用左右轮速度平均值,不准确
	CarInfo_Now.CarSpeed = (CarInfo_Now.MotorCounterLeft + CarInfo_Now.MotorCounterRight) / 20;

	LeftSum += CarInfo_Now.MotorCounterLeft;
	RightSum += CarInfo_Now.MotorCounterRight;
        RightSum=RightSum;
	/*printf("QD Counter1 = %d\r\n", CarInfo_Now.MotorCounterLeft);
	printf("QD Counter2 = %d\r\n", CarInfo_Now.MotorCounterRight);*/
}

void SpeedControlValueCalc(void)
{
	static SpeedPID_TypeDef *p = &Speed_PID;
	//static SpeedPID_TypeDef *Pr = &Speed_PID_Right;
	//PI
/*#define INTEGRAL_MAX 200000
	short Index = 0;
	float TempIntegral = 0;
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
	Speed_PID.IntegralSum += Speed_PID.Ki*Speed_PID.ThisError;//;*Index;//抗饱和//先取消抗饱和..因为Index的判断可能有问题
	TempIntegral = Speed_PID.IntegralSum;
	if (TempIntegral >= INTEGRAL_MAX)
		TempIntegral = INTEGRAL_MAX;
	else if (TempIntegral <= -INTEGRAL_MAX)
		TempIntegral = -INTEGRAL_MAX;
	Speed_PID.OutValue = Speed_PID.Kp*Speed_PID.ThisError + TempIntegral;
	Speed_PID.OutValue /= 100;//比例因子,转换为PWM占空比
	TempValue.Old_SpeedOutValue = TempValue.New_SpeedOutValue;
	TempValue.New_SpeedOutValue = Speed_PID.OutValue;*/
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


	SpeedGet();
	p->ThisError = p->SpeedSet - CarInfo_Now.CarSpeed ;
	if (p->ThisError > 80)
		p->ThisError = 80;
	else if (p->ThisError < -80)
		p->ThisError = -80;
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
	TempValue.Old_SpeedOutValue= TempValue.New_SpeedOutValue;
	TempValue.New_SpeedOutValue = p->OutValueSum;

}


float Dir_Kp_Correction[100];

void DirControlValueCale(void)
{
	// 2  1
 	//50   40  30  20  10 
	float Dir_Diff;
	static float PreError=0;
	static float temp = 0;
	/*Dir_PID.LastError = Dir_PID.ThisError;
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

	
	PreError = Dir_PID.LastError;
	Dir_PID.LastError = Dir_PID.ThisError;
	Dir_PID.ThisError = Dir_PID.ControlValue;
	Dir_Diff = Dir_PID.LastError - Dir_PID.ThisError;//为了迎合微分项的负号
	temp = PreError*0.2 + Dir_PID.LastError*0.3 + Dir_PID.ThisError*0.5;
	//Dir_PID.OutValue = -temp*Dir_PID.Kp + Dir_PID.Kd*Dir_Diff;
	Dir_PID.OutValue = Dir_Kp_Correction[(int)temp+50]*Dir_PID.Kp + Dir_Diff*Dir_PID.Kd;
	TempValue.DirOutValue_Old = TempValue.DirOutValue_New;
	TempValue.DirOutValue_New = Dir_PID.OutValue;
}

void ControlSmooth(void)
{
	static float TempF = 0;
// 	TempF = TempValue.New_SpeedOutValueRight - TempValue.Old_SpeedOutValueRight;
// 	TempValue.SpeedOutValueRight = TempF*(SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + TempValue.Old_SpeedOutValueRight;
// 
// 	TempF = TempValue.New_SpeedOutValueLeft - TempValue.Old_SpeedOutValueLeft;
// 	TempValue.SpeedOutValueLeft = TempF*(SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + TempValue.Old_SpeedOutValueLeft;

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
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch5, 0);
		LPLD_SYSTICK_DelayUs(1);//手动插入死区时间
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch4, MotorControl.LeftMotorOutValue + DeathValueLeft);
	}
	else
	{
		MotorControl.LeftMotorOutValue = -MotorControl.LeftMotorOutValue; //为负值取反
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch4, 0);
		LPLD_SYSTICK_DelayUs(1);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch5, MotorControl.LeftMotorOutValue + DeathValueLeft);
	}
	if (MotorControl.RightMotorOutValue >= 0)
	{
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch7, 0);
		LPLD_SYSTICK_DelayUs(1);
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch6, MotorControl.RightMotorOutValue + DeathValueRight);
	}
	else
	{
		MotorControl.RightMotorOutValue = -MotorControl.RightMotorOutValue;
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch6, 0);
		LPLD_SYSTICK_DelayUs(1);//
		LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch7, MotorControl.RightMotorOutValue + DeathValueRight);
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
