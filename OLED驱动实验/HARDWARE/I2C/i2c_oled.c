#include "i2c_oled.h"
#include "delay.h"
//软件串口驱动I2C
void IIC_Init(void)              //初始化IIC的IO口	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);
}	
//发送IIC开始信号
void IIC_Start(void)				
{
	SDA_OUT();
	IIC_SCL = 1;
	IIC_SDA = 1;
	delay_us(4);
	IIC_SDA = 0;
	delay_us(4);
	IIC_SCL = 0;
}
//发送IIC停止信号
void IIC_Stop(void)	  			
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
//IIC等待ACK信号
u8 IIC_Wait_Ack(void) 				
{
	u8 ucErrTime=0;
	SDA_IN();
	IIC_SDA=1;
	delay_us(1);
	IIC_SCL=1;
	delay_us(1);
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL = 0;
	return 0;
}
//IIC发送ACK信号
void IIC_Ack(void)	
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//IIC不发送ACK信号
void IIC_NAck(void)				
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//IIC发送一个字节
void IIC_Send_Byte(u8 txd)			
{
	u8 t;
	SDA_OUT();
	IIC_SCL=0;
	for(t=0;t<8;t++)
	{
		if((txd&0x80)>>7) IIC_SDA=1;
		else IIC_SDA=0;
		txd<<=1;
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
	}
}
//IIC读取一个字节
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 

void IIC_Write_Command(unsigned char IIC_Command)
{
	IIC_Start();
	IIC_Send_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
	IIC_Send_Byte(0x00);			//write command
	IIC_Wait_Ack();	
	IIC_Send_Byte(IIC_Command); 
	IIC_Wait_Ack();	
	IIC_Stop();
}	
void IIC_Write_Data(unsigned char IIC_Data)
{
	IIC_Start();
	IIC_Send_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
	IIC_Send_Byte(0x40);			//write data
	IIC_Wait_Ack();	
	IIC_Send_Byte(IIC_Data);
	IIC_Wait_Ack();	
	IIC_Stop();
}
