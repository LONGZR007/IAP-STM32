/**
  ******************************************************************************
  * @file    xmodem.c
  * @author  long
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   xmodem-1k 协议
  ******************************************************************************
***/

#include "./xmodem/xmodem.h"
#include <string.h>

/* 全局变量. */
static x_uint8_t recv_buf[X_PROT_FRAME_LEN_RECV];                     /* 接收数据缓冲区 */
static x_uint32_t recv_len;                                           /* 接收到数据的长度 */
static x_uint8_t xmodem_packet_number = 1u;                           /* 包计数. */
static x_uint8_t x_first_packet_received = X_IS_PACKET;               /* 是不是包头. */
static void *file_ptr = 0;

/* 局部函数. */
static x_uint16_t xmodem_calc_crc(x_uint8_t *data, x_uint16_t length);
static xmodem_status xmodem_handle_packet(x_uint8_t *header);
static xmodem_status xmodem_error_handler(x_uint8_t *error_number, x_uint8_t max_error_number);
static x_uint16_t get_active_length(x_uint8_t *data, x_uint16_t len);
static int get_receive_data(x_uint8_t **data, x_uint32_t len);
static void reset_recv_len(void);
static x_uint32_t get_recv_len(void);

/**
 * @brief   这个函数是Xmodem协议的基础.
 *          接收数据并处理数据.
 * @param   void
 * @return  0:文件接收成功 -1:文件接收失败
 */
