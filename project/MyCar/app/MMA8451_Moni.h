#ifndef _MMA_H_
#define _MMA_H_
#include "My_IIC.h"

//==========MMA8451 寄存器地址==================//
#define MMA8451_REG_STATUS         0x00    //状态寄存器
#define MMA8451_REG_OUTX_MSB       0x01    //14位采样结果高8位[7:0]
#define MMA8451_REG_OUTX_LSB       0x02    //14位采样结果低6位[7:2]
#define MMA8451_REG_OUTY_MSB       0x03    //14位采样结果高8位[7:0]
#define MMA8451_REG_OUTY_LSB       0x04    //14位采样结果低6位[7:2]
#define MMA8451_REG_OUTZ_MSB       0x05    //14位采样结果高8位[7:0]
#define MMA8451_REG_OUTZ_LSB       0x06    //14位采样结果低6位[7:2]
#define MMA8451_REG_SYSMOD         0x0B    //当前系统模式
#define MMA8451_REG_WHOAMI         0x0D    //设备ID寄存器
#define MMA8451_REG_CTRL_REG1      0x2A    //系统控制寄存器1
#define MMA8451_REG_CTRL_REG2      0x2B    //系统控制寄存器2

//=========MMA8451 功能参数==================//
#define MMA8451_DEV_ADDR   0x1C //Normally,can range 0x08 to 0xEF
#define MMA8451_DEV_WRITE  MMA8451_DEV_ADDR<<1 | 0x00
#define MMA8451_DEV_READ   MMA8451_DEV_ADDR<<1 | 0x01
#define MMA8451_DEV_ID     0x1A   //MMA8451设备ID

//=========MMA8451 STATUS寄存器 寄存器位==================//
#define MMA8451_STATUS_X_READY   0x01
#define MMA8451_STATUS_Y_READY   0x02
#define MMA8451_STATUS_Z_READY   0x04
#define MMA8451_STATUS_XYZ_READY 0x08
#define MMA8451_STATUS_X_OW      0x10
#define MMA8451_STATUS_Y_OW      0x20
#define MMA8451_STATUS_Z_OW      0x40
#define MMA8451_STATUS_XYZ_OW    0x80

//定义SCL Bus Speed取值，外设总线为50Mhz时的计算结果
#define MMA8451_SCL_50KHZ                   (0x33) 
#define MMA8451_SCL_100KHZ                  (0x2B)  
#define MMA8451_SCL_150KHZ                  (0x28)
#define MMA8451_SCL_200KHZ                  (0x23)
#define MMA8451_SCL_250KHZ                  (0x21)
#define MMA8451_SCL_300KHZ                  (0x20)
#define MMA8451_SCL_400KHZ                  (0x17)  

//函数声明
uint8 MMA8451_Inital(void);
uint8 MMA8451_ReadByte(unsigned char address);
void MMA8451_WriteByte(unsigned char address, unsigned char thedata);




#endif