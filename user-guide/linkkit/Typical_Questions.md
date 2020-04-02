# <a name="目录">目录</a>
+ [附录B 典型咨询问题](#附录B 典型咨询问题)
    * [B.1 基础问题](#B.1 基础问题)
        - [获取技术支持](#获取技术支持)
        - [信息安全](#信息安全)
        - [使用限制](#使用限制)
        - [可调节参数](#可调节参数)
    * [B.2 移植阶段问题](#B.2 移植阶段问题)
        - [如何编译SDK能够减小二进制尺寸](#如何编译SDK能够减小二进制尺寸)
        - [如何解决嵌入式平台上 `strtod()` 不工作问题](#如何解决嵌入式平台上 `strtod()` 不工作问题)
    * [B.3 编译阶段问题](#B.3 编译阶段问题)
        - [SDK几个分库的链接顺序](#SDK几个分库的链接顺序)
        - [获取动态链接库形态的SDK编译产物](#获取动态链接库形态的SDK编译产物)
    * [B.4 错误码自查](#B.4 错误码自查)
        - [TLS/SSL连接错误](#TLS/SSL连接错误)
        - [MQTT连接错误](#MQTT连接错误)
    * [B.5 MQTT上云问题](#B.5 MQTT上云问题)
        - [心跳和重连](#心跳和重连)
        - [关于 IOT_MQTT_Yield](#关于 IOT_MQTT_Yield)
        - [订阅相关](#订阅相关)
        - [发布相关](#发布相关)
    * [B.6 高级版问题](#B.6 高级版问题)
    * [B.7 云端/协议问题](#B.7 云端/协议问题)
    * [B.8 本地通信问题](#B.8 本地通信问题)
    * [B.9 WiFi配网问题](#B.9 WiFi配网问题)
    * [B.10 CoAP上云问题](#B.10 CoAP上云问题)
        - [为什么有时认证会超时失败](#为什么有时认证会超时失败)
        - [认证连接的地址](#认证连接的地址)
        - [报文上行](#报文上行)
        - [关于 IOT_CoAP_Yield](#关于 IOT_CoAP_Yield)
    * [B.11 HTTP上云问题](#B.11 HTTP上云问题)
        - [认证连接](#认证连接)
    * [B.12 TLS连接问题](#B.12 TLS连接问题)
        - [TLS对接自测程序](#TLS对接自测程序)
        - [设备端TLS密码算法](#设备端TLS密码算法)
        - [云端TLS密码算法](#云端TLS密码算法)
    * [B.13 配网绑定问题](#B.13 配网绑定问题)
    * [B.14 OTA升级问题](#B.14 OTA升级问题)


# <a name="附录B 典型咨询问题">附录B 典型咨询问题</a>
## <a name="B.1 基础问题">B.1 基础问题</a>
### <a name="获取技术支持">获取技术支持</a>

如果您在使用官方SDK的过程中遇到问题, 通过本文档又难以自查, 可通过 [工单系统](https://selfservice.console.aliyun.com/ticket/createIndex) 链接提工单, 会有专业的技术支持团队为您服务
---
*注: 工单系统是使用SDK的客户除本文档外获取技术支持的统一入口, 请勿尝试其它途径*

### <a name="信息安全">信息安全</a>
使用C-SDK连接物联网平台, 传输的安全性怎么保证
---
设备和服务端之间的链路可以通过TLS加密的, 并且使用 productKey/deviceName/deviceSecret 这一组设备相关的信息进行认证, 任何一个环节鉴权失败都会导致连接无法建立

### <a name="使用限制">使用限制</a>
**更多产品使用上的限制边界, 以 [官网限制说明页面](https://help.aliyun.com/document_detail/30527.html) 为准**

一组设备标识支持多个设备登录吗
---
不可以, 一组设备信息(productKey/deviceName/deviceSecret), 同时多个设备登录, 只能连接一个

每次调用 [IOT_MQTT_Construct](#IOT_MQTT_Construct) 或者 [IOT_Linkkit_Connect](#IOT_Linkkit_Connect) 之类的连云接口时都会成功

不过先前建立的链路会被服务器断开, 只保留最近建立的链路, 使同一组设备标识, 同一时间只能有一台实际设备在线

Topic订阅设备超过1000怎么处理
---
广播topic, 最多1000个订阅者. 如果设备超过数1000, 可以对设备进行分组, 1000个一组

比如有5000个设备, 使用广播topic, 用户调5次接口

查看控制台的日志中有以下错误: [error]rate limiter, 请问是什么原因
---
这代表设备被限流了, 单个设备数据上报的上限在 QoS0 下为30条每秒, 在 QoS1 下为10条每秒, 下行接收限制为50条每秒

设备掉线了, 查看控制台日志怎么理解掉线原因
---
<img src="https://linkkit-export.oss-cn-shanghai.aliyuncs.com/mqtt-disconn-reason.png" width="800">

### <a name="可调节参数">可调节参数</a>

> 下表说明 `SDK-3.0.1` 以上版本中, 可调节的连接参数, 其它版本可以类推, 命名大致相似

| 连接参数                                | 所属模块    | 含义                                                    | 默认值      | 备注
|-----------------------------------------|-------------|---------------------------------------------------------|:-----------:|-------------------------------------------------------------
| `CONFIG_MQTT_REQUEST_TIMEOUT`           | MQTT        | MQTT request timeout 间隔时间                           | `2s`        | 曾用名`IOTX_MC_REQUEST_TIMEOUT_DEFAULT_MS`
| `IOTX_MC_KEEPALIVE_PROBE_MAX`           | MQTT        | MQTT keepalive 允许心跳丢失最大次数                     | `1`         | 改的越小, 重连越快, 也可能掉线次数越多
| `CONFIG_MQTT_KEEPALIVE_INTERVAL`        | MQTT        | MQTT keepalive 心跳默认间隔时间                         | `60s`       | 不在 `mqtt_params` 中指定时, SDK默认填上的时间
| `CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN`    | MQTT        | 可被设置的最短心跳间隔                                  | `30s`       | 曾用名`KEEP_ALIVE_INTERVAL_DEFAULT_MIN`
| `CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX`    | MQTT        | 可被设置的最长心跳间隔                                  | `180s`      | 曾用名`KEEP_ALIVE_INTERVAL_DEFAULT_MAX`
| `IOTX_MC_RECONNECT_INTERVAL_MIN_MS`     | MQTT        | MQTT reconnect 最小间隔时间                             | `1000ms`    | 首次断线重连时的起始重试间隔
| `IOTX_MC_RECONNECT_INTERVAL_MAX_MS`     | MQTT        | MQTT reconnect 最大间隔时间                             | `60s`       | 断线的间隔会越来越稀疏, 最终达到这个数值不再增加
| `CONFIG_MQTT_MESSAGE_MAXLEN`            | MQTT        | MQTT 最大 Packet Length                                 | `1024`      | 曾用值`1280`
| `CONFIG_GUIDER_AUTH_TIMEOUT`            | MQTT        | 使用HTTP预认证方式鉴权连接时, 等待云端应答的最长超时    | `10s`       | 超过这个时间将会不再等待放弃建连
| `DM_MSG_CACHE_TIMEOUT_MS_DEFAULT`       | DevModel    | 使用物模型时, 等待云端应答的最长超时                    | `10s`       | 超时无应答, 会报错 `{"id":0,"code":100000,...}`
| `CONFIG_MQTT_TX_MAXLEN`                 | DevModel    | 使用物模型时, MQTT的最大上行报文长度                    | `1024`      |
| `CONFIG_MQTT_RX_MAXLEN`                 | DevModel    | 使用物模型时, MQTT的最大下行报文长度                    | `1024`      |
| `IOTX_DM_CLIENT_KEEPALIVE_INTERVAL_MS`  | DevModel    | 使用物模型时, MQTT建连将会使用的心跳间隔                | `30s `      |
| `MBEDTLS_SSL_MAX_CONTENT_LEN`           | TLS         | 使用TLS加密通道连接时, 报文缓冲区的最大长度             | `4096`      | 对应握手连接内存峰值为25KB, 标准TLS需16KB, 峰值52KB


## <a name="B.2 移植阶段问题">B.2 移植阶段问题</a>

### <a name="如何编译SDK能够减小二进制尺寸">如何编译SDK能够减小二进制尺寸</a>

请注意以下的编译选项在 `CFLAGS` 中能够起到的作用

| 选项                        | 说明
|-----------------------------|-----------------------------------------------------------------------------------------------------
| **`-Os`**                   | 尺寸优化选项, GNU系列的工具链一般都会支持
| **`-g3`**                   | 调试附加选项, 如果不需要使用`gdb`调试, 可以去掉来减小尺寸
| **`--coverage`**            | 覆盖率统计选项, 如果不需要用`lcov`统计代码覆盖率, 可以去掉来减小尺寸
| **`-ffunction-sections`**   | 将函数分段摆放, 不被使用的函数将不进入最终的二进制, **加上此选项可减小最终的可执行程序/镜像大小**
| **`-fdata-sections`**       | 将数据分段摆放, 不被使用的变量将不进入最终的二进制, **加上此选项可减小最终的可执行程序/镜像大小**
| **`-Wl,--gc-sections`**     | 链接的时候让未使用的符号不进入最终的二进制, 减小尺寸, **本选项需要和上面的2个选项组合使用**

同时如下的功能开关可以考虑关闭, 以减小尺寸

| 开关                                    | 说明
|-----------------------------------------|-----------------------------------------------------------------------------
| **`FEATURE_AWSS_SUPPORT_ROUTER`**       | 配网中的路由器配网模式, 一般可以直接关闭, 以减小尺寸
| **`FEATURE_AWSS_SUPPORT_PHONEASAP`**    | 配网中的手机热点配网模式, 一般不使用这种模式的时候也可以关闭, 以减小尺寸

### <a name="如何解决嵌入式平台上 `strtod()` 不工作问题">如何解决嵌入式平台上 `strtod()` 不工作问题</a>

在有些嵌入式平台上, 由于C库被定制, 标准的C99库函数 `strtod()` 可能不工作甚至引起崩溃和死机, 可通过

    setlocale(LC_ALL, "C");

的方式使能C库能力全集来解决

## <a name="B.3 编译阶段问题">B.3 编译阶段问题</a>

### <a name="SDK几个分库的链接顺序">SDK几个分库的链接顺序</a>

请务必保持使用

    -liot_sdk -liot_hal -liot_tls

这样的顺序来链接SDK提供的几个分库, 因为写在后面的都是对前面库的支撑

---
对于移植到Linux上使用的情况, 还需要以

     -liot_sdk -liot_hal -liot_tls -lpthread -lrt

的方式来书写, 这是因为SDK在Linux下的HAL参考实现使用了pthread库和librt实时库

---
比如, 使用 `dlopen()` 接口打开我们的 `libiot_sdk.so` 这样的情况, 那么在编译SDK和使用它的应用程序的时候, 就需要写成比如

    $(TARGET): $(OBJS)
        $(CC) -o $@ $^ -liot_sdk -liot_hal -liot_tls -lpthread -lrt

的这个样子

---
不论是链接静态库, 还是链接动态库, 还是使用 `dlopen()` 等运行时动态加载的方式使用SDK, 应用程序链接的时候都请确保按照上述顺序编写链接指令

### <a name="获取动态链接库形态的SDK编译产物">获取动态链接库形态的SDK编译产物</a>

由于C-SDK大部分情况下运行在非Linux的嵌入式操作系统上, 比如`AliOS Things`, 或者`FreeRTOS`等

而这些操作系统并无Linux的动态链接库概念, 所以默认情况下SDK都是以静态库(`libiot_sdk.a` + `libiot_hal.a` + `libiot_tls.a`)的方式输出

可以用如下的修改方法调整默认的输出形态, 将SDK的编译产物从静态库的方式改成动态库

获取`libiot_sdk.so`代替`libiot_sdk.a`
---
以默认的 `config.ubuntu.x86` 配置文件为例, 如下的修改可以告诉构建系统要产生动态库形态的构建产物

    --- a/src/board/config.ubuntu.x86
    +++ b/src/board/config.ubuntu.x86
    @@ -1,6 +1,5 @@
     CONFIG_ENV_CFLAGS   += \
         -Os -Wall \
    -    -g3 --coverage \
         -D_PLATFORM_IS_LINUX_ \
         -D__UBUNTU_SDK_DEMO__ \

    @@ -19,6 +18,7 @@ CONFIG_ENV_CFLAGS   += \
         -DCONFIG_MQTT_RX_MAXLEN=5000 \
         -DCONFIG_MBEDTLS_DEBUG_LEVEL=0 \

    +CONFIG_LIB_EXPORT := dynamic

     ifneq (Darwin,$(strip $(shell uname)))
     CONFIG_ENV_CFLAGS   += -rdynamic

+ **改动点1:** 确保`CFLAGS`中没有`-g3 --coverage`这样的编译选项
+ **改动点2:** 新增一行 `CONFIG_LIB_EXPORT := dynamic`
+ **改动点3:** 重新运行 `make reconfig` 选择刚才修改到的 `config.ubuntu.x86` 配置文件, 或者被定制的config文件, **然后以 `make all` 而不是 `make` 的方式来编译**

---
按照如上改法, `make all`之后在 `output/release/lib/libiot_sdk.so` 就可以获取动态库形态的SDK了, 其内容和默认的 `libiot_sdk.a` 是一致的

    $ ls output/release/lib/*.so
    output/release/lib/libiot_sdk.so

获取`libiot_hal.so`代替`libiot_hal.a`
---
修改 `src/ref-impl/hal/iot.mk`, 新增如下这行

    LIBSO_TARGET := libiot_hal.so

然后运行:

    make reconfig
    make all

之后便可以在 `output/release/lib/libiot_hal.so` 得到动态库形式的HAL参考实现的分库, 其内容和默认的 `libiot_hal.a` 是一致的

获取`libiot_tls.so`代替`libiot_tls.a`
---
修改 `src/ref-impl/tls/iot.mk`, 新增如下这行

    LIBSO_TARGET := libiot_tls.so

然后运行:

    make reconfig
    make all

之后便可以在 `output/release/lib/libiot_tls.so` 得到动态库形式的TLS参考实现的分库, 其内容和默认的 `libiot_tls.a` 是一致的


## <a name="B.4 错误码自查">B.4 错误码自查</a>

### <a name="TLS/SSL连接错误">TLS/SSL连接错误</a>
#### <a name="-0x7880/-30848/MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY">-0x7880/-30848/MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY</a>
解释
---
云端把SSL连接断开了: `The peer notified us that the connection is going to be closed`

可能的原因和解决建议
---
- 设备端数据连接过于频繁, 触发云端限流, 断开设备
    * 建议关闭设备, 等待一段时间(5分钟以后)再发起连接重试, 观察错误仍会出现
- 有多个设备使用相同的productKey和deviceName与云端建立连接, 导致被云端踢下线
    * 建议检查当前使用的三元组是否可能被他人使用
- 设备端保活出错, 没有及时发送 MQTT ping packet, 或者被发送了没有及时到达云端
    * 建议用抓包等方式确认心跳包有成功发出或者观察有没有收到来自服务端的 MQTT ping response
- 如果一次都不能连接成功, 可以考虑是不是大小端字节序不匹配
    * 目前C-SDK 默认是适配小端设备, 如果需在大端硬件上工作, 请添加全局编译选项 `REVERSED`

#### <a name="-0x7800/-30720/MBEDTLS_ERR_SSL_PEER_VERIFY_FAILED">-0x7800/-30720/MBEDTLS_ERR_SSL_PEER_VERIFY_FAILED</a>
解释
---
认证错误: `Verification of our peer failed`

可能的原因和解决建议
---
+ 证书错误
    * 如果使用官方C-SDK对接, 证书固化在SDK内部, 不会出现. 若自行对接, 则需检查使用的证书是否和阿里云官方证书匹配
+ 日常环境SSL域名校验错误
    * 如果出错时, 连接的是日常环境, 则考虑日常不支持SSL域名校验, 请将 `FORCE_SSL_VERIFY` 的编译选项定义去掉

#### <a name="-0x7200/-29184/MBEDTLS_ERR_SSL_INVALID_RECORD">-0x7200/-29184/MBEDTLS_ERR_SSL_INVALID_RECORD</a>
解释
---
收到非法数据: `An invalid SSL record was received`

可能的原因和解决建议
---
+ TCP/IP协议栈收到的数据包出错, 需要排查协议栈方面问题
+ SSL所运行的线程栈被设置的过小, 需调整线程栈大小
+ SSL被配置的最大报文长度太小, 当网络报文长度超过该数值时, 则可能出现0x7200错误
    * 可调整 `MBEDTLS_SSL_MAX_CONTENT_LEN` 的值, 重新编译再试
    * `MBEDTLS_SSL_MAX_CONTENT_LEN` 的值, 目前已知最小不能小于 `4096`

#### <a name="-0x2700/-9984/MBEDTLS_ERR_X509_CERT_VERIFY_FAILED">-0x2700/-9984/MBEDTLS_ERR_X509_CERT_VERIFY_FAILED</a>
解释
---
证书错误: `Certificate verification failed, e.g. CRL, CA or signature check failed`

可能的原因和解决建议
---
+ 证书不匹配
    * 若未使用官方C-SDK对接, 请检查证书是否和阿里云官网提供下载的一致
+ 时钟不对
    * 确认系统时间是否准确, 系统时间不对(比如默认的1970-01-01), 也会导致证书校验失败

#### <a name="-0x0052/-82/MBEDTLS_ERR_NET_UNKNOWN_HOST">-0x0052/-82/MBEDTLS_ERR_NET_UNKNOWN_HOST</a>
解释
---
DNS域名解析错误: `Failed to get an IP address for the given hostname`

可能的原因和解决建议
---
+ 大概率是设备端当前网络故障, 无法访问公网
    * 如果是WiFi设备, 检查其和路由器/热点之间的连接状况, 以及上联路由器是否可以正常访问公网
+ 需要通过类似 `res_init()` 之类的调用强制C库刷新DNS解析文件
    * 在Linux系统上, 域名解析的系统调用 `getaddrinfo()` 的工作是依赖域名解析文件 `/etc/resolv.conf`
    * 对某些嵌入式Linux, 可能会有 `libc` 库读取过时的 `/etc/resolv.conf` 问题
    * 对于这类系统, 域名解析请求不论是早于还是晚于域名解析文件的更新, 都会读到过时的信息, 进而造成域名解析失败

> 这种现象产生的原因是DNS文件的更新晚于MQTT的域名解析请求, 这样 `getaddrinfo()` 系统调用就会得到一个 `EAI_AGAIN` 错误
>
> 然而, 如果不通过 `res_init()`, C库中的 `getaddrinfo()` 即使被重试逻辑调用也仍然读取过时的DNS文件, 并继续得到 `EAI_AGAIN` 错误

---
*例如, 可以改动 `HAL_TLS_mbedtls.c`*

    #include <arpa/nameser.h>
    #include <resolv.h>

    ...
    ...

    if ((ret = getaddrinfo(host, port, &hints, &addr_list)) != 0) {
            if (ret == EAI_AGAIN)
                res_init();

                return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
    }

*以上只是一个示意的改法, `res_init()`本身也是一个过时的函数, 所以不建议进入SDK的官方代码, 用户可以酌情加在合适的位置*

#### <a name="-0x0044/-68/MBEDTLS_ERR_NET_CONNECT_FAILED">-0x0044/-68/MBEDTLS_ERR_NET_CONNECT_FAILED</a>
解释
---
socket连接失败: `The connection to the given server / port failed`

可能的原因和解决建议
---
- TCP连接失败
    * 请确认连接的目标IP地址/域名, 以及端口号是否正确

#### <a name="-0x0043/-67/MBEDTLS_ERR_NET_BUFFER_TOO_SMALL">-0x0043/-67/MBEDTLS_ERR_NET_BUFFER_TOO_SMALL</a>
解释
---
SSL报文缓冲区过短: `Buffer is too small to hold the data`

可能的原因和解决建议
---
+ SSL被配置的最大报文长度太小
    * 可调整 `MBEDTLS_SSL_MAX_CONTENT_LEN` 的值, 重新编译再试
    * `MBEDTLS_SSL_MAX_CONTENT_LEN` 的值, 目前已知最小不能小于 `4096`

#### <a name="-0x0042/-66/MBEDTLS_ERR_NET_SOCKET_FAILED">-0x0042/-66/MBEDTLS_ERR_NET_SOCKET_FAILED</a>
解释
---
创建socket失败: `Failed to open a socket`

可能的原因和解决建议
---
- 系统可用的socket可能已全部被申请, 在有些平台上, 能被开启的socket数量, 有上限的
    * 检查当前的流程是否有socket的泄漏, 有些socket使用完后没有close
    * 如果流程正常, 确需同时建立多个socket, 请调整socket的个数上限

### <a name="MQTT连接错误">MQTT连接错误</a>
#### <a name="-35/MQTT_CONNACK_BAD_USERDATA_ERROR">-35/MQTT_CONNACK_BAD_USERDATA_ERROR</a>
解释
---
发起MQTT协议中的Connect操作失败: `Connect request failed with the server returning a bad userdata error`

可能的原因和解决建议
---
+ 使用了错误的设备三元组
    * 建议检查 productKey/deviceName/deviceSecret 是否正确并且是同一套
+ 三元组传入的参数不正确
    * 建议检查C函数的传参过程
+ 设备三元组被禁用
    * 若经检查确系禁用, 请在控制台或是使用服务端相应API解除禁用

#### <a name="-37/MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR">-37/MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR</a>
解释
---
云端认为设备权限不足而拒绝设备端的连接请求: `CONNACK` 报文中的错误码是 `2`

可能的原因和解决建议
---
+ MQTT的Connect报文参数中, clientId的上报内容和协议预期不符, 这通常发生在不使用官方SDK而使用第三方MQTT自行对接时
+ 建议检查在控制台创建当前设备三元组的账号, 是否有权限, 如是否有发生欠费等

#### <a name="-42/MQTT_PUSH_TO_LIST_ERROR">-42/MQTT_PUSH_TO_LIST_ERROR</a>
解释
---
订阅或是发布(QoS1)太多太快, 导致SDK内部队列满了而出错

可能的原因和解决建议
---
+ 如果是订阅过程中发生的错误, 说明当前订阅使用的队列长度不够
    * 可以考虑调整 `IOTX_MC_SUB_NUM_MAX` 的值
+ 如果是发布过程中发生的错误, 可能是发送的频率太快, 或是网络状态不佳
    * 可以考虑使用QoS0来发布
    * 可以考虑调整 `IOTX_MC_REPUB_NUM_MAX` 的值

## <a name="B.5 MQTT上云问题">B.5 MQTT上云问题</a>

MQTT站点地址和端口如何自定义
---
> 在使用阿里云物联网套件连接阿里云时, 可指定MQTT连接的服务器站点, 配置方法如下

枚举类型 `iotx_cloud_region_types_t` 定义了当前可连接的MQTT站点

```c
    /* region type */
    typedef enum IOTX_CLOUD_REGION_TYPES {
        /* Shanghai */
        IOTX_CLOUD_REGION_SHANGHAI,

        /* Singapore */
        IOTX_CLOUD_REGION_SINGAPORE,

        /* Japan */
        IOTX_CLOUD_REGION_JAPAN,

        /* America */
        IOTX_CLOUD_REGION_USA_WEST,

        /* Germany */
        IOTX_CLOUD_REGION_GERMANY,

        /* Custom setting */
        IOTX_CLOUD_REGION_CUSTOM,

        /* Maximum number of domain */
        IOTX_CLOUD_DOMAIN_MAX
    } iotx_cloud_region_types_t;
```

+ 首先使用 `IOT_Ioctl` 的 `IOTX_IOCTL_SET_DOMAIN` 选项, 配合上面的枚举值, 设置要连接的站点
+ 其次使用 `IOT_Ioctl` 的 `IOTX_IOCTL_SET_MQTT_PORT` 选项, 可以设置要连接的端口号
+ 然后使用 `IOT_MQTT_Construct` 或者 `IOT_Linkkit_Connect` 来建立设备到阿里云的连接

> 例如

```c
    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    uint16_t custom_port = 1883;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
    IOT_Ioctl(IOTX_IOCTL_SET_MQTT_PORT, (void *)&custom_port);
```

**注意事项: 如果在阿里云物联网控制台申请的三元组与连接时使用的域名不符合, 连接站点时会出现认证错误(错误码-35)**

*例如: 在阿里云物联网控制台的华东2站点申请的三元组, 在物联网套件中应该连接华东2(上海)站点*


MQTT云端服务器IP地址和端口号
---
端口: 1883

IP地址:

+ `218.11.0.64`
+ `116.211.167.65`
+ `118.178.217.16`
+ `106.15.100.2`
+ `139.196.135.135`

什么是域名直连, 如何开启
---
C-SDK 的 2.0 以上版本, MQTT连接有两种方式, 一种是认证模式, 一种是域名直连模式

+ 认证模式是设备先用 HTTPS 方式访问类似 https://iot-auth.cn-shanghai.aliyuncs.com:443 的地址, 进行鉴权
+ 鉴权通过之后, 再用 MQTT 方式连接到类似 public.iot-as-mqtt.cn-shanghai.aliyuncs.com:1883 的 MQTT 服务器地址
+ 域名直连模式是设备直接用 MQTT 方式连接到类似 ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com:1883的 MQTT 服务器地址
+ 这样不需要访问 HTTPS+MQTT 两台服务器才建立连接, 更快建立连接, 耗费的端上资源也更少

---
2.0 以上的版本, 默认已经开启了直连模式, 如果配置文件 `make.settings` 不是默认的状态了, 那么

打开 `make.setting` 文件, 在文件尾部写入 `FEATURE_MQTT_DIRECT = y`, 然后运行 `make` 命令重新编译, 即可确保开启直连模式

MQTT协议版本是多少
---
C-SDK 的 2.0 以上版本, 封装的MQTT协议版本号是 `3.1.1`

例程 mqtt-example 连接上线后会很快下线, 如何修改可以一直处于在线状态
---
mqtt-example 例程本身的逻辑是发送一次消息后会自动退出

若需要此例程保持长期在线, 执行命令时加上 "loop" 参数即可, 例如:

    .output/release/bin/mqtt-example loop

**当您这样做时, 请确保使用自己申请的 productKey/deviceName/deviceSecret 后重新编译例程**

**因为官方SDK的代码中默认编写的是一台公用设备的信息, 有其他人同时使用时, 您会被踢下线, 同一组设备信息只能同时在线一个连接**

如何持续的接收MQTT消息
---
需要循环多次的调用 [IOT_MQTT_Yield](#IOT_MQTT_Yield) 函数, 在其内部会自动的维持心跳和接收云端下发的MQTT消息

可以参考 examples 目录下, mqtt-example.c 里面的 while 循环部分

### <a name="心跳和重连">心跳和重连</a>
心跳的时间间隔如何设置
---
在 [IOT_MQTT_Construct](#IOT_MQTT_Construct) 的入参中可以设置 `keepalive_interval_ms` 的值, C-SDK用这个值作为心跳间隔时间

keepalive_interval_ms 的取值范围是 60000 - 300000, 即最短心跳间隔为1分钟, 最长心跳间隔为5分钟

设备端是如何侦测到需要重连(reconnect)的
---
设备端C-SDK会在 `keepalive_interval_ms` 时间间隔过去之后, 再次发送 MQTT ping request, 并等待 MQTT ping response

如果在接下来的下一个 `keepalive_interval_ms` 时间间隔内, 没有收到对应的 ping response

或是在进行报文的上行发送(send)或者下行接收(recv)时发生异常, 则 C-SDK 就认为此时网络断开了, 需要进行重连

设备端的重连机制是什么
---
C-SDK 的重连动作是内部触发, 无需用户显式干预. 对于使用基础版的用户, 需要时常调用 [IOT_MQTT_Yield](#IOT_MQTT_Yield) 函数将 CPU 交给SDK, 重连就在其中发生

重连如果不成功, 会在间隔一段时间之后持续重试, 直到再次连接成功为止

相邻的两次重连之间的间隔时间是指数退避+随机数的关系, 比如间隔1s+随机秒数, 间隔2s+随机秒数, 间隔4s+随机秒数, 间隔8s+随机秒数... 直到间隔达到了间隔上限(默认60s)

### <a name="关于 IOT_MQTT_Yield">关于 IOT_MQTT_Yield</a>
IOT_MQTT_Yield 的作用
---
[IOT_MQTT_Yield](#IOT_MQTT_Yield) 的作用主要是给SDK机会去尝试从网络上接收数据

因此在需要接收数据时(subscribe/unsubscribe之后, publish之后, 以及希望收到云端下推数据时), 都需要主动调用该函数

IOT_MQTT_Yield 参数 timeout 的意义
---
[IOT_MQTT_Yield](#IOT_MQTT_Yield) 会阻塞住 timeout 指定的的时间(单位毫秒)去尝试接收数据, 直到超出这个时间, 才会返回调用它的函数

IOT_MQTT_Yield 与 HAL_SleepMs 的区别
---
都会阻塞一段时间才返回, 但是 [IOT_MQTT_Yield](#IOT_MQTT_Yield) 实质是去从网络上接收数据

而 [HAL_SleepMs](#HAL_SleepMs) 则是什么也不做, 单纯等待入参指定的时间间隔过去后返回

### <a name="订阅相关">订阅相关</a>
如果订阅了多个topic, 调用一次IOT_MQTT_Yield, 可能接收到多个topic的消息吗
---
调用一次 [IOT_MQTT_Yield](#IOT_MQTT_Yield), 如果多个topic都被订阅成功并且都有数据下发, 则可以一次性接收到来自多个topic的消息

什么情况下会发生订阅超时
---
+ 调用 [IOT_MQTT_Construct](#IOT_MQTT_Construct) 和云端建立MQTT连接时, 其入参中可以设置请求超时时间 `request_timeout_ms` 的值
+ 如果两倍 `request_timeout_ms` 时间过去, 仍未收到来自云端的 `SUBACK` 消息, 则会触发订阅(Subscribe)超时
+ 超时的事件会通过 [IOT_MQTT_Construct](#IOT_MQTT_Construct) 入参中设置的 `event_handle` 回调函数通知到用户
+ 在调用 [IOT_MQTT_Subscribe](#IOT_MQTT_Subscribe) 之后, 需要尽快执行 [IOT_MQTT_Construct](#IOT_MQTT_Construct) 以接收SUBACK, 请勿使用 [HAL_SleepMs](#HAL_SleepMs)

### <a name="发布相关">发布相关</a>
发布(Publish)的消息最长是多少, 超过会怎么样
---
+ C-SDK的代码上来说, MQTT的消息报文长度, 受限于 [IOT_MQTT_Construct](#IOT_MQTT_Construct) 入参中 `write_buf` 和 `read_buf` 的大小
+ 从云端协议上来说, MQTT消息报文长度不能超过256KB, 具体查阅 [官网限制说明页面](https://help.aliyun.com/document_detail/30527.html) 为准
+ 如果实际上报消息长度大于C-SDK的限制, 消息会被丢掉

被发布消息payload格式是怎么样的
---
阿里云物联网平台并没有指定pub消息的payload格式

需要客户根据应用场景制定自己的协议, 然后以JSON格式或其它格式, 放到pub消息载体里面传给服务端

## <a name="B.6 高级版问题">B.6 高级版问题</a>

## <a name="B.7 云端/协议问题">B.7 云端/协议问题</a>
物联网平台云端是否一有消息就立刻推送给订阅的设备而不做保存
---
消息一到达云端, 就会分发给不同的设备订阅者, 云端服务器不进行保存, 目前不支持 MQTT 协议中的 will 和 retain 特性

MQTT长连接时, 云端如何侦测到设备离线的
---
云端会根据用户调用 [IOT_MQTT_Construct](#IOT_MQTT_Construct) 时传入的 `keepalive_interval_ms`, 作为心跳间隔, 等待 MQTT ping request

如果在约定的心跳间隔过去之后, 最多再等5秒钟, 若还是没有收到 MQTT ping request, 则认为设备离线

## <a name="B.8 本地通信问题">B.8 本地通信问题</a>

## <a name="B.9 WiFi配网问题">B.9 WiFi配网问题</a>

## <a name="B.10 CoAP上云问题">B.10 CoAP上云问题</a>

### <a name="为什么有时认证会超时失败">为什么有时认证会超时失败</a>
认证超时失败, 目前总结下来分为下述两种情况

连接失败的时候, ssl->state为MBEDTLS_SSL_SERVER_CHANGE_CIPHER_SPEC的情况
---

在出现问题的一次测试中通过tcpdump进行抓包, 结果通过wireshark展示如下图. 其中client(ip地址为30.5.88.208)发出了握手数据包(包括client key exchange, change cipher spec, encrypted handshake message),但是服务端并没有回复change cipher spec的数据包, 从而导致coap连云卡在这一步. 卡在这一步后, 但如果等待2s后没有等到数据, 客户端会再进行两次尝试超时等待(4s,  8s);如果3次都等不到服务端发来的change cipher spec的数据包, 则退出handshake过程, 导致连接失败
由上分析可知, 这个问题的根因是在服务端.

![image](<https://linkkit-export.oss-cn-shanghai.aliyuncs.com/cipher_spec.png>)

连接失败的时候, ssl->state为MBEDTLS_SSL_HANDSHAKE_OVER的情况
---
 在出现问题的一次测试中通过tcpdump进行抓包, 结果通过wireshark展示如下图. 服务端(106.15.213.197)发来的certificate数据包的顺序出现了错乱, server_hello_done这个certificate数据包并不是作为最后一个certicate包, 而是作为倒数第二个(对比上图即能发现). 在这种情况下, 客户端解析服务端的certificate信息失败, 重新发送client hello消息等待server端回复. 客户端尝试过进行三次尝试超时等待(2s, 4s,  8s)后还没收到数据包, 就主动放弃, 导致连接失败. 由上分析可知, 这个问题的根因是在服务端未能正确返回握手信息
![image](<https://linkkit-export.oss-cn-shanghai.aliyuncs.com/handshak_over.jpg>)


### <a name="认证连接的地址">认证连接的地址</a>
CoAP上云连接的云端URI是什么
---
+ 在调用 [IOT_CoAP_Init](#IOT_CoAP_Init) 的时候, 可以设置其参数 `iotx_coap_config_t` 里面的 `p_url`
+ 如果 `p_url` 值为NULL, 则C-SDK会自动使用 `IOTX_ONLINE_DTLS_SERVER_URL` 这个宏所定义的默认URL

        #define IOTX_ONLINE_DTLS_SERVER_URL "coaps://%s.iot-as-coap.cn-shanghai.aliyuncs.com:5684"

+ 其中 `%s` 是使用 `p_devinfo` 里面的 `product_key`, 所以请确保在初始化 `iotx_coap_config_t` 的时候一定要对 `p_devinfo` 赋值

### <a name="报文上行">报文上行</a>
IOT_CoAP_SendMessage 发送的消息必须是JSON格式吗, 如果不是JSON会出现什么错误
---
+ 目前, 除了支持JSON格式外, 也可以支持CBOR格式
+ 因为是与云端通信, 需要使用指定格式, 否则可能会出现无法解析的问题

### <a name="关于 IOT_CoAP_Yield">关于 IOT_CoAP_Yield</a>
如何设置 IOT_CoAP_Yield 处理结果最大等待时间
---
目前默认设置是2000ms, 可以修改 `COAP_WAIT_TIME_MS` 这个宏进行调整

## <a name="B.11 HTTP上云问题">B.11 HTTP上云问题</a>
### <a name="认证连接">认证连接</a>
HTTPS进行设备认证时, Server会返回的错误码及其含义
---
+ **10000:** common error(未知错误)
    - HTTPS报文是有一定格式要求, 必须符合要求server才能支持
    - `ContentType`只支持"application/json"
    - 只支持HTTPS
    - 只支持POST方法
+ **40000:** request too many(请求次数过多, 流控限制)
    + 同一个设备在一天内的认证次数是有限制的
    + 解决方法: 每次认证获得的token是有48小时有效期的, 过期前可以反复使用, 无需每次都去认证获取新的token
+ **400:** authorized failure(认证失败)
    + 服务器认为鉴权参数是不合法的, 鉴权失败
    + 解决方法: 检查`IOTX_PRODUCT_KEY`, `IOTX_DEVICE_NAME`, `IOTX_DEVICE_SECRET`, `IOTX_DEVICE_ID`是不是从控制台获得的正确参数

## <a name="B.12 TLS连接问题">B.12 TLS连接问题</a>
### <a name="TLS对接自测程序">TLS对接自测程序</a>
C-SDK需要用户实现以下4个TLS的HAL接口, 来支撑自身的正常工作

+ `HAL_SSL_Establish`
+ `HAL_SSL_Read`
+ `HAL_SSL_Write`
+ `HAL_SSL_Destroy`

在调试SDK的MQTT/HTTP能否在TLS上运行**之前**, 用户必须保证这些接口在设备上能正常运行, 尤其是改动过参考实现的话, 更是如此

---
> 以下提供了1个简单的对接自测程序: [hal_tls_test.c](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/hal_tls_test.c)

**使用方式**
+ 把自测程序跟自己实现的4个HAL接口, 通常是`libiot_hal.a`这样的库, 编译到一起
+ 运行自测程序, 如果能打印出`TEST PASS`的字样, 则说明对接没有基本的错误, 可以用于支撑SDK

**举例**
+ 在`Linux`主机上这样编译测试程序

```bash
gcc -o hal_test hal_tls_test.c libiot_hal.a libiot_tls.a -lpthread
```

+ 在`Linux`主机上运行测试程序, 看到执行效果是通过的

```bash
$ ./hal_test 
Loading the CA root certificate ...
 ok (0 skipped)
start prepare client cert .
start mbedtls_pk_parse_key[(null)]
Connecting to /public.iot-as-mqtt.cn-shanghai.aliyuncs.com/443...
setsockopt SO_SNDTIMEO timeout: 10s
connecting IP_ADDRESS: 106.15.100.2
 ok
  . Setting up the SSL/TLS structure...
 ok
Performing the SSL/TLS handshake...
 ok
  . Verifying peer X.509 certificate..
certificate verification result: 0x00
hdl = 0x0063f010

=> #1. HAL_SSL_Establish() TEST PASS!

res = 5

=> #2. HAL_SSL_Write() TEST PASS!

need release client crt&key
ssl_disconnect
res = 0

=> #3. HAL_SSL_Destroy() TEST PASS!

```

### <a name="设备端TLS密码算法">设备端TLS密码算法</a>

目前C-SDK连接云端用到的默认TLS算法是: **`TLS-RSA-WITH-AES-256-CBC-SHA256`**

### <a name="云端TLS密码算法">云端TLS密码算法</a>

目前阿里云IoT平台支持的TLS算法清单如下:

+ `TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384`
+ `TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384`
+ `TLS_RSA_WITH_AES_256_CBC_SHA256`
+ `TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384`
+ `TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384`
+ `TLS_DHE_RSA_WITH_AES_256_CBC_SHA256`
+ `TLS_DHE_DSS_WITH_AES_256_CBC_SHA256`
+ `TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA`
+ `TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA`
+ `TLS_RSA_WITH_AES_256_CBC_SHA`
+ `TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA`
+ `TLS_ECDH_RSA_WITH_AES_256_CBC_SHA`
+ `TLS_DHE_RSA_WITH_AES_256_CBC_SHA`
+ `TLS_DHE_DSS_WITH_AES_256_CBC_SHA`
+ `TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256`
+ `TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256`
+ `TLS_RSA_WITH_AES_128_CBC_SHA256`
+ `TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256`
+ `TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256`
+ `TLS_DHE_RSA_WITH_AES_128_CBC_SHA256`
+ `TLS_DHE_DSS_WITH_AES_128_CBC_SHA256`
+ `TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA`
+ `TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA`
+ `TLS_RSA_WITH_AES_128_CBC_SHA`
+ `TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA`
+ `TLS_ECDH_RSA_WITH_AES_128_CBC_SHA`
+ `TLS_DHE_RSA_WITH_AES_128_CBC_SHA`
+ `TLS_DHE_DSS_WITH_AES_128_CBC_SHA`
+ `TLS_ECDHE_ECDSA_WITH_RC4_128_SHA`
+ `TLS_ECDHE_RSA_WITH_RC4_128_SHA`
+ `SSL_RSA_WITH_RC4_128_SHA`
+ `TLS_ECDH_ECDSA_WITH_RC4_128_SHA`
+ `TLS_ECDH_RSA_WITH_RC4_128_SHA`
+ `TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384`
+ `TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256`
+ `TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384`
+ `TLS_RSA_WITH_AES_256_GCM_SHA384`
+ `TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384`
+ `TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384`
+ `TLS_DHE_RSA_WITH_AES_256_GCM_SHA384`
+ `TLS_DHE_DSS_WITH_AES_256_GCM_SHA384`
+ `TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256`
+ `TLS_RSA_WITH_AES_128_GCM_SHA256`
+ `TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256`
+ `TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256`
+ `TLS_DHE_RSA_WITH_AES_128_GCM_SHA256`
+ `TLS_DHE_DSS_WITH_AES_128_GCM_SHA256`
+ `TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA`
+ `TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA`
+ `SSL_RSA_WITH_3DES_EDE_CBC_SHA`
+ `TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA`
+ `TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA`
+ `SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA`
+ `SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA`
+ `SSL_RSA_WITH_RC4_128_MD5`
+ `TLS_EMPTY_RENEGOTIATION_INFO_SCSV`

## <a name="B.13 配网绑定问题">B.13 配网绑定问题</a>

iOS手机用云智能app绑定失败
---
这是因为iOS版本的云智能app无法解决`5683`端口上的占用问题, 使得绑定环节中, 手机收不到设备的回复.

> 可考虑在WiFi模组的`AliOS Things`开源版本上应用如下patch规避

+ [aos2.0.0的patch](https://code.aliyun.com/edward.yangx/public-docs/blob/master/patches/aos20-bugID-20066702-awss-add-connectap-info-response-to-i.patch)
+ [aos1.3.2的patch](https://code.aliyun.com/edward.yangx/public-docs/blob/master/patches/aos132_fix_ios_app.patch)

## <a name="B.14 OTA升级问题">B.14 OTA升级问题</a>

OTA升级出现下载失败
---
如果OTA升级出现长时间下载固件不成功, 设备日志显示类似以下文本

```
[LK-DBG] httpclient_recv(411): 127 bytes has been read
[LK-INF] httpclient_common(862): host: 'ota.iot-thing.cn-shanghai.aliyuncs.com', port: 443
[LK-DBG] httpclient_retrieve_content(455): Current data: 
[LK-DBG] httpclient_retrieve_content(561): Total-Payload: 416 Bytes; Read: 0 Bytes
[LK-DBG] httpclient_recv(411): 127 bytes has been read
[LK-INF] httpclient_common(862): host: 'ota.iot-thing.cn-shanghai.aliyuncs.com', port: 443
[LK-DBG] httpclient_retrieve_content(455): Current data: 
[LK-DBG] httpclient_retrieve_content(561): Total-Payload: 289 Bytes; Read: 0 Bytes
[LK-DBG] httpclient_recv(411): 127 bytes has been read
[LK-INF] httpclient_common(862): host: 'ota.iot-thing.cn-shanghai.aliyuncs.com', port: 443
[LK-DBG] httpclient_retrieve_content(455): Current data: 
[LK-DBG] httpclient_retrieve_content(561): Total-Payload: 162 Bytes; Read: 0 Bytes
[LK-DBG] httpclient_recv(411): 109 bytes has been read
[LK-DBG] httpclient_retrieve_content(601): extend countdown_ms to avoid NULL input to select, tired 1 times
[LK-DBG] MQTTKeepalive(375): len = MQTTSerialize_pingreq() = 2
[LK-INF] iotx_mc_keepalive_sub(2928): send MQTT ping...
[LK-INF] iotx_mc_cycle(1829): receive ping response!
[LK-DBG] httpclient_retrieve_content(601): extend countdown_ms to avoid NULL input to select, tired 101 times
[LK-PRT] ssl recv error: code = -30848, err_str = 'SSL - The peer notified us that '
[LK-ERR] httpclient_recv(408): Connection error (recv returned -2)
[LK-ERR] httpclient_common(893): httpclient_recv_response is error,ret = -8
[LK-PRT] ssl_disconnect
[LK-INF] httpclient_close(851): client disconnected
[LK-ERR] ofc_Fetch(65): fetch firmware failed
[LK-ERR] IOT_OTA_FetchYield(700): Fetch firmware failed
[LK-INF] MQTTPublish(514): Upstream Topic: '/ota/device/progress/a19R38p2SdQ/horizon_real'
[LK-INF] MQTTPublish(515): Upstream Payload:

> {
>     "id": 0,
>     "params": {
>         "step": "-2",
>         "desc": ""
>     }
> }
```

这往往是因为调用 `IOT_Linkkit_Query()` 接口时传入的 `buffer_length` 太短, 下载太慢导致的

```c
char buffer[128] = {0};
int buffer_length = 128;

IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
```

例如以上的错误日志, 对应的就是上面的调用代码, 每次只下载 `127` 字节, 而整个固件有 `50MB` 以上, 花费时间太长出现的

**因为OTA服务器有个限制, 如果下载速度低于`10KB/s`达到累计`1分钟`以上, 则会主动断开设备连接**

从上面的日志中, 可以看到

```
[LK-PRT] ssl recv error: code = -30848, err_str = 'SSL - The peer notified us that '
```

这就是服务端断开了设备连接造成的, 之后SDK按照OTA协议从MQTT通道上报了 `"step": "-2"` 的JSON报文, 控制台就会显示下载失败

---
优化的办法就是对固件比较大( > `100KB` )的情况, 调整每次下载的buffer长度, 比如将以上代码改为

```c
char buffer[8192] = {0};
int buffer_length = 8192;

IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
```

