#include "datastructure.h"
#include "common.h"
#include "AngGet.h"

extern AngPID_InitTypeDef Ang_PID;
//extern float AngToMotorRatio;
uint8 FlashReadBuffer[20]={0x01,0x01};
uint8 FlashWriteBuffer[20]={0x01,0x01};
float FlashFloatBuffer[5]={0x01,0x01};//存放读取出来的数据
int FlashIntBuffer[5]={0x01,0x01};

void delay();
void Flash_ReadAllData(void)
{
	uint8 *ptr;
	uint8 len = 0;
	ptr = (uint8*) FLASH_ADDR;
	for (len = 0; len <= DATA_SIZE; len++)
	{
		FlashReadBuffer[len] = *(ptr + len);
		FlashWriteBuffer[len] = FlashReadBuffer[len]; //将读出来的数据放到写入buffer里面,防止在未对FlashWriteBuffer初始化的情况下写入
	}
	for(len=0;len<=NumOfFloat;len++)
	{
		Byte2Float(&FlashFloatBuffer[len],FlashReadBuffer,len*4); 
	}
	Ang_PID.AngSet=FlashFloatBuffer[0];
	Ang_PID.Kp=FlashFloatBuffer[1];
	Ang_PID.Kd=FlashFloatBuffer[2];
	//AngToMotorRatio=FlashFloatBuffer[3];
}
void Flash_WriteAllData(void)
{
  uint8 result;
    DisableInterrupts;
    result=LPLD_Flash_SectorErase(FLASH_ADDR);
    EnableInterrupts; //先擦除
    delay();

    DisableInterrupts;
    Flash_DataToBuffer(FlashFloatBuffer[0],0);
    Flash_DataToBuffer(FlashFloatBuffer[1],1);
    Flash_DataToBuffer(FlashFloatBuffer[2],2);
    Flash_DataToBuffer(FlashFloatBuffer[3],3);
    result=LPLD_Flash_ByteProgram((uint32)FLASH_ADDR, (uint32*)FlashWriteBuffer, DATA_SIZE);
     delay();
    EnableInterrupts;
    delay();

    if(result==FLASH_OK)
    {
    	LPLD_UART_PutChar(UART5,0xfb);
    }
    else
    {

    }
}
void Flash_DataToBuffer(float data,uint8 num)//num为第n个数据
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
  for(i=0;i<3000;i++)
  {
    for(n=0;n<200;n++)
    {
      asm("nop");
    }
  }//PLL在100MHz的时候大概延迟5ms
}
