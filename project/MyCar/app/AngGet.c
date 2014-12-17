#include "AngGet.h"
#include "common.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "datastructure.h"
#include "Kalman.h"
#define GYROSCOPE_ANGLE_RATIO  0.2// 0.1336// (3300/4096)/(0.67*6) //陀螺仪当前的静态为2360  //这个是放大9倍

extern CarInfo_TypeDef CarInfo_Now; //当前车子的信息
extern CarControl_TypeDef MotorControl; //电机控制的值
extern float angle_com, angle_dot_com;
extern short acc_x, acc_y, acc_z, gyro_1, gyro_2;
extern int gyro_avg;

AngPID_InitTypeDef Ang_PID; //角度控制的PID结构体
TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
float GravityAngle, GyroscopeAngleSpeed;
float AngleIntegraed = 0;//对角速度的积分值

float temp_x;
int gyro_avg = 2360; //角速度AD平均值

void AngleIntegration(float Anglespeed);

void AngleGet(void)
{
	acc_x = LPLD_MMA8451_GetResult(MMA8451_STATUS_Y_READY,
			MMA8451_REG_OUTY_MSB);
	temp_x = (float) acc_x / 4096.0;
	if (temp_x > 1)
		temp_x = 1;
	else if (temp_x < -1)
		temp_x = -1;
//		acc_y = LPLD_MMA8451_GetResult(MMA8451_STATUS_Y_READY,
//				MMA8451_REG_OUTY_MSB);
//	acc_z = LPLD_MMA8451_GetResult(MMA8451_STATUS_X_READY,
//			MMA8451_REG_OUTZ_MSB);
	//gyro_1 = LPLD_ADC_Get(ADC1, AD14);
	gyro_2 = (LPLD_ADC_Get(ADC1, AD15) - gyro_avg);
	AngleIntegration((float)(-gyro_2));//确定了当前的值合适
	GyroscopeAngleSpeed = (float) gyro_2 * GYROSCOPE_ANGLE_RATIO;
	GravityAngle = asinf(temp_x) * 57.3;

	complement_filter(GravityAngle, -GyroscopeAngleSpeed);
/*	complement2(GravityAngle, -GyroscopeAngleSpeed);*/
	CarInfo_Now.CarAngle = angle_com;
	CarInfo_Now.CarAngSpeed = angle_dot_com;
// 	Kalman_Filter(GravityAngle, -GyroscopeAngleSpeed);
// 	CarInfo_Now.CarAngSpeed = angle_dot;
// 	CarInfo_Now.CarAngle = angle;

}

void AngleIntegration(float Anglespeed)//对角速度积分得到角度确定GYROSCOPE_ANGLE_RATIO的值
{
	float dt = 0.02;
    
	AngleIntegraed += Anglespeed*dt*GYROSCOPE_ANGLE_RATIO;
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
	point = (unsigned char*) target; //得到float的地址
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
	buf[beg + 2] = point[2];
	buf[beg + 3] = point[3];
}
void Byte2Float(float *target,unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*) target; //得到float的地址
	point[0] = buf[beg];
	point[1] = buf[beg + 1];
	point[2] = buf[beg + 2];
	point[3] = buf[beg + 3];
}

void Short2Byte(int16 *target, int8 *buf, int8 beg)
{
	int8 *point;
	point = (int8*) target;
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
}

void Byte2Int(int *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*) target; //得到float的地址
	point[0] = buf[beg];
	point[1] = buf[beg + 1];
	point[2] = buf[beg + 2];
	point[3] = buf[beg + 3];
}

void Int2Byte(int *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*) target; //得到float的地址
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
	buf[beg + 2] = point[2];
	buf[beg + 3] = point[3];
}
