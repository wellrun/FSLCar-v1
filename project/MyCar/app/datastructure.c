#include "datastructure.h"
#include "common.h"
#include "AngGet.h"

extern AngPID_InitTypeDef Ang_PID;

extern CarInfo_TypeDef CarInfo_Now; //��ǰ���ӵ���Ϣ
extern CarControl_TypeDef MotorControl; //������Ƶ�ֵ
extern AngPID_InitTypeDef Ang_PID; //�Ƕȿ��Ƶ�PID�ṹ��
extern TempOfMotor_TypeDef TempValue; //��ʱ�洢�ǶȺ��ٶȿ��Ƹ�������Ľṹ��
extern SpeedPID_TypeDef Speed_PID;
extern DirPID_TypeDef Dir_PID;
//extern float AngToMotorRatio;
// uint8 FlashReadBuffer[20]={0x01,0x01};
// uint8 FlashWriteBuffer[20]={0x01,0x01};
// float FlashFloatBuffer[5]={0x01,0x01};//��Ŷ�ȡ����������
// int FlashIntBuffer[5]={0x01,0x01};

 uint8 FlashReadBuffer[DATA_SIZE];
 uint8 FlashWriteBuffer[DATA_SIZE];
 float FlashFloatBuffer[NumOfFloat];
 int FlashIntBuffer[NumOfInt];


void delay();
uint8 Flash_ReadAllData(void)
{
	uint8 *ptr;
	uint8 len = 0;
	ptr = (uint8*) FLASH_ADDR;
	if (*ptr != 0x88)
	{
		return 1;//flash��������
	}
	ptr++;
	for (len = 0; len < DATA_SIZE; len++)
	{
		FlashReadBuffer[len] = *(ptr + len);
		FlashWriteBuffer[len] = FlashReadBuffer[len]; //�������������ݷŵ�д��buffer����,��ֹ��δ��FlashWriteBuffer��ʼ���������д��
	}
	for(len=0;len<NumOfFloat;len++)
	{
		Byte2Float(&FlashFloatBuffer[len],FlashReadBuffer,len*4); 
	}
	Ang_PID.Kp = FlashFloatBuffer[0];
	Ang_PID.Kd = FlashFloatBuffer[1];
	Ang_PID.AngSet = FlashFloatBuffer[2];

	Speed_PID.Kp = FlashFloatBuffer[3];
	Speed_PID.Ki = FlashFloatBuffer[4];
	Speed_PID.SpeedSet = FlashFloatBuffer[5];

	Dir_PID.Kp = FlashFloatBuffer[6];
	Dir_PID.Kd = FlashFloatBuffer[7];

	return 0;
}
void Flash_WriteAllData(void)
{
  uint8 result;
  uint8 *p = NULL;
    DisableInterrupts;
    result=LPLD_Flash_SectorErase(FLASH_ADDR);
    EnableInterrupts; //�Ȳ���
    delay(); 
	FlashFloatBuffer[0] = Ang_PID.Kp;
	FlashFloatBuffer[1] = Ang_PID.Kd;
	FlashFloatBuffer[2] = Ang_PID.AngSet;
	FlashFloatBuffer[3] = Speed_PID.Kp;
	FlashFloatBuffer[4] = Speed_PID.Ki;
	FlashFloatBuffer[5] = Speed_PID.SpeedSet;
	FlashFloatBuffer[6] = Dir_PID.Kp;
	FlashFloatBuffer[7] = Dir_PID.Kd;
    Flash_DataToBuffer(FlashFloatBuffer[0],0);
    Flash_DataToBuffer(FlashFloatBuffer[1],1);
    Flash_DataToBuffer(FlashFloatBuffer[2],2);
    Flash_DataToBuffer(FlashFloatBuffer[3],3);
	Flash_DataToBuffer(FlashFloatBuffer[4], 4);
	Flash_DataToBuffer(FlashFloatBuffer[5], 5);
	Flash_DataToBuffer(FlashFloatBuffer[6], 6);
	Flash_DataToBuffer(FlashFloatBuffer[7], 7);
	result = 0x88;
	p = &result;
	DisableInterrupts;
	result = LPLD_Flash_ByteProgram((uint32)FLASH_ADDR, (uint32*)p, 1);
    result=LPLD_Flash_ByteProgram((uint32)(FLASH_ADDR+1), (uint32*)FlashWriteBuffer, DATA_SIZE);
     delay();
    EnableInterrupts;
    delay();

    if(result==FLASH_OK)
    {
    	//LPLD_UART_PutChar(UART5,0xfb);
    }
    else
    {

    }
}
void Flash_DataToBuffer(float data,uint8 num)//numΪ��n������
{
	uint8 temp;
	temp=DATA_SIZE/4;
	if(num<=temp)
	{
	Float2Byte(&data,FlashWriteBuffer,num*4);
	}
}
void Flash_WriteTest(void)
{
	uint8 i;
	for(i=0;i<DATA_SIZE;i++)
	{
		FlashWriteBuffer[i]=i;
	}
	Flash_WriteAllData();
	Flash_ReadAllData();
	LPLD_UART_PutCharArr(UART5,FlashReadBuffer,DATA_SIZE);
}

void delay()
{
  uint16 i, n;
  for(i=0;i<1000;i++)
  {
    for(n=0;n<200;n++)
    {
      asm("nop");
    }
  }//PLL��100MHz��ʱ�����ӳ�5ms
}
