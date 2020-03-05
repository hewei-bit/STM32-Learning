#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "led.h"
#include "bmp.h"
#include "adc.h"
#include "dma.h"

/************************************************
 ALIENTEK ս��STM32F103������ʵ��0
 ����ģ��
 ע�⣬�����ֲ��е��½������½�ʹ�õ�main�ļ� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

#define pi 3.1415926535
#define accur 0.015295
//accur=18*3.3/4096��3.3/4096����ADC�������ȣ�18��Ϊ���ò���ת��һ���ܹ���ʾ���ʵ�λ�ӣ�
extern uint16_t ConvData;//ADC��������
extern unsigned char BMP1[];
int main(void)
{	

//	OLED_ShowCHinese(0,0,0);//��
//	OLED_ShowCHinese(16,0,1);//ȫ
//	OLED_ShowCHinese(32,0,2);//��
//	OLED_ShowCHinese(48,0,3);//ѧ
//	OLED_ShowCHinese(64,0,4);//��
//	OLED_ShowCHinese(80,0,5);//��
//OLED_ShowCHinese(0,0,6);//��


	u8 t,x;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	LED_Init();			     //LED�˿ڳ�ʼ��
	OLED_Init();			//��ʼ��OLED  
	Adc1_init();
	ADCx_DMA_Config();
	uart_init(115200);
	Before_State_Update(accur*ConvData);
	OLED_Clear();
//	while(1)
//	{
//		for(x=0;x<128;x=(x+1)%128)
//		{
//			OLED_DrawWave(x,accur*ConvData);//���Ǹ������εĺ���
//			//֮ǰд�˸����㺯������ʾ�Ĳ��β�������Ȼ���Ҿ͸���һ�»��㺯������������Ļ�Ͼ�������
//		}
//		LED0=!LED0;
//	}
//    
	OLED_ShowChinese2424(0,16,1,12);
	OLED_ShowChinese2424(16,16,1,16);
	OLED_ShowString(32,16,"HeWei",16);  
	OLED_ShowString(0,34, "OLED TEST",12);  
 	OLED_ShowString(0,48,"2020/02/27",12);  
 
 	OLED_ShowString(0,0,"ASCII:",12);  
 	OLED_ShowString(64,0,"CODE:",12);  
  
	OLED_Refresh_Gram();		//������ʾ��OLED 
	t=' ';  
	while(1) 
	{		
		OLED_ShowChar(48,0,t,16,1);//��ʾASCII�ַ�	   
		OLED_Refresh_Gram();
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,0,t,3,12);//��ʾASCII�ַ�����ֵ 
		delay_ms(500);
		//OLED_Fill(0,0,12,12,1);
		delay_ms(1000);		
		LED0=!LED0;
	}	
}
