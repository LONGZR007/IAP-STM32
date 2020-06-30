/**
  ******************************************************************************
  * @file    gtxx.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   i2c电容屏驱动函数gt9157/gt911/gt5688芯片
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./touch/bsp_touch_gtxx.h"
#include "./touch/bsp_i2c_touch.h"
#include "./lcd/bsp_lcd.h"
#include "./touch/palette.h"

// 5寸屏GT9157驱动配置
const uint8_t CTP_CFG_GT9157[] ={ 
	0x00,0x20,0x03,0xE0,0x01,0x05,0x3C,0x00,0x01,0x08,
	0x28,0x0C,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x17,0x19,0x1E,0x14,0x8B,0x2B,0x0D,
	0x33,0x35,0x0C,0x08,0x00,0x00,0x00,0x9A,0x03,0x11,
	0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,
	0x00,0x20,0x58,0x94,0xC5,0x02,0x00,0x00,0x00,0x04,
	0xB0,0x23,0x00,0x93,0x2B,0x00,0x7B,0x35,0x00,0x69,
	0x41,0x00,0x5B,0x4F,0x00,0x5B,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,
	0x12,0x14,0x16,0x18,0x1A,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0F,
	0x10,0x12,0x13,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,
	0x21,0x22,0x24,0x26,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0x48,0x01
};

// 7寸屏GT911驱动配置
const uint8_t CTP_CFG_GT911[] =  {
  0x00,0x20,0x03,0xE0,0x01,0x05,0x0D,0x00,0x01,0x08,
  0x28,0x0F,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8A,0x2A,0x0C,
  0x45,0x47,0x0C,0x08,0x00,0x00,0x00,0x02,0x02,0x2D,
  0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x32,0x00,0x00,
  0x00,0x28,0x64,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
  0x9C,0x2C,0x00,0x8F,0x34,0x00,0x84,0x3F,0x00,0x7C,
  0x4C,0x00,0x77,0x5B,0x00,0x77,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x18,0x16,0x14,0x12,0x10,0x0E,0x0C,0x0A,
  0x08,0x06,0x04,0x02,0xFF,0xFF,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,
  0x22,0x24,0x13,0x12,0x10,0x0F,0x0A,0x08,0x06,0x04,
  0x02,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x24,0x01	
};

// 4.3寸屏GT5688驱动配置,注意gt5688第一个参数要写成0x97才会更新配置
const uint8_t CTP_CFG_GT5688[] =  {
0x97,0xE0,0x01,0x10,0x01,0x05,0x0D,0x00,0x01,0x00,
0x00,0x05,0x5A,0x46,0x53,0x11,0x00,0x00,0x11,0x11,
0x14,0x14,0x14,0x22,0x0A,0x04,0x00,0x00,0x00,0x00,
0x00,0x00,0x53,0x00,0x14,0x00,0x00,0x84,0x00,0x00,
0x3C,0x00,0x00,0x64,0x1E,0x28,0x87,0x27,0x08,0x32,
0x34,0x05,0x0D,0x20,0x33,0x60,0x11,0x02,0x24,0x00,
0x00,0x64,0x80,0x80,0x14,0x02,0x00,0x00,0x54,0x89,
0x68,0x85,0x6D,0x82,0x72,0x80,0x76,0x7D,0x7B,0x7B,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x50,0x3C,
0xFF,0xFF,0x07,0x00,0x00,0x00,0x02,0x14,0x14,0x03,
0x04,0x00,0x21,0x64,0x0A,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x32,0x20,0x50,0x3C,0x3C,0x00,0x00,0x00,0x00,0x00,
0x0D,0x06,0x0C,0x05,0x0B,0x04,0x0A,0x03,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x00,0x01,0x02,0x03,0x04,0x05,
0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x3C,0x00,0x05,0x1E,0x00,0x02,
0x2A,0x1E,0x19,0x14,0x02,0x00,0x03,0x0A,0x05,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0x86,
0x22,0x03,0x00,0x00,0x33,0x00,0x0F,0x00,0x00,0x00,
0x50,0x3C,0x50,0x00,0x00,0x00,0x00,0x2A,0x01
};

//5寸屏GT917S驱动配置
uint8_t CTP_CFG_GT917S[] ={ 
  0x97,0x20,0x03,0xE0,0x01,0x0A,0x35,0x04,0x00,0x69,
  0x09,0x0F,0x50,0x32,0x33,0x11,0x00,0x32,0x11,0x11,
  0x28,0x8C,0xAA,0xDC,0x58,0x04,0x00,0x00,0x1E,0x3C,
  0x00,0x00,0x00,0x31,0x00,0x00,0x00,0x00,0x00,0x40,
  0x32,0x00,0x00,0x50,0x38,0x00,0x8D,0x20,0x16,0x4E,
  0x4C,0x7C,0x05,0x28,0x3E,0x28,0x0D,0x43,0x24,0x00,
  0x01,0x39,0x6B,0xC0,0x94,0x84,0x2D,0x00,0x54,0xB0,
  0x41,0x9D,0x49,0x8D,0x52,0x7F,0x5A,0x75,0x62,0x6C,
  0x42,0x50,0x14,0x00,0x00,0x00,0x00,0xF0,0x50,0x3C,
  0x88,0x88,0x27,0x50,0x3C,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x00,0x02,0x78,
  0x0A,0x50,0xFF,0xE4,0x04,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x3C,0xB0,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x56,0xA2,0x07,0x50,0x1E,
  0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
  0x0F,0x10,0x12,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,
  0x1D,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0x1F,0x1E,0x1D,0x1C,0x1B,0x1A,0x19,0x18,
  0x17,0x15,0x14,0x13,0x12,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0x00,0x30,0x7F,0x7F,0x7F,0xFF,
  0x54,0x64,0x00,0x80,0x46,0x07,0x50,0x3C,0x32,0x14,
  0x0A,0x64,0x32,0x00,0x00,0x00,0x00,0x11,0x02,0x62,
  0x32,0x03,0x14,0x50,0x0C,0xE2,0x14,0x50,0x00,0x54,
  0x10,0x00,0x32,0xA2,0x07,0x64,0xA4,0xB6,0x01
};

//uint8_t config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
//                = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};

/* 触摸IC类型默认为5寸屏的ic */
TOUCH_IC touchIC = GT917S;		

