#ifndef _CCD_H_
#define _CCD_H_
#include "common.h"
#include "datastructure.h"
#define CCD_DataLen 50
extern unsigned char CCDM_Arr[128]; //�ⲿ���õ�ʱ����
extern unsigned char CCDS_Arr[128];
extern unsigned char CCDReady ;
extern uint16 send_data;
extern unsigned char CCDTimeMs;





// typedef struct
// {
// 	char LeftLineArr[CCD_DataLen]; //��������
// 	char RightLineArr[CCD_DataLen]; //��������
// 	char MidpointArr[CCD_DataLen]; //�����������������
// 	char SearchBegin;//һ��ʼ��Ѱ�����,Ĭ��64
// 	char LostLeftLine;// �����߱�־
// 	char LostRightLine; //�����߱�־
// 	char LostLine;//����
// 	unsigned char PointCounter; //��ǰ������ֵ
// 	char LastMidpoint;  //�ϴε��е�
// 	char ThisMidpoint; //���ε��е�
// 	char ThisLeftLine; //��������λ��
// 	char ThisRightLine; //��������λ��
// 	int LandingMode;// ������ʽ,-1Ϊ��������,0Ϊ��������,1Ϊ��������;
// 	int PointDiff;//���ֵ
// 	char MidSet;  //���߳�ʼ����
// 	char LeftSet;  //���߳�ʼ����
// 	char RightSet;//���߳�ʼ����
// 	char InitOK;//�Ƿ�����˳�ʼ��������
// 	char LetfLineDiff;
// 	char RightLineDiff; //�����ߵľ���,���л�������������ʱ����Ҫ�õ���
// 	unsigned short LandingModeCounter;//ÿ������ģʽ���ٳ���5�����ڲ��л�
// 	char ControlValue;//����ֵ,������ΪPID������
// 	unsigned int ErrorCnt;//�������
// }CCD_Status_Struct;
typedef struct 
{
	short LeftLineArr[100];
	short RightLineArr[100];
	short SearchBegin;
	short LeftPoint;
	short RightPoint;
	short LastLeftPoint;
	short LastRightPoint;
	signed char  MidPoint;
	signed char  MidSet;
	signed char  LeftSet;
	signed char  RightSet;
	short InitOK;
	float ControlValue;
	short SearchMode;
	uint32 PointCnt;
	signed char  Left_LostFlag;
	signed char  Right_LostFlag;
	short Threshold_Left;
	short Threshold_Right;
	short LineWidth;
	signed char  Flag_CenterLine;
}CCD_SLave_Status_Struct;


typedef struct
{
	short LeftLineArr[100];
	short RightLineArr[100];
	short SearchBegin;
	short LeftPoint;
	short RightPoint;
	short LastLeftPoint;
	short LastRightPoint;
	signed char  MidPoint;
	signed char  MidSet;
	signed char  LeftSet;
	signed char  RightSet;
	signed char  InitOK;
	float ControlValue;
	short SearchMode;
	uint32 PointCnt;
	signed char  Left_LostFlag;
	signed char  Right_LostFlag;
	short Threshold_Left;
	short Threshold_Right;
	short LineWidth;
	signed char  Flag_CenterLine;
}CCD_Status_Struct;
extern CCD_Status_Struct CCDMain_Status;
extern CCD_SLave_Status_Struct CCDSlave_Status;
extern DirPID_TypeDef Dir_PID;
void CCDLineInit(void);
void CCD_GetLine_Slave(void);
void CCD_GetLine_Main(void);

void ImageCapture_M(unsigned char * ImageData, unsigned char * ImageData2);
void ImageCapture_S(unsigned char * ImageData);
void StartIntegration_M(void);
void StartIntegration_S(void);
void ccd_exposure(void);
void CalculateIntegrationTime(void);
unsigned char PixelAverage(unsigned char len, unsigned char *data);
void SendImageData(unsigned char *ImageData);
void SendHex(unsigned char hex);
void CCD_ControlValueCale(void);
void CCD_GetLine(void);
void CCD_Deal_Both(void);
void CCD_Median_Filtering(void);
#endif






