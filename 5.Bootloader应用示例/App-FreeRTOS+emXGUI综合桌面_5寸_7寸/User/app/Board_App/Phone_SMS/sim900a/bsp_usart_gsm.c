/**
  ******************************************************************************
  * @file    bsp_usart_gsm.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */   
#include "bsp_usart_gsm.h"
#include <stdarg.h>
#include "emXGUI_Arch.h"

 /**
  * @brief  GSM_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void GSM_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd( GSM_USART_RX_GPIO_CLK|GSM_USART_TX_GPIO_CLK, ENABLE);

  /* Enable UART clock */
  GSM_USART_CLKCMD(GSM_USART_CLK, ENABLE);
  
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GSM_USART_RX_GPIO_PORT,GSM_USART_RX_SOURCE, GSM_USART_RX_AF);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(GSM_USART_TX_GPIO_PORT,GSM_USART_TX_SOURCE,GSM_USART_TX_AF);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = GSM_USART_TX_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Pin = GSM_USART_RX_PIN;
  GPIO_Init(GSM_USART_RX_GPIO_PORT, &GPIO_InitStructure);
			
  /* USART1 mode config */
  USART_InitStructure.USART_BaudRate = GSM_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(GSM_USART, &USART_InitStructure); 
  
  USART_Cmd(GSM_USART, ENABLE);	
	USART_ClearFlag(GSM_USART, USART_FLAG_TC);  

  /* Configure the NVIC Preemption Priority Bits */
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  /* Enable the USARTy Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = GSM_USART_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
  /* 使能串口2接收中断 */
    
	USART_ITConfig(GSM_USART, USART_IT_RXNE, ENABLE);
  /* 使能串口空闲中断 */
  USART_ITConfig(GSM_USART, USART_IT_IDLE, ENABLE);
  
}


/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART2_printf()调用
 */
static char *gsm_itoa(int value, char *string, int radix)
{
  int i, d;
  int flag = 0;
  char *ptr = string;
  /* This implementation only works for decimal numbers. */
  if (radix != 10)
  {
    *ptr = 0;
    return string;
  }
  if (!value)
  {
    *ptr++ = 0x30;
    *ptr = 0;
    return string;
  }
  /* if this is a negative value insert the minus sign. */
  if (value < 0)
  {
    *ptr++ = '-';
    /* Make the value positive. */
    value *= -1;
  }
  for (i = 10000; i > 0; i /= 10)
  {
    d = value / i;
    if (d || flag)
    {
      *ptr++ = (char)(d + 0x30);
      value -= (d * i);
      flag = 1;
    }
  }
  /* Null terminate the string. */
  *ptr = 0;
  return string;
} /* NCL_Itoa */

/* 声明外部变量 */
extern GUI_SEM *Call_Sem;
extern uint8_t Sim900aReceiveAcc;    // 接收完成

//中断缓存串口数据
#define UART_BUFF_SIZE      500
volatile    uint16_t gsm_pBuffer = 0;
uint8_t     gsm_usart_buf[UART_BUFF_SIZE] __EXRAM;
void GSM_USART_IRQHandler(void)
{
  if(gsm_pBuffer<UART_BUFF_SIZE)
  {
    if(USART_GetITStatus(GSM_USART, USART_IT_RXNE) != RESET)
    {
        gsm_usart_buf[gsm_pBuffer] = USART_ReceiveData(GSM_USART);
        gsm_pBuffer++;
    }
  }

  if(USART_GetITStatus(GSM_USART, USART_IT_IDLE) != RESET)    // 产生空闲中断
  {
    USART_ReceiveData(GSM_USART);
    GUI_SemPostISR(Call_Sem);
    USART_ClearITPendingBit(GSM_USART, USART_IT_IDLE);    // 清除中断挂起位
    USART_ClearFlag(GSM_USART, USART_IT_IDLE);            // 清除空闲中断
    Sim900aReceiveAcc=1;
  }
}

//获取接收到的数据和长度
char *gsm_get_rebuff(uint16_t *len)
{
    *len = gsm_pBuffer;
    return (char *)&gsm_usart_buf;
}
//清空数据
void gsm_clean_rebuff(void)
{
	uint16_t i=UART_BUFF_SIZE+1;
  gsm_pBuffer = 0;
	while(i) 
  {
    gsm_usart_buf[--i]=0;
  }
}

/*
 * 函数名：GSM_USART_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里用到了GSM_USART
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用GSM_USART_printf( GSM_USART, "\r\n this is a demo \r\n" );
 *            		 GSM_USART_printf( GSM_USART, "\r\n %d \r\n", i );
 *            		 GSM_USART_printf( GSM_USART, "\r\n %s \r\n", j );
 */
void GSM_USART_printf(USART_TypeDef* USARTx, char *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;
				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          gsm_itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
	}
}

/*********************************************END OF FILE**********************/
