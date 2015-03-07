现在已经使用的模块	FTM0-控制电机
					PIT3-用来计算程序执行时间
					PIT0-作为程序时间片控制
					ADC1-采集陀螺仪的数据 
					IIC0-MMA8451
					ADC0-CCD

占用的端口:	USART5 	PTE8-PTE9;
			FTM0   	PTD4,PTD5,PTD6,PTD7;
			LED		PTA17;
			ADC1	PTB10,PTB11
			IIC0	PTD8,PTD9;
			ADC0	PTB0//CCD的AD
			CCD		PTA28-CCD的SI,PTA29-CCD的CLK
			
			
需要调试的参数:	Debug.c:		Ang_PID.Proportion	//比例
								Ang_PID.Integral 	//积分
								Ang_PID.Derivative	//微分
								Ang_PID.AngSet		//调试时调试这一行
								Ang_PID.AngSpeedSet	//角速度的设定值
				AngGet.c		#define AngToMotorRatio 300  //角度转换成电机控制的比例因子..我也不知道取多少合适..以后再调试

		



使用I2C的DMA来传送数据..因为I2C消耗的时间大概在1.2ms

通信流程:
调试器发送	0xFE 
k60应答		0xEF
调试器发送	0xFD
应答			0xDF

调试器开始尝试读取Flash	{ 0xf1, 0x04, ReadFlash, 0xf2 }
应答						{ 0xf1,	0x04, K60StarSendFlash,0xf2}
调试器等待接受数据
K60发送					{ 0xf1,	0x08, 数据类型字,xx,xx,xx,xx,0xf2}
调试器应答				0xfc
发送完成					{ 0xf1,	0x04,K60SendDataComplete,0xf2}
调试器不断用	0xfa请求发送数据,如果调试器没有应答0xfc则表示数据接受失败,

更新为:调试器请求标号为 0xXX的数据,回复只发送数据,调试器接受到数据后不应答,只检查是否完整接受到数据

2014年11月3日 21:40:59  更新了流程控制和IIC的速度以后..IIC读取加速度值大概在0.5Ms左右.总共计算加上滤波 0.8Ms,加上发送数据到串口需要1.7Ms

2014年11月5日 21:38:03  写一个头文件定义所有的数据结构和Extern..暂时定为DataStructure.h

2014年11月6日 21:50:07  因为发送数据的时间太长(三个轴的数据发送需要1ms),暂时决定使用DMA来发送串口数据

2014年11月9日 15:15:11  DMA配置不成功,发送时间需要1MS,直接发送,

2014年11月11日 13:05:16	改用了卡尔曼滤波,角速度收敛有点慢

2014年11月11日 23:37:26	更新了CCD的代码,暂时没有写黑线识别的代码,角度控制增量PID需要改

2014年11月12日 17:03:23	修改了角度增量PID的代码,

2014年11月15日 17:25:55	准备增加与调试器的通信协议,

2014年11月15日 21:10:41	通信协议保存在 /app/通信协议.xlsx,未完成,增加了Flash读写代码,将PID和Angle_Set等参数写入Flash,开机的时候调试,
						目前暂时决定在调试器开机的时候所有数据为0,通过按键与车子通信,车子发回所有的参数,调试器再载入参数
						卡尔曼滤波跟随出现问题,只有到车上实际调试了.
					   