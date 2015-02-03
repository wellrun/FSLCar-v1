#include "common.h"
#include <math.h>

/*#define SDA_OUT() GPIOC_PDDR|=1<<11
#define SDA_IN() GPIOC_PDDR&=~(1<<11)
#define SCL_OUT() GPIOC_PDDR|=1<<10

#define IIC_SDA(val) set_PORT(2,11,val)
#define IIC_SCL(val) set_PORT(2,10,val)
#define READ_SDA() read_PORT(2,11)
#define IIC_DelayUs(x)  DelayUs(x)
#define IIC_DelayMs(x)  DelayMs(x)*/

#define SDA_OUT() GPIOD_PDDR|=1<<9
#define SDA_IN() GPIOD_PDDR&=~(1<<9)
#define SCL_OUT() GPIOD_PDDR|=1<<8

#define IIC_SDA(val) set_PORT(3,9,val)
#define IIC_SCL(val) set_PORT(3,8,val)
#define READ_SDA() read_PORT(3,9)
#define IIC_DelayUs(x)  DelayUs(x)
#define IIC_DelayMs(x)  DelayMs(x)

#define SLAVE_WRITE_ADDRESS 0x18
#define SLAVE_READ_ADDRESS 0x19

#define HXL 0x03
#define HXH 0x04
#define HYL 0x05
#define HYH 0x06
#define HZL 0x07
#define HZH 0x08

void send_client(void);
void init_I2C(void);
//void init_MPU9150(void);
//void init_UART(void);
void set_PORT(unsigned char port, unsigned char num, unsigned char value);
unsigned char read_PORT(unsigned char port, unsigned char num);
void DelayUs(unsigned int us);
void waitms(unsigned long time);
/*iic的基本实现函数*/
void DelayUs(unsigned int us);
void DelayMs(unsigned int ms);
void get_ms(unsigned long *time);
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);

unsigned char IIC_RecvACK(void);
void IIC_SendByte(unsigned char dat);
uint8 IIC_ReadByte(void);
char i2cWriteBuffer(unsigned char addr, unsigned char reg, unsigned char len, unsigned char * data);
char i2cRead(unsigned char addr, unsigned char reg, unsigned char len, unsigned char *buf);
//unsigned char HMC5883_RecvByte(void);
void HMC5883_SendByte(unsigned char dat);
