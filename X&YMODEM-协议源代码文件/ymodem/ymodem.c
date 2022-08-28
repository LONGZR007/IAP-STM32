/**
  ******************************************************************************
  * @file    ymodem.c
  * @author  long
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ymodem-1k 协议
  ******************************************************************************
***/

#include "./ymodem/ymodem.h"
#include <string.h>

/* 全局变量. */
static y_uint8_t recv_buf[Y_PROT_FRAME_LEN_RECV];                     /* 接收数据缓冲区 */
static y_uint32_t recv_len;                                           /* 接收到数据的长度 */
static y_uint8_t ymodem_packet_number = 0u;                           /* 包计数. */
static y_uint16_t ymodem_file_number = 0u;                            /* 文件计数. */
static y_uint8_t y_first_packet_received = Y_IS_PACKET;               /* 是不是包头. */
static void *file_ptr = 0;

/* 局部函数. */
static y_uint16_t ymodem_calc_crc(y_uint8_t *data, y_uint16_t length);
static ymodem_status ymodem_handle_packet(y_uint8_t *header);
static ymodem_status ymodem_error_handler(y_uint8_t *error_number, y_uint8_t max_error_number);
static y_uint16_t get_active_length(y_uint8_t *data, y_uint16_t len);
static y_uint32_t get_file_len(y_uint8_t *data);
static int get_receive_data(y_uint8_t **data, y_uint32_t len);
static void reset_recv_len(void);
static y_uint32_t get_recv_len(void);

/**
 * @brief   这个函数是Ymodem协议的基础.
 *          接收数据并处理数据.
 * @param   rec_num:需要接收的文件数量
 * @return  返回接收到的文件数量，如果最高位是1则说明接收异常，否则正常
 */
