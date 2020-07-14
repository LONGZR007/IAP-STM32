/**
  ******************************************************************************
  * @file    ymodem.c
  * @author  long
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ymodem-1k Э��
  ******************************************************************************
***/

#include "./ymodem/ymodem.h"
#include <string.h>

/* ȫ�ֱ���. */
static y_uint8_t recv_buf[Y_PROT_FRAME_LEN_RECV];                     /* �������ݻ����� */
static y_uint32_t recv_len;                                           /* ���յ����ݵĳ��� */
static y_uint8_t ymodem_packet_number = 0u;                           /* ������. */
static y_uint16_t ymodem_file_number = 0u;                            /* �ļ�����. */
static y_uint8_t y_first_packet_received = Y_IS_PACKET;               /* �ǲ��ǰ�ͷ. */
static ymodem_status_t y_received_status = Y_DIS;                     /* Ymodem��״̬. */
static void *file_ptr = 0;                                            /* ���û�ʹ�õ�ָ�� */

/* �ֲ�����. */
static y_uint16_t ymodem_calc_crc(y_uint8_t *data, y_uint16_t length);
static ymodem_fun_status_t ymodem_handle_packet(y_uint8_t *header);
static ymodem_fun_status_t ymodem_error_handler(y_uint8_t *error_number, y_uint8_t max_error_number);
static y_uint16_t get_active_length(y_uint8_t *data, y_uint16_t len);
static y_uint32_t get_file_len(y_uint8_t *data);
static int get_receive_data(y_uint8_t **data, y_uint32_t len);
static void reset_recv_len(void);
static y_uint32_t get_recv_len(void);

/**
 * @brief   ���������YmodemЭ��Ļ���.
 *          �������ݲ���������.
 * @param   rec_num:��Ҫ���յ��ļ�����
 * @return  ���ؽ��յ����ļ�������������λ��1��˵�������쳣����������
 */
y_uint16_t ymodem_receive(void)
{
  volatile ymodem_fun_status_t status = Y_OK;
  y_uint8_t error_number = 0u;
  y_uint8_t eot_num = 0;     /* �յ� EOT �Ĵ��� */
  ymodem_status_t run_status = Y_DIS;

  y_first_packet_received = Y_NO_PACKET;
  ymodem_packet_number = 0u;
  ymodem_file_number = 0u;

  /* ʹ��ʱ��ʼ���������ļ�. */
  if (y_received_status == Y_EN)
  {
    y_uint8_t *header = 0x00u;

    run_status = Y_RUN_RECV;

    /* ��ȡ����ͷ. */
    int receive_status = get_receive_data(&header, 1u);

    /* ��ʱ�˳�����û�п�ʼ�����ļ�. */
    if ((-1 == receive_status) && (Y_NO_PACKET == y_first_packet_received))
    {
      (void)y_transmit_ch(Y_C);    // ����λ������ ACSII "C" ��������λ����ʹ�� CRC-16 
    }
    /* ��ʱ����������. */
    else if ((-1  ==  receive_status) && (Y_IS_PACKET == y_first_packet_received))
    {
      status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
    }

    /* ��ͷ����ʹ: SOH, STX, EOT and CAN. */
		ymodem_fun_status_t packet_status = Y_ERROR;
    switch(header[0])
    {
      /* 128��1024�ֽڵ�����. */
      case Y_SOH:
      case Y_STX:
        /* ���ݴ��� */
        packet_status = ymodem_handle_packet(header);
				/* �������ɹ�������һ�� ACK. */
        if (Y_OK == packet_status)
        {
          (void)y_transmit_ch(Y_ACK);
          if (y_first_packet_received == Y_NO_PACKET)
          {
            y_transmit_ch(Y_C);
          }
        }
        /* ���������flash��أ����������������������Ϊ���ֵ (������ֹ����). */
        else if (Y_ERROR_FLASH == packet_status)
        {
          error_number = Y_MAY_ERRORS;
          status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
        }
        /* �����ļ��������. */
        else if (Y_EOY == packet_status)
        {
          (void)y_transmit_ch(Y_ACK);
          run_status = Y_END;    // �ļ�������������
          y_received_status = Y_DIS;    // ������������Э��
        }
        /* �������ݰ�ʱ����Ҫô����һ�� NAK��Ҫôִ�д�����ֹ. */
        else
        {
          status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
        }
        break;
      /* һ���ļ��������. */
      case Y_EOT:
        /* ACK����������λ��(���ı���ʽ). */
        if (++eot_num > 1)
        {
          y_transmit_ch(Y_ACK);
          
          /* һ���ļ�������� */
          y_first_packet_received = Y_NO_PACKET;
          ymodem_packet_number = 0;
          receive_file_callback(file_ptr);
          file_ptr = 0;
          eot_num = 0;
          ymodem_file_number++;

          (void)y_transmit_ch(Y_C);    // ����λ������ ACSII "C" ��������һ�δ���
        }
        else
        {
          y_transmit_ch(Y_NAK);    /* ��һ���յ�EOT */
        }
        break;
      /* Abort from host. */
      case Y_CAN:
        run_status = Y_CANCEL;         // �ļ���������λ��ȡ������
        y_received_status = Y_DIS;     // ��������Э��
        break;
      default:
          /* �����ͷ. */
        if (0 == receive_status)
        {
          status = ymodem_error_handler(&error_number, Y_MAY_ERRORS);
        }
        break;
    }

    if (status == Y_ERROR)                   // ���䷢���������
    {
      run_status = Y_MAX_ERR;
      if (Y_ERROR_FLASH == packet_status)    // �ײ����
      {
        run_status = Y_DISK_ERR;             // ��ǵײ����
      }
      y_received_status = Y_DIS;             // ���ý���
    }
  }
  
  return y_received_status;    // �ļ����ճ���,���ؽ��յ�������ʹ�����Ϣ
}

