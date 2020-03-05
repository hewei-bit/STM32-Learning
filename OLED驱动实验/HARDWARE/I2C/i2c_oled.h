#ifndef __I2C_OLED_H
#define __I2C_OLED_H

#include "sys.h"
//����IO����
#define SDA_IN() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define SDA_OUT(){GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}
//IO������
#define IIC_SCL  PBout(6)
#define IIC_SDA  PBout(7)
#define READ_SDA PBin(7)

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 
void IIC_Write_Command(unsigned char IIC_Command);
void IIC_Write_Data(unsigned char IIC_Data);


#endif

