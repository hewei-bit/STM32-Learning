#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "led.h"
#include "bmp.h"
#include "adc.h"
#include "dma.h"

/************************************************
 ALIENTEK 战舰STM32F103开发板实验0
 工程模板
 注意，这是手册中的新建工程章节使用的main文件 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

#define pi 3.1415926535
#define accur 0.015295
//accur=18*3.3/4096（3.3/4096就是ADC采样精度，18是为了让波形转化一下能够显示在适当位子）
extern uint16_t ConvData;//ADC采样数据
extern unsigned char BMP1[];
int main(void)
{	

//	OLED_ShowCHinese(0,0,0);//安
//	OLED_ShowCHinese(16,0,1);//全
//	OLED_ShowCHinese(32,0,2);//科
//	OLED_ShowCHinese(48,0,3);//学
//	OLED_ShowCHinese(64,0,4);//与
//	OLED_ShowCHinese(80,0,5);//工
//OLED_ShowCHinese(0,0,6);//程


	u8 t,x;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
 	LED_Init();			     //LED端口初始化
	OLED_Init();			//初始化OLED  
	Adc1_init();
	ADCx_DMA_Config();
	uart_init(115200);
	Before_State_Update(accur*ConvData);
	OLED_Clear();
//	while(1)
//	{
//		for(x=0;x<128;x=(x+1)%128)
//		{
//			OLED_DrawWave(x,accur*ConvData);//这是个画波形的函数
//			//之前写了个画点函数，显示的波形不连续，然后我就改了一下画点函数，波形在屏幕上就连续了
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
  
	OLED_Refresh_Gram();		//更新显示到OLED 
	t=' ';  
	while(1) 
	{		
		OLED_ShowChar(48,0,t,16,1);//显示ASCII字符	   
		OLED_Refresh_Gram();
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,0,t,3,12);//显示ASCII字符的码值 
		delay_ms(500);
		//OLED_Fill(0,0,12,12,1);
		delay_ms(1000);		
		LED0=!LED0;
	}	
}
