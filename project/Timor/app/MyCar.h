#ifndef _MYCAR_H_
#define  _MYCAR_H_
#define  Debug_Usart_Port UART0
#include "common.h"
#include "Init.h"
#include "angget.h"
#include "math.h"
#include "complement.h"
#include "DEV_MMA8451.h"
#include "debug.h"
#include "datastructure.h"
#include "Control.h"
#include "CCD.h"
#include "Communicate.h"
#include "oled.h"
#include "L3G4200.h"
#include "key.h"
#define ABS(x)  ( (x)>0?(x):-(x) ) 
extern float GravityAngle; //重力角
extern short TimeFlag_5Ms, TimeFlag_40Ms, TimeFlag_20Ms, TimeFlag_2Ms;
extern unsigned char CCDReady;
extern unsigned char CCDTimeMs;//时间片标志
extern uint8 debugerConnected; //是否连接到调试器
extern TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
extern float AngleIntegraed;//对角速度积分的角度值
extern uint8 IntegrationTime;
extern float SpeedSet_Variable;
extern float IntSum;
extern float Dir_AngSpeed;
extern short SpeedControlPeriod, DirectionConrtolPeriod;
extern short Screen_WhichCCDImg;

extern void Beep_Isr(void);
extern void AngleIntegration(float Anglespeed);
extern float GravityAngle, GyroscopeAngleSpeed;





#endif // !_MYCAR_H_
