#include "AngGet.h"
#include "common.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "datastructure.h"
#include "Kalman.h"
//#include "MPU6050.h"
#include "L3G4200.h"
float GYROSCOPE_ANGLE_RATIO = 0.078;// 0.1336// (3300/4096)/(0.67*6) //�����ǵ�ǰ�ľ�̬Ϊ2360  //����ǷŴ�9��
									//L3G4200�ٷ�����ֵΪ0.0175

extern CarInfo_TypeDef CarInfo_Now; //��ǰ���ӵ���Ϣ
extern CarControl_TypeDef MotorControl; //������Ƶ�ֵ
extern float angle_com, angle_dot_com;
extern short acc_x, acc_y, acc_z, gyro_1, gyro_2;
extern int gyro_avg;

AngPID_InitTypeDef Ang_PID; //�Ƕȿ��Ƶ�PID�ṹ��
TempOfMotor_TypeDef TempValue; //��ʱ�洢�ǶȺ��ٶȿ��Ƹ�������Ľṹ��
float GravityAngle, GyroscopeAngleSpeed;
float AngleIntegraed = 0;//�Խ��ٶȵĻ���ֵ

float temp_x;
/*int gyro_avg = 2360; //���ٶ�ADƽ��ֵ*/

void AngleIntegration(float Anglespeed);

void AngleGet(void)
{
  static int initok=0;
	acc_x =LPLD_MMA8451_GetResult(MMA8451_STATUS_X_READY,MMA8451_REG_OUTX_MSB);
	//acc_x = MPU6050_GetResult(ACCEL_ZOUT_H);
	//GyroscopeAngleSpeed = MPU6050_GetResult(GYRO_YOUT_H)*GYROSCOPE_ANGLE_RATIO;//����ı���������������..׼ȷֵӦ����16.4
	GyroscopeAngleSpeed = L3G4200_GetResult(OUT_Y_L)*GYROSCOPE_ANGLE_RATIO;
	//Dir_Diff = MPU6050_GetResult(GYRO_XOUT_H)*DirGyro_Ratio;
	//Dir_Diff = GyroscopeAngleSpeed;
	//acc_x = LPLD_ADC_Get(ADC1, AD15)-1950;
        //GravityAngle=acc_x*(180.0/(2900.0-1000.0));
	//GyroscopeAngleSpeed = (LPLD_ADC_Get(ADC1, AD14)-1250)*GYROSCOPE_ANGLE_RATIO;
	GravityAngle =acc_x*(180.0/(4096.0*2));

 	//complement_filter(GravityAngle, -GyroscopeAngleSpeed);
        if(initok==0)
        {
          angle_com=GravityAngle;
          initok=1;
        }
        complement2(GravityAngle, GyroscopeAngleSpeed);
 	
 	CarInfo_Now.CarAngle = angle_com;
 	CarInfo_Now.CarAngSpeed = angle_dot_com;
// 	Kalman_Filter(GravityAngle, -GyroscopeAngleSpeed);
// 	CarInfo_Now.CarAngSpeed = angle_dot;
// 	CarInfo_Now.CarAngle = angle;
	AngleIntegration((float)(CarInfo_Now.CarAngSpeed));//ȷ���˵�ǰ��ֵ����

}

void AngleIntegration(float Anglespeed)//�Խ��ٶȻ��ֵõ��Ƕ�ȷ��GYROSCOPE_ANGLE_RATIO��ֵ
{
	float dt = 0.005;
    
	AngleIntegraed += Anglespeed*dt;
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
	point = (unsigned char*) target; //�õ�float�ĵ�ַ
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
	buf[beg + 2] = point[2];
	buf[beg + 3] = point[3];
}
void Byte2Float(float *target,unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*) target; //�õ�float�ĵ�ַ
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
	point = (unsigned char*) target; //�õ�float�ĵ�ַ
	point[0] = buf[beg];
	point[1] = buf[beg + 1];
	point[2] = buf[beg + 2];
	point[3] = buf[beg + 3];
}

void Int2Byte(int *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char*) target; //�õ�float�ĵ�ַ
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
	buf[beg + 2] = point[2];
	buf[beg + 3] = point[3];
}
