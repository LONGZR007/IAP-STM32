/**
  ******************************************************************************
  * @file    bsp_i2c_touch.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   电容触摸屏的专用iic驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./touch/bsp_i2c_touch.h"
#include "./touch/bsp_touch_gtxx.h"
#include "./usart/bsp_debug_usart.h"

/* STM32 I2C 快速模式 */
#define I2C_Speed              400000

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C_OWN_ADDRESS7      0x0A



static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}


/**
  * @brief  使能触摸屏中断
  * @param  无
  * @retval 无
  */
void I2C_GTP_IRQEnable(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;  
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;  
  /*配置 INT 为浮空输入 */   
  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);  
  
  /* 连接 EXTI 中断源 到INT 引脚 */
  SYSCFG_EXTILineConfig(GTP_INT_EXTI_PORTSOURCE, GTP_INT_EXTI_PINSOURCE);

  /* 选择 EXTI 中断源 */
  EXTI_InitStructure.EXTI_Line = GTP_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);  
  
  /* 配置中断优先级 */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
	/*使能中断*/
  NVIC_InitStructure.NVIC_IRQChannel = GTP_INT_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/**
  * @brief  关闭触摸屏中断
  * @param  无
  * @retval 无
  */
void I2C_GTP_IRQDisable(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  /*配置 INT 为浮空输入 */   
  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);

  /* 连接 EXTI 中断源 到INT 引脚 */
  SYSCFG_EXTILineConfig(GTP_INT_EXTI_PORTSOURCE, GTP_INT_EXTI_PINSOURCE);

  /* 选择 EXTI 中断源 */
  EXTI_InitStructure.EXTI_Line = GTP_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* 配置中断优先级 */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* 关闭中断 */
  NVIC_InitStructure.NVIC_IRQChannel = GTP_INT_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/**
  * @brief  触摸屏 I/O配置
  * @param  无
  * @retval 无
  */
