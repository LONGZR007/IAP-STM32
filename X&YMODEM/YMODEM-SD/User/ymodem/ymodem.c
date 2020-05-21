/**
  ******************************************************************************
  * @file    ymodem.c
  * @author  long
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ymodem-1k 协议
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./ymodem/ymodem.h"
#include "./boot_loader/boot_loader.h" 
#include "./iap_arch/iap_arch.h"
#include "./FATFS/ff.h"
#include <string.h>

/* 全局变量. */
static uint8_t ymodem_packet_number = 0u;                           /* 包计数. */
static uint8_t x_first_packet_received = Y_IS_PACKET;               /* 是不是包头. */
static uint8_t eot_num = 0;                                         /* 收到 EOT 的次数 */

FIL fnew;													/* 文件对象 */
FRESULT res_sd;                   /* 文件操作结果 */
UINT fnum;            					  /* 文件成功读写数量 */

/* 局部函数. */
static uint16_t ymodem_calc_crc(uint8_t *data, uint16_t length);
static ymodem_status ymodem_handle_packet(uint8_t *header, uint16_t *num);
static ymodem_status ymodem_error_handler(uint8_t *error_number, uint8_t max_error_number);

void delay(void)
{
  __IO uint32_t conut = 0xFFFFFF;
  while(conut--);
}

/**
 * @brief   这个函数是Xmodem协议的基础.
 *          接收数据并处理数据.
 * @param   rec_num:需要接收的文件数量
 * @return  无
 */
void ymodem_receive(uint16_t rec_num)
{
  volatile ymodem_status status = Y_OK;
  uint8_t error_number = 0u;

  x_first_packet_received = Y_NO_PACKET;
  ymodem_packet_number = 0u;

  /* 循环，直到没有任何错误(或者直到跳转到用户应用程序). */
  while (Y_OK == status && rec_num)
  {
    uint8_t *header = 0x00u;

    /* 获取数据头. */
    int receive_status = x_receive(&header, 1u);

    /* 用ACSII "C"发送给上位机(直到我们收到一些东西), 告诉上位机我们要使用 CRC-16 . */
    if ((0 != receive_status) && (Y_NO_PACKET == x_first_packet_received))
    {
      (void)x_transmit_ch(Y_C);    // 给上位机返回 ACSII "C" ，告诉上位机将使用 CRC-16 
    }
    /* 超时或其他错误. */
    else if ((0 != receive_status) && (Y_IS_PACKET == x_first_packet_received))
    {
      status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
    }
    else
    {
      /* 没有错误. */
//			header = data_rx_buff;
    }

    /* 包头可以使: SOH, STX, EOT and CAN. */
		ymodem_status packet_status = Y_ERROR;
    switch(header[0])
    {
      /* 128或1024字节的数据. */
      case Y_SOH:
      case Y_STX:
        /* 数据处理 */
        packet_status = ymodem_handle_packet(header, &rec_num);
				/* 如果处理成功，发送一个 ACK. */
        if (Y_OK == packet_status)
        {
          (void)x_transmit_ch(Y_ACK);
        }
        /* 如果错误与flash相关，则立即将错误计数器设置为最大值 (立即终止传输). */
        else if (Y_ERROR_FLASH == packet_status)
        {
          error_number = Y_MAY_ERRORS;
          status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
        }
        /* 处理数据包时出错，要么发送一个 NAK，要么执行传输中止. */
        else
        {
          status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
        }
        break;
      /* 传输结束. */
      case Y_EOT:
        /* ACK，反馈给上位机(以文本形式)，然后跳转到用户应用程序. */
        if (++eot_num > 1)
        {
          x_transmit_ch(Y_ACK);
          delay();    // 延时一会在发送
          x_transmit_ch(Y_C);
          
//          rec_num--;           // 接收完一个文件
//          x_first_packet_received = Y_NO_PACKET;
//          ymodem_packet_number = 0;
//          f_close(&fnew);
        }
        else
        {
          x_transmit_ch(Y_NAK);    /* 第一次收到EOT */
        }
        break;
      /* Abort from host. */
      case Y_CAN:
        status = Y_ERROR;
        break;
      default:
        /* Wrong header. */
       if (0 == receive_status)
        {
          status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
        }
        break;
    }
  }
}

/**
 * @brief   计算接收到包的 CRC-16.
 * @param   *data:  要计算的数据的数组.
 * @param   length: 数据的大小，128字节或1024字节.
 * @return  status: 计算CRC.
 */
static uint16_t ymodem_calc_crc(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0u;
    while (length)
    {
        length--;
        crc = crc ^ ((uint16_t)*data++ << 8u);
        for (uint8_t i = 0u; i < 8u; i++)
        {
            if (crc & 0x8000u)
            {
                crc = (crc << 1u) ^ 0x1021u;
            }
            else
            {
                crc = crc << 1u;
            }
        }
    }
    return crc;
}