// char CCD_Deal_Main(unsigned char *CCDArr) //ѭ������ҪCCD,����һ��Զհ��������д
// {
// 	unsigned char i, j;
// 	short SumTemp1_short = 0, SumTemp2_short = 0;
// 	//	short Temp1_short,Temp2_short;
// 	char CCD_TempLeft[50];//ÿһ�δ�����ʱ�õ�
// 	char CCD_TempRight[50];//һ��Ѱ����ʱ�õ�
// 	char Counter_L = 0;//��ߵ�ļ���
// 	char Counter_R = 0;//�ұ�
// 	char Counter_Low = 0;//��ʱ����,�ű������ص͵ĵ�ĸ���
// 	short SearhBeginTemp = 0;
// 	//unsigned char CCD_Point_Max = 0, CCD_Point_Min = 0;//��չ8�����е����ֵ����Сֵ
// 	if (CCDMain_Status.InitOK == 0)
// 	{
// 		CCDMain_Status.SearchBegin = 64;//��ʼ����ʼ�е�,�����Ŀ��Բ���ʼ��
// 		CCDMain_Status.LandingMode = 0;//��ʼ״̬����������
// 	}
// 	//	unsigned char CCD_Max_i,CCD_Min_i;//�����Сֵ��λ��
// 	if (CCDMain_Status.PointCounter > (CCD_DataLen - 1))
// 		CCDMain_Status.PointCounter = 0;
// 
// 	for (i = CCDMain_Status.SearchBegin; i < RightBoundary; i++)
// 	{
// 		SumTemp1_short = CCDArr[i] + CCDArr[i + 1];
// 		SumTemp2_short = CCDArr[i + 4] + CCDArr[i + 5];//110011�����
// 		if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//��ʾ�½����ر��
// 		{
// 			CCD_TempRight[Counter_R] = i + 2;
// 			Counter_R++;
// 		}
// 	}
// 
// 
// 	for (i = CCDMain_Status.SearchBegin; i > LeftBoundary; i--)
// 	{
// 		SumTemp1_short = CCDArr[i] + CCDArr[i - 1];
// 		SumTemp2_short = CCDArr[i - 4] + CCDArr[i - 5];
// 		if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//��ʾ�½����ر��
// 		{
// 			CCD_TempLeft[Counter_L] = i - 2; //Ϊ�����뷨,��Ҫ�ж�������Ƿ�Ϊ����
// 			Counter_L++;
// 		}  //�ҵ�һ�������...��������һ�ԵñȽ�..���м������߱Ƚϵó��½���,��λ�÷ų���ʱ��������,�Ժ����� ����;
// 	}
// 	//�����ж��ҵ���������Ƿ�������ߵ�Ҫ��
// 	//��ߵĺ���,������չ8����,�ҵ�8�����е���ߵ����͵�,�ж������غ���ߵ�֮��Ĳ���Ƿ�������߿��
// 	//����,�����ص���͵㼸����ĺ�Ӧ�ñ��������ұߵ�N����ĺ�СN*��ֵ��,�����Break��,�ϰ����,,�Ժ��ٴ���,С����
// 	CCDMain_Status.LostLeftLine = 1;
// 	CCDMain_Status.LostRightLine = 1;//ÿ�δ����ʱ���ȶ���
// 	CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] = 0;
// 	CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] = 127;
// 	Counter_Low = 0;
// 	for (i = 0; i < Counter_L; i++)
// 	{
// 		/*for(j=0;j<4;j++)
// 		{
// 		if(CCDArr[CCD_TempLeft[i]]>CCDArr[CCD_TempLeft[i]-j-1]);
// 		Counter_Low++;
// 		}
// 		if(Counter_Low>=2)*/
// 		//for(j=0;j<8;j++)
// 		//{
// 		//	if(CCD_Point_Max<CCDArr[CCD_TempLeft[i]-j])
// 		//		CCD_Point_Max=CCDArr[CCD_TempLeft[i]-j];
// 		//	if(CCD_Point_Min>CCDArr[CCD_TempLeft[i]-j])
// 		//		CCD_Point_Min=CCDArr[CCD_TempLeft[i]-j];
// 		//}
// 		//for(j=0;j<8;j++)
// 		//{
// 		//	if(CCD_Point_Max==CCDArr[CCD_TempLeft[i]-j])
// 		//		CCD_Max_i=CCD_TempLeft[i]-j;
// 		//	if(CCD_Point_Min==CCDArr[CCD_TempLeft[i]-j])
// 		//		CCD_Min_i=CCD_TempLeft[i]-j;
// 		//}
// 		//if((CCD_TempLeft-CCD_Max_i)>4)//���߿������4
// 		//�е�����..������:���ʼ��ʱ������ҵ�һ������������Ҫ��,�� �����ص�����ֵ֮�ͳ������ڽ�������չ4����ĺ�,��˵�������ؼ����ȷ
// 		//�����ж�������Ƿ�Ϊ����--�жϸõ��Ƿ�����Ա�����̫��..
// 		//��������ֵ�˲���
// 		//�����ж��ҵ��ĵ��Ƿ�Ϊ����
// 		if ((CCDArr[CCD_TempLeft[i] + 1] - ((CCDArr[CCD_TempLeft[i] + 2] + CCDArr[CCD_TempLeft[i] + 3]) / 2)) < (CCD_Threshold / 2))//�����������һ����Ǹ���С�ں��������ƽ����ֵ,˵������,�ż���
// 			if ((CCDArr[CCD_TempLeft[i]] - ((CCDArr[CCD_TempLeft[i] - 1] + CCDArr[CCD_TempLeft[i] + 1]) / 2)) < (CCD_Threshold / 2))//����������������������������ƽ��ֵ��ֵ,��Ϊ����
// 			{
// 				//�ж��Ƿ�������߿��
// 				SumTemp1_short = CCDArr[CCD_TempLeft[i]] + CCDArr[CCD_TempLeft[i] + 1];
// 				for (j = 1; j<8; j++)
// 				{
// 					SumTemp2_short = CCDArr[CCD_TempLeft[i] - j] + CCDArr[CCD_TempLeft[i] - j - 1];//�������ǰ��������֮��
// 					if (SumTemp1_short - SumTemp2_short>CCD_Threshold)
// 						Counter_Low++;
// 				}
// 				if (Counter_Low >= 4)
// 				{
// 					
// // 					if (CCD_TempLeft[i] - CCDMain_Status.ThisLeftLine <15 || CCD_TempLeft[i] - CCDMain_Status.ThisLeftLine>-15)
// // 					{//��Ч���ж�..����һ�������ڲ���ͻ��15�����ص�
// // 						CCDMain_Status.LostLeftLine = 0;
// // 						break;//�ҵ����߾��˳�
// // 					}
// 				/*	if (CCDMain_Status.LandingMode==0)
// 					{ 
// 						if (CCD_TempLeft[i] - CCDMain_Status.ThisLeftLine <15 || CCD_TempLeft[i] - CCDMain_Status.ThisLeftLine>-15)
// 						{//��Ч���ж�..����һ�������ڲ���ͻ��15�����ص�
// 							CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] = CCD_TempLeft[i];
// 							CCDMain_Status.LostLeftLine = 0;
// 							break;//�ҵ����߾��˳�
// 						}
// 					}
// 					else if (CCDMain_Status.LandingMode == 1)
// 					{
// 						if (CCD_TempLeft[i] >( LeftBoundary +5))//���ߺ�,���߱������߽�+5�ĵط�����
// 						{
// 							CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] = CCD_TempLeft[i];
// 							CCDMain_Status.LostLeftLine = 0;
// 							break;//�ҵ����߾��˳�
// 						}
// 					}
// 					else
// 					{
// 						//����,��Ϊ�������������ֳ���һ������,˵����ǰ���д���
// 					}
// 					//�Ժ���Ҫ�ж�����һ�κ���λ��������ֵ,������̫��Ͳ���*/
// 					CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] = CCD_TempLeft[i];
// 					CCDMain_Status.LostLeftLine = 0;
// 					break;//�ҵ����߾��˳�
// 				}
// 			}
// 	}
// // 	if (CCDMain_Status.LostLeftLine == 1 && CCDMain_Status.LandingMode == 0)
// // 	{
// // 		CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] = CCDMain_Status.ThisLeftLine;
// // 	}//���
// 	Counter_Low = 0;
// 	for (i = 0; i <= Counter_R; i++)
// 	{
// 		if ((CCDArr[CCD_TempRight[i] - 1] - ((CCDArr[CCD_TempRight[i] - 2] + CCDArr[CCD_TempRight[i] - 3]) / 2)) < (CCD_Threshold / 2))//�����������һ����Ǹ���С�ں��������ƽ����ֵ,˵������,�ż���
// 			if ((CCDArr[CCD_TempRight[i]] - ((CCDArr[CCD_TempRight[i] - 1] + CCDArr[CCD_TempRight[i] + 1]) / 2)) < (CCD_Threshold / 2))//����������������������������ƽ��ֵ��ֵ,��Ϊ����
// 			{
// 				//�ж��Ƿ�������߿��
// 				SumTemp1_short = CCDArr[CCD_TempRight[i]] + CCDArr[CCD_TempRight[i] - 1];
// 				for (j = 1; j<8; j++)
// 				{
// 					SumTemp2_short = CCDArr[CCD_TempRight[i] + j] + CCDArr[CCD_TempRight[i] + j + 1];//�������ǰ��������֮��
// 					if (SumTemp1_short - SumTemp2_short>CCD_Threshold)
// 						Counter_Low++;
// 				}
// 				if (Counter_Low >= 4)
// 				{
// // 					CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] = CCD_TempRight[i];
// // 					CCDMain_Status.LostRightLine = 0;
// // 					break;//�ҵ����߾��˳�
// 				/*	if (CCDMain_Status.LandingMode == 0)
// 					{
// 						if ((CCD_TempRight[i] - CCDMain_Status.ThisRightLine <15) || (CCD_TempRight[i] - CCDMain_Status.ThisRightLine>-15))
// 						{
// 							CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] = CCD_TempRight[i];
// 							CCDMain_Status.LostRightLine = 0;
// 							break;//�ҵ����߾��˳�
// 						}
// 					}
// 					//�Ժ���Ҫ�ж�����һ�κ���λ��������ֵ,������̫��Ͳ���
// 					else if (CCDMain_Status.LandingMode == -1)
// 					{
// 						if (CCD_TempRight[i] < (RightBoundary -5))
// 						{
// 							CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] = CCD_TempRight[i];
// 							CCDMain_Status.LostRightLine = 0;
// 							break;//�ҵ����߾��˳�
// 						}
// 					}
// 					else
// 					{
// 						CCDMain_Status.ErrorCnt++;
// 					}*/
// 					CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] = CCD_TempRight[i];
// 					CCDMain_Status.LostRightLine = 0;
// 					break;//�ҵ����߾��˳�
// 				}
// 			}
// 	}
// 	if (CCDMain_Status.LostLeftLine == 1 && CCDMain_Status.LostRightLine == 1)
// 		CCDMain_Status.LostLine = 1;
// 	else
// 		CCDMain_Status.LostLine = 0;
// 	if (CCDMain_Status.LostLine != 1)
// 	{
// 		//�����Ƕ���ǰ��׼������
// 
// 
// 		//�����Ƕ���ǰ��׼������
// // 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] > RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] > LeftLostPrepare))//Ҫ׼��������
// // 			if (CCDMain_Status.LandingModeCounter >= 5 && (CCDMain_Status.LandingMode != -1))//����Ҫ��һ������ģʽ��5������
// // 			{//�ұ�׼������//׼���������߲��ֵ
// // 				CCDMain_Status.PointDiff = 0;
// // 				i = CCDMain_Status.PointCounter;
// // 				j = 0;
// // 				while (1)
// // 				{
// // 					// 			if(i == 1)
// // 					// 			{
// // 					// 				CCDMain_Status.PointDiff +=(CCDMain_Status.RightLineArr[i]-CCDMain_Status.RightLineArr[i-1]);
// // 					// 				i--;
// // 					// 				j++;
// // 					// 				if(j>=20)
// // 					// 					break;
// // 					// 			}
// // 					if (i == 0)
// // 					{
// // 						CCDMain_Status.PointDiff += (CCDMain_Status.LeftLineArr[i] - CCDMain_Status.LeftLineArr[CCD_DataLen - 1]);
// // 						i = CCD_DataLen - 1;
// // 						j++;
// // 						if (j >= 20)
// // 							break;
// // 					}
// // 					else
// // 					{
// // 						CCDMain_Status.PointDiff += (CCDMain_Status.LeftLineArr[i] - CCDMain_Status.LeftLineArr[i - 1]);
// // 						i--;
// // 						j++;
// // 						if (j >= 20)
// // 							break; //ֻ��20�ε�������������ֵ
// // 					}
// // 				}
// // 				if (CCDMain_Status.PointDiff >= LostLineDiffValue)
// // 				{//�ڶ���ǰ�л�Ϊ ��������
// // 					CCDMain_Status.LandingMode = -1;
// // 					CCDMain_Status.LandingModeCounter = 0;
// // 				}
// // 			}
// // 
// // 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] < RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] < LeftLostPrepare))
// // 			if (CCDMain_Status.LandingModeCounter >= 5 && (CCDMain_Status.LandingMode != 1))
// // 			{//���߳�����ֵ,׼���������߲��ֵ
// // 				CCDMain_Status.PointDiff = 0;
// // 				i = CCDMain_Status.PointCounter;
// // 				j = 0;
// // 				while (1)
// // 				{
// // 					if (i == 0)
// // 					{
// // 						CCDMain_Status.PointDiff += (CCDMain_Status.RightLineArr[i] - CCDMain_Status.RightLineArr[CCD_DataLen - 1]);
// // 						i = CCD_DataLen - 1;
// // 						j++;
// // 						if (j >= 20)
// // 							break;
// // 					}
// // 					else
// // 					{
// // 						CCDMain_Status.PointDiff += (CCDMain_Status.RightLineArr[i] - CCDMain_Status.RightLineArr[i - 1]);
// // 						i--;
// // 						j++;
// // 						if (j >= 20)
// // 							break; //ֻ��20�ε�������������ֵ
// // 					}
// // 				}
// // 				if (CCDMain_Status.PointDiff >= LostLineDiffValue)
// // 				{//�ڶ���ǰ�л�Ϊ ��������
// // 					CCDMain_Status.LandingMode = 1;
// // 					CCDMain_Status.LandingModeCounter = 0;
// // 				}
// // 			}
// 		//������߶��ڶ���׼������ֵ��,���л�Ϊ��������,���������Ƚ��ȶ�
// 		// 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] > RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] > LeftLostPrepare))//Ҫ׼��������
// // 			if (CCDMain_Status.LandingModeCounter >= 5 && (CCDMain_Status.LandingMode != -1))//����Ҫ��һ������ģʽ��5������
// // 			{//�ұ�׼������//׼���������߲��ֵ
// // 				CCDMain_Status.PointDiff = 0;
// // 				i = CCDMain_Status.PointCounter;
// // 				j = 0;
// // 				while (1)
// // 				{
// // 					// 			if(i == 1)
// // 					// 			{
// // 					// 				CCDMain_Status.PointDiff +=(CCDMain_Status.RightLineArr[i]-CCDMain_Status.RightLineArr[i-1]);
// // 					// 				i--;
// // 					// 				j++;
// // 					// 				if(j>=20)
// // 					// 					break;
// // 					// 			}
// // 					if (i == 0)
// // 					{
// // 						CCDMain_Status.PointDiff += (CCDMain_Status.LeftLineArr[i] - CCDMain_Status.LeftLineArr[CCD_DataLen - 1]);
// // 						i = CCD_DataLen - 1;
// // 						j++;
// // 						if (j >= 20)
// // 							break;
// // 					}
// // 					else
// // 					{
// // 						CCDMain_Status.PointDiff += (CCDMain_Status.LeftLineArr[i] - CCDMain_Status.LeftLineArr[i - 1]);
// // 						i--;
// // 						j++;
// // 						if (j >= 20)
// // 							break; //ֻ��20�ε�������������ֵ
// // 					}
// // 				}
// // 				if (CCDMain_Status.PointDiff >= LostLineDiffValue)
// // 				{//�ڶ���ǰ�л�Ϊ ��������
// // 					CCDMain_Status.LandingMode = -1;
// // 					CCDMain_Status.LandingModeCounter = 0;
// // 				}
// // 			}
// // 
// // 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] < RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] < LeftLostPrepare))
// // 			if (CCDMain_Status.LandingModeCounter >= 5 && (CCDMain_Status.LandingMode != 1))
// 
// //
// 
// 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] > RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] > LeftLostPrepare))//Ҫ׼��������
// 			if ((CCDMain_Status.LandingMode != -1))
// 			{
// 				CCDMain_Status.LandingMode = -1;
// 			}
// 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] < RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] < LeftLostPrepare))
// 			if ( (CCDMain_Status.LandingMode != 1))
// 			{
// 				CCDMain_Status.LandingMode = 1;
// 			}
// 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] < RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter]>LeftLostPrepare))
// 			if (CCDMain_Status.LandingMode != 0 ) //��������߶�����ֵ��Χ��,�����л�������������
// 			{
// 				CCDMain_Status.LandingMode = 0;
// 			}
// 
// 		//�����Ǹ�������ģʽ��ѡ��
// 		//��ģʽ0
// 
// 		//�����Ǹ�������ģʽ��ѡ��
// 		//���洦���ߵ����
// 		if (CCDMain_Status.LandingMode == 0)
// 		{//���Ϊ����������,�����´����������е�����
// 			SearhBeginTemp = (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] + CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter]) / 2;
// 			if (((CCDMain_Status.SearchBegin - SearhBeginTemp) < 20) &&((CCDMain_Status.SearchBegin - SearhBeginTemp) >- 20))
// 			{
// 				CCDMain_Status.SearchBegin = SearhBeginTemp;
// 				CCDMain_Status.MidpointArr[CCDMain_Status.PointCounter] = CCDMain_Status.SearchBegin;
// 				//��ͣ������,��Ϊ�漰��˫CCD��ϵ�����//��δ����
// 				CCDMain_Status.ControlValue = CCDMain_Status.MidSet - CCDMain_Status.MidpointArr[CCDMain_Status.PointCounter];
// 				//��򵥵�����������߶�����
// 				CCDMain_Status.ThisMidpoint = CCDMain_Status.MidpointArr[CCDMain_Status.PointCounter];
// 			}
// 			else
// 			{
// 				CCDMain_Status.ErrorCnt++;
// 			}
// 		}
// 		else if (CCDMain_Status.LandingMode == -1)
// 		{
// 			if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] ) >= (LeftBoundary + 5))
// 				SearhBeginTemp = (CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] + 5);
// 			else
// 				SearhBeginTemp = (LeftBoundary - 8);
// 			if (((CCDMain_Status.SearchBegin - SearhBeginTemp) < 20) && ((CCDMain_Status.SearchBegin - SearhBeginTemp) >-20))
// 			{
// 				CCDMain_Status.SearchBegin = SearhBeginTemp;
// 				CCDMain_Status.ControlValue = CCDMain_Status.RightSet - CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter];
// 				CCDMain_Status.ThisMidpoint = 0;
// 			}
// 		}
// 		else if (CCDMain_Status.LandingMode == 1)
// 		{
// 			if ((CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter]) <= (RightBoundary - 5))
// 				SearhBeginTemp = CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] + 5;
// 			else
// 				SearhBeginTemp = RightBoundary + 8;
// 			if (((CCDMain_Status.SearchBegin - SearhBeginTemp) < 20) && ((CCDMain_Status.SearchBegin - SearhBeginTemp) >-20))
// 			{
// 				CCDMain_Status.SearchBegin = SearhBeginTemp;
// 				CCDMain_Status.ControlValue = CCDMain_Status.LeftSet - CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter];
// 				CCDMain_Status.ThisMidpoint = 0;
// 			}
// 		}
// 		//������ȫ����ֵ
// 		//���߸�ֵ��Ҫע��
// 		CCDMain_Status.ThisRightLine = CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter];
// 		CCDMain_Status.ThisLeftLine = CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter];
// 		//CCDMain_Status.ThisMidpoint = CCDMain_Status.MidpointArr[CCDMain_Status.PointCounter];
// 	}
// 	//��������й�����Ҫ�����ҵ��ߵ�����²Ž���
// 	else
// 	{
// // 		CCDMain_Status.ThisRightLine = 0;
// // 		CCDMain_Status.ThisLeftLine = 0;
// // 		CCDMain_Status.ThisMidpoint = 0;
// // 		CCDMain_Status.ControlValue = 0;
// 		//CCDMain_Status.SearchBegin = CCDMain_Status.MidSet;
// 		//�������,ʲô������,���δ����˳�
// 		if (CCDMain_Status.PointCounter > 0)
// 			CCDMain_Status.PointCounter--;
// 		else
// 			CCDMain_Status.PointCounter = CCD_DataLen - 1;
// 
// 	}
// 
// 	if (CCDMain_Status.InitOK == 0)
// 	{
// 		i = (CCDMain_Status.ThisRightLine - CCDMain_Status.ThisLeftLine) / 2;//�����������ߵĿ��
// 		CCDMain_Status.LeftSet = 64 - i;
// 		CCDMain_Status.RightSet = 64 + i;
// 		CCDMain_Status.MidSet = 64;
// 		CCDMain_Status.InitOK = 1; //���CCDû�н��г�ʼ��,��ô�ͽ��ҵ�����������ƽ�ֵ�CCD�ӳ��ȷֵĵ�
// 	}
// 	CCDMain_Status.PointCounter++;
// 	return 0;
// }