static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
   
  /*使能I2C时钟 */
  GTP_I2C_CLK_INIT(GTP_I2C_CLK, ENABLE);
  
  /*使能触摸屏使用的引脚的时钟*/
  RCC_AHB1PeriphClockCmd(GTP_I2C_SCL_GPIO_CLK|
                       	 GTP_I2C_SDA_GPIO_CLK|
	                       GTP_RST_GPIO_CLK|GTP_INT_GPIO_CLK, 
	                       ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

#if !(SOFT_IIC)   //使用硬件IIC     
    /* 配置I2C_SCL源*/
    GPIO_PinAFConfig(GTP_I2C_SCL_GPIO_PORT, 
                     GTP_I2C_SCL_SOURCE, GTP_I2C_SCL_AF);
    /* 配置I2C_SDA 源*/
    GPIO_PinAFConfig(GTP_I2C_SDA_GPIO_PORT, 
                     GTP_I2C_SDA_SOURCE, GTP_I2C_SDA_AF);  
    
    /*配置SCL引脚 */   
    GPIO_InitStructure.GPIO_Pin = GTP_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GTP_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    /*配置SDA引脚 */
    GPIO_InitStructure.GPIO_Pin = GTP_I2C_SDA_PIN;
    GPIO_Init(GTP_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

#else  //使用软件IIC
    /*配置SCL引脚 */   
    GPIO_InitStructure.GPIO_Pin = GTP_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GTP_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    /*配置SDA引脚 */
    GPIO_InitStructure.GPIO_Pin = GTP_I2C_SDA_PIN;
    GPIO_Init(GTP_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
#endif
 
  /*配置RST引脚，下拉推挽输出 */   
  GPIO_InitStructure.GPIO_Pin = GTP_RST_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_Init(GTP_RST_GPIO_PORT, &GPIO_InitStructure);
  
  /*配置 INT引脚，下拉推挽输出，方便初始化 */   
  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//设置为下拉，方便初始化
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);
}


/**
  * @brief  对GT91xx芯片进行复位
  * @param  无
  * @retval 无
  */
void I2C_ResetChip(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;

  /*配置 INT引脚，下拉推挽输出，方便初始化 */   
	  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;       //设置为下拉，方便初始化
	  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);

	  /*初始化GT9157,rst为高电平，int为低电平，则gt9157的设备地址被配置为0xBA*/

	  /*复位为低电平，为初始化做准备*/
	  GPIO_ResetBits (GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN);
	  Delay(0x0FFFFF);

	  /*拉高一段时间，进行初始化*/
	  GPIO_SetBits (GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN);
	  Delay(0x0FFFFF);

	  /*把INT引脚设置为浮空输入模式，以便接收触摸中断信号*/
	  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  I2C 工作模式配置
  * @param  无
  * @retval 无
  */
 void I2C_Mode_Config(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 

  /* I2C 模式配置 */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;	
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		                    
  I2C_InitStructure.I2C_OwnAddress1 =I2C_OWN_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;	
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	/* I2C的寻址模式 */
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;	                          /* 通信速率 */
  I2C_Init(GTP_I2C, &I2C_InitStructure);	                                  /* I2C1 初始化 */
  I2C_Cmd(GTP_I2C, ENABLE);  	                                              /* 使能 I2C1 */

  I2C_AcknowledgeConfig(GTP_I2C, ENABLE);

}


/**
  * @brief  I2C 外设(GT91xx)初始化
  * @param  无
  * @retval 无
  */
void I2C_Touch_Init(void)
{
  I2C_GPIO_Config(); 
 
#if !(SOFT_IIC) //硬件IIC模式 
    I2C_Mode_Config();
#endif
  
  I2C_ResetChip();

  I2C_GTP_IRQDisable();
}


#if !(SOFT_IIC)   //使用硬件IIC     


__IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;   
static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

/**
  * @brief   使用IIC读取数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:存放由从机读取的数据的缓冲区指针
  *		@arg NumByteToRead:读取的数据长度
  * @retval  无
  */
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead)
{  
    I2CTimeout = I2CT_LONG_TIMEOUT;

    while(I2C_GetFlagStatus(GTP_I2C, I2C_FLAG_BUSY))   
    {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(0);
    }

		/* Send STRAT condition  */
		I2C_GenerateSTART(GTP_I2C, ENABLE);
  
     I2CTimeout = I2CT_FLAG_TIMEOUT;

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(GTP_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(1);
   }
  /* Send GT91xx address for read */
  I2C_Send7bitAddress(GTP_I2C, ClientAddr, I2C_Direction_Receiver);
  
     I2CTimeout = I2CT_FLAG_TIMEOUT;

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(GTP_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(2);
   }
  /* While there is data to be read */
  while(NumByteToRead)  
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(GTP_I2C, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(GTP_I2C, ENABLE);
    }


		I2CTimeout = I2CT_LONG_TIMEOUT;
		while(I2C_CheckEvent(GTP_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
		{
			if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
		} 	
		{
		  /* Read a byte from the device */
      *pBuffer = I2C_ReceiveData(GTP_I2C);

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;
		}			

		
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(GTP_I2C, ENABLE);
  
  return 0;
}


/**
  * @brief   使用IIC写入数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:缓冲区指针
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite)
{
  
  I2CTimeout = I2CT_LONG_TIMEOUT;

  while(I2C_GetFlagStatus(GTP_I2C, I2C_FLAG_BUSY))  
   {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(4);
  } 
  
  /* Send START condition */
  I2C_GenerateSTART(GTP_I2C, ENABLE);
  
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(GTP_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(5);
  } 
  
  /* Send GT91xx address for write */
  I2C_Send7bitAddress(GTP_I2C, ClientAddr, I2C_Direction_Transmitter);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(GTP_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(6);
  } 
  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(GTP_I2C, *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(GTP_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(7);
    } 
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(GTP_I2C, ENABLE);
  
  return 0;  

}


/**
  * @brief  IIC等待超时调用本函数输出调试信息
  * @param  None.
  * @retval 返回0xff，表示IIC读取数据失败
  */
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  GTP_ERROR("I2C 等待超时!errorCode = %d",errorCode);
  
  return 0xFF;
}

#else //使用软件IIC

/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频180MHz ，MDK编译环境，1级优化
      
		循环次数为50时，SCL频率 = 333KHz 
		循环次数为30时，SCL频率 = 533KHz，  
	 	循环次数为20时，SCL频率 = 727KHz， 
  */
	for (i = 0; i < 10*5; i++);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();	
		I2C_SCL_0();
		if (i == 7)
		{
			 I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_0();
	i2c_Delay();
	return re;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();	/* CPU释放SDA总线 */
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();	
}



#define I2C_DIR_WR	0		/* 写控制bit */
#define I2C_DIR_RD	1		/* 读控制bit */

/**
  * @brief   使用IIC读取数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:存放由从机读取的数据的缓冲区指针
  *		@arg NumByteToRead:读取的数据长度
  * @retval  无
  */
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead)
{
	
	/* 第1步：发起I2C总线启动信号 */
	i2c_Start();
	
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(ClientAddr | I2C_DIR_RD);	/* 此处是读指令 */
	
	/* 第3步：等待ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* 器件无应答 */
	}

	while(NumByteToRead) 
  {
    *pBuffer = i2c_ReadByte();
    
    /* 读指针自增 */
    pBuffer++; 
      
    /*计数器自减 */
    NumByteToRead--;
       
    if(NumByteToRead == 0)
			i2c_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */      
    else
      i2c_Ack();	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */  
  }

	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 0;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 1;
}

/**
  * @brief   使用IIC写入数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:缓冲区指针
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite)
{
	uint16_t m;	

  /*　第0步：发停止信号，启动内部写操作　*/
  i2c_Stop();
  
  /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms 			
    CLK频率为200KHz时，查询次数为30次左右
  */
  for (m = 0; m < 1000; m++)
  {				
    /* 第1步：发起I2C总线启动信号 */
    i2c_Start();
    
    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(ClientAddr | I2C_DIR_WR);	/* 此处是写指令 */
    
    /* 第3步：发送一个时钟，判断器件是否正确应答 */
    if (i2c_WaitAck() == 0)
    {
      break;
    }
  }
  if (m  == 1000)
  {
    goto cmd_fail;	/* EEPROM器件写超时 */
  }	
	
  while(NumByteToWrite--)
  {
  /* 第4步：开始写入数据 */
  i2c_SendByte(*pBuffer);

  /* 第5步：检查ACK */
  if (i2c_WaitAck() != 0)
  {
    goto cmd_fail;	/* 器件无应答 */
  }
  
      pBuffer++;	/* 地址增1 */		
  }
	
	/* 命令执行成功，发送I2C总线停止信号 */
	i2c_Stop();
	return 0;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 1;
}

#endif



/*********************************************END OF FILE**********************/