/**
 * @brief   这个函数处理我们从ymodem协议中获得的数据包.
 * @param   header: SOH 或者 STX.
 * @return  status: 处理结果.
 */
static ymodem_status ymodem_handle_packet(uint8_t *header, uint16_t *num)
{
  ymodem_status status = Y_OK;
  uint16_t size = 0u;
  char file_name[50];
  
  if (Y_SOH == header[0])
  {
    size = Y_PACKET_128_SIZE;
  }
  else if (Y_STX == header[0])
  {
    size = Y_PACKET_1024_SIZE;
  }
  else
  {
    /* 错误的类型. */
    status = Y_ERROR;
  }
  uint16_t length = size + Y_PACKET_DATA_INDEX + Y_PACKET_CRC_SIZE;
  uint8_t received_data[Y_PACKET_1024_SIZE + Y_PACKET_DATA_INDEX + Y_PACKET_CRC_SIZE];

  /* 接收数据. */
  int receive_status = 0;
		
	memcpy(&received_data[0u], header, length);
	
  /* 最后两个字节是来自主机的CRC. */
  uint16_t crc_received = ((uint16_t)received_data[length-2u] << 8u) | ((uint16_t)received_data[length-1u]);
  /* 校验. */
  uint16_t crc_calculated = ymodem_calc_crc(&received_data[Y_PACKET_DATA_INDEX], size);

  /* 错误处理或者写入 flash. */
  if (Y_OK == status)
  {
    if (0 != receive_status)
    {
      /* 传输错误. */
      status |= Y_ERROR_UART;
    }
    
    if (ymodem_packet_number != received_data[Y_PACKET_NUMBER_INDEX])
    {
      /* 包数量与计数器不匹配. */
      status |= Y_ERROR_NUMBER;
    }
    
    if (255u != (received_data[Y_PACKET_NUMBER_INDEX] +  received_data[Y_PACKET_NUMBER_COMPLEMENT_INDEX]))
    {
      /* 包号和包号补数之和不是255. */
      /* 总和应该总是255. */
      status |= Y_ERROR_NUMBER;
    }
    
    if (crc_calculated != crc_received)
    {
      /* 计算的CRC和接收的CRC不同. */
      status |= Y_ERROR_CRC;
    }
    
    if (received_data[Y_PACKET_NUMBER_INDEX] == 0x00 && x_first_packet_received == Y_NO_PACKET)    // 第一个包
    {
      strcpy(file_name, (char *)&received_data[Y_PACKET_DATA_INDEX]);
      if (strlen(file_name) == 0)
      {
        /* Flash 错误. */
        status |= Y_ERROR_FLASH;
      }
      else
      {
        char buff[50];
        sprintf(buff, "0:/%s", file_name);
        res_sd = f_open(&fnew, buff, FA_OPEN_APPEND | FA_WRITE );
        if ( res_sd != FR_OK )
        {	
          /* Flash 错误. */
          status |= Y_ERROR_FLASH;
        }
      }
    }
    else
    {
      if (received_data[Y_PACKET_NUMBER_INDEX] == 0x00 && eot_num > 1)
      {
        (*num)--;           // 接收完一个文件
        x_first_packet_received = Y_NO_PACKET;
        ymodem_packet_number = 0;
        f_close(&fnew);
      }
      else
      {
        res_sd=f_write(&fnew, (uint8_t *)&received_data[Y_PACKET_DATA_INDEX], (uint32_t)size, &fnum);
        if(res_sd != FR_OK)
        {
          /* 不再读写，关闭文件 */
          f_close(&fnew);
          /* Flash 错误. */
          status |= Y_ERROR_FLASH;
        }
        
        /* 标记接收到一个包. */
        x_first_packet_received = Y_IS_PACKET;
      }
    }
  }

  /* 增加包计数和地址，减少当前扇区的剩余数量 (如果没有任何错误的话). */
  if (Y_OK == status)
  { 
    ymodem_packet_number++;
  }

  return status;
}

/**
 * @brief   处理ymodem错误.
*          增加错误计数器，然后如果错误数量达到临界，则发送终止，否则发送一个 NAK.
 * @param   *error_number:    当前错误数(作为指针传递).
 * @param   max_error_number: 允许的最大错误数.
 * @return  status: Y_ERROR 达到错误数量上限, Y_OK 继续接受.
 */
static ymodem_status ymodem_error_handler(uint8_t *error_number, uint8_t max_error_number)
{
  ymodem_status status = Y_OK;
  /* 错误计数器自增. */
  (*error_number)++;
  /* 如果计数器达到最大值，则中止. */
  if ((*error_number) >= max_error_number)
  {
    /* 终止传输. */
    (void)x_transmit_ch(Y_CAN);
    (void)x_transmit_ch(Y_CAN);
    status = Y_ERROR;
  }
  /* 发送一个NAK进行重复. */
  else
  {
    (void)x_transmit_ch(Y_NAK);
    status = Y_OK;
  }
  return status;
}
  