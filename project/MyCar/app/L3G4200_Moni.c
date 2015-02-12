#include "L3G4200_Moni.h"

uint8 L3G4200_ReadByte(unsigned char address)
{
	uint8 ret = 100;
  IIC_Start();		
  IIC_SendByte(0xD2);
  IIC_SendByte(address);
  IIC_Start();	
  IIC_SendByte(0xD3);
  ret = IIC_ReadByte();
  IIC_Stop();
  return ret;
}

void L3G4200_WriteByte(unsigned char address,unsigned char thedata)
{
  IIC_Start();
  IIC_SendByte(0xD2);
  IIC_SendByte(address);
  IIC_SendByte(thedata);
  IIC_Stop();
}
void L3G4200_Inital(void)
{	
  IIC_Init();
  IIC_DelayMs(100);
  //解除休眠
  	/*L3G4200_WriteByte(PWR_MGMT_1, 0x80);
	LPLD_SYSTICK_DelayMs(20);
	L3G4200_WriteByte(PWR_MGMT_1, 0x00); //解除休眠状态
	LPLD_SYSTICK_DelayMs(20);
	L3G4200_WriteByte(SMPLRT_DIV, 0x04);
	L3G4200_WriteByte(CONFIG, 0x02);//低通的值..低通频率不能太低了..
	L3G4200_WriteByte(GYRO_CONFIG, 0x18);
	L3G4200_WriteByte(ACCEL_CONFIG, 0x00);*/
  IIC_DelayMs(100);	
}
int16 L3G4200_GetData(unsigned char REG_Address)
{
  int16 H,L;
  H = L3G4200_ReadByte(REG_Address);
  L = L3G4200_ReadByte(REG_Address +1);
  H=H<<8;
  H=H|L;
  return H ;  
}
