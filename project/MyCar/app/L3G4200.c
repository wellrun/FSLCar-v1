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

  //��ʼ��I2C1
  i2c_init_param.I2C_I2Cx = I2C1;       //��DEV_MMA8451.h���޸ĸ�ֵ
  i2c_init_param.I2C_IntEnable = FALSE;
  i2c_init_param.I2C_ICR = 0x17;  //�ɸ���ʵ�ʵ�·����SCLƵ��
  i2c_init_param.I2C_SclPin = PTC10;   //MPU
  i2c_init_param.I2C_SdaPin = PTC11;   //MPU
  i2c_init_param.I2C_Isr = NULL;
//i2c_init_param.I2C_OpenDrainEnable=TRUE;
 LPLD_I2C_Init(i2c_init_param);

//��ȡ�豸ID
	
	/*L3G4200_WriteReg(PWR_MGMT_1, 0x80);
	LPLD_SYSTICK_DelayMs(20);
	L3G4200_WriteReg(PWR_MGMT_1, 0x00); //�������״̬
	LPLD_SYSTICK_DelayMs(20);
	L3G4200_WriteReg(SMPLRT_DIV, 0x04);
	L3G4200_WriteReg(CONFIG, 0x02);//��ͨ��ֵ..��ͨƵ�ʲ���̫����..
	L3G4200_WriteReg(GYRO_CONFIG, 0x10);
	L3G4200_WriteReg(ACCEL_CONFIG, 0x00);*/
	 L3G4200_WriteReg(CTRL_REG1, 0xff);//ODR400Hz��BW50Hz
	 L3G4200_WriteReg(CTRL_REG2, 0x00);   //��ͨ��ͨģʽ,��ֹ0.5Hz
	 L3G4200_WriteReg(CTRL_REG3, 0x08);   //
	 L3G4200_WriteReg(CTRL_REG4, 0xa0);  //+-2000dps
	 L3G4200_WriteReg(CTRL_REG5, 0x00);//OUTREG������ͨ�˲�
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
	// LPLD_I2C_StartTrans(I2C1, MMA8451_DEV_ADDR, I2C_MWSR);
	LPLD_I2C_Start(I2C1);
	LPLD_I2C_WriteByte(I2C1, SlaveAddress);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//дMMA8451�Ĵ�����ַ
	LPLD_I2C_WriteByte(I2C1, RegisterAddress);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//��Ĵ�����д��������
	LPLD_I2C_WriteByte(I2C1, Data);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	LPLD_I2C_Stop(I2C1);

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
	LPLD_I2C_Start(I2C1);
	LPLD_I2C_WriteByte(I2C1, SlaveAddress);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//дMMA8451�Ĵ�����ַ
	LPLD_I2C_WriteByte(I2C1, RegisterAddress);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//�ٴβ�����ʼ�ź�
	LPLD_I2C_ReStart(I2C1);

	//���ʹӻ���ַ�Ͷ�ȡλ
	LPLD_I2C_WriteByte(I2C1, SlaveAddress + 1);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//ת������ģʽΪ��
	LPLD_I2C_SetMasterWR(I2C1, I2C_MRSW);

	//�ر�Ӧ��ACK
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_OFF); //�ر�ACK

	//��IIC����
	result = LPLD_I2C_ReadByte(I2C1);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//����ֹͣ�ź�
	LPLD_I2C_Stop(I2C1);

	//��IIC����
	result = LPLD_I2C_ReadByte(I2C1);

	MMA8451_Delay();

	return result;
}

/*
 * �������ܣ���MAA8451���ٶ����
 * �����k
 *       Regs_Addr - ���ݼĴ�����ַ
 * ��������ֵ�����ٶ�ֵ��int16��
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
