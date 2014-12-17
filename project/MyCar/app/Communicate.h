#ifndef _COMMUNICATE_H_
#define _COMMUNICATE_H_

#define ReadFlash 0xb0	//读取数据到调试器的起始
#define SaveToFlash 0xb1  //存储数据到Flash的起始
#define AnswerToOther 0xb2  //双方关于数据接受的应答
#define FrameStart	0xf1 //帧起始
#define FrameEnd		0xf2 //帧结束
#define K60StarSendFlash	0xb5 //K60开始发送FLash数据
#define K60SendDataComplete 0xb3  //K60发送完数据
//#define DataEnd		0xf2

#define DATANumMAX 0x15  //数据最大数量

#define Data_AngSet 0x10
#define Data_AngPID_P 0x11
#define Data_AngPID_D 0x12
#define Data_AngToMotor_Ratio 0x13
#define Data_AngNow 0x14 //第一页的数据







#endif
