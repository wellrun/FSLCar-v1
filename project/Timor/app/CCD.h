#ifndef _CCD_H_
#define _CCD_H_
#include "common.h"
#include "datastructure.h"
#define CCD_DataLen 50
extern unsigned char CCDM_Arr[128]; //外部调用的时候用
extern unsigned char CCDS_Arr[128];
extern unsigned char CCDReady ;
extern uint16 send_data;
extern unsigned char CCDTimeMs;





// typedef struct
// {
// 	char LeftLineArr[CCD_DataLen]; //左线数组
// 	char RightLineArr[CCD_DataLen]; //右线数组
// 	char MidpointArr[CCD_DataLen]; //计算出来的中线数组
// 	char SearchBegin;//一开始的寻找起点,默认64
// 	char LostLeftLine;// 丢左线标志
// 	char LostRightLine; //丢右线标志
// 	char LostLine;//出界
// 	unsigned char PointCounter; //当前计数的值
// 	char LastMidpoint;  //上次的中点
// 	char ThisMidpoint; //本次的中点
// 	char ThisLeftLine; //本次左线位置
// 	char ThisRightLine; //本次右线位置
// 	int LandingMode;// 引导方式,-1为左线引导,0为中线引导,1为右线引导;
// 	int PointDiff;//差分值
// 	char MidSet;  //中线初始化点
// 	char LeftSet;  //左线初始化点
// 	char RightSet;//右线初始化点
// 	char InitOK;//是否进行了初始化左右线
// 	char LetfLineDiff;
// 	char RightLineDiff; //到右线的距离,在切换到边线引导的时候需要用到的
// 	unsigned short LandingModeCounter;//每种引导模式至少持续5个周期才切换
// 	char ControlValue;//控制值,用来作为PID的输入
// 	unsigned int ErrorCnt;//错误计数
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






