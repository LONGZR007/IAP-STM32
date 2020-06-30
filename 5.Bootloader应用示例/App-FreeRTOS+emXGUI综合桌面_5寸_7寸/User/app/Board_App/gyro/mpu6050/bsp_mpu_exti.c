/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   陀螺仪数据中断配置
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./mpu6050/bsp_mpu_exti.h"

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = MPU_INT_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
}

 /**
  * @brief  配置中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_MPU_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(MPU_INT_GPIO_CLK ,ENABLE);
  
  /* 使能 SYSCFG 时钟 ，使用GPIO外部中断时必须使能SYSCFG时钟*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = MPU_INT_GPIO_PIN;       /*选择引脚*/ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    			/*设置引脚为输入模式*/ 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      /*设置引脚为上拉*/
  GPIO_Init(MPU_INT_GPIO_PORT, &GPIO_InitStructure);            /*使用上面的结构体初始化*/

	/* 连接 EXTI 中断源 */
  SYSCFG_EXTILineConfig(MPU_INT_EXTI_PORTSOURCE, MPU_INT_EXTI_PINSOURCE);

  /* 选择 EXTI 中断源 */
  EXTI_InitStructure.EXTI_Line = MPU_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  

  /* 配置 NVIC */
  NVIC_Configuration();
}

/**
* @brief  禁用中断口
* @param  无
* @retval 无
*/
void EXTI_MPU_Disable(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* 选择 EXTI 中断源 */
  EXTI_InitStructure.EXTI_Line = MPU_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* 配置 NVIC */
  NVIC_InitStructure.NVIC_IRQChannel = MPU_INT_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**************************************中断服务函数********************************************/
extern void gyro_data_ready_cb(void);

void MPU_IRQHandler(void)
{
	
	if(EXTI_GetITStatus(MPU_INT_EXTI_LINE) != RESET) //确保是否产生了EXTI Line中断
	{
		/* Handle new gyro*/
		gyro_data_ready_cb();
	
		EXTI_ClearITPendingBit(MPU_INT_EXTI_LINE);     //清除中断标志位
	}  
}

/*********************************************END OF FILE**********************/