const TOUCH_PARAM_TypeDef touch_param[4] = 
{
  /* GT9157,5寸屏 */
  {
    .max_width = 800,
    .max_height = 480,
    .config_reg_addr = 0x8047,
  },
  /* GT911,7寸屏 */
  {
    .max_width = 800,
    .max_height = 480,
    .config_reg_addr = 0x8047,
  },
  /* GT5688,4.3寸屏 */
  {
    .max_width = 480,
    .max_height = 272,
    .config_reg_addr = 0x8050,
  },
  
  /* GT917S,5寸屏 */
  {
    .max_width = 800,
    .max_height = 480,
    .config_reg_addr = 0x8050,
  },
};

static int8_t GTP_I2C_Test(void);
//static void GT91xx_Config_Read_Proc(void);

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}


/**
  * @brief   使用IIC进行数据传输
  * @param
  *		@arg i2c_msg:数据传输结构体
  *		@arg num:数据传输结构体的个数
  * @retval  正常完成的传输结构个数，若不正常，返回0xff
  */
static int I2C_Transfer( struct i2c_msg *msgs,int num)
{
	int im = 0;
	int ret = 0;

	GTP_DEBUG_FUNC();

	for (im = 0; ret == 0 && im != num; im++)
	{
		if ((msgs[im].flags&I2C_M_RD))																//根据flag判断是读数据还是写数据
		{
			ret = I2C_ReadBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);		//IIC读取数据
		} else
		{
			ret = I2C_WriteBytes(msgs[im].addr,  msgs[im].buf, msgs[im].len);	//IIC写入数据
		}
	}

	if(ret)
		return ret;

	return im;   													//正常完成的传输结构个数
}

/**
  * @brief   从IIC设备中读取数据
  * @param
  *		@arg client_addr:设备地址
  *		@arg  buf[0~1]: 读取数据寄存器的起始地址
  *		@arg buf[2~len-1]: 存储读出来数据的缓冲buffer
  *		@arg len:    GTP_ADDR_LENGTH + read bytes count（寄存器地址长度+读取的数据字节数）
  * @retval  i2c_msgs传输结构体的个数，2为成功，其它为失败
  */