/**
 * @brief   ������յ����� CRC-16.
 * @param   *data:  Ҫ��������ݵ�����.
 * @param   length: ���ݵĴ�С��128�ֽڻ�1024�ֽ�.
 * @return  status: ����CRC.
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
 * @brief   ��������������Ǵ�ymodemЭ���л�õ����ݰ�.
 * @param   header: SOH ���� STX.
 * @return  status: ������.
 */
static ymodem_fun_status_t ymodem_handle_packet(y_uint8_t *header)
{
  ymodem_fun_status_t status = Y_OK;
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
    /* ���������. */
    status = Y_ERROR;
  }
  y_uint16_t length = size + Y_PACKET_DATA_INDEX + Y_PACKET_CRC_SIZE;

#if 1    // 0:���������Լӿ촫���ٶȣ�����������������
  y_uint8_t received_data[Y_PACKET_1024_SIZE + Y_PACKET_DATA_INDEX + Y_PACKET_CRC_SIZE];

	memcpy(&received_data[0u], header, length);
#else
  y_uint8_t *received_data = header;
#endif
  /* ��������. */
  int receive_status = 0;

  /* ���������д�� flash. */
  if (Y_OK == status)
  {
  #if 1    // 0:��У����ԼӼӿ촫���ٶȣ�����������������
    /* ��������ֽ�������������CRC. */
    y_uint16_t crc_received = ((y_uint16_t)received_data[length-2u] << 8u) | ((y_uint16_t)received_data[length-1u]);
    /* У��. */
    y_uint16_t crc_calculated = ymodem_calc_crc(&received_data[Y_PACKET_DATA_INDEX], size);
    if (crc_calculated != crc_received)
    {
      /* �����CRC�ͽ��յ�CRC��ͬ. */
      status |= Y_ERROR_CRC;
    }
  #endif
    if (0 != receive_status)
    {
      /* �������. */
      status |= Y_ERROR_UART;
    }
    
    if (ymodem_packet_number != received_data[Y_PACKET_NUMBER_INDEX])
    {
      /* ���������������ƥ��. */
      status |= Y_ERROR_NUMBER;
    }
    
    if (255u != (received_data[Y_PACKET_NUMBER_INDEX] +  received_data[Y_PACKET_NUMBER_COMPLEMENT_INDEX]))
    {
      /* ���źͰ��Ų���֮�Ͳ���255. */
      /* �ܺ�Ӧ������255. */
      status |= Y_ERROR_NUMBER;
    }
    
    if (received_data[Y_PACKET_NUMBER_INDEX] == 0x00 && y_first_packet_received == Y_NO_PACKET)    // ��һ����
    {
      strcpy(file_name, (char *)&received_data[Y_PACKET_DATA_INDEX]);
      if (strlen(file_name) == 0)
      {
        /* �ļ�����Ϊ��˵���ļ��������. */
        status |= Y_EOY;
      }
      else
      {
        file_len = get_file_len((y_uint8_t *)&received_data[Y_PACKET_DATA_INDEX]);
        if (receive_nanme_size_callback(file_ptr, file_name, file_len) != 0)     // ���ý�����ɻص�����
        {
          /* Ӳ������. */
          status |= Y_ERROR_FLASH;
        }
      }
    }
    else
    { 
      if (file_len == 0xFFFFFFFF)    // ������Ч�ĳ���
      {
        /* file_len ������Ч����ֵ���ж������� 0x1A ��ô�Ͷ������� */
        size = get_active_length((y_uint8_t *)&received_data[Y_PACKET_DATA_INDEX], size);        // ��ȡ��Ч�ĳ���
      }
      
      if (file_len < size)    /* ���һ֡�ˣ���������û��size��ô�� */
      {
        size = file_len;
      }

      if (receive_file_data_callback(file_ptr, (char *)&received_data[Y_PACKET_DATA_INDEX], size) != 0)
      {
        /* Ӳ������. */
        status |= Y_ERROR_FLASH;
      }

      if (file_len != 0xFFFFFFFF)
      {
        file_len -= size;
      }
      
      /* ��ǽ��յ�һ����. */
      y_first_packet_received = Y_IS_PACKET;
    }
  }

  /* ���Ӱ������͵�ַ�����ٵ�ǰ������ʣ������ (���û���κδ���Ļ�). */
  if (Y_OK == status)
  { 
    ymodem_packet_number++;
  }

  return status;
}