// char CCD_Deal_Main(unsigned char *CCDArr) //循迹的主要CCD,还有一个远瞻的另外再写
// {
// 	unsigned char i, j;
// 	short SumTemp1_short = 0, SumTemp2_short = 0;
// 	//	short Temp1_short,Temp2_short;
// 	char CCD_TempLeft[50];//每一次处理临时用的
// 	char CCD_TempRight[50];//一次寻找临时用的
// 	char Counter_L = 0;//左边点的计数
// 	char Counter_R = 0;//右边
// 	char Counter_Low = 0;//临时变量,放比跳变沿低的点的个数
// 	short SearhBeginTemp = 0;
// 	//unsigned char CCD_Point_Max = 0, CCD_Point_Min = 0;//扩展8个点中的最大值和最小值
// 	if (CCDMain_Status.InitOK == 0)
// 	{
// 		CCDMain_Status.SearchBegin = 64;//初始化起始中点,其他的可以不初始化
// 		CCDMain_Status.LandingMode = 0;//初始状态用中线引导
// 	}
// 	//	unsigned char CCD_Max_i,CCD_Min_i;//最大最小值的位置
// 	if (CCDMain_Status.PointCounter > (CCD_DataLen - 1))
// 		CCDMain_Status.PointCounter = 0;
// 
// 	for (i = CCDMain_Status.SearchBegin; i < RightBoundary; i++)
// 	{
// 		SumTemp1_short = CCDArr[i] + CCDArr[i + 1];
// 		SumTemp2_short = CCDArr[i + 4] + CCDArr[i + 5];//110011来检测
// 		if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//表示下降的特别快
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
// 		if (SumTemp1_short - SumTemp2_short >= CCD_Threshold)//表示下降的特别快
// 		{
// 			CCD_TempLeft[Counter_L] = i - 2; //为了新想法,需要判断这个点是否为坏点
// 			Counter_L++;
// 		}  //找到一个跳变点...两个两个一对得比较..从中间向两边比较得出下降沿,将位置放出临时数组里面,以后再做 处理;
// 	}
// 	//下面判断找到的跳变点是否满足黑线的要求
// 	//左边的黑线,向外扩展8个点,找到8个点中的最高点和最低点,判断跳变沿和最高点之间的差距是否满足黑线宽度
// 	//并且,跳变沿到最低点几个点的和应该比跳变沿右边的N个点的和小N*阈值倍,满足就Break吧,障碍物的,,以后再处理,小问题
// 	CCDMain_Status.LostLeftLine = 1;
// 	CCDMain_Status.LostRightLine = 1;//每次处理的时候先丢线
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
// 		//if((CCD_TempLeft-CCD_Max_i)>4)//黑线宽度至少4
// 		//有点问题..现在用:在最开始的时候如果找到一个跳变沿满足要求,且 跳变沿的两个值之和持续大于接下来扩展4个点的和,则说明跳变沿检测正确
// 		//首先判断这个点是否为坏点--判断该点是否大于旁边两点太多..
// 		//先试试中值滤波吧
// 		//首先判断找到的点是否为坏点
// 		if ((CCDArr[CCD_TempLeft[i] + 1] - ((CCDArr[CCD_TempLeft[i] + 2] + CCDArr[CCD_TempLeft[i] + 3]) / 2)) < (CCD_Threshold / 2))//如果和跳变沿一组的那个点小于后两个点的平均阈值,说明正常,才继续
// 			if ((CCDArr[CCD_TempLeft[i]] - ((CCDArr[CCD_TempLeft[i] - 1] + CCDArr[CCD_TempLeft[i] + 1]) / 2)) < (CCD_Threshold / 2))//如果跳变沿这个点大于两边两个点的平均值阈值,则为坏点
// 			{
// 				//判断是否满足黑线宽度
// 				SumTemp1_short = CCDArr[CCD_TempLeft[i]] + CCDArr[CCD_TempLeft[i] + 1];
// 				for (j = 1; j<8; j++)
// 				{
// 					SumTemp2_short = CCDArr[CCD_TempLeft[i] - j] + CCDArr[CCD_TempLeft[i] - j - 1];//如果大于前面两个点之和
// 					if (SumTemp1_short - SumTemp2_short>CCD_Threshold)
// 						Counter_Low++;
// 				}
// 				if (Counter_Low >= 4)
// 				{
// 					
// // 					if (CCD_TempLeft[i] - CCDMain_Status.ThisLeftLine <15 || CCD_TempLeft[i] - CCDMain_Status.ThisLeftLine>-15)
// // 					{//有效性判断..线在一个周期内不会突变15个像素点
// // 						CCDMain_Status.LostLeftLine = 0;
// // 						break;//找到黑线就退出
// // 					}
// 				/*	if (CCDMain_Status.LandingMode==0)
// 					{ 
// 						if (CCD_TempLeft[i] - CCDMain_Status.ThisLeftLine <15 || CCD_TempLeft[i] - CCDMain_Status.ThisLeftLine>-15)
// 						{//有效性判断..线在一个周期内不会突变15个像素点
// 							CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] = CCD_TempLeft[i];
// 							CCDMain_Status.LostLeftLine = 0;
// 							break;//找到黑线就退出
// 						}
// 					}
// 					else if (CCDMain_Status.LandingMode == 1)
// 					{
// 						if (CCD_TempLeft[i] >( LeftBoundary +5))//丢线后,左线必须从左边界+5的地方出现
// 						{
// 							CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] = CCD_TempLeft[i];
// 							CCDMain_Status.LostLeftLine = 0;
// 							break;//找到黑线就退出
// 						}
// 					}
// 					else
// 					{
// 						//错误,因为左线引导并且又出现一次左线,说明以前的有错误
// 					}
// 					//以后还需要判断与上一次黑线位置相差的阈值,如果相差太多就不算*/
// 					CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] = CCD_TempLeft[i];
// 					CCDMain_Status.LostLeftLine = 0;
// 					break;//找到黑线就退出
// 				}
// 			}
// 	}
// // 	if (CCDMain_Status.LostLeftLine == 1 && CCDMain_Status.LandingMode == 0)
// // 	{
// // 		CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] = CCDMain_Status.ThisLeftLine;
// // 	}//如果
// 	Counter_Low = 0;
// 	for (i = 0; i <= Counter_R; i++)
// 	{
// 		if ((CCDArr[CCD_TempRight[i] - 1] - ((CCDArr[CCD_TempRight[i] - 2] + CCDArr[CCD_TempRight[i] - 3]) / 2)) < (CCD_Threshold / 2))//如果和跳变沿一组的那个点小于后两个点的平均阈值,说明正常,才继续
// 			if ((CCDArr[CCD_TempRight[i]] - ((CCDArr[CCD_TempRight[i] - 1] + CCDArr[CCD_TempRight[i] + 1]) / 2)) < (CCD_Threshold / 2))//如果跳变沿这个点大于两边两个点的平均值阈值,则为坏点
// 			{
// 				//判断是否满足黑线宽度
// 				SumTemp1_short = CCDArr[CCD_TempRight[i]] + CCDArr[CCD_TempRight[i] - 1];
// 				for (j = 1; j<8; j++)
// 				{
// 					SumTemp2_short = CCDArr[CCD_TempRight[i] + j] + CCDArr[CCD_TempRight[i] + j + 1];//如果大于前面两个点之和
// 					if (SumTemp1_short - SumTemp2_short>CCD_Threshold)
// 						Counter_Low++;
// 				}
// 				if (Counter_Low >= 4)
// 				{
// // 					CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] = CCD_TempRight[i];
// // 					CCDMain_Status.LostRightLine = 0;
// // 					break;//找到黑线就退出
// 				/*	if (CCDMain_Status.LandingMode == 0)
// 					{
// 						if ((CCD_TempRight[i] - CCDMain_Status.ThisRightLine <15) || (CCD_TempRight[i] - CCDMain_Status.ThisRightLine>-15))
// 						{
// 							CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] = CCD_TempRight[i];
// 							CCDMain_Status.LostRightLine = 0;
// 							break;//找到黑线就退出
// 						}
// 					}
// 					//以后还需要判断与上一次黑线位置相差的阈值,如果相差太多就不算
// 					else if (CCDMain_Status.LandingMode == -1)
// 					{
// 						if (CCD_TempRight[i] < (RightBoundary -5))
// 						{
// 							CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] = CCD_TempRight[i];
// 							CCDMain_Status.LostRightLine = 0;
// 							break;//找到黑线就退出
// 						}
// 					}
// 					else
// 					{
// 						CCDMain_Status.ErrorCnt++;
// 					}*/
// 					CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] = CCD_TempRight[i];
// 					CCDMain_Status.LostRightLine = 0;
// 					break;//找到黑线就退出
// 				}
// 			}
// 	}
// 	if (CCDMain_Status.LostLeftLine == 1 && CCDMain_Status.LostRightLine == 1)
// 		CCDMain_Status.LostLine = 1;
// 	else
// 		CCDMain_Status.LostLine = 0;
// 	if (CCDMain_Status.LostLine != 1)
// 	{
// 		//下面是丢线前的准备工作
// 
// 
// 		//下面是丢线前的准备工作
// // 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] > RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] > LeftLostPrepare))//要准备丢线了
// // 			if (CCDMain_Status.LandingModeCounter >= 5 && (CCDMain_Status.LandingMode != -1))//至少要在一种引导模式下5个周期
// // 			{//右边准备丢线//准备计算左线差分值
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
// // 							break; //只用20次的数据来计算差分值
// // 					}
// // 				}
// // 				if (CCDMain_Status.PointDiff >= LostLineDiffValue)
// // 				{//在丢线前切换为 左线引导
// // 					CCDMain_Status.LandingMode = -1;
// // 					CCDMain_Status.LandingModeCounter = 0;
// // 				}
// // 			}
// // 
// // 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] < RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] < LeftLostPrepare))
// // 			if (CCDMain_Status.LandingModeCounter >= 5 && (CCDMain_Status.LandingMode != 1))
// // 			{//左线超过阈值,准备计算右线差分值
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
// // 							break; //只用20次的数据来计算差分值
// // 					}
// // 				}
// // 				if (CCDMain_Status.PointDiff >= LostLineDiffValue)
// // 				{//在丢线前切换为 左线引导
// // 					CCDMain_Status.LandingMode = 1;
// // 					CCDMain_Status.LandingModeCounter = 0;
// // 				}
// // 			}
// 		//如果两边都在丢线准备的阈值内,则切换为中线引导,中线引导比较稳定
// 		// 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] > RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] > LeftLostPrepare))//要准备丢线了
// // 			if (CCDMain_Status.LandingModeCounter >= 5 && (CCDMain_Status.LandingMode != -1))//至少要在一种引导模式下5个周期
// // 			{//右边准备丢线//准备计算左线差分值
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
// // 							break; //只用20次的数据来计算差分值
// // 					}
// // 				}
// // 				if (CCDMain_Status.PointDiff >= LostLineDiffValue)
// // 				{//在丢线前切换为 左线引导
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
// 		if ((CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter] > RightLostPrepare) && (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] > LeftLostPrepare))//要准备丢线了
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
// 			if (CCDMain_Status.LandingMode != 0 ) //如果两条线都在阈值范围内,立即切换到中心线引导
// 			{
// 				CCDMain_Status.LandingMode = 0;
// 			}
// 
// 		//下面是根据引导模式来选择
// 		//在模式0
// 
// 		//下面是根据引导模式来选择
// 		//下面处理丢线的情况
// 		if (CCDMain_Status.LandingMode == 0)
// 		{//如果为中心线引导,更新下次搜索起点和中点序列
// 			SearhBeginTemp = (CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter] + CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter]) / 2;
// 			if (((CCDMain_Status.SearchBegin - SearhBeginTemp) < 20) &&((CCDMain_Status.SearchBegin - SearhBeginTemp) >- 20))
// 			{
// 				CCDMain_Status.SearchBegin = SearhBeginTemp;
// 				CCDMain_Status.MidpointArr[CCDMain_Status.PointCounter] = CCDMain_Status.SearchBegin;
// 				//暂停在这里,因为涉及到双CCD拟合的问题//尚未处理
// 				CCDMain_Status.ControlValue = CCDMain_Status.MidSet - CCDMain_Status.MidpointArr[CCDMain_Status.PointCounter];
// 				//最简单的情况是两条线都存在
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
// 		//将变量全部赋值
// 		//中线赋值需要注意
// 		CCDMain_Status.ThisRightLine = CCDMain_Status.RightLineArr[CCDMain_Status.PointCounter];
// 		CCDMain_Status.ThisLeftLine = CCDMain_Status.LeftLineArr[CCDMain_Status.PointCounter];
// 		//CCDMain_Status.ThisMidpoint = CCDMain_Status.MidpointArr[CCDMain_Status.PointCounter];
// 	}
// 	//上面的所有工作是要求在找到线的情况下才进行
// 	else
// 	{
// // 		CCDMain_Status.ThisRightLine = 0;
// // 		CCDMain_Status.ThisLeftLine = 0;
// // 		CCDMain_Status.ThisMidpoint = 0;
// // 		CCDMain_Status.ControlValue = 0;
// 		//CCDMain_Status.SearchBegin = CCDMain_Status.MidSet;
// 		//如果丢线,什么都不做,本次处理退出
// 		if (CCDMain_Status.PointCounter > 0)
// 			CCDMain_Status.PointCounter--;
// 		else
// 			CCDMain_Status.PointCounter = CCD_DataLen - 1;
// 
// 	}
// 
// 	if (CCDMain_Status.InitOK == 0)
// 	{
// 		i = (CCDMain_Status.ThisRightLine - CCDMain_Status.ThisLeftLine) / 2;//计算两条黑线的宽度
// 		CCDMain_Status.LeftSet = 64 - i;
// 		CCDMain_Status.RightSet = 64 + i;
// 		CCDMain_Status.MidSet = 64;
// 		CCDMain_Status.InitOK = 1; //如果CCD没有进行初始化,那么就将找到的左线右线平分到CCD视场等分的点
// 	}
// 	CCDMain_Status.PointCounter++;
// 	return 0;
// }
