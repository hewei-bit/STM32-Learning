#include "adc.h"
#include "delay.h"

//初始化ADC1
//这里我们仅以规则通道为例
//我们默认将开启通道1
void Adc1_init()
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//独立工作模式
	ADC_InitStructure.ADC_NbrOfChannel=ADC_Channel_1;
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}
u16 Get_Adc(u8 ch)
{
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t=0;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}


