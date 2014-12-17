#ifndef _COMMUNICATE_H_
#define _COMMUNICATE_H_

#define ReadFlash 0xb0	//��ȡ���ݵ�����������ʼ
#define SaveToFlash 0xb1  //�洢���ݵ�Flash����ʼ
#define AnswerToOther 0xb2  //˫���������ݽ��ܵ�Ӧ��
#define FrameStart	0xf1 //֡��ʼ
#define FrameEnd		0xf2 //֡����
#define K60StarSendFlash	0xb5 //K60��ʼ����FLash����
#define K60SendDataComplete 0xb3  //K60����������
//#define DataEnd		0xf2

#define DATANumMAX 0x15  //�����������

#define Data_AngSet 0x10
#define Data_AngPID_P 0x11
#define Data_AngPID_D 0x12
#define Data_AngToMotor_Ratio 0x13
#define Data_AngNow 0x14 //��һҳ������







#endif
