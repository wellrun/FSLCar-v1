
#include "DEV_MMA8451.h"

static void MMA8451_Delay(void);

/*
 *   LPLD_MMA8451_Init
 *   ��ʼ��MMA8451��������ʼ��8451�����I2C�ӿ��Լ�8451�ļĴ���
 *
 *   ������
 *    ��
 *
 *   ����ֵ
 *      �豸ID
 */
uint8 LPLD_MMA8451_Init(void)
{
  I2C_InitTypeDef i2c_init_param;
  uint8 device_id=1;
    
  //��ʼ��MMA8451_I2CX
  i2c_init_param.I2C_I2Cx = I2C0;       //��DEV_MMA8451.h���޸ĸ�ֵ
  i2c_init_param.I2C_IntEnable = FALSE;
  i2c_init_param.I2C_ICR = 0x17;  //�ɸ���ʵ�ʵ�·����SCLƵ��
  i2c_init_param.I2C_SclPin = PTB2;   //MPU
  i2c_init_param.I2C_SdaPin = PTB3;   //MPU
  i2c_init_param.I2C_Isr = NULL;
  
  LPLD_I2C_Init(i2c_init_param);
  
  //��ȡ�豸ID
  for(device_id=0;device_id<100;device_id++)
  {
    MMA8451_Delay();
    MMA8451_Delay();
  }
  device_id = LPLD_MMA8451_ReadReg(MMA8451_REG_WHOAMI);

  //���мĴ�������
  LPLD_MMA8451_WriteReg(MMA8451_REG_SYSMOD, 0x00);       //Ĭ��ģʽStandby Mode
  LPLD_MMA8451_WriteReg(MMA8451_REG_CTRL_REG2, 0x02);    //High Resolution
  LPLD_MMA8451_WriteReg(MMA8451_REG_CTRL_REG1, 0x09);    //����ģʽ,800HZ
  
  return device_id;
}

/*
 *   LPLD_MMA8451_WriteReg
 *   �ú�����������MMA8451�ļĴ���
 *
 *   ������
 *   RegisterAddress 
 *    |__ MMA8451�Ĵ�����ַ
 *   Data
 *    |__ ����д����ֽ������� 
 */
void LPLD_MMA8451_WriteReg(uint8 RegisterAddress, uint8 Data)
{
  //���ʹӻ���ַ
  LPLD_I2C_StartTrans(MMA8451_I2CX, MMA8451_DEV_ADDR, I2C_MWSR);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);
  
  //дMMA8451�Ĵ�����ַ
  LPLD_I2C_WriteByte(MMA8451_I2CX, RegisterAddress);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);
  
  //��Ĵ�����д��������
  LPLD_I2C_WriteByte(MMA8451_I2CX, Data);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  LPLD_I2C_Stop(MMA8451_I2CX);

  //MMA8451_Delay();
}

/*
 *   LPLD_MMA8451_WriteReg
 *   �ú������ڶ�ȡMMA8451������
 *
 *   ������
 *     RegisterAddress 
 *        |__ MMA8451�Ĵ�����ַ
 *   ����ֵ
 *      ���ٴ������Ĳ���ֵ
 */
uint8 LPLD_MMA8451_ReadReg(uint8 RegisterAddress)
{
  uint8 result;

  //���ʹӻ���ַ
  disable_irq((IRQn_Type)(PORTD_IRQn));
  LPLD_I2C_StartTrans(MMA8451_I2CX, MMA8451_DEV_ADDR, I2C_MWSR);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  //дMMA8451�Ĵ�����ַ
  LPLD_I2C_WriteByte(MMA8451_I2CX, RegisterAddress);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  //�ٴβ�����ʼ�ź�
  LPLD_I2C_ReStart(MMA8451_I2CX);

  //���ʹӻ���ַ�Ͷ�ȡλ
  LPLD_I2C_WriteByte(MMA8451_I2CX, MMA8451_DEV_READ);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  //ת������ģʽΪ��
  LPLD_I2C_SetMasterWR(MMA8451_I2CX, I2C_MRSW);

  //�ر�Ӧ��ACK
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_OFF);//�ر�ACK

  //��IIC����
  result =LPLD_I2C_ReadByte(MMA8451_I2CX);
  LPLD_I2C_WaitAck(MMA8451_I2CX, I2C_ACK_ON);

  //����ֹͣ�ź�
  LPLD_I2C_Stop(MMA8451_I2CX);

   //��IIC����
  result = LPLD_I2C_ReadByte(MMA8451_I2CX);
  
  MMA8451_Delay();
  enable_irq((IRQn_Type)(PORTD_IRQn));
  return result;
}

/*
 * �������ܣ���MAA8451���ٶ����
 * �����w
 *       Status - ���ݼĴ���״̬
 *       Regs_Addr - ���ݼĴ�����ַ
 * ��������ֵ�����ٶ�ֵ��int16��
 */   
int16 LPLD_MMA8451_GetResult(uint8 Status, uint8 Regs_Addr) 
{
  int16 result,temp;
  
  result= LPLD_MMA8451_ReadReg( Regs_Addr);
  temp  = LPLD_MMA8451_ReadReg( Regs_Addr+1);
  result=result<<8;
  result=result|temp;
  
  return result>>2;
}
int16 LPLD_MMA8451_GetResult_H(uint8 Regs_Addr)
{
	int16 result;

	result = LPLD_MMA8451_ReadReg(Regs_Addr);
	result = result << 8;
	return result >> 2;
}

/*
 * ��ʱ����
 */
static void MMA8451_Delay(void){
  int n;
  for(n=1;n<400;n++) {
    asm("nop");
  }
}
