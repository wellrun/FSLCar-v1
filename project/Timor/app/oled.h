#ifndef _OLED_H
#define _OLED_H
#include "common.h"
//OLED显示屏
//定义输出
#define LED_SCLK(x) LPLD_GPIO_Output_b(PTB, 23, x)// 时钟定义 
#define LED_SDA(x)  LPLD_GPIO_Output_b(PTB, 22, x)//数据口D0
#define LED_RST(x)  LPLD_GPIO_Output_b(PTB, 21, x)//复位低能电平
#define LED_DC(x)  LPLD_GPIO_Output_b(PTB, 20, x)//偏置常低
//OLED显示函数
/*定义ASCII码表*/


//void OLED_GPIO_Init(void);
//写数据函数
void LED_WrDat(unsigned char data) ;
//写入命令函数
void LED_WrCmd(unsigned char cmd) ;
//设置显示坐标位置
void LED_Set_Pos(unsigned char x, unsigned char y);
void LED_Fill(unsigned char bmp_data);

void LED_Init(void);
//显示一个6x8标准ASCII字符
void LED_P6x8Char(unsigned char x,unsigned char y,unsigned char ch);
//功能描述：写入一组6x8标准ASCII字符串
void LED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
//将一个char型数转换成3位数进行显示
void LED_PrintValueC(unsigned char x, unsigned char y, signed char data);
//功能描述：将一个int型数转换成5位数进行显示
void LED_PrintValueI(unsigned char x, unsigned char y, int data);
//显示unsigned int型
 void LED_PrintValueFP(unsigned char x, unsigned char y, unsigned int data, unsigned char num);
 //功能描述：将一个float型数转换成整数部分5位带小数和符号的数据并进行显示
 void LED_PrintValueF(unsigned char x, unsigned char y, float data, unsigned char num);

 void showimage(unsigned char *image);




#endif 