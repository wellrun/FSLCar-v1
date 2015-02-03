#include "MPU6050.h"

static void MMA8451_Delay(int t);

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
  I2C_InitTypeDef i2c_init_param;
	uint8 device_id;

  //��ʼ��I2C1
  i2c_init_param.I2C_I2Cx = I2C1;       //��DEV_MMA8451.h���޸ĸ�ֵ
  i2c_init_param.I2C_IntEnable = FALSE;
  i2c_init_param.I2C_ICR = 0x2B;  //�ɸ���ʵ�ʵ�·����SCLƵ��
  i2c_init_param.I2C_SclPin = PTC10;   //MPU
  i2c_init_param.I2C_SdaPin = PTC11;   //MPU
  i2c_init_param.I2C_Isr = NULL;
//i2c_init_param.I2C_OpenDrainEnable=TRUE;
 LPLD_I2C_Init(i2c_init_param);

//��ȡ�豸ID
	
	MPU6050_WriteReg(PWR_MGMT_1, 0x80);
	LPLD_SYSTICK_DelayMs(20);
	MPU6050_WriteReg(PWR_MGMT_1, 0x00); //�������״̬
	LPLD_SYSTICK_DelayMs(20);
	MPU6050_WriteReg(SMPLRT_DIV, 0x04);
	MPU6050_WriteReg(CONFIG, 0x02);//��ͨ��ֵ..��ͨƵ�ʲ���̫����..
	MPU6050_WriteReg(GYRO_CONFIG, 0x18);
	MPU6050_WriteReg(ACCEL_CONFIG, 0x00);
     device_id = MPU6050_ReadReg(0x75);
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
	// LPLD_I2C_StartTrans(I2C1, MMA8451_DEV_ADDR, I2C_MWSR);
	LPLD_I2C_Start(I2C1); 
	//MMA8451_Delay(10);
	LPLD_I2C_WriteByte(I2C1, SlaveAddress);
	//MMA8451_Delay(10);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//дMMA8451�Ĵ�����ַ
	//MMA8451_Delay(300);
	LPLD_I2C_WriteByte(I2C1, RegisterAddress);
	//MMA8451_Delay(10);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//��Ĵ�����д��������
	//MMA8451_Delay(10);
	LPLD_I2C_WriteByte(I2C1, Data);
	//MMA8451_Delay(10);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	LPLD_I2C_Stop(I2C1);

	MMA8451_Delay(5000);
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
	LPLD_I2C_Start(I2C1);
	//MMA8451_Delay(20);
	LPLD_I2C_WriteByte(I2C1, SlaveAddress);
	//MMA8451_Delay(5);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//дMMA8451�Ĵ�����ַ
	//MMA8451_Delay(100);
    //�����λ�ÿ���
	LPLD_I2C_WriteByte(I2C1, RegisterAddress);
	//MMA8451_Delay(5);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);
	//MMA8451_Delay(10);
	//�ٴβ�����ʼ�ź�
	LPLD_I2C_ReStart(I2C1);

	//���ʹӻ���ַ�Ͷ�ȡλ
	//MMA8451_Delay(10);
	LPLD_I2C_WriteByte(I2C1, SlaveAddress + 1);
	//MMA8451_Delay(10);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//ת������ģʽΪ��
	//MMA8451_Delay(10);
	LPLD_I2C_SetMasterWR(I2C1, I2C_MRSW);

	//�ر�Ӧ��ACK
	//MMA8451_Delay(3);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_OFF); //NACK

	//��IIC����
	//MMA8451_Delay(100);
	result = LPLD_I2C_ReadByte(I2C1);
	//MMA8451_Delay(10);
	LPLD_I2C_WaitAck(I2C1, I2C_ACK_ON);

	//����ֹͣ�ź�
	LPLD_I2C_Stop(I2C1);

	//��IIC����
	//MMA8451_Delay(50);
	result = LPLD_I2C_ReadByte(I2C1);

	MMA8451_Delay(5000);

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
static void MMA8451_Delay(int t)
{
	int n;
	for (n = 1; n < t; n++)
	{
		asm("nop");
	}
}