y_uint16_t ymodem_receive(void)
{
  volatile ymodem_status status = Y_OK;
  y_uint8_t error_number = 0u;
  y_uint8_t eot_num = 0;     /* 收到 EOT 的次数 */

  y_first_packet_received = Y_NO_PACKET;
  ymodem_packet_number = 0u;
  ymodem_file_number = 0u;

  reset_recv_len();    // 清空接收缓冲区（防止在这之前有接收到垃圾数据）
  
  (void)y_transmit_ch(Y_C);    // 给上位机返回 ACSII "C" ，告诉上位机将使用 CRC-16 

  /* 循环，直到没有任何错误(或者或者所有文件接收完成). */
  while (Y_OK == status)
  {
    y_uint8_t *header = 0x00u;

    /* 获取数据头. */
    int receive_status = get_receive_data(&header, 1u);

    /* 用ACSII "C"发送给上位机(直到我们收到一些东西), 告诉上位机我们要使用 CRC-16 . */
    if (0 != receive_status)
    {
      if (Y_NO_PACKET == y_first_packet_received)
      {
        (void)y_transmit_ch(Y_C);    // 给上位机返回 ACSII "C" ，告诉上位机将使用 CRC-16 
      }
      else
      {
        status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
      }
    }
    /* 超时或其他错误. */
    else
    {
      /* 包头可以使: SOH, STX, EOT and CAN. */
      ymodem_status packet_status = Y_ERROR;
      switch(header[0])
      {
        /* 128或1024字节的数据. */
        case Y_SOH:
        case Y_STX:
          /* 数据处理 */
          packet_status = ymodem_handle_packet(header);
          /* 如果处理成功，发送一个 ACK. */
          if (Y_OK == packet_status)
          {
            (void)y_transmit_ch(Y_ACK);
            if (y_first_packet_received == Y_NO_PACKET)
            {
              y_transmit_ch(Y_C);
            }
          }
          /* 如果错误与flash相关，则立即将错误计数器设置为最大值 (立即终止传输). */
          else if (Y_ERROR_FLASH == packet_status)
          {
            error_number = Y_MAY_ERRORS;
            status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
          }
          /* 所有文件接收完成. */
          else if (Y_EOY == packet_status)
          {
            (void)y_transmit_ch(Y_ACK);
            return ymodem_file_number;    // 文件接收正常,返回接收到的数里
          }
          /* 处理数据包时出错，要么发送一个 NAK，要么执行传输中止. */
          else
          {
            status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
          }
          break;
        /* 传输结束. */
        case Y_EOT:
          /* ACK，反馈给上位机(以文本形式). */
          if (++eot_num > 1)
          {
            y_transmit_ch(Y_ACK);
            
            /* 一个文件传输完成 */
            y_first_packet_received = Y_NO_PACKET;
            ymodem_packet_number = 0;
            receive_file_callback(file_ptr);
            file_ptr = 0;
            eot_num = 0;
            ymodem_file_number++;

            (void)y_transmit_ch(Y_C);    // 给上位机返回 ACSII "C" ，开启下一次传输
          }
          else
          {
            y_transmit_ch(Y_NAK);    /* 第一次收到EOT */
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
  
  /* 复位 */
  receive_file_callback(file_ptr);
  file_ptr = 0;
  ymodem_file_number |= (1 << 15);
  
  return ymodem_file_number;    // 文件接收出错,返回接收到的数里和错误信息
}

/**
 * @brief   计算接收到包的 CRC-16.
 * @param   *data:  要计算的数据的数组.
 * @param   length: 数据的大小，128字节或1024字节.
 * @return  status: 计算CRC.
 */
static y_uint16_t ymodem_calc_crc(y_uint8_t *data, y_uint16_t length)
{
  y_uint16_t crc = 0u;
  while (length)
  {
      length--;
      crc = crc ^ ((y_uint16_t)*data++ << 8u);
      for (y_uint8_t i = 0u; i < 8u; i++)
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
static ymodem_status ymodem_handle_packet(y_uint8_t *header)
{
  ymodem_status status = Y_OK;
  y_uint16_t size = 0u;
  static y_uint32_t file_len = 0;
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
  y_uint16_t length = size + Y_PACKET_DATA_INDEX + Y_PACKET_CRC_SIZE;

#if 1    // 0:不拷贝可以加快传输速度，不过不建议这样做
  y_uint8_t received_data[Y_PACKET_1024_SIZE + Y_PACKET_DATA_INDEX + Y_PACKET_CRC_SIZE];

	memcpy(&received_data[0u], header, length);
#else
  y_uint8_t *received_data = header;
#endif
  /* 接收数据. */
  int receive_status = 0;

  /* 错误处理或者写入 flash. */
  if (Y_OK == status)
  {
  #if 1    // 0:不校验可以加加快传输速度，不过不建议这样做
    /* 最后两个字节是来自主机的CRC. */
    y_uint16_t crc_received = ((y_uint16_t)received_data[length-2u] << 8u) | ((y_uint16_t)received_data[length-1u]);
    /* 校验. */
    y_uint16_t crc_calculated = ymodem_calc_crc(&received_data[Y_PACKET_DATA_INDEX], size);
    if (crc_calculated != crc_received)
    {
      /* 计算的CRC和接收的CRC不同. */
      status |= Y_ERROR_CRC;
    }
  #endif
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
    
    if (received_data[Y_PACKET_NUMBER_INDEX] == 0x00 && y_first_packet_received == Y_NO_PACKET)    // 第一个包
    {
      strcpy(file_name, (char *)&received_data[Y_PACKET_DATA_INDEX]);
      if (strlen(file_name) == 0)
      {
        /* 文件名字为空说明文件传输结束. */
        status |= Y_EOY;
      }
      else
      {
        file_len = get_file_len((y_uint8_t *)&received_data[Y_PACKET_DATA_INDEX]);
        if (receive_nanme_size_callback(file_ptr, file_name, file_len) != 0)     // 调用接收完成回调函数
        {
          /* 硬件错误. */
          status |= Y_ERROR_FLASH;
        }
      }
    }
    else
    { 
      if (file_len == 0xFFFFFFFF)    // 不是有效的长度
      {
        /* file_len 不是有效的数值，判断数据是 0x1A 那么就丢弃数据 */
        size = get_active_length((y_uint8_t *)&received_data[Y_PACKET_DATA_INDEX], size);        // 获取有效的长度
      }
      
      if (file_len < size)    /* 最后一帧了，并且数据没有size这么大 */
      {
        size = file_len;
      }

      if (receive_file_data_callback(file_ptr, (char *)&received_data[Y_PACKET_DATA_INDEX], size) != 0)
      {
        /* 硬件错误. */
        status |= Y_ERROR_FLASH;
      }

      if (file_len != 0xFFFFFFFF)
      {
        file_len -= size;
      }
      
      /* 标记接收到一个包. */
      y_first_packet_received = Y_IS_PACKET;
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
static ymodem_status ymodem_error_handler(y_uint8_t *error_number, y_uint8_t max_error_number)
{
  ymodem_status status = Y_OK;

  reset_recv_len();    // 清空接收缓冲区

  /* 错误计数器自增. */
  (*error_number)++;
  /* 如果计数器达到最大值，则中止. */
  if ((*error_number) >= max_error_number)
  {
    /* 终止传输. */
    (void)y_transmit_ch(Y_CAN);
    (void)y_transmit_ch(Y_CAN);
    status = Y_ERROR;
  }
  /* 发送一个NAK进行重复. */
  else
  {
    (void)y_transmit_ch(Y_NAK);
    status = Y_OK;
  }

  return status;
}

/**
 * @brief   获取文件有效的长度（除去后面为0x1A的字节）.
 * @param   *data: 指向数据的指针.
 * @param   len: 数据长度.
 * @return  有效的数据长度.
 */
static y_uint16_t get_active_length(y_uint8_t *data, y_uint16_t len)
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
 * @brief   获取文件长度.
 * @param   *data: 指向数据的指针.
 * @param   len: 数据长度.
 * @return  找到则返回文件长度，否则返回0xFFFFFFFF.
 */
static y_uint32_t get_file_len(y_uint8_t *data)
{
  y_uint32_t len = 0xFFFFFFFF;
  y_uint16_t count = 0;
  y_uint16_t index = 0;

  /* 找文件大小的起始位置 */
  for (count=0; count<128; count++)
  {
    if (*(data + count) == '\0')
    {
      index = ++count;
      break;
    }
  }
  
  if (count >= 128)
    return len;
  
  /* 找文件大小的末位 */
  for (count=index; count<128; count++)
  {
    if (*(data + count) < 0x30 || *(data + count) > 0x39)
    {
      break;
    }
  }
  
  if (count >= 128)
    return len;
  
  len = 0;
  /* 转为数值 */
  while(count > index)
  {
    len = *(data + index) - 0x30 + len * 10;
    index++;
  }
  
  return len;
}

/**
 * @brief   Ymodem 接收数据的接口.
 * @param   *data ：接收数据
 * @param   *len ：接收数据的长度
 * @return  接收数据的状态
 */
static int get_receive_data(y_uint8_t **data, y_uint32_t len)
{
	volatile y_uint32_t timeout = Y_RECEIVE_TIMEOUT;
  y_uint8_t *data_temp = 0;
  y_uint16_t max_len = 1;
  y_uint16_t data_len[2] = {128, 1024};
  
#if TIMEOUT_CONFIG
	while (timeout--)   // 等待数据接收完成
	{
    if (get_recv_len() >= max_len)
    {
      if (max_len != 1)
        break;
      
      data_temp = recv_buf;                                 // 获取接收到的数据
      if (*data_temp == Y_SOH || *data_temp == Y_STX)       // 第一个是SOH，说明本次需要接收133个字节
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
  y_uint32_t tickstart = y_get_tick();
  while (1)   // 等待数据接收完成
	{
    if (y_get_tick() - tickstart > timeout)    // 检查是否超时
    {
			return -1;    // 超时错误
		}
    
    if (get_recv_len() >= max_len)
    {
      if (max_len != 1)
        break;
      
      data_temp = recv_buf;                                 // 获取接收到的数据
      if (*data_temp == Y_SOH || *data_temp == Y_STX)       // 第一个是SOH，说明本次需要接收133个字节
      {
        max_len = data_len[*data_temp - 1] + 3 + 2;             // 根据不同的头记录不同的长度
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
static y_uint32_t get_recv_len(void)
{
  return recv_len;
}

/**
 * @brief   接收数据处理
 * @param   *data:  要计算的数据的数组.
 * @param   data_len: 数据的大小
 * @return  void.
 */
void ymodem_data_recv(y_uint8_t *data, y_uint16_t data_len)
{
  if (recv_len + data_len >= Y_PROT_FRAME_LEN_RECV)
    recv_len = 0;
  
  memcpy(recv_buf + recv_len, data, data_len); 
  recv_len += data_len;
}

/**
 * @brief   获取毫秒时间戳.
 * @param   void
 * @return  时间戳
 */
__weak y_uint32_t y_get_tick(void)
{
	
	return 0;
}


/**
 * @brief   Ymodem 发送一个字符的接口.
 * @param   ch ：发送的数据
 * @return  返回发送状态
 */
__weak int y_transmit_ch(y_uint8_t ch)
{
	Y_UNUSED(ch);
	
	return -1;
}

/**
 * @brief   文件名和大小接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
__weak int receive_nanme_size_callback(void *ptr, char *file_name, y_uint32_t file_size)
{
  Y_UNUSED(ptr);
  Y_UNUSED(file_name);
  Y_UNUSED(file_size);
  
  /* 用户应该在外部实现这个函数 */
  return -1;
}

/**
 * @brief   文件数据接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
__weak int receive_file_data_callback(void *ptr, char *file_data, y_uint32_t w_size)
{
  Y_UNUSED(ptr);
  Y_UNUSED(file_data);
  Y_UNUSED(w_size);
  
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
  Y_UNUSED(ptr);
  
  /* 用户应该在外部实现这个函数 */
  return -1;
}