static int32_t GTP_I2C_Read(uint8_t client_addr, uint8_t *buf, int32_t len)
{
    struct i2c_msg msgs[2];
    int32_t ret=-1;
    int32_t retries = 0;

    GTP_DEBUG_FUNC();
    /*一个读数据的过程可以分为两个传输过程:
     * 1. IIC  写入 要读取的寄存器地址
     * 2. IIC  读取  数据
     * */

    msgs[0].flags = !I2C_M_RD;					//写入
    msgs[0].addr  = client_addr;					//IIC设备地址
    msgs[0].len   = GTP_ADDR_LENGTH;	//寄存器地址为2字节(即写入两字节的数据)
    msgs[0].buf   = &buf[0];						//buf[0~1]存储的是要读取的寄存器地址
    
    msgs[1].flags = I2C_M_RD;					//读取
    msgs[1].addr  = client_addr;					//IIC设备地址
    msgs[1].len   = len - GTP_ADDR_LENGTH;	//要读取的数据长度
    msgs[1].buf   = &buf[GTP_ADDR_LENGTH];	//buf[GTP_ADDR_LENGTH]之后的缓冲区存储读出的数据

    while(retries < 5)
    {
        ret = I2C_Transfer( msgs, 2);					//调用IIC数据传输过程函数，有2个传输过程
        if(ret == 2)break;
        retries++;
    }
    if((retries >= 5))
    {
        GTP_ERROR("I2C Read: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((uint16_t)(buf[0] << 8)) | buf[1]), len-2, ret);
    }
    return ret;
}



/**
  * @brief   向IIC设备写入数据
  * @param
  *		@arg client_addr:设备地址
  *		@arg  buf[0~1]: 要写入的数据寄存器的起始地址
  *		@arg buf[2~len-1]: 要写入的数据
  *		@arg len:    GTP_ADDR_LENGTH + write bytes count（寄存器地址长度+写入的数据字节数）
  * @retval  i2c_msgs传输结构体的个数，1为成功，其它为失败
  */
static int32_t GTP_I2C_Write(uint8_t client_addr,uint8_t *buf,int32_t len)
{
    struct i2c_msg msg;
    int32_t ret = -1;
    int32_t retries = 0;

    GTP_DEBUG_FUNC();
    /*一个写数据的过程只需要一个传输过程:
     * 1. IIC连续 写入 数据寄存器地址及数据
     * */
    msg.flags = !I2C_M_RD;			//写入
    msg.addr  = client_addr;			//从设备地址
    msg.len   = len;							//长度直接等于(寄存器地址长度+写入的数据字节数)
    msg.buf   = buf;						//直接连续写入缓冲区中的数据(包括了寄存器地址)

    while(retries < 5)
    {
        ret = I2C_Transfer(&msg, 1);	//调用IIC数据传输过程函数，1个传输过程
        if (ret == 1)break;
        retries++;
    }
    if((retries >= 5))
    {

        GTP_ERROR("I2C Write: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((uint16_t)(buf[0] << 8)) | buf[1]), len-2, ret);

    }
    return ret;
}



/**
  * @brief   使用IIC读取再次数据，检验是否正常
  * @param
  *		@arg client:设备地址
  *		@arg  addr: 寄存器地址
  *		@arg rxbuf: 存储读出的数据
  *		@arg len:    读取的字节数
  * @retval
  * 	@arg FAIL
  * 	@arg SUCCESS
  */
 int32_t GTP_I2C_Read_dbl_check(uint8_t client_addr, uint16_t addr, uint8_t *rxbuf, int len)
{
    uint8_t buf[16] = {0};
    uint8_t confirm_buf[16] = {0};
    uint8_t retry = 0;
    
    GTP_DEBUG_FUNC();

    while (retry++ < 3)
    {
        memset(buf, 0xAA, 16);
        buf[0] = (uint8_t)(addr >> 8);
        buf[1] = (uint8_t)(addr & 0xFF);
        GTP_I2C_Read(client_addr, buf, len + 2);
        
        memset(confirm_buf, 0xAB, 16);
        confirm_buf[0] = (uint8_t)(addr >> 8);
        confirm_buf[1] = (uint8_t)(addr & 0xFF);
        GTP_I2C_Read(client_addr, confirm_buf, len + 2);

      
        if (!memcmp(buf, confirm_buf, len+2))
        {
            memcpy(rxbuf, confirm_buf+2, len);
            return SUCCESS;
        }
    }    
    GTP_ERROR("I2C read 0x%04X, %d bytes, double check failed!", addr, len);
    return FAIL;
}


/**
  * @brief   关闭GT91xx中断
  * @param 无
  * @retval 无
  */
void GTP_IRQ_Disable(void)
{

    GTP_DEBUG_FUNC();

    I2C_GTP_IRQDisable();
}

/**
  * @brief   使能GT91xx中断
  * @param 无
  * @retval 无
  */
void GTP_IRQ_Enable(void)
{
    GTP_DEBUG_FUNC();
     
	  I2C_GTP_IRQEnable();    
}


/**
  * @brief   用于处理或报告触屏检测到按下
  * @param
  *    @arg     id: 触摸顺序trackID
  *    @arg     x:  触摸的 x 坐标
  *    @arg     y:  触摸的 y 坐标
  *    @arg     w:  触摸的 大小
  * @retval 无
  */
/*用于记录连续触摸时(长按)的上一次触摸位置，负数值表示上一次无触摸按下*/
//static int16_t pre_x[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};
//static int16_t pre_y[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};

static void GTP_Touch_Down(int32_t id,int32_t x,int32_t y,int32_t w)
{
  
	GTP_DEBUG_FUNC();

	/*取x、y初始值大于屏幕像素值*/
    GTP_DEBUG("ID:%d, X:%d, Y:%d, W:%d", id, x, y, w);

	
    /* 处理触摸按钮，用于触摸画板 */
//    Touch_Button_Down(x,y); 
	

    /*处理描绘轨迹，用于触摸画板 */
//    Draw_Trail(pre_x[id],pre_y[id],x,y,&brush);
	
		/************************************/
		/*在此处添加自己的触摸点按下时处理过程即可*/
		/* (x,y) 即为最新的触摸点 *************/
		/************************************/
	
		/*prex,prey数组存储上一次触摸的位置，id为轨迹编号(多点触控时有多轨迹)*/
//    pre_x[id] = x; pre_y[id] =y;
	
}


/**
  * @brief   用于处理或报告触屏释放
  * @param 释放点的id号
  * @retval 无
  */
static void GTP_Touch_Up( int32_t id)
{
	

    /*处理触摸释放,用于触摸画板*/
//    Touch_Button_Up(pre_x[id],pre_y[id]);

		/*****************************************/
		/*在此处添加自己的触摸点释放时的处理过程即可*/
		/* pre_x[id],pre_y[id] 即为最新的释放点 ****/
		/*******************************************/	
		/***id为轨迹编号(多点触控时有多轨迹)********/
	
	
    /*触笔释放，把pre xy 重置为负*/
//	  pre_x[id] = -1;
//	  pre_y[id] = -1;		
  
    GTP_DEBUG("Touch id[%2d] release!", id);

}


/**
  * @brief   触屏处理函数，轮询或者在触摸中断调用
  * @param 无
  * @retval 无
  */
static void Goodix_TS_Work_Func(void)
{
    uint8_t  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
    uint8_t  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1]={GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
    uint8_t  touch_num = 0;
    uint8_t  finger = 0;
    static uint16_t pre_touch = 0;
    static uint8_t pre_id[GTP_MAX_TOUCH] = {0};

    uint8_t client_addr=GTP_ADDRESS;
    uint8_t* coor_data = NULL;
    int32_t input_x = 0;
    int32_t input_y = 0;
    int32_t input_w = 0;
    uint8_t id = 0;
 
    int32_t i  = 0;
    int32_t ret = -1;

    GTP_DEBUG_FUNC();

    ret = GTP_I2C_Read(client_addr, point_data, 12);//10字节寄存器加2字节地址
    if (ret < 0)
    {
        GTP_ERROR("I2C transfer error. errno:%d\n ", ret);

        return;
    }
    
    finger = point_data[GTP_ADDR_LENGTH];//状态寄存器数据

    if (finger == 0x00)		//没有数据，退出
    {
        return;
    }

    if((finger & 0x80) == 0)//判断buffer status位
    {
        goto exit_work_func;//坐标未就绪，数据无效
    }

    touch_num = finger & 0x0f;//坐标点数
    if (touch_num > GTP_MAX_TOUCH)
    {
        goto exit_work_func;//大于最大支持点数，错误退出
    }

    if (touch_num > 1)//不止一个点
    {
        uint8_t buf[8 * GTP_MAX_TOUCH] = {(GTP_READ_COOR_ADDR + 10) >> 8, (GTP_READ_COOR_ADDR + 10) & 0xff};

        ret = GTP_I2C_Read(client_addr, buf, 2 + 8 * (touch_num - 1));
        memcpy(&point_data[12], &buf[2], 8 * (touch_num - 1));			//复制其余点数的数据到point_data
    }

    
    
    if (pre_touch>touch_num)				//pre_touch>touch_num,表示有的点释放了
    {
        for (i = 0; i < pre_touch; i++)						//一个点一个点处理
         {
            uint8_t j;
           for(j=0; j<touch_num; j++)
           {
               coor_data = &point_data[j * 8 + 3];
               id = coor_data[0] & 0x0F;									//track id
              if(pre_id[i] == id)
                break;

              if(j >= touch_num-1)											//遍历当前所有id都找不到pre_id[i]，表示已释放
              {
                 GTP_Touch_Up( pre_id[i]);
              }
           }
       }
    }


    if (touch_num)
    {
        for (i = 0; i < touch_num; i++)						//一个点一个点处理
        {
            coor_data = &point_data[i * 8 + 3];

            id = coor_data[0] & 0x0F;									//track id
            pre_id[i] = id;

            input_x  = coor_data[1] | (coor_data[2] << 8);	//x坐标
            input_y  = coor_data[3] | (coor_data[4] << 8);	//y坐标
            input_w  = coor_data[5] | (coor_data[6] << 8);	//size
        
            {
                GTP_Touch_Down( id, input_x, input_y, input_w);//数据处理
            }
        }
    }
    else if (pre_touch)		//touch_ num=0 且pre_touch！=0
    {
      for(i=0;i<pre_touch;i++)
      {
          GTP_Touch_Up(pre_id[i]);
      }
    }


    pre_touch = touch_num;


exit_work_func:
    {
        ret = GTP_I2C_Write(client_addr, end_cmd, 3);
        if (ret < 0)
        {
            GTP_INFO("I2C write end_cmd error!");
        }
    }

}


/**
  * @brief   给触屏芯片重新复位
  * @param 无
  * @retval 无
  */
 int8_t GTP_Reset_Guitar(void)
{
    GTP_DEBUG_FUNC();
#if 1
    I2C_ResetChip();
    return 0;
#else 		//软件复位
    int8_t ret = -1;
    int8_t retry = 0;
    uint8_t reset_command[3]={(uint8_t)GTP_REG_COMMAND>>8,(uint8_t)GTP_REG_COMMAND&0xFF,2};

    //写入复位命令
    while(retry++ < 5)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, reset_command, 3);
        if (ret > 0)
        {
            GTP_INFO("GTP enter sleep!");

            return ret;
        }

    }
    GTP_ERROR("GTP send sleep cmd failed.");
    return ret;
#endif

}



 /**
   * @brief   进入睡眠模式
   * @param 无
   * @retval 1为成功，其它为失败
   */
