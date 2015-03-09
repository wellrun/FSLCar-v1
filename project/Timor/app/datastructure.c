#include "datastructure.h"
#include "common.h"
#include "AngGet.h"

extern AngPID_InitTypeDef Ang_PID;

extern CarInfo_TypeDef CarInfo_Now; //当前车子的信息
extern CarControl_TypeDef MotorControl; //电机控制的值
extern AngPID_InitTypeDef Ang_PID; //角度控制的PID结构体
extern TempOfMotor_TypeDef TempValue; //临时存储角度和速度控制浮点变量的结构体
extern SpeedPID_TypeDef Speed_PID;
extern DirPID_TypeDef Dir_PID;

#define Raw_Data_SECTOR   (0)
#define Saved_Data_SECTOR   (1)
#define FLASH_RAW_ADDR     (Raw_Data_SECTOR*2048)
#define FLASH_SAVED_ADDR     (Saved_Data_SECTOR*2048)
#define DATA_SIZE           (9*4)

 uint8 Raw_DataBuffer[DATA_SIZE];
 uint8 Saved_DataBuffer[DATA_SIZE];
 uint8 Write_DataBuffer[DATA_SIZE];
 uint8 CodeData_U8_Buffer[DATA_SIZE];
 float CodeData_Float_Buffer[DATA_SIZE / 4];
 float SavedData_Float_Buffer[DATA_SIZE / 4];
void delay();
uint8 Flash_ReadAllData(void)
{
	uint8 *ptr;
	int i;
	uint8 Flag_SameData = 0;
	uint8 Cnt_SameData = 0;
	static uint8 result = 85;
	ptr = (uint8*)FLASH_RAW_ADDR;
	for (i = 0; i < DATA_SIZE; i++)
	{
		Raw_DataBuffer[i] = *(ptr + i);
	}
	CodeData_Float_Buffer[0] = Ang_PID.Kp;
	CodeData_Float_Buffer[1] = Ang_PID.Kd;
	CodeData_Float_Buffer[2] = Ang_PID.AngSet;
	CodeData_Float_Buffer[3] = Speed_PID.SpeedSet;
	CodeData_Float_Buffer[4] = Speed_PID.Kp;
	CodeData_Float_Buffer[5] = Speed_PID.Ki;
	CodeData_Float_Buffer[6] = Speed_PID.Kd;
	CodeData_Float_Buffer[7] = Dir_PID.Kp;
	CodeData_Float_Buffer[8] = Dir_PID.Kd;
	for (i = 0; i < (DATA_SIZE / 4); i++)
	{
		Float2Byte(&CodeData_Float_Buffer[i], CodeData_U8_Buffer, i*4);
	}
	for (i = 0; i < (DATA_SIZE);i++)
	{
		if (Raw_DataBuffer[i]==CodeData_U8_Buffer[i])
		{
			Cnt_SameData++;
		}
	}
	if (Cnt_SameData==DATA_SIZE)
	{
		ptr = (uint8*)FLASH_SAVED_ADDR;
		for (i = 0; i < DATA_SIZE; i++)
		{
			Saved_DataBuffer[i] = *(ptr + i);
		}
		for (i = 0; i < (DATA_SIZE / 4); i++)
		{
			Float2Byte(&SavedData_Float_Buffer[i], Saved_DataBuffer, i*4);
		}
		Ang_PID.Kp = SavedData_Float_Buffer[0];
		Ang_PID.Kd = SavedData_Float_Buffer[1];
		Ang_PID.AngSet = SavedData_Float_Buffer[2];
		Speed_PID.SpeedSet = SavedData_Float_Buffer[3];
		Speed_PID.Kp = SavedData_Float_Buffer[4];
		Speed_PID.Ki = SavedData_Float_Buffer[5];
		Speed_PID.Kd = SavedData_Float_Buffer[6];
		Dir_PID.Kp = SavedData_Float_Buffer[7];
		Dir_PID.Kd = SavedData_Float_Buffer[8];
	}
	else
	{
		DisableInterrupts;
		delay();
		result = LPLD_Flash_SectorErase(Raw_Data_SECTOR);
		delay();
		result = LPLD_Flash_SectorErase(Saved_Data_SECTOR);
		delay();
		result = LPLD_Flash_ByteProgram((uint32)Raw_Data_SECTOR, (uint32*)CodeData_U8_Buffer, DATA_SIZE);
		delay();
		result = LPLD_Flash_ByteProgram((uint32)Saved_Data_SECTOR, (uint32*)CodeData_U8_Buffer, DATA_SIZE);
		delay();
		EnableInterrupts; //先擦除
                result=result;
	}
	
}
void Flash_WriteAllData(void)
{
  uint8 result;
  uint8 *p = NULL;
  /*  DisableInterrupts;
	result = LPLD_Flash_SectorErase(Saved_Data_SECTOR);
    EnableInterrupts; //先擦除
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

    }*/
}
void Flash_DataToBuffer(float data,uint8 num)//num为第n个数据
{
	/*uint8 temp;
	temp=DATA_SIZE/4;
	if(num<=temp)
	{
	Float2Byte(&data,FlashWriteBuffer,num*4);
	}*/
}
void delay()
{
	uint16 i, n;
	for (i = 0; i < 30000; i++)
	{
		for (n = 0; n < 200; n++)
		{
			asm("nop");
		}
	}
}