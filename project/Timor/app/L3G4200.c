#include "L3G4200.h"

static void MMA8451_Delay(void);

/*
 *   L3G4200_Init
 *   ��ʼ��MMA8451��������ʼ��8451�����I2C�ӿ��Լ�8451�ļĴ���
 *
 *   ������
 *    ��
 *
 *   ����ֵ
 *      �豸ID
 */
uint8 L3G4200_Init(void)
{
  I2C_InitTypeDef i2c_init_param;
	uint8 device_id;

  //��ʼ��I2C0
  i2c_init_param.I2C_I2Cx = I2C0;       //��DEV_MMA8451.h���޸ĸ�ֵ
  i2c_init_param.I2C_IntEnable = FALSE;
  i2c_init_param.I2C_ICR = 0x17;  //�ɸ���ʵ�ʵ�·����SCLƵ��
  i2c_init_param.I2C_SclPin = PTB2;   //MPU
  i2c_init_param.I2C_SdaPin = PTB3;   //MPU
  i2c_init_param.I2C_Isr = NULL;
 // i2c_init_param.I2C_OpenDrainEnable=TRUE;
 LPLD_I2C_Init(i2c_init_param);

//��ȡ�豸ID
	
	 L3G4200_WriteReg(CTRL_REG1, 0xff);
	 L3G4200_WriteReg(CTRL_REG2, 0x00);   
	 L3G4200_WriteReg(CTRL_REG3, 0x18);  
	 L3G4200_WriteReg(CTRL_REG4, 0x00);  
	 L3G4200_WriteReg(CTRL_REG5, 0x02);
         device_id = L3G4200_ReadReg(WHO_AM_I);
	return device_id;
}

/*
 *   L3G4200_WriteReg
 *   �ú�����������MMA8451�ļĴ���
 *
 *   ������
 *   RegisterAddress
 *    |__ MMA8451�Ĵ�����ַ
 *   Data
 *    |__ ����д����ֽ�������
 */
void L3G4200_WriteReg(uint8 RegisterAddress, uint8 Data)
{
	//���ʹӻ���ַ
	// LPLD_I2C_StartTrans(I2C0, MMA8451_DEV_ADDR, I2C_MWSR);
	LPLD_I2C_Start(I2C0);
	LPLD_I2C_WriteByte(I2C0, SlaveAddress);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);

	//дMMA8451�Ĵ�����ַ
	LPLD_I2C_WriteByte(I2C0, RegisterAddress);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);

	//��Ĵ�����д��������
	LPLD_I2C_WriteByte(I2C0, Data);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);

	LPLD_I2C_Stop(I2C0);

	MMA8451_Delay();
}

/*
 *   L3G4200_WriteReg
 *   �ú������ڶ�ȡMMA8451������
 *
 *   ������
 *     RegisterAddress
 *        |__ MMA8451�Ĵ�����ַ
 *   ����ֵ
 *      ���ٴ������Ĳ���ֵ
 */
uint8 L3G4200_ReadReg(uint8 RegisterAddress)
{
	uint8 result;

	//���ʹӻ���ַ
	LPLD_I2C_Start(I2C0);
	LPLD_I2C_WriteByte(I2C0, SlaveAddress);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);

	//дMMA8451�Ĵ�����ַ
	LPLD_I2C_WriteByte(I2C0, RegisterAddress);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);


	//�ٴβ�����ʼ�ź�
	LPLD_I2C_ReStart(I2C0);

	//���ʹӻ���ַ�Ͷ�ȡλ
	LPLD_I2C_WriteByte(I2C0, SlaveAddress+1);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);

	//ת������ģʽΪ��
	LPLD_I2C_SetMasterWR(I2C0, I2C_MRSW);

	//�ر�Ӧ��ACK
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_OFF);//�ر�ACK

	//��IIC����
	result = LPLD_I2C_ReadByte(I2C0);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);

	//����ֹͣ�ź�
	LPLD_I2C_Stop(I2C0);

	//��IIC����
	result = LPLD_I2C_ReadByte(I2C0);

	MMA8451_Delay();

	return result;
}

/*
 * �������ܣ���MAA8451���ٶ����
 * �����k
 *       Regs_Addr - ���ݼĴ�����ַ
 * ��������ֵ�����ٶ�ֵ��int16��
 */
int16 L3G4200_MultipleRead(uint8 RegisterAddress)
{
	int16 result_H = 0, result_L = 0;
	//���ʹӻ���ַ
	LPLD_I2C_Start(I2C0);
	LPLD_I2C_WriteByte(I2C0, SlaveAddress);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);

	//дMMA8451�Ĵ�����ַ
	LPLD_I2C_WriteByte(I2C0, RegisterAddress);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);


	//�ٴβ�����ʼ�ź�
	LPLD_I2C_ReStart(I2C0);

	//���ʹӻ���ַ�Ͷ�ȡλ
	LPLD_I2C_WriteByte(I2C0, SlaveAddress + 1);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);

	//ת������ģʽΪ��
	LPLD_I2C_SetMasterWR(I2C0, I2C_MRSW);

	//�ر�Ӧ��ACK
	

	//��IIC����
	/*result_L = LPLD_I2C_ReadByte(I2C0);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);*/

	//����ֹͣ�ź�
	

	//��IIC����
	result_L = LPLD_I2C_ReadByte(I2C0);
	I2C0->C1 &= ~I2C_C1_TXAK_MASK;
	//LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);
	result_H = LPLD_I2C_ReadByte(I2C0);
	I2C0->C1 &= ~I2C_C1_TXAK_MASK;
       // LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_OFF);//�ر�ACK
	LPLD_I2C_Stop(I2C0);

	MMA8451_Delay();
	result_H = result_H << 8;
	result_H = result_H | result_L;
	return result_H;
}
int16 L3G4200_GetResult(uint8 Regs_Addr)
{

	int16 result, temp;
	result = L3G4200_ReadReg(Regs_Addr);
	temp = L3G4200_ReadReg(Regs_Addr + 1);
	temp = temp << 8;
	result = result | temp;
	return result;
}

int16 L3G4200_GetResult_H(uint8 Regs_Addr)
{

	int16 result;
	result = L3G4200_ReadReg(Regs_Addr);
	result = result << 8;

	return result;
}

/*
 * ��ʱ����
 */
static void MMA8451_Delay(void)
{
	int n;
	for (n = 1; n < 200; n++)
	{
		asm("nop");
	}
}
