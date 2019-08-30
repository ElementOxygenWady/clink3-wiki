# <a name="目录">目录</a>
+ [应用场景说明](#应用场景说明)
+ [文档目标](#文档目标)
+ [设备端开发过程](#设备端开发过程)
    * [SDK配置与代码抽取](#SDK配置与代码抽取)
        - [配置SDK](#配置SDK)
        - [运行配置命令](#运行配置命令)
        - [使能需要的SDK功能](#使能需要的SDK功能)
        - [抽取选中功能的源代码](#抽取选中功能的源代码)
    * [实现HAL对接函数](#实现HAL对接函数)
        - [MCU系统相关HAL](#MCU系统相关HAL)
        - [AT MQTT相关HAL](#AT MQTT相关HAL)
        - [调用接收函数](#调用接收函数)
        - [产品相关HAL](#产品相关HAL)
    * [代码集成](#代码集成)
    * [参照example实现产品功能](#参照example实现产品功能)
        - [功能调试](#功能调试)
+ [设备端高级版开发过程](#设备端高级版开发过程)
    * [SDK高级版配置与代码抽取](#SDK高级版配置与代码抽取)
    * [高级版HAL对接函数](#高级版HAL对接函数)
    * [参照高级版example实现产品功能](#参照高级版example实现产品功能)

# <a name="应用场景说明">应用场景说明</a>

应用场景: 设备的硬件由一个MCU加上一个通信模组构成, 设备的应用逻辑运行在MCU上, 通信模组支持MQTT功能并提供AT指令给MCU使用, MCU控制模组何时连接云端服务以及收发数据

![1](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/1.png)

对于这样的场景, 设备厂商需要将Link Kit SDK集成并运行在MCU上, 让Link Kit SDK通过通信模组连接到阿里云物联网平台

# <a name="文档目标">文档目标</a>

下面的文档关注于讲解用户如何把SDK移植到MCU, 并与通信模组协作来与阿里云物联网平台通信. 为了简化移植过程, 下面的文档在MCU上以开发一个基础版产品作为案例进行讲解, 如果用户需要在MCU上使用SDK的其它功能, 可以在MCU上将基础版的example正确运行之后, 再重新配置SDK, 选中其它功能再进行产品功能开发

# <a name="设备端开发过程">设备端开发过程</a>

如何在阿里云物联网平台创建基础版产品和设备请参见"创建产品(基础版)"

设备端的开发过程如下所示:

![2](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/2.png)

## <a name="SDK配置与代码抽取">SDK配置与代码抽取</a>

SDK中有各种功能模块, 用户需要决定:

+ 需要使用哪些功能(SDK配置)


SDK提供了配置工具用于配置需要使能哪些功能, 每个功能的配置选项名称类似FEATURE_MQTT_XXX, 下面的章节中会讲解具体有哪些功能可供配置

+ SDK如何与外部模组进行数据交互

![3](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/3.png)

上图中的三根红色虚线代表SDK可以与MQTT模组进行数据交互的三种方式:

+ MQTT Wrapper

MQTT Wrapper提供了接口函数定义用于与MQTT Client交互, 当MCU外接MQTT模组时指驱动MQTT模组中的MQTT Client与阿里云物联网平台上的MQTT Broker/Server建连/收发MQTT消息. 开发者可以实现相关的wrapper函数来代码来驱动MQTT模组进行MQTT的连接, 无需使能ATM/AT MQTT/AT Parser等功能

+ AT MQTT

当MQTT模组发送MQTT消息给MCU时, 如果模组发送给MCU的数据的速度超过了MCU上处理MQTT消息的速度, 可能导致丢包, 因此SDK中实现了一个AT MQTT模块用于对收到的MQTT消息进行缓存. 开发者如果使能本模块, 本模块将提供MQTT Wrapper函数的实现, 开发者需要实现的函数是AT MQTT HAL中定义的函数, 在这些函数中驱动MQTT模组

+ AT Parser

MCU与模组之间通常使用UART进行连接, 因此开发者需要开发代码对UART进行初始化, 通过UART接收来自模组的数据, 由于UART是一个字符一个字符的接收数据, 因此开发者还需要对收到的数据组装并判断AT指令是否承载MQTT数据, 如果是才能将MQTT数据发送给AT MQTT模块. SDK中提供了AT Parser模块用于完成这些功能, 如果开发者尚未实现在UART上的数据收发/解析等功能, 可以使能AT Parser功能来减少开发工作量

当开发者使能AT Parser后, AT Parser将会提供AT MQTT HAL的实现, 因此开发者需要实现的函数是AT Parser HAL中定义的函数

### <a name="配置SDK">配置SDK</a>

SDK包含的功能较多, 为了节约对MCU RAM/Flash资源的消耗, 用户需要根据自己的产品功能定义需要SDK中的哪些功能

### <a name="运行配置命令">运行配置命令</a>

+ Linux系统

进入SDK的根目录下, 运行命令

    make menuconfig

+ Windows系统

运行SDK根目录下的config.bat

    config.bat

### <a name="使能需要的SDK功能">使能需要的SDK功能</a>

运行上面的命令之后, 将会跳出下面的功能配置界面. 按下空格键可以选中或者失效某个功能, 使用小键盘的上下键来在不同功能之间切换. 如果想知道每个选项的具体含义, 先用方向键将高亮光条移到那个选项上, 再按键盘上的"h"按键, 将出现帮助文本, 说明选项是什么含义, 打开了和关闭了意味着什么


![4](http://http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/4.png)


如果编译环境有自带标准头文件<stdint.h>, 请使能选项

+ PLATFORM_HAS_STDINT

如果目标系统上运行有嵌入式操作系统, 请使能选项

+ PLATFORM_HAS_OS

请务必使能:

+ FEATURE_MQTT_COMM_ENABLED, 用于让SDK提供MQTT API供应用程序调用, 并关闭

+ FEATURE_MQTT_DEFAULT_IMPL, 该选项用于包含阿里提供的MQTT Client实现, 因为模组支持MQTT Client, 所以关闭该选项


SDK连接MQTT模组有几种不同的对接方法, 为了简化对接, 本文档中使能

+ FEATURE_ATM_ENABLED


该选项使能之后具有下面的子选项可供选择, 需要使能

+ FEATURE_AT_MQTT_ENABLED

![5](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/19.png)

【注】: FEATURE_AT_MQTT_ENABLED选项会在FRETURE_MQTT_DEFUALT_IMPL去掉后出现

如果用户没有用于AT命令收发/解析的框架, 可以选择(非必须)使用at_parser框架:

+ FEATURE_AT_PARSER_ENABLED

![6](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/6.png)

SDK基于at_parser提供了已对接示例, 如果模组是支持MQTT的sim800 2G模组或者支持ICA MQTT的WiFi模组, 可以进行进一步选择相应选项, 这样开发的工作量将进一步减少. 如果不需要对接示例, 请忽略该步骤

![7](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/7.png)

完整的配置开关说明表格如下, 但最终解释应以上面提到的"h"按键触发文本为准

| 配置开关 |说明
|------------------------------|----------------------
|PLATFORM_HAS_STDINT | 告诉SDK当前要移植的嵌入式平台是否有自带标准头文件<stdint.h>
|PLATFORM_HAS_OS     |目标系统是否运行某个操作系统FEATURE_MQTT_COMM_ENABLEDMQTT长连接功能, 打开后将使SDK提供MQTT网络收发的能力和接口
|FEATURE_MQTT_DEFAULT_IMPLSDK | 内包含的MQTT Client实现, 打开则表示使用SDK内置的MQTT客户端实现
|FEATURE_ASYNC_PROTOCOL_STACK | 对于使用SDK内置的MQTT客户端实现的时候, 需要用户实现TCP相关的HAL, 这些HAL的TCP发送数据/接收数据的定义是同步机制的, 如果目标系统的TCP基于异步机制, 可以使能该开关实现SDK从同步到异步机制的转换
|FEATURE_DYNAMIC_REGISTER    |  动态注册能力, 即设备端只保存了设备的ProductKey和ProductSecret和设备的唯一标识, 通过该功能从物联网平台换取DeviceSecret
|FEATURE_DEVICE_MODEL_ENABLE  |使能设备物模型编程相关的接口以及实现FEATURE_DEVICE_MODEL_GATEWAY网关的功能以及相应接口
|FEATURE_THREAD_COST_INTERNAL  |为收包启动一个独立线程FEATURE_SUPPORT_TLS标准TLS连接, 打开后SDK将使用标准的TLS1.2安全协议连接服务器
|FEATURE_SUPPORT_ITLS  |阿里iTLS连接, 打开后SDK将使用阿里自研的iTLS代替TLS建立安全连接
|FEATURE_ATM_ENABLED  |如果系统是使用MCU+外接模组的架构, 并且SDK运行在MCU上, 必须打开该选项, 然后进行配置
|FEATURE_AT_MQTT_ENABLED | 如果MCU连接的通信模组支持MQTT AT, 则使用该选项
|FEATURE_AT_PARSER_ENABLED  |如果用户需要使用SDK提供的AT收发/解析的框架, 则可以使用该选项
|FEATURE_AT_MQTT_HAL_ICA    |基于at_parser的ICA MQTT AT对接示例
|FEATURE_AT_MQTT_HAL_SIM800  |基于at_parser的SIM800 MQTT对接示例

使能需要的SDK配置后, 保持配置并退出SDK配置工具。

### <a name="抽取选中功能的源代码">抽取选中功能的源代码</a>

运行SDK根目录下的extract.bat, 客户选中的功能所对应的代码将会被放置到文件夹output:

![8](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/8.png)

## <a name="实现HAL对接函数">实现HAL对接函数</a>

Link Kit SDK被设计为可以在不同的操作系统上运行, 或者甚至在不支持操作系统的MCU上运行, 因此与系统相关的操作被定义成一些HAL函数, 需要客户进行实现. 另外, 由于不同的通信模组支持的AT指令集不一样, 所以与通信模组上TCP相关的操作也被定义成HAL函数需要设备开发者进行实现。

由于不同的用户使能的SDK的功能可能不一样, 因此需要对接的HAL函数会不一样, 设备开发者只需要实现位于文件output/eng/wrappers/wrapper.c中的HAL函数. 下面对所有可能出现在文件wrapper.c的HAL函数进行讲解:

### <a name="MCU系统相关HAL">MCU系统相关HAL</a>

#### <a name="必须实现函数:">必须实现函数:</a>

|   |  函数名      |        说明 |
|----|-------------|-------------|----------
|1  |HAL_Malloc   |对应标准C库中的malloc(), 按入参长度开辟一片可用内存, 并返回首地址
|2  |HAL_Free     |对应标准C库中的free(), 将入参指针所指向的内存空间释放, 不再使用
|3  |HAL_Printf   |对应标准C库中的printf(), 根据入参格式字符串将字符文本显示到终端
|4  |HAL_Snprintf |类似printf, 但输出的结果不再是显示到终端, 而是存入指定的缓冲区内存
|5  |HAL_UptimeMs |返回一个uint64_t类型的数值, 表达设备启动后到当前时间点过去的毫秒数
|6  |HAL_SleepMs  |按照指定入参的数值, 睡眠相应的毫秒, 比如参数是10, 那么就会睡眠10毫秒

对以上函数若需了解更多细节, 可访问[SDK官方文档页面](https://help.aliyun.com/document_detail/100111.html)

#### <a name="OS相关可选函数">OS相关可选函数</a>

如果MCU没有运行OS, 或者SDK的MQTT API并没有在多个线程中被调用, 以下函数可以不用修改wrapper.c中相关的函数实现. 在有OS场景下并且MQTT API被APP在多个线程中调用, 则需要用户对接以下函数:

|     |函数名              |   说明
|----|--------------------|------------
|1   | HAL_MutexCreate   | 创建一个互斥锁, 返回值可以传递给HAL_MutexLock/Unlock
|2   |HAL_MutexDestroy   | 销毁一个互斥锁, 这个锁由入参标识
|3   |HAL_MutexLock      | 申请互斥锁, 如果当前该锁由其它线程持有, 则当前线程睡眠, 否则继续
|4   |HAL_MutexUnlock    | 释放互斥锁, 此后当前在该锁上睡眠的其它线程将取得锁并往下执行
|5   |HAL_SemaphoreCreate | 创建一个信号量, 返回值可以传递给HAL_SemaphorePost/Wait
|6   |HAL_SemaphoreDestroy | 销毁一个信号量, 这个信号量由入参标识
|7   |HAL_SemaphorePost | 在指定的计数信号量上做自增操作, 解除其它线程的等待
|8   |HAL_SemaphoreWait | 在指定的计数信号量上等待并做自减操作
|9   |HAL_ThreadCreate | 根据配置参数创建thread

对以上函数接口若需了解更多细节, 可以直接访问[SDK官方文档页面](https://help.aliyun.com/document_detail/100112.html) .

### <a name="AT MQTT相关HAL">AT MQTT相关HAL</a>

AT  MQTT相关HAL函数位于抽取出来的文件wrapper.c中, 客户需要在这些函数中调用模组提供的AT指令和模组进行数据交互. 函数说明如下:

|     |函数名              |   说明
|----|--------------------|------------
|1    |HAL_AT_MQTT_Init   |初始化MQTT参数配置. 比如初始化MCU与通信模组之间的UART串口设置, 初始化MQTT配置参数: clientID/clean session/user name/password/timeout/MQTT Broker的地址和端口等数值. 返回值类型为iotx_err_t, 其定义位于文件infra_defs.h
|2    |HAL_AT_MQTT_Deinit |如果在HAL_AT_MQTT_Init创建了一些资源, 可以在本函数中相关资源释放掉
|3    |HAL_AT_MQTT_Connect |连接MQTT服务器. 入参: proKey:产品密码devName:设备名devSecret:设备密码注: 只有通信模组集成了阿里的SDK的时候会使用到该函数的这几个入参, 如果模组上并没有集成阿里的SDK, 那么略过这几个参数. 该函数的入参并没有指定服务器的地址/端口, 这两个参数需要在HAL_AT_MQTT_Init()中记录下来
|4    |HAL_AT_MQTT_Disconnect |断开MQTT服务器
|5    |HAL_AT_MQTT_Subscribe  |向服务器订阅指定的TOPIC. 入参: topic:主题qos:服务器质量mqtt_packet_id: 数据包的IDmqtt_status: mqtt状态timeout_ms:超时时间
|6    |HAL_AT_MQTT_Unsubscribe  |向服务器取消对指定topoic的订阅. 入参: topic:主题mqtt_packet_id:数据包的IDmqtt_status:mqtt状态
|7    |HAL_AT_MQTT_Publish      |向服务器指定的Topic发送消息
|8    |HAL_AT_MQTT_State        |返回MQTT的状态, 状态值定义在文件mal.h的数据结构iotx_mc_state_t中

### <a name="调用接收函数">调用接收函数</a>

MCU从模组收到MQTT消息之后, 需要调用SDK提供的函数IOT_ATM_Input()(见atm/at_api.h)将MQTT 消息交付给SDK


```
void handle_recv_data()
{
    struct at_mqtt_input param;
    ...

    param.topic = topic_ptr;
    param.topic_len = strlen(topic_ptr);
    param.message = msg_ptr;
    param.msg_len = strlen(msg_ptr);

    if (IOT_ATM_Input(&param) != 0) {
        mal_err("hand data to uplayer fail!\n");
    }
}
```

#### <a name="AT Parser相关HAL">AT Parser相关HAL</a>

如果选择了at_parser框架, 则需要对接以下四个UART HAL函数, 函数声明见at_wrapper.h. 如果用户不使用at_parser框架请忽略该步

|     |函数名              |   说明
|----|--------------------|------------
|1   |HAL_AT_Uart_Init    |该接口对UART进行配置(波特率/停止位等)并初始化
|2   |HAL_AT_Uart_Deinit  |该接口对UART去初始化
|3   |HAL_AT_Uart_Send    |该接口用于向指定的UART口发送数据
|4   |HAL_AT_Uart_Recv    |该接口用于从底层UART buffer接收数据

### <a name="产品相关HAL">产品相关HAL</a>

下面的HAL用于获取产品的身份认证信息, 设备厂商需要设计如何在设备上烧写设备身份信息, 并通过下面的HAL函数将其读出后提供给SDK:

|     |函数名              |   说明
|----|--------------------|------------
|1   |HAL_GetProductKey   |获取设备的ProductKey, 用于标识设备的产品型号
|2   |HAL_GetDeviceName   |获取设备的DeviceName, 用于唯一标识单个设备
|3   |HAL_GetDeviceSecret |获取设备的DeviceSecret, 用于标识单个设备的密钥

对以上函数接口若需了解更多细节, 可以直接访问[SDK官方文档页面](https://help.aliyun.com/document_detail/100112.html)

## <a name="代码集成">代码集成</a>

如果设备商的开发环境使用makefile编译代码, 可以将SDK抽取出来的代码加入其编译环境进行编译. 如果设备商使用KEIL/IAR这样的开发工具, 可以将SDK抽取出来的代码文件加入到IDE的工程中进行编译

下面是将抽取出来的output目录复制到Linux下, 并在output目录下创建的一个makefile的示例:

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

## <a name="参照example实现产品功能">参照example实现产品功能</a>

如果要使用MQTT连云, 可参考抽取文件夹中的 eng/examples/mqtt_example_at.c .  设备厂商可以将该文件复制到产品工程中, 对其进行修改后使用

该example将连接设备到阿里云, 订阅一个指定的topic并发送数据给该topic, 即设备上报的消息会被物联网平台发送给设备, 下面是example的大概过程说明:

![9](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/9.png)

注意: 需要在云端将该topic从默认的权限从"订阅"修改为"发布和订阅", 如下图所示:

![10](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/10.png)

从程序入口的 main() 函数看起, 第一步是调用AT模块初始化函数IoT_ATM_Init(), 使模组处于ready状态, 第二步是调用用户提供的HAL函数获取产品信息

```
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

![11](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/11.png)

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

![12](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/12.png)

注:

+ 上面的代码是周期性的将固定的消息发送给云端, 设备商需要根据自己的产品功能, 在必要的时候才上传数据给物联网平台
+ 客户可以删除main函数中example_publish(pclient)语句, 避免周期发送无效数据给到云端
+ IOT_MQTT_Yield是让SDK去接收来自MQTT Broker的数据, 其中200毫秒是等待时间, 如果用户的消息数量比较大/或者实时性要求较高, 可以将时间改小

### <a name="功能调试">功能调试</a>

下面的信息截图以mqtt_example_at.c为例编写

#### <a name="如何判断设备已连接到阿里云">如何判断设备已连接到阿里云</a>
下面的打印是HAL_Printf函数将信息打印到串口后运行example的输出内容, 其中使用橙色圈选的信息表明设备已成功连接到阿里云物联网平台:

![13](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/13.png)

#### <a name="如何判断设备已成功发送数据到云端">如何判断设备已成功发送数据到云端</a>

登录阿里网物联网平台的商家后台, 选中指定的设备, 可以查看是否收到来自设备的消息, 如下图所示:


![14](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/14.png)

注:
+ 上图中的内容只能看见消息发送到了哪个topic, 消息的内容并不会显示出来

#### <a name="如何判断设备已可成功接收来自云端数据">如何判断设备已可成功接收来自云端数据</a>

在商家后台的"下行消息分析"分析中可以看见由物联网平台发送给设备的消息:

![15](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/15.png)

也可在设备端查看是否已收到来自云端的数据, exmaple代码中收到云端发送的数据的打印信息如下所示:


![16](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/16.png)

至此, SDK在MCU与模组之间的适配开发已结束, 用户可以进行产品业务功能的实现

# <a name="设备端高级版开发过程">设备端高级版开发过程</a>

上面的讲解直接基于MQTT消息进行设备与阿里云物联网平台之间的通信, 设备开发者需要自己定义topic以及在topic上通信的数据的格式. 阿里云物联网提供物模型的方式来描述设备功能, 物模型包括了设备属性/服务/事件的定义, 设备与阿里云物联网之间的数据通信格式为JSON格式, 使用物模型定义和开发的产品在阿里云IoT又称为高级版设备

## <a name="SDK高级版配置与代码抽取">SDK高级版配置与代码抽取</a>

设备端高级版具备所有基础版的功能, 基础版的功能选择在本文档前半部分中已经有详细描述, 这里不再赘述. 高级版功能需选中FEATURE_DEVICE_MODEL_ENABLED:

![17](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/17.png)

使用抽取脚本后, 将比基础版多产生一个文件夹:

![18](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/18.png)

仿照基础版的操作, 将该文件夹加入到自己的编译系统中


## <a name="高级版HAL对接函数">高级版HAL对接函数</a>


在仅选中FEATURE_DEVICE_MODEL_ENABLED时, 高级版无另外需要对接的HAL函数

## <a name="参照高级版example实现产品功能">参照高级版example实现产品功能</a>


高级版example文件位于代码抽取目录的examples/alink_example_solo.c. 下面将对该example进行讲解:

从程序入口的 main() 函数看起, 第一步初始化ATM,如果用到ATM模块:

```
#ifdef ATM_ENABLED
    if (IOT_ATM_Init() < 0) {
        EXAMPLE_TRACE("IOT ATM init failed!\n");
        return -1;
    }
#endif
```

注册事件通知回调函数:

```
    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, user_down_raw_data_arrived_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);
    IOT_RegisterCallback(ITE_COTA, user_cota_event_handler);

```

参数设置,如连接站点选择/是否动态建连/是否需要事件应答等:

```
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

    /* Choose Login Server, domain should be configured before IOT_Linkkit_Open() */
#if USE_CUSTOME_DOMAIN
    IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)CUSTOME_DOMAIN_MQTT);
    IOT_Ioctl(IOTX_IOCTL_SET_HTTP_DOMAIN, (void *)CUSTOME_DOMAIN_HTTP);
#else
    domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
#endif

    /* Choose Login Method */
    dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Choose Whether You Need Post Property/Event Reply */
    post_event_reply = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);
```

创建本地设备资源, 建立与云端的连接:

```
    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }

```

数据上报示例:

```
    time_begin_sec = user_update_sec();
    while (1) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);

        time_now_sec = user_update_sec();
        if (time_prev_sec == time_now_sec) {
            continue;
        }
        if (max_running_seconds && (time_now_sec - time_begin_sec > max_running_seconds)) {
            EXAMPLE_TRACE("Example Run for Over %d Seconds, Break Loop!\n", max_running_seconds);
            break;
        }

        /* Post Proprety Example */
        if (time_now_sec % 11 == 0 && user_master_dev_available()) {
            user_post_property();
        }
        /* Post Event Example */
        if (time_now_sec % 17 == 0 && user_master_dev_available()) {
            user_post_event();
        }
        /* Post Raw Example */
        if (time_now_sec % 37 == 0 && user_master_dev_available()) {
            user_post_raw_data();
        }

        time_prev_sec = time_now_sec;
    }

```

关闭linkkit:

```
    IOT_Linkkit_Close(user_example_ctx->master_devid);
```

与基础版示例相同, 登录阿里网物联网平台的商家后台, 选中指定的设备, 可以查看是否收到来自设备的消息

更多物模型开发信息请参考物模型编程指南  与基础版示例相同, 登录阿里网物联网平台的商家后台, 选中指定的设备, 可以查看是否收到来自设备的消息

更多物模型开发信息请参考[物模型编程指南](https://help.aliyun.com/document_detail/96628.html)

