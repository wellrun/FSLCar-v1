#include "AngGet.h"
#include "common.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "datastructure.h"
#include "Kalman.h"
//#include "MPU6050.h"
#include "L3G4200.h"
float GYROSCOPE_ANGLE_RATIO = 0.09;// 0.1336// (3300/4096)/(0.67*6) //陀螺仪当前的静态为2360  //这个是放大9倍
									//L3G4200官方典型值为0.0175
float Dir_SpeedRatio = 0.08;
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
  int i=200,j=300;
  for(i=0;i<200;i++)
  for(j=0;j<3000;j++)
    asm("nop");
}
void AngleIntegration(float Anglespeed);

void AngleGet(void)
{
	static int initok = 0;
	static float GyroTemp = 0;
        static char whoam=0;
        static int error=0;
	acc_x = LPLD_MMA8451_GetResult(MMA8451_STATUS_Z_READY, MMA8451_REG_OUTZ_MSB);
	Dir_AngSpeed = L3G4200_GetResult(OUT_Z_L)*Dir_SpeedRatio;
	GyroscopeAngleSpeed = L3G4200_GetResult(OUT_Y_L)*GYROSCOPE_ANGLE_RATIO;
    whoam=L3G4200_ReadReg(WHO_AM_I);
	if ( whoam!= 0xd3)
	{
		while (1)
		{
                  error++;
			errordealy();
                      LPLD_GPIO_Output_b(PTE, 5,1);
                      LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch4, 0);
                      LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch5, 0);
                      LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch6, 0);
                      LPLD_FTM_PWM_ChangeDuty(FTM0, FTM_Ch7, 0);
					  whoam = L3G4200_Init();
                      if ( whoam== 0xd3)
                        break;
		}
	}
	GravityAngle = acc_x*(180.0 / (4096.0 * 2));

	
	if (initok == 0)
	{
		angle_com = GravityAngle;
		initok = 1;
	}
       // complement_filter(GravityAngle, GyroscopeAngleSpeed);
	complement2(GravityAngle, GyroscopeAngleSpeed);
        CarInfo_Now.CarAngle = angle_com;

	CarInfo_Now.CarAngSpeed = angle_dot_com;

// 	Kalman_Filter(GravityAngle, -GyroscopeAngleSpeed);
// 	CarInfo_Now.CarAngSpeed = angle_dot;
// 	CarInfo_Now.CarAngle = angle;
	AngleIntegration(Dir_AngSpeed);//确定了当前的值合适

}

void AngleIntegration(float Anglespeed)//对角速度积分得到角度确定GYROSCOPE_ANGLE_RATIO的值
{
	float dt = 0.005;
    
	AngleIntegraed += Anglespeed*dt;
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
