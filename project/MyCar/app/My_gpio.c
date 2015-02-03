#include "My_IIC.h"

//����IO����ں���
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

//��IO����ں���
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
  
  //ʹ��PORT��ʱ��
  SIM_SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK 
    | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK 
      | SIM_SCGC5_PORTE_MASK;
  
//  PORTB_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);//PB0��������ΪGPIOģʽ���½����жϣ����ж�
   
//  PORTB_PCR10 = PORT_PCR_MUX(1);   //PB10��ΪGPIOģʽ����LCD
//  PORTB_PCR11 = PORT_PCR_MUX(1);   //PB11��ΪGPIOģʽ����LCD
//  PORTB_PCR16 = PORT_PCR_MUX(1);   //PB16��ΪGPIOģʽ����LCD
//  PORTB_PCR17 = PORT_PCR_MUX(1);   //PB17��ΪGPIOģʽ����LCD
  
  PORTB_PCR2 = PORT_PCR_MUX(1);    //IOģ��IIC SCL
  PORTB_PCR3 = PORT_PCR_MUX(1);    //IOģ��IIC SDA
  
//  PORTD_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);   //PD1��ΪGPIOģʽ������ת����2���½����ж�
//  PORTD_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(10);   //PD3��ΪGPIOģʽ������ת����1���½����ж�
  
//  PORTD_PCR0 = PORT_PCR_MUX(1);   //PC10��ΪGPIOģʽ������ת����2
//  PORTD_PCR2 = PORT_PCR_MUX(1);   //PC11��ΪGPIOģʽ������ת����1
//  PORTD_PCR4 = PORT_PCR_MUX(1);    //PC12��ΪGPIOģʽ������ת����2
//  PORTD_PCR5 = PORT_PCR_MUX(1);   //PC13��ΪGPIOģʽ������ת����1
  
//  PORTE_PCR0  = PORT_PCR_MUX(1);  //PE0��ΪGPIOģʽ������ͷ�����ź�����
  
//  GPIOA_PDDR |= 0x00008000;  //��A��15����Ϊ1����������Ӻ��İ��ϵ�LED������Ϊ0����Ϊ���롣
//  GPIOB_PDDR |= 0x00030C00;  //PB10 11 16 17��ΪLCD���

}
*/
