#include "usart.h"
#include "rt_misc.h"
#pragma import(__use_no_semihosting_swi)
extern int GetKey(void);
struct __FILE {
int handle; 
};

FILE __stdout;
FILE __stdin;

int SendChar (int ch) 
{
	while((USART1->SR&USART_SR_TXE)==0);
	USART1->DR = (ch & 0x1FF);
	return (ch);
}

int fputc(int ch, FILE *f) 
{
	return (SendChar(ch));
}



void _ttywrch(int ch) 
{
	SendChar (ch);
}

int ferror(FILE *f) 
{ // Your implementation of ferror
	return EOF;
}

void _sys_exit(int return_code) 
{
label: goto label; // endless loop
}


void USART1_Init(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  														//��λ����1
	
	//USART1_TX��������   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 						//PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); 							//��ʼ��PA9

	//USART1���ڳ�ʼ������
	USART_InitStructure.USART_BaudRate = baud;						
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); 						//��ʼ������
 	USART_Cmd(USART1, ENABLE);                    					//ʹ�ܴ��� 
}


