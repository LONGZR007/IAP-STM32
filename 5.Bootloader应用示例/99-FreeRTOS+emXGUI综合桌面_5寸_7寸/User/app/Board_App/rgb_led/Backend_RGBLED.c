#include "emXGUI.h"
#include "x_libc.h"
#include "./led/bsp_led.h"  


#define PWM_LEDR_GPIO_PORT         GPIOH
#define PWM_LEDR_GPIO_PIN          GPIO_Pin_10
#define PWM_LEDR_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define PWM_LEDR_PINSOURCE         GPIO_PinSource10
#define PWM_LEDR_AF                GPIO_AF_TIM5

#define PWM_LEDG_GPIO_PORT         GPIOH
#define PWM_LEDG_GPIO_PIN          GPIO_Pin_11
#define PWM_LEDG_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define PWM_LEDG_PINSOURCE         GPIO_PinSource11
#define PWM_LEDG_AF                GPIO_AF_TIM5

#define PWM_LEDB_GPIO_PORT         GPIOH
#define PWM_LEDB_GPIO_PIN          GPIO_Pin_12
#define PWM_LEDB_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define PWM_LEDB_PINSOURCE         GPIO_PinSource12
#define PWM_LEDB_AF                GPIO_AF_TIM5

#define PWM_LEDRGB_TIM             TIM5
#define PWM_LEDRGB_TIM_CLK         RCC_APB1Periph_TIM5

 /**
  * @brief  配置TIM3复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
void TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  /* GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(PWM_LEDR_GPIO_CLK|PWM_LEDG_GPIO_CLK|PWM_LEDB_GPIO_CLK, ENABLE); 
  
  GPIO_PinAFConfig(PWM_LEDR_GPIO_PORT,PWM_LEDR_PINSOURCE,PWM_LEDR_AF); 
  GPIO_PinAFConfig(PWM_LEDG_GPIO_PORT,PWM_LEDG_PINSOURCE,PWM_LEDG_AF); 
  GPIO_PinAFConfig(PWM_LEDB_GPIO_PORT,PWM_LEDB_PINSOURCE,PWM_LEDB_AF); 
  
  /* 配置呼吸灯用到的PB0引脚 */
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(PWM_LEDR_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDG_GPIO_PIN;
  GPIO_Init(PWM_LEDG_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDB_GPIO_PIN;
  GPIO_Init(PWM_LEDB_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  配置TIM3输出的PWM信号的模式，如周期、极性
  * @param  无
  * @retval 无
  */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> 中断 & TIMxCNT 重新计数
 *                    TIMx_CCR(电平发生变化)
 *
 * 信号周期=(TIMx_ARR +1 ) * 时钟周期
 * 
 */
/*    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 */
void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	/* 设置TIM3CLK 时钟为72MHZ */
   RCC_APB1PeriphClockCmd(PWM_LEDRGB_TIM_CLK, ENABLE); 					//使能TIM5时钟

   /* 基本定时器配置 */		 
   TIM_TimeBaseStructure.TIM_Period = 255;       							  //当定时器从0计数到255，即为266次，为一个定时周期
   TIM_TimeBaseStructure.TIM_Prescaler = 2499;	    							//设置预分频：
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//设置时钟分频系数：不分频(这里用不到)
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
   TIM_TimeBaseInit(PWM_LEDRGB_TIM, &TIM_TimeBaseStructure);

   /* PWM模式配置 */
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//配置为PWM模式1
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
   TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //当定时器计数值小于CCR1_Val时为低电平
 
   TIM_OC1Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//使能通道3
   TIM_OC1PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//使能预装载	

   TIM_OC2Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//使能通道3
   TIM_OC2PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//使能预装载	
  
   TIM_OC3Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//使能通道3
   TIM_OC3PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//使能预装载	
  
   TIM_ARRPreloadConfig(PWM_LEDRGB_TIM, ENABLE);			 										//使能TIM5重载寄存器ARR
 
   /* TIM5 enable counter */
   TIM_Cmd(PWM_LEDRGB_TIM, ENABLE);                   										//使能定时器5
	
}

//RGBLED显示颜色
void SetRGBColor(uint32_t rgb)
{
	uint8_t r=0,g=0,b=0;
	r=(uint8_t)(rgb>>16);
	g=(uint8_t)(rgb>>8);
	b=(uint8_t)rgb;
	PWM_LEDRGB_TIM->CCR1 = r;	//根据PWM表修改定时器的比较寄存器值
	PWM_LEDRGB_TIM->CCR2 = g;	//根据PWM表修改定时器的比较寄存器值        
	PWM_LEDRGB_TIM->CCR3 = b;	//根据PWM表修改定时器的比较寄存器值
}

//RGBLED显示颜色
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	PWM_LEDRGB_TIM->CCR1 = r;	//根据PWM表修改定时器的比较寄存器值
	PWM_LEDRGB_TIM->CCR2 = g;	//根据PWM表修改定时器的比较寄存器值        
	PWM_LEDRGB_TIM->CCR3 = b;	//根据PWM表修改定时器的比较寄存器值
}

//停止pwm输出
void TIM_RGBLED_Close(void)
{
	SetColorValue(0,0,0);
	TIM_ForcedOC1Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC2Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC3Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ARRPreloadConfig(PWM_LEDRGB_TIM, DISABLE);
	TIM_Cmd(PWM_LEDRGB_TIM, DISABLE);                   							//失能定时器3						
	RCC_APB1PeriphClockCmd(PWM_LEDRGB_TIM_CLK, DISABLE); 	//失能定时器3时钟
	LED_GPIO_Config();
}


