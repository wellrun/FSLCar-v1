#include "MPU6050.h"

static void MMA8451_Delay(void);

/*
 *   MPU6050_Init
 *   ��ʼ��MMA8451��������ʼ��8451�����I2C�ӿ��Լ�8451�ļĴ���
 *
 *   ������
 *    ��
 *
 *   ����ֵ
 *      �豸ID
 */
uint8 MPU6050_Init(void)
{
//  I2C_InitTypeDef i2c_init_param;
	uint8 device_id;
//
//  //��ʼ��I2C0
//  i2c_init_param.I2C_I2Cx = I2C0;       //��DEV_MMA8451.h���޸ĸ�ֵ
//  i2c_init_param.I2C_IntEnable = FALSE;
//  i2c_init_param.I2C_ICR = MMA8451_SCL_200KHZ;  //�ɸ���ʵ�ʵ�·����SCLƵ��
//  i2c_init_param.I2C_SclPin = MMA8451_SCLPIN;   //��DEV_MMA8451.h���޸ĸ�ֵ
//  i2c_init_param.I2C_SdaPin = MMA8451_SDAPIN;   //��DEV_MMA8451.h���޸ĸ�ֵ
//  i2c_init_param.I2C_Isr = NULL;
//
//  LPLD_I2C_Init(i2c_init_param);

//��ȡ�豸ID
	device_id = MPU6050_ReadReg(WHO_AM_I);
	MPU6050_WriteReg(PWR_MGMT_1, 0x80);
	LPLD_SYSTICK_DelayMs(20);
	MPU6050_WriteReg(PWR_MGMT_1, 0x00); //�������״̬
	LPLD_SYSTICK_DelayMs(20);
	MPU6050_WriteReg(SMPLRT_DIV, 0x07);
	MPU6050_WriteReg(CONFIG, 0x06);
	MPU6050_WriteReg(GYRO_CONFIG, 0x18);
	MPU6050_WriteReg(ACCEL_CONFIG, 0x01);

	return device_id;
}

/*
 *   MPU6050_WriteReg
 *   �ú�����������MMA8451�ļĴ���
 *
 *   ������
 *   RegisterAddress
 *    |__ MMA8451�Ĵ�����ַ
 *   Data
 *    |__ ����д����ֽ�������
 */
void MPU6050_WriteReg(uint8 RegisterAddress, uint8 Data)
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
 *   MPU6050_WriteReg
 *   �ú������ڶ�ȡMMA8451������
 *
 *   ������
 *     RegisterAddress
 *        |__ MMA8451�Ĵ�����ַ
 *   ����ֵ
 *      ���ٴ������Ĳ���ֵ
 */
uint8 MPU6050_ReadReg(uint8 RegisterAddress)
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
	LPLD_I2C_WriteByte(I2C0, SlaveAddress + 1);
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_ON);

	//ת������ģʽΪ��
	LPLD_I2C_SetMasterWR(I2C0, I2C_MRSW);

	//�ر�Ӧ��ACK
	LPLD_I2C_WaitAck(I2C0, I2C_ACK_OFF); //�ر�ACK

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
int16 MPU6050_GetResult(uint8 Regs_Addr)
{

	int16 result, temp;
	result = MPU6050_ReadReg(Regs_Addr);
	temp = MPU6050_ReadReg(Regs_Addr + 1);
	result = result << 8;
	result = result | temp;

	return result >> 2;
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