/**
 * @brief   ����ymodem����.
 *          ���Ӵ����������Ȼ��������������ﵽ�ٽ磬������ֹ��������һ�� NAK.
 * @param   *error_number:    ��ǰ������(��Ϊָ�봫��).
 * @param   max_error_number: �������������.
 * @return  status: Y_ERROR �ﵽ������������, Y_OK ��������.
 */
static ymodem_fun_status_t ymodem_error_handler(y_uint8_t *error_number, y_uint8_t max_error_number)
{
  ymodem_fun_status_t status = Y_OK;
  /* �������������. */
  (*error_number)++;
  /* ����������ﵽ���ֵ������ֹ. */
  if ((*error_number) >= max_error_number)
  {
    /* ��ֹ����. */
    (void)y_transmit_ch(Y_CAN);
    (void)y_transmit_ch(Y_CAN);
    status = Y_ERROR;
  }
  /* ����һ��NAK�����ظ�. */
  else
  {
    (void)y_transmit_ch(Y_NAK);
    status = Y_OK;
  }
  return status;
}

/**
 * @brief   ��ȡ�ļ���Ч�ĳ��ȣ���ȥ����Ϊ0x1A���ֽڣ�.
 * @param   *data: ָ�����ݵ�ָ��.
 * @param   len: ���ݳ���.
 * @return  ��Ч�����ݳ���.
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
 * @brief   ��ȡ�ļ�����.
 * @param   *data: ָ�����ݵ�ָ��.
 * @param   len: ���ݳ���.
 * @return  �ҵ��򷵻��ļ����ȣ����򷵻�0xFFFFFFFF.
 */
static y_uint32_t get_file_len(y_uint8_t *data)
{
  y_uint32_t len = 0xFFFFFFFF;
  y_uint16_t count = 0;
  y_uint16_t index = 0;

  /* ���ļ���С����ʼλ�� */
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
  
  /* ���ļ���С��ĩλ */
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
  /* תΪ��ֵ */
  while(count > index)
  {
    len = *(data + index) - 0x30 + len * 10;
    index++;
  }
  
  return len;
}

/**
 * @brief   Ymodem �������ݵĽӿ�.
 * @param   *data ����������
 * @param   *len ���������ݵĳ���
 * @return  �������ݵ�״̬
 */
static int get_receive_data(y_uint8_t **data, y_uint32_t len)
{
	static volatile y_uint32_t timeout = Y_RECEIVE_TIMEOUT;
  y_uint8_t *data_temp = 0;
  y_uint16_t max_len = get_recv_len();
  y_uint16_t data_len[2] = {128, 1024};
  
#if TIMEOUT_CONFIG
	if (timeout--)   // �ȴ����ݽ������
	{
    if (timeout == 0)
    {
      timeout = Y_RECEIVE_TIMEOUT;
      return -1;    // ��ʱ����
    }

    if (max_len >= 1)
    {
      data_temp = recv_buf;                                 // ��ȡ���յ�������
      if (*data_temp == Y_SOH || *data_temp == Y_STX)       // ��һ����SOH��˵��������Ҫ����133���ֽ�
      {
        if (max_len >= data_len[*data_temp - 1] + 3 + 2);             // ���ݲ�ͬ��ͷ��¼��ͬ�ĳ���
        {
          /* ��ȡ�������� */
          *data = recv_buf;
          reset_recv_len();
          timeout = Y_RECEIVE_TIMEOUT;
        }
        else
        {
          return  1;
        }
      }
      else
      {
        /* ��ȡ�������� */
        *data = recv_buf;
        reset_recv_len();
        timeout = Y_RECEIVE_TIMEOUT;
      }
    }
    else
    {
      return  1;
    }
	}
#else
  y_uint32_t tickstart = y_get_tick();
  while (1)   // �ȴ����ݽ������
	{
    if (y_get_tick() - tickstart > timeout)    // ����Ƿ�ʱ
    {
			return -1;    // ��ʱ����
		}
    
    if (get_recv_len() >= max_len)
    {
      if (max_len != 1)
        break;
      
      data_temp = recv_buf;                                 // ��ȡ���յ�������
      if (*data_temp == Y_SOH || *data_temp == Y_STX)       // ��һ����SOH��˵��������Ҫ����133���ֽ�
      {
        max_len = data_len[*data_temp] + 3 + 2;             // ���ݲ�ͬ��ͷ��¼��ͬ�ĳ���
      }
      else
      {
        break;
      }
    }
	}
#endif

	return 0;
}

