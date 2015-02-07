#include "MPU6050_Moni.h"

uint8 MPU6050_ReadByte(unsigned char address)
{
	uint8 ret = 100;
  IIC_Start();		
  IIC_SendByte(0xd0);	
  IIC_SendByte(address);
  IIC_Start();	
  IIC_SendByte(0xd1);
  ret = IIC_ReadByte();
  IIC_Stop();
  return ret;
}

void MPU6050_WriteByte(unsigned char address,unsigned char thedata)
{
  IIC_Start();
  IIC_SendByte(0Xd0);
  IIC_SendByte(address);
  IIC_SendByte(thedata);
  IIC_Stop();
}
void MPU6050_Inital(void)
{	
  IIC_Init();
  IIC_DelayMs(100);
  //解除休眠
  	MPU6050_WriteByte(PWR_MGMT_1, 0x80);
	LPLD_SYSTICK_DelayMs(20);
	MPU6050_WriteByte(PWR_MGMT_1, 0x00); //解除休眠状态
	LPLD_SYSTICK_DelayMs(20);
	MPU6050_WriteByte(SMPLRT_DIV, 0x04);
	MPU6050_WriteByte(CONFIG, 0x02);//低通的值..低通频率不能太低了..
	MPU6050_WriteByte(GYRO_CONFIG, 0x18);
	MPU6050_WriteByte(ACCEL_CONFIG, 0x00);
  
  /*MPU6050_WriteByte(PWR_MGMT_1 , 0x00 );
  MPU6050_WriteByte(SMPLRT_DIV , 0x07 );
  MPU6050_WriteByte(CONFIG , 0x07 );
  MPU6050_WriteByte(AUX_VDDIO,0x80);
  MPU6050_WriteByte(GYRO_CONFIG , 0x18 );
  MPU6050_WriteByte(ACCEL_CONFIG , 0x01 );
  MPU6050_WriteByte(I2C_MST_CTRL,0x00);
  MPU6050_WriteByte(INT_PIN_CFG,0x02);*/
  //  /**************HMC5883寄存器配置通过6050输出******************/
  //  HMC_SingleWrite(HMC_CFG1,0x78);
  //  HMC_SingleWrite(HMC_CFG2,0x00);
  //  HMC_SingleWrite(HMC_MOD,0x00);
  IIC_DelayMs(100);	
}
int16 MPU6050_GetData(unsigned char REG_Address)
{
  int16 H,L;
  H = MPU6050_ReadByte(REG_Address);
  L = MPU6050_ReadByte(REG_Address +1);
  H=H<<8;
  H=H|L;
  return H >> 2;  
}


void HMC_SingleWrite(unsigned char regAddress,unsigned char data)
{
  IIC_Start();
  IIC_SendByte(SLAVE_WRITE_ADDRESS);
  IIC_SendByte(regAddress);
  IIC_SendByte(data);
  IIC_Stop();
}
//**************************************
//从HMC5883L-I2C设备寄存器读取一个字节数据
//**************************************

unsigned char HMC_SingleRead(unsigned char regAddress)
{
  unsigned char data;
  IIC_Start();
  IIC_SendByte(SLAVE_WRITE_ADDRESS);
  IIC_SendByte(regAddress);
  IIC_Start();
  IIC_SendByte(SLAVE_READ_ADDRESS);
  data = IIC_ReadByte();
  IIC_Stop();
  return data;
}