//int8_t GTP_Enter_Sleep(void)
//{
//    int8_t ret = -1;
//    int8_t retry = 0;
//    uint8_t reset_comment[3] = {(uint8_t)(GTP_REG_COMMENT >> 8), (uint8_t)GTP_REG_COMMENT&0xFF, 5};//5
//
//    GTP_DEBUG_FUNC();
//
//    while(retry++ < 5)
//    {
//        ret = GTP_I2C_Write(GTP_ADDRESS, reset_comment, 3);
//        if (ret > 0)
//        {
//            GTP_INFO("GTP enter sleep!");
//
//            return ret;
//        }
//
//    }
//    GTP_ERROR("GTP send sleep cmd failed.");
//    return ret;
//}


int8_t GTP_Send_Command(uint8_t command)
{
    int8_t ret = -1;
    int8_t retry = 0;
    uint8_t command_buf[3] = {(uint8_t)(GTP_REG_COMMAND >> 8), (uint8_t)GTP_REG_COMMAND&0xFF, GTP_COMMAND_READSTATUS};

    GTP_DEBUG_FUNC();

    while(retry++ < 5)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, command_buf, 3);
        if (ret > 0)
        {
            GTP_INFO("send command success!");

            return ret;
        }

    }
    GTP_ERROR("send command fail!");
    return ret;
}

