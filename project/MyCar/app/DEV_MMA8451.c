
#include "DEV_MMA8451.h"

static void MMA8451_Delay(void);

/*
 *   LPLD_MMA8451_Init
 *   初始化MMA8451，包括初始化8451所需的I2C接口以及8451的寄存器
 *
 *   参数：
 *    无
 *
 *   返回值
 *      设备ID
 */
uint8 LPLD_MMA8451_Init(void)
{
  I2C_InitTypeDef i2c_init_param;
  uint8 device_id=1;
    
  //初始化MMA8451_I2CX
  i2c_init_param.I2C_I2Cx = I2C0;       //在DEV_MMA8451.h中修改该值
  i2c_init_param.I2C_IntEnable = FALSE;
  i2c_init_param.I2C_ICR = 0x23;
  i2c_init_param.I2C_SclPin = PTD8;   //在DEV_MMA8451.h中修改该值
  i2c_init_param.I2C_SdaPin = PTD9;   //在DEV_MMA8451.h中修改该值
  i2c_init_param.I2C_Isr = NULL;
  i2c_init_param.I2C_OpenDrainEnable=TRUE;
  
  LPLD_I2C_Init(i2c_init_param);
  
  //读取设备ID
  for(device_id=0;device_id<100;device_id++)
  {
    MMA8451_Delay();
    MMA8451_Delay();
  }
  device_id = LPLD_MMA8451_ReadReg(MMA8451_REG_WHOAMI);

  //进行寄存器配置
  LPLD_MMA8451_WriteReg(MMA8451_REG_SYSMOD, 0x00);       //默认模式Standby Mode
  LPLD_MMA8451_WriteReg(MMA8451_REG_CTRL_REG2, 0x02);    //High Resolution
  LPLD_MMA8451_WriteReg(MMA8451_REG_CTRL_REG1, 0x09);    //主动模式,800HZ
  //LPLD_MMA8451_WriteReg(0x0f, 0x33);//绕过高通滤波对冲击的响应,开启对于冲击的低通
 // LPLD_MMA8451_WriteReg(0x0e, 0x00);//开启高通滤波....截止频率2Hz,
 // LPLD_MMA8451_WriteReg(0x0f, 0x23);//开启低通滤波器
  
  return device_id;
}

/*
 *   LPLD_MMA8451_WriteReg
 *   该函数用于配置MMA8451的寄存器
 *
 *   参数：
 *   RegisterAddress 
 *    |__ MMA8451寄存器地址
 *   Data
 *    |__ 具体写入的字节型数据 
 */
void LPLD_MMA8451_WriteReg(uint8 RegisterAddress, uint8 Data)
{
  //发送从机地址
  LPLD_I2C_StartTrans(MMA8451_I2CX, MMA8451_DEV_ADDR, I2C_MWSR);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);
  
  //写MMA8451寄存器地址
  LPLD_I2C_WriteByte(MMA8451_I2CX, RegisterAddress);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);
  
  //向寄存器中写具体数据
  LPLD_I2C_WriteByte(MMA8451_I2CX, Data);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  LPLD_I2C_Stop(MMA8451_I2CX);

  //MMA8451_Delay();
}

/*
 *   LPLD_MMA8451_WriteReg
 *   该函数用于读取MMA8451的数据
 *
 *   参数：
 *     RegisterAddress 
 *        |__ MMA8451寄存器地址
 *   返回值
 *      加速传感器的测量值
 */
uint8 LPLD_MMA8451_ReadReg(uint8 RegisterAddress)
{
  uint8 result;

  //发送从机地址
  LPLD_I2C_StartTrans(MMA8451_I2CX, MMA8451_DEV_ADDR, I2C_MWSR);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  //写MMA8451寄存器地址
  LPLD_I2C_WriteByte(MMA8451_I2CX, RegisterAddress);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  //再次产生开始信号
  LPLD_I2C_ReStart(MMA8451_I2CX);

  //发送从机地址和读取位
  LPLD_I2C_WriteByte(MMA8451_I2CX, MMA8451_DEV_READ);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  //转换主机模式为读
  LPLD_I2C_SetMasterWR(MMA8451_I2CX, I2C_MRSW);

  //关闭应答ACK
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_OFF);//关闭ACK

  //读IIC数据
  result =LPLD_I2C_ReadByte(MMA8451_I2CX);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  //发送停止信号
  LPLD_I2C_Stop(MMA8451_I2CX);

   //读IIC数据
  result = LPLD_I2C_ReadByte(MMA8451_I2CX);
  
  MMA8451_Delay();

  return result;
}

/*
 * 函数功能：读MAA8451加速度输出
 * 参数
 *       Status - 数据寄存器状态
 *       Regs_Addr - 数据寄存器地址
 * 函数返回值：加速度值（int16）
 */   
int16 LPLD_MMA8451_GetResult(uint8 Status, uint8 Regs_Addr) 
{
  int16 result,temp;
  
  if(Regs_Addr>MMA8451_REG_OUTZ_LSB)
    return 0;
  
  // 等待转换完成并取出值 
 /* while(!(ret&Status)) 
  {
    ret = LPLD_MMA8451_ReadReg( MMA8451_REG_STATUS);
    if(++cnt>100)
      break;
  }*/
  
  result= LPLD_MMA8451_ReadReg( Regs_Addr);
  temp  = LPLD_MMA8451_ReadReg( Regs_Addr+1);
  result=result<<8;
  result=result|temp;
  
  return result>>2;
}


/*
 * 延时函数
 */
static void MMA8451_Delay(void){
  int n;
  for(n=1;n<400;n++) {
    asm("nop");
  }
}