/**
 * @brief   ��λ���ݽ��ճ���
  * @param  void.
 * @return  void.
 */
static void reset_recv_len(void)
{
  recv_len = 0;
}

/**
 * @brief   ��ȡ���ݽ��ճ���
 * @param   void.
 * @return  ���յ����ݵĳ���.
 */
static y_uint32_t get_recv_len(void)
{
  return recv_len;
}

/**
 * @brief   ����Э�鿪ʼ��������.
 * @param   void.
 * @return  void.
 */
void ymodem_start_recv(void)
{
  if (y_received_status == Y_DIS)
  {
    y_received_status = Y_EN;
    y_transmit_ch(Y_C);
  }
}

/**
 * @brief   ȡ������.
 * @param   void.
 * @return  void.
 */
void ymodem_cancel_recv(void)
{
  y_received_status = Y_DIS;
  y_transmit_ch(Y_CAN);
  y_transmit_ch(Y_CAN);
}

/**
 * @brief   ��ȡ����״̬.
 * @param   void.
 * @return  EN or DIS.
 */
ymodem_fun_status_t get_ymodem_recv_status(void)
{
  return y_received_status;
}

/**
 * @brief   �������ݴ���
 * @param   *data:  Ҫ��������ݵ�����.
 * @param   data_len: ���ݵĴ�С
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
 * @brief   ��ȡ����ʱ���.
 * @param   void
 * @return  ʱ���
 */
__weak y_uint32_t y_get_tick(void)
{
	
	return 0;
}


/**
 * @brief   Ymodem ����һ���ַ��Ľӿ�.
 * @param   ch �����͵�����
 * @return  ���ط���״̬
 */
__weak int y_transmit_ch(y_uint8_t ch)
{
	Y_UNUSED(ch);
	
	return -1;
}

/**
 * @brief   �ļ����ʹ�С������ɻص�.
 * @param   *ptr: ���ƾ��.
 * @param   *file_name: �ļ�����.
 * @param   file_size: �ļ���С����Ϊ0xFFFFFFFF����˵����С��Ч.
 * @return  ����д��Ľ����0���ɹ���-1��ʧ��.
 */
__weak int receive_nanme_size_callback(void *ptr, char *file_name, y_uint32_t file_size)
{
  Y_UNUSED(ptr);
  Y_UNUSED(file_name);
  Y_UNUSED(file_size);
  
  /* �û�Ӧ�����ⲿʵ��������� */
  return -1;
}

/**
 * @brief   �ļ����ݽ�����ɻص�.
 * @param   *ptr: ���ƾ��.
 * @param   *file_name: �ļ�����.
 * @param   file_size: �ļ���С����Ϊ0xFFFFFFFF����˵����С��Ч.
 * @return  ����д��Ľ����0���ɹ���-1��ʧ��.
 */
__weak int receive_file_data_callback(void *ptr, char *file_data, y_uint32_t w_size)
{
  Y_UNUSED(ptr);
  Y_UNUSED(file_data);
  Y_UNUSED(w_size);
  
  /* �û�Ӧ�����ⲿʵ��������� */
  return -1;
}

/**
 * @brief   һ���ļ�������ɻص�.
 * @param   *ptr: ���ƾ��.
 * @return  ����д��Ľ����0���ɹ���-1��ʧ��.
 */
__weak int receive_file_callback(void *ptr)
{
  Y_UNUSED(ptr);
  
  /* �û�Ӧ�����ⲿʵ��������� */
  return -1;
}

