#ifndef __MPU_EXTI_H
#define	__MPU_EXTI_H

#include "stm32f4xx.h"


#define MPU_INT_GPIO_PORT                GPIOI
#define MPU_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOI
#define MPU_INT_GPIO_PIN                 GPIO_Pin_1
#define MPU_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOI
#define MPU_INT_EXTI_PINSOURCE           EXTI_PinSource1
#define MPU_INT_EXTI_LINE                EXTI_Line1
#define MPU_INT_EXTI_IRQ                 EXTI1_IRQn

#define MPU_IRQHandler                   EXTI1_IRQHandler

void EXTI_MPU_Config(void);
void EXTI_MPU_Disable(void);

#endif /* __EXTI_H */
