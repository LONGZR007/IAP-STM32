#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f4xx.h"

#define BASIC_TIM           		TIM6
#define BASIC_TIM_CLK       		RCC_APB1Periph_TIM6

#define BASIC_TIM_IRQn					TIM6_DAC_IRQn
#define BASIC_TIM_IRQHandler    TIM6_DAC_IRQHandler


void TIMx_Configuration(void);
void ms_timestamp_disable(void);
void ms_timestamp_enable(void);
uint32_t get_ms_timestamp(void);
void reset_ms_timestamp(void);

#endif /* __BASIC_TIM_H */

