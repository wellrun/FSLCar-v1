#ifndef _DEBUG_H_
#define _DEBUG_H_
#include "common.h"

void Timer_Init(void);  //��ʼ������ʱ�����  ÿһС��Ϊ100us
void PIT3_ISR(void);        //PIT3���жϷ���
void Timer_ReSet(void);  //����������
void Struct_Init(void);
void ReciveArr(uint8 *ch, uint8 len);
//����Ϊ�������ݵı��
void SendFlashData(uint8 sendcount); //����FLash��������ݵ�������
extern uint8 SendTemp8b[8];
#endif