/**
  * @brief   唤醒触摸屏
  * @param 无
  * @retval 0为成功，其它为失败
  */
int8_t GTP_WakeUp_Sleep(void)
{
    uint8_t retry = 0;
    int8_t ret = -1;

    GTP_DEBUG_FUNC();

    while(retry++ < 10)
    {
        ret = GTP_I2C_Test();
        if (ret > 0)
        {
            GTP_INFO("GTP wakeup sleep.");
            return ret;
        }
        GTP_Reset_Guitar();
    }

    GTP_ERROR("GTP wakeup sleep failed.");
    return ret;
}

static int32_t GTP_Get_Info(void)
{
    uint8_t opr_buf[10] = {0};
    int32_t ret = 0;

    uint16_t abs_x_max = GTP_MAX_WIDTH;
    uint16_t abs_y_max = GTP_MAX_HEIGHT;
    uint8_t int_trigger_type = GTP_INT_TRIGGER;
        
    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA+1) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA+1) & 0xFF);
    
    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 10);
    if (ret < 0)
    {
        return FAIL;
    }
    
    abs_x_max = (opr_buf[3] << 8) + opr_buf[2];
    abs_y_max = (opr_buf[5] << 8) + opr_buf[4];
		GTP_DEBUG("RES");   
		GTP_DEBUG_ARRAY(&opr_buf[0],10);

    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA+6) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA+6) & 0xFF);

    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 3);
    if (ret < 0)
    {
        return FAIL;
    }
    int_trigger_type = opr_buf[2] & 0x03;
    
    GTP_INFO("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x",
            abs_x_max,abs_y_max,int_trigger_type);
    
    return SUCCESS;    
}

