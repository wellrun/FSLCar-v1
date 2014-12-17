#ifndef _DEBUG_H_
#define _DEBUG_H_
#include "common.h"

void Timer_Init(void);  //初始化程序时间计数  每一小格为100us
void PIT3_ISR(void);        //PIT3的中断服务
void Timer_ReSet(void);  //计数器清零
void Struct_Init(void);
void ReciveArr(uint8 *ch, uint8 len);
//参数为发送数据的编号
void SendFlashData(uint8 sendcount); //发送FLash里面的数据到调试器
extern uint8 SendTemp8b[8];
#endif
