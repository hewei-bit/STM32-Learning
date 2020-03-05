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
 ALIENTEK战舰STM32开发板实验37
 STemWin移植 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
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
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	MY_LCD_Init();			   		//初始化LCD   
 	TP_Init();

	 FSMC_SRAM_Init();			//初始化外部SRAM  
	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);
	GUI_Init();
	GUIDEMO_Main();
//  	Mytouch_MainTask();
	while(1)
	{	
		
	}	   
}



