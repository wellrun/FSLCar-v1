#ifndef __HAL_I2C_EX__
#define __HAL_I2C_EX__

typedef unsigned int  uint;
typedef unsigned char byte;
typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef int   s32;
typedef short s16;
typedef char  s8;

typedef unsigned int bool;
#define true  1
#define false 0

#define I2C_READ  1
#define I2C_WRITE 0



void i2c_set_tx_mode(I2C_MemMapPtr p);
void i2c_set_rx_mode(I2C_MemMapPtr p);
void i2c_set_slave_mode(I2C_MemMapPtr p);
void i2c_set_master_mode(I2C_MemMapPtr p);
void i2c_give_nack(I2C_MemMapPtr p);
void i2c_give_ack(I2C_MemMapPtr p);
void i2c_repeated_start(I2C_MemMapPtr p);
void i2c_write_byte(I2C_MemMapPtr p, byte data);
byte i2c_read_byte(I2C_MemMapPtr p);
void i2c_start(I2C_MemMapPtr p);
void i2c_stop(I2C_MemMapPtr p);
void i2c_wait(I2C_MemMapPtr p);
bool i2c_get_ack(I2C_MemMapPtr p);
void hal_i2c_init(void);



#endif

