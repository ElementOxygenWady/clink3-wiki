# <a name="目录">目录</a>
+ [MCU+支持TCP的模组](#MCU+支持TCP的模组)
    * [应用场景说明](#应用场景说明)
    * [文档目标](#文档目标)
    * [设备端开发过程](#设备端开发过程)
    * [SDK配置与代码抽取](#SDK配置与代码抽取)
+ [SDK与模组对接结构说明](#SDK与模组对接结构说明)
    * [配置SDK](#配置SDK)
        - [运行配置命令](#运行配置命令)
        - [使能需要的SDK功能](#使能需要的SDK功能)
    * [抽取选中功能的源代码](#抽取选中功能的源代码)
    * [实现HAL对接函数](#实现HAL对接函数)
        - [MCU系统相关HAL](#MCU系统相关HAL)
        - [参照example实现产品功能](#参照example实现产品功能)
        - [功能调试](#功能调试)
        - [设备端高级版开发过程](#设备端高级版开发过程)

# <a name="MCU+支持TCP的模组">MCU+支持TCP的模组</a>

## <a name="应用场景说明">应用场景说明</a>
应用场景: 设备的硬件由一个MCU加上一个通信模组构成, 设备的应用逻辑运行在MCU上, 模组上支持了TCP但是并不支持MQTT, MCU通过模组提供的AT指令来控制模组何时连接云端服务以及收发数据

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f1.png)


对于这样的场景, 设备厂商需要将Link Kit SDK集成并运行在MCU上, 让Link Kit SDK通过通信模组连接到阿里云物联网平台

## <a name="文档目标">文档目标</a>
下面的文档关注于讲解用户如何把SDK移植到MCU, 并与通信模组协作来与阿里云物联网平台通信. 为了简化移植过程, 下面的文档在MCU上以开发一个基础版产品作为案例进行讲解, 如果用户需要在MCU上使用SDK的其它功能, 可以在MCU上将基础版的example正确运行之后, 再重新配置SDK, 选中其它功能再进行产品功能开发


## <a name="设备端开发过程">设备端开发过程</a>
如何在阿里云物联网平台创建基础版产品和设备请参见[产品创建(基础版)](https://help.aliyun.com/document_detail/73724.html)

设备端的开发过程如下所示:

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f2.png)


## <a name="SDK配置与代码抽取">SDK配置与代码抽取</a>
SDK中有各种功能模块, 用户需要决定:

+ 需要使用哪些功能(SDK配置)

SDK提供了配置工具用于配置需要使能哪些功能, 每个功能的配置选项名称类似FEATURE_MQTT_XXX, 下面的章节中会讲解具体有哪些功能可供配置

+ SDK如何与外部模组进行数据交互

SDK使用MQTT与阿里云物联网平台通信, MCU连接的模组有的只支持TCP/有的支持MQTT, 因此SDK与模组的数据交互模式存在差异, 比如如果模组支持MQTT Client/那么在MCU上就可以不用编译SDK中自带的MQTT Client了, SDK收到用户APP发送过来的数据时只需要将数据内容发送给模组即可, 由模组将数据组装成MQTT协议报文后发送到物联网平台

对于模组只支持TCP的情况, 意味着MCU上需要使能SDK自带的MQTT Client, 由MQTT Client将用户数据封装成MQTT协议之后通过通信模组上的TCP模块将数据发送到阿里云物联网平台

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f3.png)

MQTT Client与模组协作的时候, 开发者需要编写一个TCP连接管理模块去实现如下功能:

+ 控制模组发起到阿里云物联网平台的TCP连接, 并记录模组返回的TCP连接ID
+ 当MQTT Client发送数据时, 将数据通过MQTT Client创建的模组TCP连接ID进行数据发送
+ 接收来自模组的字符流数据, 只有是从MQTT Client建立的TCP连接ID中的数据才能发送给MQTT Client
+ 如果MQTT Client接收和处理数据的速度慢于模组发送数据给MCU的速度, 开发者还需要将数据进行缓存以避免数据丢失

该TCP连接管理模组与SDK/TCP模组关系如下图所示:

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f4.png)

Link Kit SDK中包含了一个模组TCP连接和数据缓存的模块, 称为AT TCP, 如果开发者在MCU上尚未实现这样一个模组TCP连接和数据缓存的模块, 可以使用SDK提供的AT TCP

MCU与模组之间通常使用UART进行连接, 因此开发者需要开发代码对UART进行初始化, 通过UART接收来自模组的数据, 由于UART是一个字符一个字符的接收数据, 因此开发者还需要对收到的数据组装并判断AT指令是否承载TCP数据, 如果是才能将TCP数据发送给TCP连接管理模块. 这个模块与SDK/模组的关系如下图所示:

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f5.png)

Link Kit SDK中包含了一个AT解析的模块, 称为AT Parser,   如果开发者尚未实现这样的一个功能模块, 可以使能Link Kit SDK中的AT Parser模块以减少开发工作量

# <a name="SDK与模组对接结构说明">SDK与模组对接结构说明</a>

下面是SDK与TCP模组的可能对接方式的图示, 以及相关的配置选项:

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f6.png)

上图中SDK外部的<font color=blue>蓝色配置</font>选项表示该选项使能后将会使能的SDK功能模块, 每根<font color=red>红色虚线</font>代表一种可能的SDK与模组的对接方式, HAL表示该模块与外部模块交互的函数定义:


| 配置选项 | 说明 | 可选项
|-------|------|------
| FEATURE_MQTT_COMM_ENABLED | 是否需要为MQTT提供API | 必选 |
| FEATURE_MQTT_DEFAULT_IMPL | 使能后将包含SDK中的MQTT实现, 并提供TCP HAL函数用于在TCP上收发MQTT数据 | 必选 |
| FEATURE_MQTT_ATM_ENABLED | 是否使能SDK中的ATM模块 | 可选 |
| FEATURE_AT_TCP_ENABLED | 是否使能AT TCP模块, 当本模块被使能后, AT TCP将会提供TCP HAL实现<br>也就是说开发者无需再实现TCP HAL, 但是开发者需要实现AT TCP HAL | 可选 |
| FEATURE_AT_PARSER_ENABLED | 是否使能AT Parser模块, 当本模块被使能后, AT Parser将会提供AT TCP HAL实现<br>也就是说开发者无需再实现AT TCP HAL, 但是开发者需要实现AT Parser HAL | 可选 |


## <a name="配置SDK">配置SDK</a>
SDK包含的功能较多, 为了节约对MCU RAM/Flash资源的消耗, 用户需要根据自己的产品功能定义需要SDK中的哪些功能

### <a name="运行配置命令">运行配置命令</a>
+ Linux系统

进入SDK的根目录下, 运行命令

```
make menuconfig
```

+ Windows系统

运行SDK根目录下的config.bat

```
config.bat
```

### <a name="使能需要的SDK功能">使能需要的SDK功能</a>

运行上面的命令之后, 将会跳出下面的功能配置界面. 按下空格键可以选中或者失效某个功能, 使用小键盘的上下键来在不同功能之间切换. 如果想知道每个选项的具体含义, 先用方向键将高亮光条移到那个选项上, 再按键盘上的"h"按键, 将出现帮助文本, 说明选项是什么含义, 打开了和关闭了意味着什么

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f7_upadted.jpg)


如果编译环境有自带标准头文件<stdint.h>, 请使能选项

+ PLATFORM_HAS_STDINT

如果目标系统上运行有嵌入式操作系统, 请使能选项
+ PLATFORM_HAS_OS

本场景中由于模组支持TCP但是不支持MQTT, 因此必须使能下面两项配置:

+ FEATURE_MQTT_COMM_ENABLED, 使用阿里SDK提供的MQTT API与云端通信
+ FEATURE_MQTT_DEFAULT_IMPL, 使用阿里SDK中自带的MQTT Client实现, 用户需要实现相关的TCP连接的创建/连接/数据收发过程
+ FEATURE_ATM_ENABLED

是否使能ATM模块, 当本模块使能之后, 还会出现是否使能AT TCP的配置选项:

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f8.png)


开发者可以根据产品的实际情况选择是否使能ATM以及AT TCP. 如果开发者使能了ATM, 但是开发者没有用于AT收发/解析的框架, 可以选择使用at_parser框架(非必须):

+ FEATURE_AT_PARSER_ENABLED

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f9.png?x-oss-process=image/resize,w_1108)

SDK基于at_parser提供了已对接示例, 如果模组是sim800 2G模组或者mk3060 Wi-Fi模组, 可以进行进一步选择模组的型号, 可以让SDK将相应的HAL实现也包含在抽取的代码中. 如果不需要对接示例, 请忽略该步骤

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f10.png?x-oss-process=image/resize,w_1108)

完整的配置开关说明表格如下, 但最终解释应以上面提到的"h"按键触发文本为准

| 配置开关 | 说明
|------|------
| PLATFORM_HAS_STDINT | 告诉SDK当前要移植的嵌入式平台是否有自带标准头文件<stdint.h> |
| PLATFORM_HAS_OS | 目标系统是否运行某个操作系统 |
| FEATURE_MQTT_COMM_ENABLED	MQTT | 长连接功能, 打开后将使SDK提供MQTT网络收发的能力和接口 |
| FEATURE_MQTT_DEFAULT_IMPL | SDK内包含的MQTT Client实现, 打开则表示使用SDK内置的MQTT客户端实现 |
| FEATURE_ASYNC_PROTOCOL_STACK | 对于使用SDK内置的MQTT客户端实现的时候, 需要用户实现TCP相关的HAL, 这些HAL的TCP发送数据/接收数据的定义是同步机制的, 如果目标系统的TCP基于异步机制, 可以使能该开关实现SDK从同步到异步机制的转换 |
| FEATURE_DYNAMIC_REGISTER | 动态注册能力, 即设备端只保存了设备的ProductKey和ProductSecret和设备的唯一标识, 通过该功能从物联网平台换取DeviceSecret |
| FEATURE_DEVICE_MODEL_ENABLE | 使能设备物模型编程相关的接口以及实现 |
| FEATURE_DEVICE_MODEL_GATEWAY | 网关的功能以及相应接口 |
| FEATURE_THREAD_COST_INTERNAL | 为收包启动一个独立线程 |
| FEATURE_SUPPORT_TLS | 标准TLS连接, 打开后SDK将使用标准的TLS1.2安全协议连接服务器 |
| FEATURE_SUPPORT_ITLS | 阿里iTLS连接, 打开后SDK将使用阿里自研的iTLS代替TLS建立安全连接 |
| FEATURE_ATM_ENABLED | 如果系统是使用MCU+外接模组的架构, 并且SDK运行在MCU上, 必须打开该选项, 然后进行配置 |
| FEATURE_AT_MQTT_ENABLED | 如果MCU连接的通信模组支持MQTT AT, 则使用该选项 |
| FEATURE_AT_PARSER_ENABLED | 如果用户需要使用SDK提供的AT收发/解析的框架, 则可以使用该选项 |
| FEATURE_AT_MQTT_HAL_ICA | 基于at_parser的ICA MQTT AT对接示例 |
| FEATURE_AT_MQTT_HAL_SIM800 | 基于at_parser的SIM800 MQTT对接示例 |



使能需要的SDK配置后, 保持配置并退出SDK配置工具

## <a name="抽取选中功能的源代码">抽取选中功能的源代码</a>

运行SDK根目录下的extract.bat(Linux下运行extract.sh), 客户选中的功能所对应的代码将会被放置到文件夹output, 如下图所示:

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f11.png)


## <a name="实现HAL对接函数">实现HAL对接函数</a>
Link Kit SDK被设计为可以在不同的操作系统上运行, 或者甚至在不支持操作系统的MCU上运行, 因此与系统相关的操作被定义成一些HAL函数, 需要客户进行实现. 另外, 由于不同的通信模组支持的AT指令集不一样, 所以与通信模组上TCP相关的操作也被定义成HAL函数需要设备开发者进行实现

由于不同的用户使能的SDK的功能可能不一样, 因此需要对接的HAL函数会不一样, 设备开发者只需要实现位于文件output/eng/wrappers/wrapper.c中的HAL函数. 下面对所有可能出现在文件wrapper.c的HAL函数进行讲解:

### <a name="MCU系统相关HAL">MCU系统相关HAL</a>


#### <a name="必须实现函数:">必须实现函数:</a>
| **#**	| 函数名 | 说明
|---|---|---
| 1	| HAL_Malloc | 对应标准C库中的malloc(), 按入参长度开辟一片可用内存, 并返回首地址 |
| 2	| HAL_Free	| 对应标准C库中的free(), 将入参指针所指向的内存空间释放, 不再使用 |
| 3	| HAL_Printf | 对应标准C库中的printf(), 根据入参格式字符串将字符文本显示到终端 |
| 4 | 	HAL_Snprintf |	类似printf, 但输出的结果不再是显示到终端, 而是存入指定的缓冲区内存 |
| 5	| HAL_UptimeMs	| 返回一个uint64_t类型的数值, 表达设备启动后到当前时间点过去的毫秒数 |
| 6	 | HAL_SleepMs	| 按照指定入参的数值, 睡眠相应的毫秒, 比如参数是10, 那么就会睡眠10毫秒 |

对以上函数若需了解更多细节, 可访问[SDK官方文档页面](https://help.aliyun.com/document_detail/100111.html)


#### <a name="OS相关可选函数">OS相关可选函数</a>
如果MCU没有运行OS, 或者SDK的MQTT API并没有在多个线程中被调用, 以下函数可以不用修改wrapper.c中相关的函数实现. 在有OS场景下并且MQTT API被APP在多个线程中调用, 则需要用户对接以下函数:

| **#** |	函数名	| 说明 |
|---|---|---
| 1	| HAL_MutexCreate |	创建一个互斥锁, 返回值可以传递给HAL_MutexLock/Unlock |
| 2	| HAL_MutexDestroy |	销毁一个互斥锁, 这个锁由入参标识 |
| 3	| HAL_MutexLock | 	申请互斥锁, 如果当前该锁由其它线程持有, 则当前线程睡眠, 否则继续 |
| 4	| HAL_MutexUnlock |	释放互斥锁, 此后当前在该锁上睡眠的其它线程将取得锁并往下执行 |
| 5	| HAL_SemaphoreCreate |	创建一个信号量, 返回值可以传递给HAL_SemaphorePost/Wait |
| 6	| HAL_SemaphoreDestroy |	销毁一个信号量, 这个信号量由入参标识 |
| 7	| HAL_SemaphorePost	| 在指定的计数信号量上做自增操作, 解除其它线程的等待 |
| 8	| HAL_SemaphoreWait |	在指定的计数信号量上等待并做自减操作 |
| 9 | HAL_ThreadCreate | 根据配置参数创建thread |

对以上函数若需了解更多细节, 可访问[SDK官方文档页面](https://help.aliyun.com/document_detail/100111.html)


#### <a name="TCP相关HAL">TCP相关HAL</a>
如果用户未选择ATM, 用户适配时调用模组提供的TCP AT指令实现四个TCP HAL函数. 下面是对这些函数的说明, 也可访问[SDK官方文档](https://help.aliyun.com/document_detail/100111.html)页面了解更多说明

| **#** | 函数名 |	说明 |
|---|---|---
| 1 | HAL_TCP_Establish | 建立一个TCP连接 <br> 注意: <br> + 入参host是一个域名, 需要转换为IP地址<br> + 返回值是tcp的socket号 |
| 2 | HAL_TCP_Destroy | 关闭tcp连接, 入参是HAL_TCP_Establish的返回值, 返回值0表示成功 |
| 3 | HAL_TCP_Write	| 通过TCP连接发送数据 <br> 注意: <br> + 该函数传入了一个超时时间, 如果超时仍未将数据发送结束那么函数也需要返回.  <br> + 如果TCP连接已断开, 需要返回一个小于0的负数 |
| 4 | HAL_TCP_Read | 在指定的时间内读取数据并返回, 该函数的入参中指定了可接收的数据的最大长度, 如果从TCP中读取到该最大长度的数据, 那么可以立即返回 |


#### <a name="AT TCP相关HAL">AT TCP相关HAL</a>

如果用户选择使用ATM, 并且未使能AT 使用, 用户适配时调用模组提供的TCP AT指令实现相应的ATM HAL函数, **无需实现上述TCP HAL+**. 下面是对这些函数的说明:

| **#** | 接口名 | 说明 |
|---|---|---
| 1 | HAL_AT_CONN_Init | 该接口需要对通信模组进行相关初始化, 使通信模组达到可以工作的状态 |
| 2 | HAL_AT_CONN_Deinit | 该接口需要提供对通信模组的去初始化操作 |
| 3 | HAL_AT_CONN_Start | 该接口需要模组启动一次连接. 上层传给底层的参数为一个结构体指针at_conn_t, 其参数说明如下:  <br > fd: 每个连接对应的句柄.  <br> type: 建立连接的类型(如TCP_client), 见at_wrapper.h.  <br>  addr: 对端ip或者域名. <br> r_port: 远端端口号. <br> l_port: 本地端口号. <br> tcp_keep_alive: tcpkeep alive的时间 |
| 4 | HAL_AT_CONN_Close | 该接口关闭模组的一个连接. 入参说明如下: <br> fd: 需要关闭的socket句柄. <br> remote_port: 对端端口号, 该参数为可选参数, 小于0时为无效参数. |
| 5 | HAL_AT_CONN_Send | 该接口通过模块发送数据的接口, 该接口为阻塞接口, 直到模组通知底层控制模块数据发送成功才会返回. 入参说明如下: <br> fd: 发送数据所操作的句柄 <br> data: 待发送数据的指针 <br> len: 待发送数据的长度 <br>  remote_ip[16], 对端ip地址, 为可选参数, 为NULL时无效 <br> remote_port: 对端端口号, 为可选参数, 小于0时无效
| 6 | HAL_AT_CONN_DomainToIp |该接口提供获取对应域名ip地址的功能, 注意: 1/即使该域名对应多个ip, 也只会返回一个ip地址. 2/目前该接口只需要支持ipv4. 入参说明如下: <br> domain: 域名信息 <br> ip[16]: 点格式的ip字符串, 目前只支持ipv4 |

#### <a name="AT TCP HAL对接示例">AT TCP HAL对接示例</a>
##### <a name="(1)HAL_AT_CONN_Init">(1)HAL_AT_CONN_Init</a>


该函数完成HAL层数据初始化. 下面的代码示例中, 创建了容量为LINK_ID_MAX的TCP连接数组, 用于对应模组上创建的TCP连接. 用户还需要在此处完成与模组必要的交互, 例如sim800模组对接时需要附着网络/获取ip地址等
```
typedef struct link_s {
    int fd;
    ....
} link_t;
static link_t g_link[LINK_ID_MAX];
int HAL_AT_CONN_Init(void)
{
    int link;

    memset(g_link, 0, sizeof(g_link));
    for (link = 0; link < LINK_ID_MAX; link++) {
        g_link[link].fd = -1;
    }

    ...
    inited = true;
    return 0;
}
```
##### <a name="(2)HAL_AT_CONN_Deinit">(2)HAL_AT_CONN_Deinit</a>

该函数完去初始化, 将HAL_AT_CONN_Init()中分配的资源释放. 下面的示例中只是简单的将inited变量设置为false:
```
int HAL_AT_CONN_Deinit(void)
{
    if (!inited) {
        return 0;
    }
    inited = false;
    return 0;
}
```
##### <a name="(3)HAL_AT_CONN_Start">(3)HAL_AT_CONN_Start</a>
该函数用于建立TCP连接. 下面的示例代码主要包括从g_link数组中获得空闲的TCP连接元素/记录该连接的地址/端口等信息, 向模组发送拼接生成的建立TCP连接的AT指令. 用户需要将该函数实现修改为实际使用的模组提供的AT指令, 并对返回数据进行相应处理
```
int HAL_AT_CONN_Start(at_conn_t *c)
{
    int link_id;
    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd >= 0) {
            continue;
        } else {
            g_link[link_id].fd = c->fd;
            break;
        }
    }
    ...
    /* 拼接AT命令 */
    snprintf(cmd, START_CMD_LEN, "%s=%d,%s,%s,%d",
             START_CMD, link_id, start_cmd_type_str[c->type],
             c->addr, c->r_port);
    ....
    /* 发送AT命令 */
    at_send_wait_reply(cmd, strlen(cmd), true, out, sizeof(out), NULL);
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        goto err;
    }

    return 0;
err:
    // error handle
}
```
##### <a name="(4)HAL_AT_CONN_Close">(4)HAL_AT_CONN_Close</a>
该函数用于关闭TCP连接. 下面的示例代码主要包括记录向模组发送动态生成的AT指令, 然后删除linkid与fd的映射. 其中, fd_to_linkid()是fd向linkid转换函数
```
int HAL_AT_CONN_Close(int fd, int32_t remote_port)
{
    int link_id;
    char cmd[STOP_CMD_LEN] = {0}, out[64];
    link_id = fd_to_linkid(fd);
    if (link_id < 0 || link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }
    snprintf(cmd, STOP_CMD_LEN - 1, "%s=%d", STOP_CMD, link_id);
    LOGD(TAG, "%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);
    at_send_wait_reply(cmd, strlen(cmd), true, out, sizeof(out), NULL);
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        goto err;
    }
    ...
    g_link[link_id].fd = -1;
}
```
##### <a name="(5)HAL_AT_CONN_Send">(5)HAL_AT_CONN_Send</a>
该函数用于向模组发送数据, 主要工作是向模组发送拼接生成AT命令与数据. 下面的示例代码是向模组上指定的TCP socket发送数据, 用户需要将AT指令修改为实际连接模组对应的AT指令以及进行相应处理
```
int HAL_AT_CONN_Send(int fd,
                     uint8_t *data,
                     uint32_t len,
                     char remote_ip[16],
                     int32_t remote_port,
                     int32_t timeout)
{
    int link_id;
    char cmd[SEND_CMD_LEN] = {0}, out[128] = {0};
    if (!data) {
        return -1;
    }
    link_id = fd_to_linkid(fd);
    if (link_id < 0 || link_id >= LINK_ID_MAX) {
        LOGE(TAG, "No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }
    /* AT+CIPSEND=id, */
    snprintf(cmd, SEND_CMD_LEN - 1, "%s=%d,", SEND_CMD, link_id);
    /* [remote_port,] */
    if (remote_port >= 0) {
        snprintf(cmd + strlen(cmd), 7, "%d,", remote_port);
    }
    at_send_data_2stage((const char *)cmd, (const char *)data, len, out, sizeof(out));
}
```
(6)HAL_AT_CONN_DomainToIp
该函数用于域名解析, 向模组发送动态生成的查询命令后, 根据已知格式解析回复
```
int HAL_AT_CONN_DomainToIp(char *domain, char ip[16])
{
    char cmd[DOMAIN_CMD_LEN] = {0}, out[256] = {0}, *head, *end;
    snprintf(cmd, DOMAIN_CMD_LEN - 1, "%s=%s", DOMAIN_CMD, domain);

    /* 发送查询命令 */
    at_send_wait_reply(cmd, strlen(cmd), true, out, sizeof(out), NULL);
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, AT_RECV_SUCCESS_POSTFIX) == NULL) {
        LOGE(TAG, "%s %d failed", __func__, __LINE__);
        return -1;
    }
    /* 根据已知格式解析回复 */
    ...
}
```
##### <a name="调用接收函数">调用接收函数</a>
用户在AT HAL层收到数据后需要调用API接口IOT_ATM_Input(见atm/at_api.h), 将数据交付给上层
```
void handle_recv_data()
{
    struct at_conn_input param;
    ...
    /* 读取AT指令中数据长度信息 */
    len = atoi(reader);
    if (len > MAX_DATA_LEN) {
        LOGE(TAG, "invalid input socket data len %d \r\n", len);
        return;
    }

    /*分配接收buffer, 用户也可以直接使用一个静态数组用于数据接收*/
    recvdata = (char *)aos_malloc(len);
    if (!recvdata) {
        LOGE(TAG, "Error: %s %d out of memory, len is %d. \r\n", __func__, __LINE__, len);
        return;
    }

    /* 读取数据 */
    ret = at_read(recvdata, len);
    if (ret != len) {
        LOGE(TAG, "at read error recv %d want %d!\n", ret, len);
        goto err;
    }

    if (g_link[link_id].fd >= 0) {
        param.fd = g_link[link_id].fd;
        param.data = recvdata;
        param.datalen = len;
        param.remote_ip = NULL;
        param.remote_port = 0;

        /* 向上层交付数据 */
        if (IOT_ATM_Input(&param) != 0) {
            at_conn_hal_err(" %s socket %d get data len %d fail to post to at_conn, drop it\n",
                 __func__, g_link[link_id].fd, len);
        }
    }
    ...
}

```
其中,  struct at_conn_input定义见at_wrapper.h

```
struct at_conn_input {
    int        fd;   /* 数据上送需要操作的句柄 */
    void      *data; /* 接收到的数据(该部分内存由底层自行释放)*/
    uint32_t   datalen; /* 接收到的数据长度 */
    char      *remote_ip; /* 该数据的源地址, 为可选参数, 可以传入NULL(该部分内存由底层自行释放)*/
    uint16_t   remote_port; /* 该数据的源端口, 为可选参数, 可以传入0 */
};
```

#### <a name="AT Parser相关HAL">AT Parser相关HAL</a>
如果选择了at_parser框架, 则需要对接以下四个UART HAL函数, 函数声明见at_wrapper.h. 如果用户不使用at_parser框架请忽略该步骤


| **#** | 接口名 | 说明 |
|---|---|---
| 1 | HAL_AT_Uart_Init | 该接口对UART进行配置(波特率/停止位等)并初始化 |
| 2 | HAL_AT_Uart_Deinit | 该接口对UART去初始化 |
| 3 | HAL_AT_Uart_Send | 该接口用于向指定的UART口发送数据 |
| 4 | HAL_AT_Uart_Recv | 该接口用于从底层UART buffer接收数据 |

##### <a name="产品相关HAL">产品相关HAL</a>
下面的HAL用于获取产品的身份认证信息, 设备厂商需要设计如何在设备上烧写设备身份信息, 并通过下面的HAL函数将其读出后提供给SDK:

| **#** | 函数名 | 说明 |
|---|---|---
| 1	| HAL_GetProductKey |	获取设备的ProductKey, 用于标识设备的产品型号 |
| 2	| HAL_GetDeviceName	| 获取设备的DeviceName, 用于唯一标识单个设备 |
| 3	| HAL_GetDeviceSecret |	获取设备的DeviceSecret, 用于标识单个设备的密钥 |

对以上函数若需了解更多细节, 可访问[SDK官方文档页面](https://help.aliyun.com/document_detail/100111.html)

#### <a name="代码集成">代码集成</a>
如果设备商的开发环境使用makefile编译代码, 可以将SDK抽取出来的代码加入其编译环境进行编译. 如果设备商使用KEIL/IAR这样的开发工具, 可以将SDK抽取出来的代码文件加入到IDE的工程中进行编译

下面是将抽取出来的output目录复制到Linux下, 并在output目录下创建的一个makefile的示例

注: 配置时使能了ATM模块/使用AT TCP/AT Parser/并选择使用SIM800作为外接模组

```
SDK_PWD = $(shell pwd)/eng
SDK_DIRS = $(SDK_PWD)/dev_sign $(SDK_PWD)/atm  $(SDK_PWD)/infra $(SDK_PWD)/mqtt $(SDK_PWD)/wrappers

SDK_SOURCES = $(foreach dir,$(SDK_DIRS),$(wildcard $(dir)/*.c))
SDK_OBJS = $(patsubst %.c,%.o,$(SDK_SOURCES))

SDK_INC_DIRS = $(foreach dir, $(SDK_DIRS),-I$(dir) )

TARGET = testmqtt

all:eng/examples/mqtt_example_at.o $(SDK_OBJS)
  $(CC) -o $(TARGET) $(SDK_OBJS) eng/examples/mqtt_example_at.o

clean:
  rm -rf *.o $(TARGET) $(SDK_OBJS)

%.o:%.c
  $(CC) -c $(SDK_INC_DIRS) $< -o $@
  ```
  注:


+ 上面的makefile仅供参考, 用户配置SDK时选用的功能不一样会导致目录出现差别, 用户需要将除了eng/examples外的目录加入编译系统/工具
+ 用户如果复制该makefile使用, 在复制/粘贴时all/clean/%.o:%.c下一行的命令语句可能出现多个空格/或者没有空格就直接是命令, 如果发现在命令前有空格需要将空格全部删除, 然后增加一个Tab键, 以避免make出错

### <a name="参照example实现产品功能">参照example实现产品功能</a>
如果要使用MQTT连云, 可参考抽取文件夹中的 eng/examples/mqtt_example_at.c .  设备厂商可以将该文件复制到产品工程中, 对其进行修改后使用



该example将连接设备到阿里云, 订阅一个指定的topic并发送数据给该topic, 即设备上报的消息会被物联网平台发送给设备, 下面是example的大概过程说明:


![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f12.png)


注意: 需要在云端将该topic从默认的权限从"订阅"修改为"发布和订阅", 如下图所示:


![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f13.png)


从程序入口的 main() 函数看起, 第一步是调用AT模块初始化函数IoT_ATM_Init(), 使模组处于ready状态, 第二步是调用用户提供的HAL函数获取产品信息

```C
int main(int argc, char *argv[])
{
    void *      pclient = NULL;
    int         res = 0;
    int         loop_cnt = 0;
    iotx_mqtt_region_types_t    region = IOTX_CLOUD_REGION_SHANGHAI;
    iotx_sign_mqtt_t            sign_mqtt;
    iotx_dev_meta_info_t        meta;
    iotx_mqtt_param_t           mqtt_params;
#ifdef ATM_ENABLED
    if (IOT_ATM_Init() < 0) {
        HAL_Printf("IOT ATM init failed!\n");
        return -1;
    }
#endif
    HAL_Printf("mqtt example\n");
    memset(&meta, 0, sizeof(iotx_dev_meta_info_t));
    HAL_GetProductKey(meta.product_key);
    HAL_GetDeviceName(meta.device_name);
    HAL_GetDeviceSecret(meta.device_secret);
```
注:

+ 上面的三个HAL_GetXXX函数是获取设备的三元组信息, 设备厂商需要自己设计设备的三元组存放的位置/并将其从指定位置读取出来
+ 由于设备的唯一标识DeviceName/设备密钥DeviceSecret都是机密信息, 设备厂商在设计时可以把相关信息加密后存放到Flash上, 在HAL函数里面将其解密后提供给SDK, 以避免黑客直接从Flash里面读取设备的身份信息

接下来对MQTT连接参数进行指定, 客户可以根据自己的需要对参数进行修改:

```
/* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = sign_mqtt.port;
    mqtt_params.host = sign_mqtt.hostname;
    mqtt_params.client_id = sign_mqtt.clientid;
    mqtt_params.username = sign_mqtt.username;
    mqtt_params.password = sign_mqtt.password;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = 1024;
    mqtt_params.write_buf_size = 1024;
    mqtt_params.handle_event.h_fp = example_event_handle;
    mqtt_params.handle_event.pcontext = NULL;
    pclient = IOT_MQTT_Construct(&mqtt_params);
```
通过调用接口 IOT_MQTT_Construct() 触发SDK连接云平台, 若接口返回值非NULL, 则连云成功

之后调用example_subscribe对一个指定的topic进行数据订阅:

```
res = example_subscribe(pclient);
```
example_subscribe的函数内容如下:
![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f14.png)


注:


+ 设备商需要根据自己的产品设计, 订阅自己希望订阅的TOPIC, 以及注册相应的处理函数
+ 订阅的topic的格式需要指定产品型号(product_key)以及设备标识(device_name), 如上图中第一个橙色框中的格式
+ 上图的第二个框展示了如何订阅一个指定的topic以及其处理函数

以下段落演示MQTT的发布功能, 即将业务报文上报到云平台:
```
 while (1) {
        if (0 == loop_cnt % 20) {
            example_publish(pclient);
        }

        IOT_MQTT_Yield(pclient, 200);

        loop_cnt += 1;
    }
```
下面是example_publish函数体的部分内容:


![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f15.png)


注:


+ 上面的代码是周期性的将固定的消息发送给云端, 设备商需要根据自己的产品功能, 在必要的时候才上传数据给物联网平台
+ 客户可以删除main函数中example_publish(pclient)语句, 避免周期发送无效数据给到云端
+ IOT_MQTT_Yield是让SDK去接收来自MQTT Broker的数据, 其中200毫秒是等待时间, 如果用户的消息数量比较大/或者实时性要求较高, 可以将时间改小

### <a name="功能调试">功能调试</a>
下面的信息截图以mqtt_example.c为例编写

#### <a name="如何判断设备已连接到阿里云">如何判断设备已连接到阿里云</a>
下面的打印是HAL_Printf函数将信息打印到串口后运行example的输出内容, 其中使用橙色圈选的信息表明设备已成功连接到阿里云物联网平台:

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f16.png)

#### <a name="如何判断设备已成功发送数据到云端">如何判断设备已成功发送数据到云端</a>
登录阿里网物联网平台的商家后台, 选中指定的设备, 可以查看是否收到来自设备的消息, 如下图所示:


![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f17.png)

注: 上图中的内容只能看见消息发送到了哪个topic, 消息的内容并不会显示出来

#### <a name="如何判断设备已可成功接收来自云端数据">如何判断设备已可成功接收来自云端数据</a>
在商家后台的"下行消息分析"分析中可以看见由物联网平台发送给设备的消息:
![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f18.png)
也可在设备端查看是否已收到来自云端的数据, exmaple代码中收到云端发送的数据的打印信息如下所示:
![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f19.png)

至此, SDK在MCU与模组之间的适配开发已结束, 用户可以进行产品业务功能的实现



### <a name="设备端高级版开发过程">设备端高级版开发过程</a>
上面的讲解直接基于MQTT消息进行设备与阿里云物联网平台之间的通信, 设备开发者需要自己定义topic以及在topic上通信的数据的格式. 阿里云物联网提供物模型的方式来描述设备功能, 物模型包括了设备属性/服务/事件的定义, 设备与阿里云物联网之间的数据通信格式为JSON格式, 使用物模型定义和开发的产品在阿里云IoT又称为高级版设备

#### <a name="SDK高级版配置与代码抽取">SDK高级版配置与代码抽取</a>
设备端高级版具备所有基础版的功能, 基础版的功能选择在本文档前半部分中已经有详细描述, 这里不再赘述. 高级版功能需选中FEATURE_DEVICE_MODEL_ENABLED:
![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f20.png)

使用抽取脚本后, 将比基础版多产生一个文件夹:

![image](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/atm/f21.png)

仿照基础版的操作, 将该文件夹加入到自己的编译系统中

#### <a name="高级版HAL对接函数">高级版HAL对接函数</a>

在仅选中FEATURE_DEVICE_MODEL_ENABLED时, 高级版无另外需要对接的HAL函数



#### <a name="参照高级版example实现产品功能">参照高级版example实现产品功能</a>

高级版example文件位于代码抽取目录的examples/alink_example_solo.c. 下面将对该example进行讲解:

从程序入口的 main() 函数看起, 第一步是调用用户提供的HAL函数获取产品信息:
```
int main(int argc, char **argv)
{
    user_example_ctx_t *user_example_ctx;
    uint32_t cnt = 0;
    int res = FAIL_RETURN;
    static iotx_linkkit_dev_meta_info_t dev_info;

    /* get triple metadata from HAL */
    HAL_GetProductKey(dev_info.product_key);
    HAL_GetProductSecret(dev_info.product_secret);
    HAL_GetDeviceName(dev_info.device_name);
    HAL_GetDeviceSecret(dev_info.device_secret);
```
接下来注册需要关心的回调函数:
```
    printf("alink start\r\n");

    user_example_ctx = user_example_get_ctx();
    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));
    user_example_ctx->thread_running = 1;

    /* 云端连接成功事件 */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);

    /* 物模型中收到云端设置属性请求的事件 */
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);

    /* 物模型中上报属性等信息后收到的应答事件 */
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);

    /* 物模型中收到云端调用服务请求的事件 */
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);

    /* 设备初始化完成时的事件 */
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
```
创建本地设备资源, 建立与云端的连接:
```
/* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &dev_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }
    EXAMPLE_TRACE("IOT_Linkkit_Open Succeed\n");

    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < SUCCESS_RETURN) {
        return -1;
    }
    EXAMPLE_TRACE("IOT_Linkkit_Connect, res = %d", res);
```
数据上报示例:
```
while (1) {

        /* 从网络收取云端下发的报文 */
        IOT_Linkkit_Yield(2000);

        /* 物模型属性上报示例 */
        res = IOT_Linkkit_Report(IOTX_LINKKIT_DEV_TYPE_MASTER, ITM_MSG_POST_PROPERTY, (uint8_t *)ALINK2_PROP_POST_DATA_TMP, strlen(ALINK2_PROP_POST_DATA_TMP));
        EXAMPLE_TRACE("post property, res = %d", res);

        /* 物模型事件上报示例 */
        res = IOT_Linkkit_TriggerEvent(IOTX_LINKKIT_DEV_TYPE_MASTER, "Empty", strlen("Empty"), ALINK2_EVENT_POST_EMPTY, strlen(ALINK2_EVENT_POST_EMPTY));
        EXAMPLE_TRACE("post event, res = %d", res);

        if (++cnt > 2) {
            IOT_Linkkit_Close(IOTX_LINKKIT_DEV_TYPE_MASTER);
            break;
        }
    }
```
与基础版示例相同, 登录阿里网物联网平台的商家后台, 选中指定的设备, 可以查看是否收到来自设备的消息

更多物模型开发信息请参考[物模型编程指南](https://help.aliyun.com/document_detail/96628.html)
