#include "L3G4200.h"

static void MMA8451_Delay(void);

/*
 *   L3G4200_Init
 *   初始化MMA8451，包括初始化8451所需的I2C接口以及8451的寄存器
 *
 *   参数：
 *    无
 *
 *   返回值
 *      设备ID
 */
uint8 L3G4200_Init(void)
{
  I2C_InitTypeDef i2c_init_param;
	uint8 device_id;

  //初始化I2C1
  i2c_init_param.I2C_I2Cx = I2C1;       //在DEV_MMA8451.h中修改该值
  i2c_init_param.I2C_IntEnable = FALSE;
  i2c_init_param.I2C_ICR = 0x17;  //可根据实际电路更改SCL频率
  i2c_init_param.I2C_SclPin = PTC10;   //MPU
  i2c_init_param.I2C_SdaPin = PTC11;   //MPU
  i2c_init_param.I2C_Isr = NULL;
//i2c_init_param.I2C_OpenDrainEnable=TRUE;
 LPLD_I2C_Init(i2c_init_param);

//读取设备ID
	
	/*L3G4200_WriteReg(PWR_MGMT_1, 0x80);
	LPLD_SYSTICK_DelayMs(20);
	L3G4200_WriteReg(PWR_MGMT_1, 0x00); //解除休眠状态
	LPLD_SYSTICK_DelayMs(20);
	L3G4200_WriteReg(SMPLRT_DIV, 0x04);
	L3G4200_WriteReg(CONFIG, 0x02);//低通的值..低通频率不能太低了..
	L3G4200_WriteReg(GYRO_CONFIG, 0x10);
	L3G4200_WriteReg(ACCEL_CONFIG, 0x00);*/
	 L3G4200_WriteReg(CTRL_REG1, 0xff);//ODR400Hz和BW50Hz
	 L3G4200_WriteReg(CTRL_REG2, 0x00);   //高通普通模式,截止0.5Hz
	 L3G4200_WriteReg(CTRL_REG3, 0x08);   //
	 L3G4200_WriteReg(CTRL_REG4, 0xa0);  //+-2000dps
	 L3G4200_WriteReg(CTRL_REG5, 0x00);//OUTREG开启高通滤波
    device_id = L3G4200_ReadReg(WHO_AM_I);
	return device_id;
}

/*
 *   L3G4200_WriteReg
 *   该函数用于配置MMA8451的寄存器
 *
 *   参数：
 *   RegisterAddress
 *    |__ MMA8451寄存器地址
 *   Data
 *    |__ 具体写入的字节型数据
 */
void L3G4200_WriteReg(uint8 RegisterAddress, uint8 Data)
{
	//发送从机地址
	// LPLD_I2C_StartTrans(I2C1, MMA8451_DEV_ADDR, I2C_MWSR);
	LPLD_I2C_Start(I2C1);
	LPLD_I2C_WriteByte(I2C1, SlaveAddress);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//写MMA8451寄存器地址
	LPLD_I2C_WriteByte(I2C1, RegisterAddress);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//向寄存器中写具体数据
	LPLD_I2C_WriteByte(I2C1, Data);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	LPLD_I2C_Stop(I2C1);

	MMA8451_Delay();
}

/*
 *   L3G4200_WriteReg
 *   该函数用于读取MMA8451的数据
 *
 *   参数：
 *     RegisterAddress
 *        |__ MMA8451寄存器地址
 *   返回值
 *      加速传感器的测量值
 */
uint8 L3G4200_ReadReg(uint8 RegisterAddress)
{
	uint8 result;

	//发送从机地址
	LPLD_I2C_Start(I2C1);
	LPLD_I2C_WriteByte(I2C1, SlaveAddress);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//写MMA8451寄存器地址
	LPLD_I2C_WriteByte(I2C1, RegisterAddress);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//再次产生开始信号
	LPLD_I2C_ReStart(I2C1);

	//发送从机地址和读取位
	LPLD_I2C_WriteByte(I2C1, SlaveAddress + 1);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//转换主机模式为读
	LPLD_I2C_SetMasterWR(I2C1, I2C_MRSW);

	//关闭应答ACK
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_OFF); //关闭ACK

	//读IIC数据
	result = LPLD_I2C_ReadByte(I2C1);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//发送停止信号
	LPLD_I2C_Stop(I2C1);

	//读IIC数据
	result = LPLD_I2C_ReadByte(I2C1);

	MMA8451_Delay();

	return result;
}

/*
 * 函数功能：读MAA8451加速度输出
 * 参数
 *       Regs_Addr - 数据寄存器地址
 * 函数返回值：加速度值（int16）
 */
int16 L3G4200_GetResult(uint8 Regs_Addr)
{

	int16 result, temp;
	result = L3G4200_ReadReg(Regs_Addr);
	temp = L3G4200_ReadReg(Regs_Addr + 1);
	temp = temp << 8;
	result = result | temp;

	return result;
}

/*
 * 延时函数
 */
static void MMA8451_Delay(void)
{
	int n;
	for (n = 1; n < 200; n++)
	{
		asm("nop");
	}
}
