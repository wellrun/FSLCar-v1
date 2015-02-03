#include "My_IIC.h"

//设置IO输出口函数
void set_PORT(unsigned char port, unsigned char num, unsigned char value)
{
  if(value==0)
  {
    if(port==0)
      GPIOA_PDOR &= ~(1<<num);
    else if(port==1)
      GPIOB_PDOR &= ~(1<<num);
    else if(port==2)
      GPIOC_PDOR &= ~(1<<num);
    else if(port==3)
      GPIOD_PDOR &= ~(1<<num);
    else
      GPIOE_PDOR &= ~(1<<num);
  }
  else
  {
    if(port==0)
      GPIOA_PDOR |= 1<<num;
    else if(port==1)
      GPIOB_PDOR |= 1<<num;
    else if(port==2)
      GPIOC_PDOR |= 1<<num;
    else if(port==3)
      GPIOD_PDOR |= 1<<num;
    else
      GPIOE_PDOR |= 1<<num;
  }
}

//读IO输入口函数
unsigned char read_PORT(unsigned char port, unsigned char num)
{
  if(port==0)
    return (1 & (GPIOA_PDIR >> num));
  else if(port==1)
    return (1 & (GPIOB_PDIR >> num));
  else if(port==2)
    return (1 & (GPIOC_PDIR >> num));
  else if(port==3)
    return (1 & (GPIOD_PDIR >> num));
  else
    return (1 & (GPIOE_PDIR >> num));
}

/*
void init_GPIO(void)
{
  //unsigned char *prio_reg;
  
  //使能PORT口时钟
  SIM_SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK 
    | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK 
      | SIM_SCGC5_PORTE_MASK;
  
//  PORTB_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);//PB0引脚设置为GPIO模式，下降沿中断，场中断
   
//  PORTB_PCR10 = PORT_PCR_MUX(1);   //PB10设为GPIO模式，接LCD
//  PORTB_PCR11 = PORT_PCR_MUX(1);   //PB11设为GPIO模式，接LCD
//  PORTB_PCR16 = PORT_PCR_MUX(1);   //PB16设为GPIO模式，接LCD
//  PORTB_PCR17 = PORT_PCR_MUX(1);   //PB17设为GPIO模式，接LCD
  
  PORTB_PCR2 = PORT_PCR_MUX(1);    //IO模拟IIC SCL
  PORTB_PCR3 = PORT_PCR_MUX(1);    //IO模拟IIC SDA
  
//  PORTD_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);   //PD1设为GPIO模式，接旋转开关2，下降沿中断
//  PORTD_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);   //PD3设为GPIO模式，接旋转开关1，下降沿中断
  
//  PORTD_PCR0 = PORT_PCR_MUX(1);   //PC10设为GPIO模式，接旋转开关2
//  PORTD_PCR2 = PORT_PCR_MUX(1);   //PC11设为GPIO模式，接旋转开关1
//  PORTD_PCR4 = PORT_PCR_MUX(1);    //PC12设为GPIO模式，接旋转开关2
//  PORTD_PCR5 = PORT_PCR_MUX(1);   //PC13设为GPIO模式，接旋转开关1
  
//  PORTE_PCR0  = PORT_PCR_MUX(1);  //PE0设为GPIO模式，摄像头亮度信号输入
  
//  GPIOA_PDDR |= 0x00008000;  //将A口15脚设为1，即输出，接核心板上的LED。若设为0，则为输入。
//  GPIOB_PDDR |= 0x00030C00;  //PB10 11 16 17设为LCD输出

}
*/
