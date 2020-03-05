#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "timer.h"
#include "my_lcd.h"
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "usart.h"	  
#include "sram.h"
#include "malloc.h"
#include "string.h"
#include "usmart.h"	
#include "GUI.h"
#include "GUIDEMO.h"

/************************************************
 ALIENTEKս��STM32������ʵ��37
 STemWin��ֲ ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
void Mytouch_MainTask()
{
	GUI_PID_STATE TouchState;
	int xPhys;
	int yPhys;
	GUI_Init();
	GUI_SetFont(&GUI_Font20_ASCII);
	GUI_CURSOR_Show();
	GUI_CURSOR_Select(&GUI_CursorCrossL);
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_BLACK);
	GUI_Clear();
	GUI_DispString("Measurement of\n A/D converter values ");
	while(1){
		GUI_TOUCH_GetState(&TouchState);
		xPhys = GUI_TOUCH_GetxPhys();
		yPhys = GUI_TOUCH_GetyPhys();
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt("Analog input:\n",0,40);
		GUI_GotoY(GUI_GetDispPosY()+2);
		GUI_DispString("x:");
		GUI_DispDec(xPhys,4);
		GUI_DispString("y:");
		GUI_DispDec(yPhys,4);
		GUI_SetColor(GUI_RED);
		GUI_GotoY(GUI_GetDispPosY()+4);
		GUI_DispString("\nPosition:\n");
		GUI_GotoY(GUI_GetDispPosY()+2);
		GUI_DispString("x:");
		GUI_DispDec(TouchState.x,4);
		GUI_DispString("y:");
		GUI_DispDec(TouchState.y,4);
		delay_ms(50);
	};
}
 
int main(void)
{	 
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	MY_LCD_Init();			   		//��ʼ��LCD   
 	TP_Init();

	 FSMC_SRAM_Init();			//��ʼ���ⲿSRAM  
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);
	GUI_Init();
	GUIDEMO_Main();
//  	Mytouch_MainTask();
	while(1)
	{	
		
	}	   
}



