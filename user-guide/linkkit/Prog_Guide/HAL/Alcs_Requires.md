# <a name="目录">目录</a>
+ [本地通信相关HAL接口详解](#本地通信相关HAL接口详解)
    * [HAL_UDP_bindtodevice](#HAL_UDP_bindtodevice)
    * [HAL_UDP_close](#HAL_UDP_close)
    * [HAL_UDP_close_without_connect](#HAL_UDP_close_without_connect)
    * [HAL_UDP_create_without_connect](#HAL_UDP_create_without_connect)
    * [HAL_UDP_joinmulticast](#HAL_UDP_joinmulticast)
    * [HAL_UDP_recvfrom](#HAL_UDP_recvfrom)
    * [HAL_UDP_sendto](#HAL_UDP_sendto)

# <a name="本地通信相关HAL接口详解">本地通信相关HAL接口详解</a>


## <a name="HAL_UDP_bindtodevice">HAL_UDP_bindtodevice</a>

原型
---
```
int HAL_UDP_bindtodevice(_IN_ intptr_t fd,
                         _IN_ const char *ifname);
```

接口说明
---
绑定UDP socket到指定接口, 只接收来自该接口的数据包

参数说明
---
| 参数    | 数据类型        | 方向    | 说明
|---------|-----------------|---------|-------------------------------------
| fd      | intptr_t        | 输入    | 指定用来绑定的UDP socket
| ifname  | const char *    | 输入    | 指定用来绑定socket的网络接口名字

返回值说明
---
| 值      | 说明
|---------|-----------------
| < 0     | 绑定异常或失败
| = 0     | 绑定成功

-----

## <a name="HAL_UDP_close">HAL_UDP_close</a>

原型
---
```
void HAL_UDP_close(_IN_ intptr_t p_socket);
```

接口说明
---
销毁指定的UDP连接, 释放资源

参数说明
---
| 参数        | 数据类型    | 方向    | 说明
|-------------|-------------|---------|-----------------
| p_socket    | intptr_t    | 输入    | UDP socket句柄

返回值说明
---
void

-----

## <a name="HAL_UDP_close_without_connect">HAL_UDP_close_without_connect</a>

原型
---
```
int HAL_UDP_close_without_connect(_IN_ intptr_t sockfd);
```

接口说明
---
销毁指定的UDP连接, 释放资源

参数说明
---
| 参数    | 数据类型    | 方向    | 说明
|---------|-------------|---------|-----------------
| sockfd  | intptr_t    | 输入    | UDP socket句柄

返回值说明
---
| 值      | 说明
|---------|-------------
| < 0     | 操作失败
| = 0     | 操作成功

-----

## <a name="HAL_UDP_create_without_connect">HAL_UDP_create_without_connect</a>

原型
---
```
intptr_t HAL_UDP_create_without_connect(_IN_ const char *host, _IN_ unsigned short port);
```

接口说明
---
创建一个本地的UDP socket, 但并不发起任何网络交互

参数说明
---
| 参数    | 数据类型        | 方向    | 说明
|---------|-----------------|---------|-----------------
| host    | const char *    | 输入    | UDP目的地址
| port    | unsigned short  | 输入    | UDP目的端口

返回值说明
---
| 值      | 说明
|---------|-------------------------------------
| < 0     | 创建失败
| >= 0    | 创建成功, 返回值为UDP socket句柄

-----

## <a name="HAL_UDP_joinmulticast">HAL_UDP_joinmulticast</a>

原型
---
```
int HAL_UDP_joinmulticast(_IN_ intptr_t sockfd,
                          _IN_ char *p_group);
```

接口说明
---
在指定的UDP socket上发送加入组播组的请求

参数说明
---
| 参数        | 数据类型    | 方向    | 说明
|-------------|-------------|---------|-------------------------------------
| sockfd      | intptr_t    | 输入    | 指定用来发送组播请求的UDP socket
| p_group     | char *      | 输入    | 指定要加入的组播组名称

返回值说明
---
| 值      | 说明
|---------|-----------------------------
| < 0     | 发送过程中出现错误或异常
| = 0     | 发送成功

-----


## <a name="HAL_UDP_recvfrom">HAL_UDP_recvfrom</a>

原型
---
```
int HAL_UDP_recvfrom(_IN_ intptr_t sockfd,
                     _OU_ NetworkAddr *p_remote,
                     _OU_ unsigned char *p_data,
                     _IN_ unsigned int datalen,
                     _IN_ unsigned int timeout_ms);
```

接口说明
---
从指定的UDP句柄接收指定长度数据到缓冲区, 阻塞时间不超过指定时长, 且指定长度若接收完需提前返回, 源地址保存在`p_remote`参数中

参数说明
---
| 参数        | 数据类型            | 方向    | 说明
|-------------|---------------------|---------|-----------------------------
| sockfd      | intptr_t            | 输入    | UDP socket句柄
| p_remote    | NetworkAddr *       | 输出    | 指向存放源网络地址的指针
| p_data      | unsigned char *     | 输出    | 指向数据接收缓冲区的指针
| datalen     | unsigned int        | 输入    | 接收缓冲区的字节大小
| timeout_ms  | unsigned int        | 输入    | 阻塞的超时时间, 单位ms

返回值说明
---
| 值      | 说明
|---------|-----------------------------------------------------
| < 0     | 接收过程中出现错误或异常
| = 0     | 在指定的`timeout_ms`时间内, 没有接收到任何数据
| > 0     | 在指定的`timeout_ms`时间内, 实际接收到的数据字节数

-----


## <a name="HAL_UDP_sendto">HAL_UDP_sendto</a>

原型
---
```
int HAL_UDP_sendto(_IN_ intptr_t          sockfd,
                   _IN_ const NetworkAddr *p_remote,
                   _IN_ const unsigned char *p_data,
                   _IN_ unsigned int datalen,
                   _IN_ unsigned int timeout_ms);
```

接口说明
---
向指定UDP句柄发送指定长度的数据, 阻塞时间不超过指定时长, 且指定长度若发送完需提前返回

参数说明
---
| 参数        | 数据类型                | 方向    | 说明
|-------------|-------------------------|---------|-------------------------
| sockfd      | intptr_t                | 输入    | UDP socket句柄
| p_remote    | const NetworkAddr *     | 输入    | 指向目标网络地址的指针
| p_data      | const unsigned char *   | 输入    | 指数据发送缓冲区的指针
| datalen     | unsigned int            | 输入    | 待发送数据的字节长度
| timeout_ms  | unsigned int            | 输入    | 阻塞的超时时间, 单位ms

返回值说明
---
| 值      | 说明
|---------|-----------------------------------------------------
| < 0     | 发送过程中出现错误或异常
| = 0     | 在指定的`timeout_ms`时间内, 没有任何数据发送成功
| > 0     | 在指定的`timeout_ms`时间内, 实际发送的数据字节数

-----
