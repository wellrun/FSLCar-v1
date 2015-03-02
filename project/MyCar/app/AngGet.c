#include "AngGet.h"
#include "common.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "datastructure.h"
#include "Kalman.h"
#include "MMA8451_moni.h"
//#include "MPU6050.h"
//#include "MPU6050_Moni.h"
#include "math.h"
#include "L3G4200.h"
float GYROSCOPE_ANGLE_RATIO = 0.39;
float Dir_SpeedRatio = 0.21;
int Gyro_Offset=2000;
float AngRatio=(180.0 / (4096.0 * 2));
extern CarInfo_TypeDef CarInfo_Now; //当前车子的信息
extern CarControl_TypeDef MotorControl; //电机控制的值
extern float angle_com, angle_dot_com;
extern short acc_x, acc_y, acc_z, gyro_1, gyro_2;
extern int gyro_avg;

AngPID_InitTypeDef Ang_PID; //角度控制的PID结构体
TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
float GravityAngle, GyroscopeAngleSpeed;
float AngleIntegraed = 0;//对角速度的积分值
float Dir_AngSpeed = 0;

float temp_x;
/*int gyro_avg = 2360; //角速度AD平均值*/

void errordealy(void)
{
	int i = 200, j = 300;
	for (i = 0; i < 200; i++)
		for (j = 0; j < 3000; j++)
			asm("nop");
}
void AngleIntegration(float Anglespeed);

void AngleGet(void)
{
	static int initok = 0;
	short Acc_H;
	static float bias_cf;
	Acc_H = MMA8451_ReadByte(MMA8451_REG_OUTZ_MSB);
	GyroscopeAngleSpeed = (LPLD_ADC_Get(ADC1, AD6)-Gyro_Offset)*GYROSCOPE_ANGLE_RATIO;
        Dir_AngSpeed=-(LPLD_ADC_Get(ADC1, AD7)-Gyro_Offset)*Dir_SpeedRatio;
        bias_cf = bias_cf * 0.999; //陀螺仪零飘低通滤波；500次均值；0.998
	bias_cf = bias_cf + Dir_AngSpeed * 0.001; //0.002*/
	Dir_AngSpeed = Dir_AngSpeed- bias_cf;
	AngleIntegration(Dir_AngSpeed);//确定了当前的值合适
	//ACC_L = LPLD_MMA8451_ReadReg(MMA8451_REG_OUTZ_LSB);
	acc_x = ((Acc_H << 8));
	acc_x = acc_x >> 2;
	GravityAngle = acc_x*AngRatio;
	if (initok == 0)
	{
		angle_com = GravityAngle;
		initok = 1;
	}
	complement2(GravityAngle, GyroscopeAngleSpeed);
	CarInfo_Now.CarAngle = angle_com;
	CarInfo_Now.CarAngSpeed = angle_dot_com;
}
float AngleIntegraed_Ch1, AngleIntegraed_Ch2;
unsigned char Flag_Started_Ch1, Flag_Started_Ch2;
void AngleIntegration(float Anglespeed)//对角速度积分得到角度确定GYROSCOPE_ANGLE_RATIO的值
{
	float dt = 0.005;

	AngleIntegraed += Anglespeed*dt;
	if (Flag_Started_Ch1 == 1)
	{
		AngleIntegraed_Ch1 += AngleIntegraed;
	}
	if (Flag_Started_Ch2 == 1)
	{
		AngleIntegraed_Ch2 += AngleIntegraed;
	}
}
unsigned char DirAng_StartIntegraed(unsigned char Ch)//开始对转向角度进行积分
{
	if (Ch == 1)
	{
		if (Flag_Started_Ch1 == 1)
		{
			return 1;
		}
		else
		{
			Flag_Started_Ch1 = 1;
			return 0;
		}
	}
	else if (Ch == 2)
	{
		if (Flag_Started_Ch2 == 1)
		{
			return 1;
		}
		else
		{
			Flag_Started_Ch2 = 1;
			return 0;
		}
	}
}
float DirAng_GetAng(unsigned char Ch)
{
	if (Ch == 1)
	{
		return AngleIntegraed_Ch1;
	}
	else if (Ch == 2)
	{
		return AngleIntegraed_Ch2;
	}
}
void DirAng_Clear(unsigned char Ch)
{
	if (Ch == 1)
	{
		Flag_Started_Ch1 = 0;
		AngleIntegraed_Ch1 = 0;
	}
	else if (Ch == 2)
	{
		Flag_Started_Ch2 = 0;
		AngleIntegraed_Ch2 = 0;
	}
}
//函数说明：将单精度浮点数据转成4字节数据并存入指定地址 //gittest
//附加说明：用户无需直接操作此函数
//target:目标单精度数据
//buf:待写入数组
//beg:指定从数组第几个元素开始写入
//函数无返回
void Float2Byte(float *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*)target; //得到float的地址
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
	buf[beg + 2] = point[2];
	buf[beg + 3] = point[3];
}
void Byte2Float(float *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*)target; //得到float的地址
	point[0] = buf[beg];
	point[1] = buf[beg + 1];
	point[2] = buf[beg + 2];
	point[3] = buf[beg + 3];
}

void Short2Byte(int16 *target, int8 *buf, int8 beg)
{
	int8 *point;
	point = (int8*)target;
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
}

void Byte2Int(int *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*)target; //得到float的地址
	point[0] = buf[beg];
	point[1] = buf[beg + 1];
	point[2] = buf[beg + 2];
	point[3] = buf[beg + 3];
}

void Int2Byte(int *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*)target; //得到float的地址
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
	buf[beg + 2] = point[2];
	buf[beg + 3] = point[3];
}