/*******************************************************
Function:
    Initialize gtp.
Input:
    ts: goodix private data
Output:
    Executive outcomes.
        0: succeed, otherwise: failed
*******************************************************/
 int32_t GTP_Init_Panel(void)
{
    int32_t ret = -1;

    int32_t i = 0;
    uint16_t check_sum = 0;
    int32_t retry = 0;

    const uint8_t* cfg_info;
    uint8_t cfg_info_len  ;
		uint8_t* config;

    uint8_t cfg_num =0 ;		//需要配置的寄存器个数

    GTP_DEBUG_FUNC();
	
    I2C_Touch_Init();

    ret = GTP_I2C_Test();
    if (ret < 0)
    {
        GTP_ERROR("I2C communication ERROR!");
				return ret;
    } 
		
		//获取触摸IC的型号
    GTP_Read_Version(); 
    
#if UPDATE_CONFIG

		config = (uint8_t *)malloc (GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH);

		config[0] = GTP_REG_CONFIG_DATA >> 8;
		config[1] =  GTP_REG_CONFIG_DATA & 0xff;
		
		//根据IC的型号指向不同的配置
		if(touchIC == GT9157)
		{
			cfg_info =  CTP_CFG_GT9157; //指向寄存器配置
			cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT9157);//计算配置表的大小
		}
		else if(touchIC == GT911)
		{
			cfg_info =  CTP_CFG_GT911;//指向寄存器配置
			cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT911) ;//计算配置表的大小
		}
		else if(touchIC == GT5688)			
		{
			cfg_info =  CTP_CFG_GT5688; //指向寄存器配置
			cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT5688);//计算配置表的大小
		}
    else if(touchIC == GT917S)
		{
			cfg_info =  CTP_CFG_GT917S; //指向寄存器配置
			cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT917S);//计算配置表的大小
		}
		
    memset(&config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
    memcpy(&config[GTP_ADDR_LENGTH], cfg_info, cfg_info_len);
		
		cfg_num = cfg_info_len;
		
		GTP_DEBUG("cfg_info_len = %d ",cfg_info_len);
		GTP_DEBUG("cfg_num = %d ",cfg_num);
		GTP_DEBUG_ARRAY(config,6);
		
		/*根据LCD的扫描方向设置分辨率*/
		config[GTP_ADDR_LENGTH+1] = LCD_PIXEL_WIDTH & 0xFF;
		config[GTP_ADDR_LENGTH+2] = LCD_PIXEL_WIDTH >> 8;
		config[GTP_ADDR_LENGTH+3] = LCD_PIXEL_HEIGHT & 0xFF;
		config[GTP_ADDR_LENGTH+4] = LCD_PIXEL_HEIGHT >> 8;
		
		/*根据模式设置X2Y交换*/

    //不交换
    //				config[GTP_ADDR_LENGTH+6] &= ~(X2Y_LOC);

    //交换
//    config[GTP_ADDR_LENGTH+6] |= (X2Y_LOC);

    //计算要写入checksum寄存器的值
    check_sum = 0;

    /* 计算check sum校验值 */
    if(touchIC == GT911 || touchIC == GT9157)
    {
        for (i = GTP_ADDR_LENGTH; i < cfg_num+GTP_ADDR_LENGTH; i++)
        {
            check_sum += (config[i] & 0xFF);
        }
        config[ cfg_num+GTP_ADDR_LENGTH] = (~(check_sum & 0xFF)) + 1; 	//checksum
        config[ cfg_num+GTP_ADDR_LENGTH+1] =  1; 						//refresh 配置更新标志
    }
    else if(touchIC == GT5688 || touchIC == GT917S) 
    {
      for (i = GTP_ADDR_LENGTH; i < (cfg_num+GTP_ADDR_LENGTH -3); i += 2) 
      {
        check_sum += (config[i] << 8) + config[i + 1];
      }
      
      check_sum = 0 - check_sum;
      GTP_DEBUG("Config checksum: 0x%04X", check_sum);
      //更新checksum
      config[(cfg_num+GTP_ADDR_LENGTH -3)] = (check_sum >> 8) & 0xFF;
      config[(cfg_num+GTP_ADDR_LENGTH -2)] = check_sum & 0xFF;
      config[(cfg_num+GTP_ADDR_LENGTH -1)] = 0x01;
    }

    //写入配置信息
    for (retry = 0; retry < 5; retry++)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, config , cfg_num + GTP_ADDR_LENGTH+2);
        if (ret > 0)
        {
            break;
        }
    }
    Delay(0xfffff);				//延迟等待芯片更新
		

		
#if 1	//读出写入的数据，检查是否正常写入
    //检验读出的数据与写入的是否相同
	{
    	    uint16_t i;
    	    uint8_t buf[300];
    	     buf[0] = config[0];
    	     buf[1] =config[1];    //寄存器地址

    	    GTP_DEBUG_FUNC();

    	    ret = GTP_I2C_Read(GTP_ADDRESS, buf, sizeof(buf));
			   
					GTP_DEBUG("read ");

					GTP_DEBUG_ARRAY(buf,cfg_num);
		
			    GTP_DEBUG("write ");

					GTP_DEBUG_ARRAY(config,cfg_num);

					//不对比版本号
    	    for(i=3;i<cfg_num+GTP_ADDR_LENGTH-3;i++)
    	    {

    	    	if(config[i] != buf[i])
    	    	{
    	    		GTP_ERROR("Config fail ! i = %d ",i);
							free(config);
    	    		return -1;
    	    	}
    	    }
    	    if(i==cfg_num+GTP_ADDR_LENGTH-3)
	    		GTP_DEBUG("Config success ! i = %d ",i);
	}
