#include "AngGet.h"
#include "common.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "datastructure.h"
#include "math.h"
#include "L3G4200.h"
float GYROSCOPE_ANGLE_RATIO = 0.0115;
float Dir_SpeedRatio = 0.21*0.05;
float AngRatio=(180.0 / (4096.0 * 2));
extern CarInfo_TypeDef CarInfo_Now; //��ǰ���ӵ���Ϣ
extern CarControl_TypeDef MotorControl; //������Ƶ�ֵ
extern float angle_com, angle_dot_com;
extern short acc_x, acc_y, acc_z, gyro_1, gyro_2;
extern int gyro_avg;

AngPID_InitTypeDef Ang_PID; //�Ƕȿ��Ƶ�PID�ṹ��
TempOfMotor_TypeDef TempValue; //��ʱ�洢�ǶȺ��ٶȿ��Ƹ�������Ľṹ��
float GravityAngle, GyroscopeAngleSpeed;
float AngleIntegraed = 0;//�Խ��ٶȵĻ���ֵ
float Dir_AngSpeed = 0;

float temp_x;
/*int gyro_avg = 2360; //���ٶ�ADƽ��ֵ*/

void errordealy(void)
{
	int i = 200, j = 300;
	for (i = 0; i < 200; i++)
		for (j = 0; j < 3000; j++)
			asm("nop");
}
void AngleIntegration(float Anglespeed);
 int Anginitok = 0;

void AngleGet(void)
{
	Dir_AngSpeed = L3G4200_GetResult(OUT_Z_L)*Dir_SpeedRatio;
	acc_x = LPLD_MMA8451_GetResult_H(MMA8451_REG_OUTZ_MSB);
	GyroscopeAngleSpeed = L3G4200_GetResult(OUT_Y_L)*GYROSCOPE_ANGLE_RATIO;
	GravityAngle = -acc_x*AngRatio;
	AngleIntegration(Dir_AngSpeed);
	if (Anginitok == 0)
	{
		angle_com = GravityAngle;
		Anginitok = 1;
	}
	complement2(GravityAngle, GyroscopeAngleSpeed);
	CarInfo_Now.CarAngle = angle_com;
	CarInfo_Now.CarAngSpeed = angle_dot_com;
}
float AngleIntegraed_Ch1, AngleIntegraed_Ch2;
unsigned char Flag_Started_Ch1, Flag_Started_Ch2;
void AngleIntegration(float Anglespeed)//�Խ��ٶȻ��ֵõ��Ƕ�ȷ��GYROSCOPE_ANGLE_RATIO��ֵ
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
unsigned char DirAng_StartIntegraed(unsigned char Ch)//��ʼ��ת��ǶȽ��л���
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
        return 0;
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
        return 0;
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
//����˵�����������ȸ�������ת��4�ֽ����ݲ�����ָ����ַ //gittest
//����˵�����û�����ֱ�Ӳ����˺���
//target:Ŀ�굥��������
//buf:��д������
//beg:ָ��������ڼ���Ԫ�ؿ�ʼд��
//�����޷���
void Float2Byte(float *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*)target; //�õ�float�ĵ�ַ
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
	buf[beg + 2] = point[2];
	buf[beg + 3] = point[3];
}
void Byte2Float(float *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*)target; //�õ�float�ĵ�ַ
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
	point = (unsigned char*)target; //�õ�float�ĵ�ַ
	point[0] = buf[beg];
	point[1] = buf[beg + 1];
	point[2] = buf[beg + 2];
	point[3] = buf[beg + 3];
}

void Int2Byte(int *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*)target; //�õ�float�ĵ�ַ
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
	buf[beg + 2] = point[2];
	buf[beg + 3] = point[3];
}