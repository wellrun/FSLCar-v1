#include "MMA8451_Moni.h"

uint8 MMA8451_ReadByte(unsigned char address)
{
	uint8 ret = 100;
  IIC_Start();		
  IIC_SendByte(0x38);	
  IIC_SendByte(address);
  IIC_Start();	
  IIC_SendByte(0x39);
  ret = IIC_ReadByte();
  IIC_Stop();
  return ret;
}

void MMA8451_WriteByte(unsigned char address,unsigned char thedata)
{
  IIC_Start();
  IIC_SendByte(0x38);
  IIC_SendByte(address);
  IIC_SendByte(thedata);
  IIC_Stop();
}
uint8 MMA8451_Inital(void)
{	
	uint8 id = 0;
  IIC_Init();
  IIC_DelayMs(100);
  //解除休眠
  MMA8451_WriteByte(MMA8451_REG_SYSMOD, 0x00);       //默认模式Standby Mode
  MMA8451_WriteByte(MMA8451_REG_CTRL_REG2, 0x02);    //High Resolution
  MMA8451_WriteByte(MMA8451_REG_CTRL_REG1, 0x09);    //主动模式,800HZ
  id = MMA8451_ReadByte(MMA8451_REG_WHOAMI);
  /*MMA8451_WriteByte(PWR_MGMT_1 , 0x00 );
  MMA8451_WriteByte(SMPLRT_DIV , 0x07 );
  MMA8451_WriteByte(CONFIG , 0x07 );
  MMA8451_WriteByte(AUX_VDDIO,0x80);
  MMA8451_WriteByte(GYRO_CONFIG , 0x18 );
  MMA8451_WriteByte(ACCEL_CONFIG , 0x01 );
  MMA8451_WriteByte(I2C_MST_CTRL,0x00);
  MMA8451_WriteByte(INT_PIN_CFG,0x02);*/
  //  /**************HMC5883寄存器配置通过6050输出******************/
  //  HMC_SingleWrite(HMC_CFG1,0x78);
  //  HMC_SingleWrite(HMC_CFG2,0x00);
  //  HMC_SingleWrite(HMC_MOD,0x00);
  IIC_DelayMs(100);	
  return id;
}
int16 MMA8451_GetData(unsigned char REG_Address)
{
  int16 H,L;
  H = MMA8451_ReadByte(REG_Address);
  L = MMA8451_ReadByte(REG_Address +1);
  H=H<<8;
  H=H|L;
  return H >> 2;  
}
