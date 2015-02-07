#include "MPU6050.h"
#include "hal_i2c.h"
#include "hal_i2c_ex.h"

static void pause(void)
{
	int n;
	for (n = 0; n<400; n++)
		asm("nop");
}

uint8 MPU6050_Init(void)
{
  int i;
  I2C_InitTypeDef i2c_init_param;
	uint8 device_id;

	//初始化I2C0
	i2c_init_param.I2C_I2Cx = I2C0;       //在DEV_MMA8451.h中修改该值
	i2c_init_param.I2C_IntEnable = FALSE;
	i2c_init_param.I2C_ICR = 0x17;  //可根据实际电路更改SCL频率
	i2c_init_param.I2C_SclPin = PTD8;   //MPU
	i2c_init_param.I2C_SdaPin = PTD9;   //MPU
	i2c_init_param.I2C_Isr = NULL;
	//i2c_init_param.I2C_OpenDrainEnable=TRUE;
	LPLD_I2C_Init(i2c_init_param);


         for(i=0;i<100;i++)
   pause();
		 //读取设备ID
		 device_id = hal_dev_mpu6050_read_reg(0x75);
		 hal_dev_mpu6050_write_reg(PWR_MGMT_1, 0x80);
                 for(i=0;i<100;i++)
   pause();
		 hal_dev_mpu6050_write_reg(PWR_MGMT_1, 0x00); //解除休眠状态
		 hal_dev_mpu6050_write_reg(SMPLRT_DIV, 0x04);
		 hal_dev_mpu6050_write_reg(CONFIG, 0x02);//低通的值..低通频率不能太低了..
		 hal_dev_mpu6050_write_reg(GYRO_CONFIG, 0x18);
		 hal_dev_mpu6050_write_reg(ACCEL_CONFIG, 0x00);

		 return device_id;
}



#define I2C_ADDR_mpu6050 (0x68<<1)
#define I2C0_B I2C0_BASE_PTR

/*
This delay is very important, it may cause w-r operation failure.
The delay time is dependent on the current baudrate.
It must be equal or longer than at least on clock of current baudrate.
So, if you set to a higher baudrate, the loop value can be reduced from 4000 to 20, depending on your current baudrate.
*/

u8 hal_dev_mpu6050_read_reg(u8 addr)
{
	u8 result;

	i2c_start(I2C0_B);

	i2c_write_byte(I2C0_B, I2C_ADDR_mpu6050 | I2C_WRITE);
	i2c_wait(I2C0_B);
	i2c_get_ack(I2C0_B);

	i2c_write_byte(I2C0_B, addr);
	i2c_wait(I2C0_B);
	i2c_get_ack(I2C0_B);

	i2c_repeated_start(I2C0_B);
	i2c_write_byte(I2C0_B, I2C_ADDR_mpu6050 | I2C_READ);
	i2c_wait(I2C0_B);
	i2c_get_ack(I2C0_B);

	i2c_set_rx_mode(I2C0_B);

	i2c_give_nack(I2C0_B);
	result = i2c_read_byte(I2C0_B);
	i2c_wait(I2C0_B);

	i2c_stop(I2C0_B);
	result = i2c_read_byte(I2C0_B);
	pause();
	return result;
}
void hal_dev_mpu6050_write_reg(u8 addr, u8 data)
{
	i2c_start(I2C0_B);

	i2c_write_byte(I2C0_B, I2C_ADDR_mpu6050 | I2C_WRITE);
	i2c_wait(I2C0_B);
	i2c_get_ack(I2C0_B);

	i2c_write_byte(I2C0_B, addr);
	i2c_wait(I2C0_B);
	i2c_get_ack(I2C0_B);

	i2c_write_byte(I2C0_B, data);
	i2c_wait(I2C0_B);
	i2c_get_ack(I2C0_B);

	i2c_stop(I2C0_B);
	pause();
}

int16 MPU6050_GetResult(uint8 Regs_Addr)
{

	int16 result, temp;
	result = hal_dev_mpu6050_read_reg(Regs_Addr);
	temp = hal_dev_mpu6050_read_reg(Regs_Addr + 1);
	result = result << 8;
	result = result | temp;

	return result >> 2;
}