#endif
	
    free(config);
  
#endif
	 /* emXGUI示例中不使能中断 */
		I2C_GTP_IRQDisable();
	
    GTP_Get_Info();

    return 0;
}


/*******************************************************
Function:
    Read chip version.
Input:
    client:  i2c device
    version: buffer to keep ic firmware version
Output:
    read operation return.
        2: succeed, otherwise: failed
*******************************************************/
int32_t GTP_Read_Version(void)
{
    int32_t ret = -1;
    uint8_t buf[8] = {GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff};    //寄存器地址

    GTP_DEBUG_FUNC();

    ret = GTP_I2C_Read(GTP_ADDRESS, buf, sizeof(buf));
    if (ret < 0)
    {
        GTP_ERROR("GTP read version failed");
        return ret;
    }

    if (buf[4] == '1')
    {				
      //GT911芯片
      if(buf[2] == '9' && buf[3] == '1' && buf[4] == '1')
      {
        GTP_INFO("IC1 Version: %c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[7], buf[6]);

        touchIC = GT911;
        /* 设置当前的液晶屏类型 */
        cur_lcd = INCH_7;
      }
      //GT9157芯片
      else
        GTP_INFO("Unknown IC Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
      }
      else if (buf[4] == '5')
      {
        if( buf[2] == '9' && buf[3] == '1' && buf[4] == '5' && buf[5] == '7')
        {
          GTP_INFO("IC2 Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);

          touchIC = GT9157;
          /* 设置当前的液晶屏类型 */
          cur_lcd = INCH_5;
        }
        else
          GTP_INFO("Unknown IC Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);

    }    
    else if (buf[4] == '8')
    {	
				//GT5688芯片
				if(buf[2] == '5' && buf[3] == '6' && buf[4] == '8' && buf[5] == '8')
        {
          GTP_INFO("IC3 Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);

					touchIC = GT5688;
          /* 设置当前的液晶屏类型 */
          cur_lcd = INCH_4_3;
        }
        else
           GTP_INFO("Unknown IC Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);

    }
    else if(buf[4] == '7')
    {
			  //GT917S芯片
         GTP_INFO("IC2 Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
				
				if(buf[2] == '9' && buf[3] == '1' && buf[4] == '7' && buf[5] == 'S')
					touchIC = GT917S;   
        /* 设置当前的液晶屏类型 */
        cur_lcd = INCH_5;	        
    }
    else
       GTP_INFO("Unknown IC Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);

    return ret;
}

/*******************************************************
Function:
    I2c test Function.
Input:
    client:i2c client.
Output:
    Executive outcomes.
        2: succeed, otherwise failed.
*******************************************************/
static int8_t GTP_I2C_Test( void)
{
    uint8_t test[3] = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
    uint8_t retry = 0;
    int8_t ret = -1;

    GTP_DEBUG_FUNC();
  
    while(retry++ < 5)
    {
        ret = GTP_I2C_Read(GTP_ADDRESS, test, 3);
        if (ret > 0)
        {
            return ret;
        }
        GTP_ERROR("GTP i2c test failed time %d.",retry);
    }
    return ret;
}

//检测到触摸中断时调用，
void GTP_TouchProcess(void)
{
  GTP_DEBUG_FUNC();
  Goodix_TS_Work_Func();

}

#if 0//没有到的测试函数
/*******************************************************
Function:
    Request gpio(INT & RST) ports.
Input:
    ts: private data.
Output:
    Executive outcomes.
        >= 0: succeed, < 0: failed
*******************************************************/
static int8_t GTP_Request_IO_Port(struct goodix_ts_data *ts)
{
}

/*******************************************************
Function:
    Request interrupt.
Input:
    ts: private data.
Output:
    Executive outcomes.
        0: succeed, -1: failed.
*******************************************************/
static int8_t GTP_Request_IRQ(struct goodix_ts_data *ts)
{
}

//输出要初始化的数据及芯片中的真实数据
static void GT91xx_Config_Read_Proc(void)
{
    char temp_data[GTP_CONFIG_MAX_LENGTH + 2] = {0x80, 0x47};
    int i;

    GTP_INFO("==== GT9XX config init value====\n");

    for (i = 0 ; i < GTP_CONFIG_MAX_LENGTH ; i++)
    {
        printf("reg0x%x = 0x%02X ", i+0x8047, config[i + 2]);

        if (i % 10 == 9)
            printf("\n");
    }


    GTP_INFO("==== GT9XX config real value====\n");
    GTP_I2C_Read(GTP_ADDRESS, (uint8_t *)temp_data, GTP_CONFIG_MAX_LENGTH + 2);
    for (i = 0 ; i < GTP_CONFIG_MAX_LENGTH ; i++)
    {
        printf("reg0x%x = 0x%02X ", i+0x8047,temp_data[i+2]);

        if (i % 10 == 9)
            printf("\n");
    }

}

//向芯片写入配置数据
static int32_t GT91xx_Config_Write_Proc(void)
{
    int32_t ret = -1;

    int32_t i = 0;
    uint8_t check_sum = 0;
    int32_t retry = 0;
    uint8_t cfg_num =0x80FE-0x8047+1 ;		//需要配置的寄存器个数

    uint8_t cfg_info[] = CTP_CFG_GROUP1;
    uint8_t cfg_info_len =CFG_GROUP_LEN(cfg_info) ;

    GTP_INFO("==== GT9XX send config====\n");

    memset(&config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
    memcpy(&config[GTP_ADDR_LENGTH], cfg_info,cfg_info_len);

    //计算要写入checksum寄存器的值
    check_sum = 0;
    for (i = GTP_ADDR_LENGTH; i < cfg_num+GTP_ADDR_LENGTH; i++)
    {
        check_sum += config[i];
    }
    config[ cfg_num+GTP_ADDR_LENGTH] = (~check_sum) + 1; 	//checksum
    config[ cfg_num+GTP_ADDR_LENGTH+1] =  1; 						//refresh 配置更新标志

    //写入配置信息
    for (retry = 0; retry < 5; retry++)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, config , cfg_num + GTP_ADDR_LENGTH+2);
        if (ret > 0)
        {
            break;
        }
    }


    return ret;
}

#endif

/**
  * @brief  触屏中断服务函数，emXGUI示例中没有使用中断
  * @param 无
  * @retval 无
  */
#include "emXGUI_Arch.h"
extern GUI_SEM *Input_Sem;
void GTP_IRQHandler(void)
{
  uint32_t ulReturn;
  /* 进入临界段，临界段可以嵌套 */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
	if(EXTI_GetITStatus(GTP_INT_EXTI_LINE) != RESET) //确保是否产生了EXTI Line中断
	{
    GUI_SemPostISR(Input_Sem);
    
		EXTI_ClearITPendingBit(GTP_INT_EXTI_LINE);     //清除中断标志位
	}  
  
  /* 退出临界段 */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

/**
  * @brief  触屏检测函数，本函数作为emXGUI的定制检测函数，
   *        参考Goodix_TS_Work_Func修改而来， 只读取单个触摸点坐标
  * @param x[out] y[out] 读取到的坐标
  * @retval 坐标有效返回1，否则返回0
  */
int	GTP_Execu( int *x,int *y)
{
    uint8_t  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
    //2-寄存器地址 1-状态寄存器 8*1-每个触摸点使用8个寄存器 
    uint8_t  point_data[2 + 1 + 8 * 1 + 1]={GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
    uint8_t  touch_num = 0;
    uint8_t  finger = 0;

    uint8_t client_addr=GTP_ADDRESS;
    int32_t input_x = 0;
    int32_t input_y = 0;

    int32_t ret = -1;

    GTP_DEBUG_FUNC();

    ret = GTP_I2C_Read(client_addr, point_data, 12);//10字节寄存器加2字节地址
    if (ret < 0)
    {
        GTP_ERROR("I2C transfer error. errno:%d\n ", ret);
        return 0;
    }
    
    finger = point_data[GTP_ADDR_LENGTH];//状态寄存器数据

    if (finger == 0x00)		//没有数据，退出
    {
        return 0;
    }

    if((finger & 0x80) == 0)//判断buffer status位
    {
        goto exit_work_func;//坐标未就绪，数据无效
    }

    touch_num = finger & 0x0f;//坐标点数
    if (touch_num > GTP_MAX_TOUCH)
    {
        goto exit_work_func;//大于最大支持点数，错误退出
    }    
  
    if (touch_num)
    {
//      id = point_data[0] & 0x0F;									//track id

      input_x  = point_data[3+1] | (point_data[3+2] << 8);	//x坐标
      input_y  = point_data[3+3] | (point_data[3+4] << 8);	//y坐标
//      input_w  = coor_data[5] | (coor_data[6] << 8);	//size

      if(input_x < GTP_MAX_WIDTH && input_y < GTP_MAX_HEIGHT)  
      {
        *x = input_x;
        *y = input_y;
      }
      else
      {
          //超出范围，错误退出
         goto exit_work_func;
      }
    }

exit_work_func:
    {
        //清空标志
        ret = GTP_I2C_Write(client_addr, end_cmd, 3);
        if (ret < 0)
        {
            GTP_INFO("I2C write end_cmd error!");
            return 0;
        }
    }

    return touch_num;
}
//MODULE_DESCRIPTION("GTP Series Driver");
//MODULE_LICENSE("GPL");
