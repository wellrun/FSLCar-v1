#include "complement.h"

//-------------------------------------------------------
//�����˲�
//-------------------------------------------------------
float angle_com, angle_dot_com; //�ⲿ��Ҫ���õı���
extern float CarAngle, CarAngSpeed;
//-------------------------------------------------------
static float bias_cf = 0.2;
static const float Ang_dt = 0.004;
static  float ComFilter_t = 0.09;
//-------------------------------------------------------
void complement_filter(float angle_m_cf, float gyro_m_cf)
{
	float a, b;
	a = (ComFilter_t / (ComFilter_t + Ang_dt));
	b = (1.0 - a);
	/*bias_cf = bias_cf * 0.999; //��������Ʈ��ͨ�˲���500�ξ�ֵ��0.998
	bias_cf = bias_cf + gyro_m_cf * 0.001; //0.002
	angle_dot_com = gyro_m_cf- bias_cf;*/
        angle_dot_com = gyro_m_cf;
	angle_com = (angle_com + angle_dot_com * Ang_dt) * a + angle_m_cf * b;
	//���ٶȵ�ͨ�˲���20�ξ�ֵ����100��ÿ����㣬��ͨ5Hz��0.90 0.05
//	CarAngSpeed=angle_dot_com;
//	CarAngle=angle_com;
}

float K2 = 0.4; // �Լ��ٶȼ�ȡֵ��Ȩ��
float x1, x2, y1;

void complement2(float angle_m, float gyro_m)//�ɼ������ĽǶȺͽǼ��ٶ�
{
	bias_cf = bias_cf * 0.999; //��������Ʈ��ͨ�˲���500�ξ�ֵ��0.998
	bias_cf = bias_cf + gyro_m * 0.001; //0.002
	angle_dot_com = gyro_m - bias_cf;
	x1 = (angle_m - angle_com)*(1 - K2)*(1 - K2);
	y1 = y1 + x1*Ang_dt;
	x2 = y1 + 2 * (1 - K2)*(angle_m - angle_com) + angle_dot_com;
	angle_com = angle_com + x2*Ang_dt;

}
