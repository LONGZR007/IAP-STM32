/**
  ******************************************************************************
  * @file    xmodem.c
  * @author  long
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   xmodem 协议（支持128B和1K）
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./xmodem/xmodem.h"
#include "./boot_loader/boot_loader.h" 
#include "./iap_arch/iap_arch.h"
#include <string.h>

/* 全局变量. */
static uint8_t xmodem_packet_number = 1u;               /* 包计数. */
static uint32_t xmodem_actual_flash_address = FLASH_APP_ADDR;       /* 写入的地址. */
static uint8_t x_first_packet_received = X_IS_PACKET;   /* 是不是包头. */
static uint32_t sector_size = 0;                        /* 扇区剩余大小. */

/* 局部函数. */
static uint16_t xmodem_calc_crc(uint8_t *data, uint16_t length);
static xmodem_status xmodem_handle_packet(uint8_t *size);
static xmodem_status xmodem_error_handler(uint8_t *error_number, uint8_t max_error_number);
#define RX_MAX_LEN     (128*1024)
extern uint8_t data_rx_buff[RX_MAX_LEN];
/**
 * @brief   这个函数是Xmodem协议的基础.
 *          接收数据并处理数据.
 * @param   无
 * @return  无
 */
void xmodem_receive(void)
{
  volatile xmodem_status status = X_OK;
  uint8_t error_number = 0u;

  x_first_packet_received = X_IS_PACKET;
  xmodem_packet_number = 1u;

  /* 循环，直到没有任何错误(或者直到跳转到用户应用程序). */
  while (X_OK == status)
  {
    uint8_t *header = 0x00u;

    /* 获取数据头. */
    int receive_status = x_receive(&header, 1u);

    /* 用ACSII "C"发送给上位机(直到我们收到一些东西), 告诉上位机我们要使用 CRC-16 . */
    if ((0 != receive_status) && (X_IS_PACKET == x_first_packet_received))
    {
      (void)x_transmit_ch(X_C);    // 给上位机返回 ACSII "C" ，告诉上位机将使用 CRC-16 
    }
    /* 超时或其他错误. */
    else if ((0 != receive_status) && (X_NO_PACKET == x_first_packet_received))
    {
      status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
    }
    else
    {
      /* 没有错误. */
//			header = data_rx_buff;
    }

    /* 包头可以使: SOH, STX, EOT and CAN. */
		xmodem_status packet_status = X_ERROR;
    switch(header[0])
    {
      /* 128或1024字节的数据. */
      case X_SOH:
      case X_STX:
        /* 数据处理 */
        packet_status = xmodem_handle_packet(header);
				/* 如果处理成功，发送一个 ACK. */
        if (X_OK == packet_status)
        {
          (void)x_transmit_ch(X_ACK);
        }
        /* 如果错误与flash相关，则立即将错误计数器设置为最大值 (立即终止传输). */
        else if (X_ERROR_FLASH == packet_status)
        {
          error_number = X_MAX_ERRORS;
          status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
        }
        /* 处理数据包时出错，要么发送一个 NAK，要么执行传输中止. */
        else
        {
          status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
        }
        break;
      /* 传输结束. */
      case X_EOT:
        /* ACK，反馈给上位机(以文本形式)，然后跳转到用户应用程序. */
        sector_size = 0;    // 复位扇区大小
				xmodem_actual_flash_address = FLASH_APP_ADDR;    // 复位app地址
        (void)x_transmit_ch(X_ACK);
        (void)printf("\n\rFirmware updated!\n\r");
        (void)printf("Jumping to user application...\n\r");
				iap_jump_app(FLASH_APP_ADDR);
        break;
      /* Abort from host. */
      case X_CAN:
        status = X_ERROR;
        break;
      default:
        /* Wrong header. */
       if (0 == receive_status)
        {
          status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
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
static uint16_t xmodem_calc_crc(uint8_t *data, uint16_t length)
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
 * @brief   这个函数处理我们从xmodem协议中获得的数据包.
 * @param   header: SOH 或者 STX.
 * @return  status: 处理结果.
 */
static xmodem_status xmodem_handle_packet(uint8_t *header)
{
  xmodem_status status = X_OK;
  uint16_t size = 0u;
  if (X_SOH == header[0])
  {
    size = X_PACKET_128_SIZE;
  }
  else if (X_STX == header[0])
  {
    size = X_PACKET_1024_SIZE;
  }
  else
  {
    /* 错误的类型. */
    status = X_ERROR;
  }
  uint16_t length = size + X_PACKET_DATA_INDEX + X_PACKET_CRC_SIZE;
  uint8_t received_data[X_PACKET_1024_SIZE + X_PACKET_DATA_INDEX + X_PACKET_CRC_SIZE];

  /* 接收数据. */
  int receive_status = 0;
		
	memcpy(&received_data[0u], header, length);
	
  /* 最后两个字节是来自主机的CRC. */
  uint16_t crc_received = ((uint16_t)received_data[length-2u] << 8u) | ((uint16_t)received_data[length-1u]);
  /* 校验. */
  uint16_t crc_calculated = xmodem_calc_crc(&received_data[X_PACKET_DATA_INDEX], size);

  /* 当前扇区不够了擦除下一个. */
  if (sector_size <= size)
  {
    sector_size += x_receive_flash_erasure(xmodem_actual_flash_address + sector_size);

    if (0 != sector_size)
    {
      x_first_packet_received = 1;
    }
    else
    {
      status |= X_ERROR_FLASH;
    }
  }

  /* 错误处理或者写入 flash. */
  if (X_OK == status)
  {
    if (0 != receive_status)
    {
      /* 传输错误. */
      status |= X_ERROR_UART;
    }
    if (xmodem_packet_number != received_data[X_PACKET_NUMBER_INDEX])
    {
      /* 包数量与计数器不匹配. */
      status |= X_ERROR_NUMBER;
    }
    if (255u != (received_data[X_PACKET_NUMBER_INDEX] +  received_data[X_PACKET_NUMBER_COMPLEMENT_INDEX]))
    {
      /* 包号和包号补数之和不是255. */
      /* 总和应该总是255. */
      status |= X_ERROR_NUMBER;
    }
    if (crc_calculated != crc_received)
    {
      /* 计算的CRC和接收的CRC不同. */
      status |= X_ERROR_CRC;
    }
    /* 没有错误写入 flash. */
    if (0 != x_receive_flash_writea(xmodem_actual_flash_address, (uint8_t *)&received_data[X_PACKET_DATA_INDEX], (uint32_t)size))
    {
      /* Flash 写错误. */
      status |= X_ERROR_FLASH;
    }
  }

  /* 增加包计数和地址，减少当前扇区的剩余数量 (如果没有任何错误的话). */
  if (X_OK == status)
  { 
    xmodem_packet_number++;
    xmodem_actual_flash_address += size;
    sector_size -= size;
  }

  return status;
}

/**
 * @brief   处理xmodem错误.
*          增加错误计数器，然后如果错误数量达到临界，则发送终止，否则发送一个 NAK.
 * @param   *error_number:    当前错误数(作为指针传递).
 * @param   max_error_number: 允许的最大错误数.
 * @return  status: X_ERROR 达到错误数量上限, X_OK 继续接受.
 */
static xmodem_status xmodem_error_handler(uint8_t *error_number, uint8_t max_error_number)
{
  xmodem_status status = X_OK;
  /* 错误计数器自增. */
  (*error_number)++;
  /* 如果计数器达到最大值，则中止. */
  if ((*error_number) >= max_error_number)
  {
    /* 终止传输. */
    (void)x_transmit_ch(X_CAN);
    (void)x_transmit_ch(X_CAN);
    status = X_ERROR;
  }
  /* 发送一个NAK进行重复. */
  else
  {
    (void)x_transmit_ch(X_NAK);
    status = X_OK;
  }
  return status;
}
