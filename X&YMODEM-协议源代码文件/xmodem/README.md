# XMODEM使用说明

## 用户应调用函数

**1**. 用户需要调用**ymodem_receive()**函数，在这个函数里面是现实接收到数据并进行处理，这个函数是阻塞，要等待文件传输结束后才会返回，返回的数据0表示文件接收成功，返回的数据-1则表示文件接收失败。该函数原型如下所示：

```c
/**
 * @brief   这个函数是Xmodem协议的基础.
 *          接收数据并处理数据.
 * @param   void
 * @return  0:文件接收成功 -1:文件接收失败
 */
int xmodem_receive(void);
```

**2**. 用户需要调用接收数据处理函数，将接收到的数据拷贝给XMODEM协议进行处理，例如：在串口中断服务函数中将接收到的数据拷贝给XMODEM协议进行处理。该函数原型如下所示：

```c
/**
 * @brief   接收数据处理
 * @param   *data:  要计算的数据的数组.
 * @param   data_len: 数据的大小
 * @return  void.
 */
void xmodem_data_recv(x_uint8_t *data, x_uint16_t data_len);
```

## 需用户实现函数

**1**. 关于判断接收数据是否超时这里提供了两种方法，在**xmodem.h**中有如下宏定义，**TIMEOUT_CONFIG**为0时，使用精确时间进行检测是否超时，但需要用户实现 **x_uint32_t x_get_tick(void)** 函数，该函数应返回毫秒时间戳，当然，也可以是其他值，不过需要调整 **X_RECEIVE_TIMEOUT** 的值，这个值就是超时时间；**TIMEOUT_CONFIG**为1时，使用循环减 X_RECEIVE_TIMEOUT 到0的方法，不同频率的开发板可能会需要不同的数值，推荐超该超时配置为3秒。该函数原型如下所示：

```c
/* 检测超时错误配置 */
/* 0：使用精确时间进行检测，需实现 x_uint32_t x_get_tick(void) 函数，
      该函数应返回毫秒时间戳，当然，也可以是其他值，不过需要调整 X_RECEIVE_TIMEOUT 的值
      
   1：使用循环减 X_RECEIVE_TIMEOUT 到数值0的方法，不同频率的开发板可能会需要不同的数值 */
#define TIMEOUT_CONFIG      1

/* 接收数据超时错误，推荐配置为 3 秒超时. */
#if TIMEOUT_CONFIG
#define X_RECEIVE_TIMEOUT    0xFFFFFFu
#else
#define X_RECEIVE_TIMEOUT    3000u
#endif

/**
 * @brief   获取毫秒时间戳.
 * @param   void
 * @return  时间戳
 */
x_uint32_t x_get_tick(void);
```

**2**. **x_transmit_ch()**：发送一个字符接口，用户**必要**实现该函数，协议需使用该函数发送一个字符给主机（发送端）。

```c
/**
 * @brief   Xmodem 发送一个字符的接口.
 * @param   ch ：发送的数据
 * @return  返回发送状态
 */
int x_transmit_ch(x_uint8_t ch);
```

**4**. 文件数据接收完成回调函数，用户**必须**实现这个函数，成功返回0，失败返回-1，当接收完一帧数据后后就会调用这个函数，用户可以根据自己的需求来实现这个函数，例如：将接收数据写入自己的存储器中，或者进行其他处理。

```c
/**
 * @brief   文件数据接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_data_callback(void *ptr, char *file_data, y_uint32_t w_size);
```

**5**. 文件接收完成回调函数，用户**必须**实现这个函数，如果确实用不到也需要实现，并且应始终返回0，即默认操作成功，当接收完文件后就会调用这个函数，用户可以根据自己的需求来实现这个函数。

```c
/**
 * @brief   一个文件接收完成回调.
 * @param   *ptr: 控制句柄.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_callback(void *ptr);
```