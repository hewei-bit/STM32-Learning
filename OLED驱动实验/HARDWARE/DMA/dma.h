#ifndef __DMA_H
#define __DMA_H
#include "sys.h"


/*GPIO*/
#define ADCx_PORT_CLK  		RCC_APB2Periph_GPIOA
#define ADCx_PIN  				GPIO_Pin_1
#define ADCx_PORT					GPIOA

/*ADC*/
#define ADC_x							ADC1
#define ADCx_CLK  		    RCC_APB2Periph_ADC1
#define ADCx_CHx 		    	ADC_Channel_1

/*DMA*/
#define ADCx_DMA_CLK  		RCC_AHBPeriph_DMA1
#define ADCx_DMA_CHx 		  DMA1_Channel1

void ADCx_DMA_Config(void);


void MYDMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//≈‰÷√DMA1_CHx

void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);// πƒ‹DMA1_CHx
	





#endif