int xmodem_receive(void)
{
  volatile xmodem_status status = X_OK;
  x_uint8_t error_number = 0u;

  x_first_packet_received = X_NO_PACKET;
  xmodem_packet_number = 1u;
  
  (void)x_transmit_ch(X_C);    // 给上位机返回 ACSII "C" ，告诉上位机将使用 CRC-16 

  /* 循环，直到没有任何错误(或者或者所有文件接收完成). */
  while (X_OK == status)
  {
    x_uint8_t *header = 0x00u;

    /* 获取数据头. */
    int receive_status = get_receive_data(&header, 1u);

    /* 用ACSII "C"发送给上位机(直到我们收到一些东西), 告诉上位机我们要使用 CRC-16 . */
    if ((0 != receive_status) && (X_NO_PACKET == x_first_packet_received))
    {
      (void)x_transmit_ch(X_C);    // 给上位机返回 ACSII "C" ，告诉上位机将使用 CRC-16 
    }
    /* 超时或其他错误. */
    else if ((0 != receive_status) && (X_IS_PACKET == x_first_packet_received))
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
        /* ACK，反馈给上位机(以文本形式) */
        /* 一个文件传输完成 */
        x_first_packet_received = X_NO_PACKET;
        xmodem_packet_number = 1;
        receive_file_callback(file_ptr);
        file_ptr = 0;
        return 0;
      /* 发送端发来取消. */
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
  
  return -1;
}

/**
 * @brief   计算接收到包的 CRC-16.
 * @param   *data:  要计算的数据的数组.
 * @param   length: 数据的大小，128字节或1024字节.
 * @return  status: 计算CRC.
 */
static x_uint16_t xmodem_calc_crc(x_uint8_t *data, x_uint16_t length)
{
  x_uint16_t crc = 0u;
  while (length)
  {
      length--;
      crc = crc ^ ((x_uint16_t)*data++ << 8u);
      for (x_uint8_t i = 0u; i < 8u; i++)
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
static xmodem_status xmodem_handle_packet(x_uint8_t *header)
{
  xmodem_status status = X_OK;
  x_uint16_t size = 0u;
  
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
  x_uint16_t length = size + X_PACKET_DATA_INDEX + X_PACKET_CRC_SIZE;

#if 1    // 0:不拷贝可以加快传输速度，不过不建议这样做
  x_uint8_t received_data[X_PACKET_1024_SIZE + X_PACKET_DATA_INDEX + X_PACKET_CRC_SIZE];

	memcpy(&received_data[0u], header, length);
#else
  x_uint8_t *received_data = header;
#endif
  /* 接收数据. */
  int receive_status = 0;

  /* 错误处理或者写入 flash. */
  if (X_OK == status)
  {
  #if 1    // 0:不校验可以加加快传输速度，不过不建议这样做
    /* 最后两个字节是来自主机的CRC. */
    x_uint16_t crc_received = ((x_uint16_t)received_data[length-2u] << 8u) | ((x_uint16_t)received_data[length-1u]);
    /* 校验. */
    x_uint16_t crc_calculated = xmodem_calc_crc(&received_data[X_PACKET_DATA_INDEX], size);
    if (crc_calculated != crc_received)
    {
      /* 计算的CRC和接收的CRC不同. */
      status |= X_ERROR_CRC;
    }
  #endif
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
    
    if (status == X_OK)
    {
    #if 0
      /* 判断数据是 0x1A 那么就丢弃数据 */
      size = get_active_length((x_uint8_t *)&received_data[X_PACKET_DATA_INDEX], size);        // 获取有效的长度
    #endif
      if (receive_file_data_callback(file_ptr, (char *)&received_data[X_PACKET_DATA_INDEX], size) != 0)
      {
        /* 硬件错误. */
        status |= X_ERROR_FLASH;
      }
    }
    /* 标记接收到一个包. */
    x_first_packet_received = X_IS_PACKET;
  }

  /* 增加包计数和地址，减少当前扇区的剩余数量 (如果没有任何错误的话). */
  if (X_OK == status)
  { 
    xmodem_packet_number++;
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
static xmodem_status xmodem_error_handler(x_uint8_t *error_number, x_uint8_t max_error_number)
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

/**
 * @brief   获取文件有效的长度（除去后面为0x1A的字节）.
 * @param   *data: 指向数据的指针.
 * @param   len: 数据长度.
 * @return  有效的数据长度.
 */
static x_uint16_t get_active_length(x_uint8_t *data, x_uint16_t len)
{
  while(len)
  {
    if (*(data + len - 1) == 0x1A)
    {
      len--;
    }
    else
    {
      break;
    }
  }
  
  return len;
}

/**
 * @brief   Ymodem 接收数据的接口.
 * @param   *data ：接收数据
 * @param   *len ：接收数据的长度
 * @return  接收数据的状态
 */
static int get_receive_data(x_uint8_t **data, x_uint32_t len)
{
	volatile x_uint32_t timeout = X_RECEIVE_TIMEOUT;
  x_uint8_t *data_temp = 0;
  x_uint16_t max_len = 1;
  x_uint16_t data_len[2] = {128, 1024};
  
#if TIMEOUT_CONFIG
	while (timeout--)   // 等待数据接收完成
	{
    if (get_recv_len() >= max_len)
    {
      if (max_len != 1)
        break;
      
      data_temp = recv_buf;                                 // 获取接收到的数据
      if (*data_temp == X_SOH || *data_temp == X_STX)       // 第一个是SOH，说明本次需要接收133个字节
      {
        max_len = data_len[*data_temp - 1] + 3 + 2;             // 根据不同的头记录不同的长度
      }
      else
      {
        break;
      }
    }
    
		if (timeout == 0)
		{
			return -1;    // 超时错误
		}
	}
#else
  x_uint32_t tickstart = x_get_tick();
  while (1)   // 等待数据接收完成
	{
    if (x_get_tick() - tickstart > timeout)    // 检查是否超时
    {
			return -1;    // 超时错误
		}
    
    if (get_recv_len() >= max_len)
    {
      if (max_len != 1)
        break;
      
      data_temp = recv_buf;                                 // 获取接收到的数据
      if (*data_temp == X_SOH || *data_temp == X_STX)       // 第一个是SOH，说明本次需要接收133个字节
      {
        max_len = data_len[*data_temp] + 3 + 2;             // 根据不同的头记录不同的长度
      }
      else
      {
        break;
      }
    }
	}
#endif
	
	/* 获取接收数据 */
	*data = recv_buf;
  reset_recv_len();
	
	return 0;
}

/**
 * @brief   复位数据接收长度
  * @param  void.
 * @return  void.
 */
static void reset_recv_len(void)
{
  recv_len = 0;
}

/**
 * @brief   获取数据接收长度
 * @param   void.
 * @return  接收到数据的长度.
 */
static x_uint32_t get_recv_len(void)
{
  return recv_len;
}

/**
 * @brief   接收数据处理
 * @param   *data:  要计算的数据的数组.
 * @param   data_len: 数据的大小
 * @return  void.
 */
void xmodem_data_recv(x_uint8_t *data, x_uint16_t data_len)
{
  if (recv_len + data_len >= X_PROT_FRAME_LEN_RECV)
    recv_len = 0;
  
  memcpy(recv_buf + recv_len, data, data_len); 
  recv_len += data_len;
}

/**
 * @brief   获取毫秒时间戳.
 * @param   void
 * @return  时间戳
 */
__weak x_uint32_t x_get_tick(void)
{
	
	return 0;
}


/**
 * @brief   Ymodem 发送一个字符的接口.
 * @param   ch ：发送的数据
 * @return  返回发送状态
 */
__weak int x_transmit_ch(x_uint8_t ch)
{
	X_UNUSED(ch);
	
	return -1;
}

/**
 * @brief   文件数据接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
__weak int receive_file_data_callback(void *ptr, char *file_data, x_uint32_t w_size)
{
  X_UNUSED(ptr);
  X_UNUSED(file_data);
  X_UNUSED(w_size);
  
  /* 用户应该在外部实现这个函数 */
  return -1;
}

/**
 * @brief   一个文件接收完成回调.
 * @param   *ptr: 控制句柄.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
__weak int receive_file_callback(void *ptr)
{
  X_UNUSED(ptr);
  
  /* 用户应该在外部实现这个函数 */
  return -1;
}

