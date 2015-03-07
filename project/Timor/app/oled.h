#ifndef _OLED_H
#define _OLED_H
#include "common.h"
//OLED��ʾ��
//�������
#define LED_SCLK(x) LPLD_GPIO_Output_b(PTB, 23, x)// ʱ�Ӷ��� 
#define LED_SDA(x)  LPLD_GPIO_Output_b(PTB, 22, x)//���ݿ�D0
#define LED_RST(x)  LPLD_GPIO_Output_b(PTB, 21, x)//��λ���ܵ�ƽ
#define LED_DC(x)  LPLD_GPIO_Output_b(PTB, 20, x)//ƫ�ó���
//OLED��ʾ����
/*����ASCII���*/


//void OLED_GPIO_Init(void);
//д���ݺ���
void LED_WrDat(unsigned char data) ;
//д�������
void LED_WrCmd(unsigned char cmd) ;
//������ʾ����λ��
void LED_Set_Pos(unsigned char x, unsigned char y);
void LED_Fill(unsigned char bmp_data);

void LED_Init(void);
//��ʾһ��6x8��׼ASCII�ַ�
void LED_P6x8Char(unsigned char x,unsigned char y,unsigned char ch);
//����������д��һ��6x8��׼ASCII�ַ���
void LED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
//��һ��char����ת����3λ��������ʾ
void LED_PrintValueC(unsigned char x, unsigned char y, signed char data);
//������������һ��int����ת����5λ��������ʾ
void LED_PrintValueI(unsigned char x, unsigned char y, int data);
//��ʾunsigned int��
 void LED_PrintValueFP(unsigned char x, unsigned char y, unsigned int data, unsigned char num);
 //������������һ��float����ת������������5λ��С���ͷ��ŵ����ݲ�������ʾ
 void LED_PrintValueF(unsigned char x, unsigned char y, float data, unsigned char num);

 void showimage(unsigned char *image);




#endif 