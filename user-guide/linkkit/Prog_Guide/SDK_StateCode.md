# <a name="目录">目录</a>
+ [SDK 状态码运行反馈机制](#SDK 状态码运行反馈机制)
+ [设计背景](#设计背景)
+ [设计简述](#设计简述)
    * [从返回值取状态码](#从返回值取状态码)
    * [从事件回调取状态码](#从事件回调取状态码)
+ [状态码的使用](#状态码的使用)
    * [从Event事件机制得到状态码反馈](#从Event事件机制得到状态码反馈)
    * [允许用户退订部分状态码](#允许用户退订部分状态码)
    * [演示状态码的处理](#演示状态码的处理)
        - [新增演示接口 IOT_Extension_StateDesc()](#新增演示接口 IOT_Extension_StateDesc())
        - [新增 linkkit_example_solo 中的演示](#新增 linkkit_example_solo 中的演示)
+ [部分状态码清单](#部分状态码清单)
    * [用户输入相关](#用户输入相关)
    * [系统依赖相关](#系统依赖相关)
    * [MQTT通信](#MQTT通信)
    * [WiFi配网](#WiFi配网)
    * [HTTP通信](#HTTP通信)
    * [设备绑定](#设备绑定)
    * [物模型/子设备管理](#物模型/子设备管理)

# <a name="SDK 状态码运行反馈机制">SDK 状态码运行反馈机制</a>

# <a name="设计背景">设计背景</a>

版本 `V3.1.0` 之前, Link Kit SDK 的用户API, 工作异常时返回值的取值很集中

+ 多个API不管因为什么异常失败返回值都是-1, 返回异常之前的信息表达在日志文本中
+ 工作正常时, 执行的过程同样也表达在日志的打印中

这样除非有SDK的日志文本, 无法得知SDK的运行状态, 比如, 某场景下SDK是否按期运行, 或者, 某API为什么没有如期的完成工作等

---
从 `3.1.0` 版本开始, 规范SDK运行状态的表达方式, 以返回值/用户监听事件的形式, 反馈给SDK的用户, 这就是**状态码机制**

# <a name="设计简述">设计简述</a>

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/state_code_design.png" width="1000">

SDK的内部运行状态可分为两类

- 正常运行至阶段性成功节点: 例如, WiFi密码解密成功, MQTT连云成功, MQTT掉线后重连成功等
- 异常运行至错误处理分支, 甚至返回到用户层: 例如, WiFi连AP失败, TCP建连失败, 等待云端回应超时等

对以上两种情况, 我们都以1个16位的整数来标识和反馈, 这个整数称为SDK的运行状态码(`State Code`)

- 状态码的取值范围是 `-0x0000 ~ -0xFFFF`, 16bits, 取值分布丰富, 且能唯一反推到产生来源
- 在 API 未按预期工作时, 必须在返回值处, 以状态码的形式告知 API 的调用方
- 在 API 已按预期工作时, 仍需在事件处理回调函数的入参处, 已状态码的形式告知回调的注册方
- 以返回值表达运行状态, 与以事件矛盾不严格互斥, 必要时允许在两条途径上, 同时表达

## <a name="从返回值取状态码">从返回值取状态码</a>

例如, 以下代码可定位MQTT连云时的异常

```c
int retval = 0;

retval = IOT_MQTT_Connect(handle);	/* 实际运行得到 retval 的值不是成功的0, 而是 -0x0305 */
...

#define STATE_MQTT_BASE                  (-0x0300)
#define STATE_MQTT_CONNACK_BAD_USERDATA	 (STATE_MQTT_BASE - 0x0005)
```

由于SDK底层只有在所收MQTT服务器返回 CONNACK 报文中表示用户名/密码错的分支上才会返回 -0x0305 这个状态码, 则不必查询日志, 从 API 的返回值就能得知, 当前连不上MQTT服务器的原因是鉴权失败

## <a name="从事件回调取状态码">从事件回调取状态码</a>

例如, 以下代码可界定绑定失败时设备是否按预期工作

```c
static int dev_bind_event_handler(int ev, char *msg)
{
    printf("received event of dev_bind as -0x%04x(%s)\n", -ev, msg);
    return 0;
}

IOT_RegisterCallback(ITE_STATE_DEV_BIND, dev_bind_event_handler);
```

如果绑定失败, 而用户日志中出现(注意, 不是SDK的日志)
```
received event of dev_bind as -0x0809(4786F0F6E1CC7BF86017FD4D1429EE4F)
```

而在SDK的头文件中查询到
```c
#define STATE_BIND_BASE                      (-0x0800)
#define STATE_BIND_TOKEN_NOTIFY_DONE         (STATE_BIND_BASE - 0x0009)
```

则可确定设备已将值为 `4786F0F6E1CC7BF86017FD4D1429EE4F` 的Token广播发送, 可界定SDK工作至此正常

# <a name="状态码的使用">状态码的使用</a>

状态码列表定义在 `src/infra/infra_state.h`, 这个头文件被用户的程序或者SDK的demo程序包含

```c
#define STATE_BASE                                (0x0000)

#define STATE_USER_INPUT                          (-0x0100)
#define STATE_SYS_DEPEND                          (-0x0200)
#define STATE_MQTT_BASE                           (-0x0300)
...
#define STATE_BIND_BASE                           (-0x0800)
#define STATE_DEV_MODEL_BASE                      (-0x0900)
...
#define STATE_BOTTOM_BOUNDARY                     (-0xFFFF)
```

以上高8位增长时, 体现的是状态码的"初步"分类, 是按照SDK的功能点粗略来分的, 在低8位上, 还有"初步"分类基础上的"细分"分类

## <a name="从Event事件机制得到状态码反馈">从Event事件机制得到状态码反馈</a>

> 从返回值接收状态码不必详细分析
>
> 从事件通知接收状态码涉及的改造定义如下

Event如 `ITE_CONNECT_SUCC` (表达连云成功)列表之后, 新增如下

```c
typedef enum {
    ITE_AWSS_STATUS,
    ...
    ITE_MQTT_CONNECT_SUCC,
    ITE_STATE_EVENYTHING,		/* 表示收听所有的状态码 */
    ITE_STATE_USER_INPUT,		/* 表示接收到的用户输入异常或对用户未输入参数有默认填充 */
    ...
} iotx_ioctl_event_t;
```

完整的列表如下

| **新增事件**            | **适用范围**
|:-----------------------:|-----------------------------------------------------------------
| `ITE_STATE_EVENYTHING`  | 所有的状态码
| `ITE_STATE_USER_INPUT`  | 接收到的用户输入异常或对用户未输入参数SDK有默认填充
| `ITE_STATE_SYS_DEPEND`  | 调用底层系统的接口(如TCP建连), 返回了成功或者失败
| `ITE_STATE_MQTT_COMM`   | MQTT连云范畴内的事件, 如鉴权失败, 心跳超时, 重连成功
| `ITE_STATE_WIFI_PROV`   | WiFi配网范畴内的事件, 如解密成功, 连AP失败, 开启热点
| `ITE_STATE_COAP_LOCAL`  | CoAP局域网通信范畴内的事件, 如三层配网, 绑定, 本地通信
| `ITE_STATE_HTTP_COMM`   | HTTP连云范畴内的事件, 如预认证, 动态注册, OTA
| `ITE_STATE_OTA`         | 固件/配置下载范畴内的事件, 如收到固件推送, 获取cota配置成功
| `ITE_STATE_DEV_BIND`    | 绑定环节中的事件, 如刷新Token, 广播Token, 上报复位请求
| `ITE_STATE_DEV_MODEL`   | 物模型/子设备管理中的时间, 如子设备登录, 主设备断开/连接等


## <a name="允许用户退订部分状态码">允许用户退订部分状态码</a>

如果用户只关心某些范畴的状态码, 可以选择性的订阅, 也可以选择性的退订

```c
IOT_RegisterCallback(ITE_STATE_EVERYTHING, my_handler);
IOT_RegisterCallback(ITE_STATE_MQTT_COMM, NULL);        /* 显式退订MQTT相关的状态码上报 */

IOT_RegisterCallback(ITE_STATE_EVERYTHING, NULL);       /* 显式退订所有的状态码上报 */
IOT_RegisterCallback(ITE_STATE_DEV_BIND, my_handler);   /* 单独订阅设备绑定的状态码 */
```

## <a name="演示状态码的处理">演示状态码的处理</a>

### <a name="新增演示接口 IOT_Extension_StateDesc()">新增演示接口 IOT_Extension_StateDesc()</a>

在SDK外部, 作为辅助可提供状态码的文本解释字符串列表, 并通过接口 `IOT_Extension_StateDesc` 返回

```c
typedef struct state_desc_s {
    int code;
    char * desc;
} state_desc_t;

state_t state_descriptions[] = {
    {STATE_MQTT_CONN_SUCCESS, "MQTT cloud connect established successfully"},
    {STATE_MQTT_CONNACK_BAD_USERDATA, "MQTT connect failed due to bad userdata"},
    {STATE_BOTTOM_BOUNDARY, "Should not reach, state code ran out already"}
};

static char _desc_error_msg[128];
const char * IOT_Extension_StateDesc(const int state_code) {
	int iter;

    for(iter = 0; state_descriptions[iter].code != STATE_BOTTOM_BOUNDARY; ++iter) {
        if (state_descriptions[iter].code == state_code) {
            return state_descriptions[iter].desc;
        }
    }

    snprintf(_desc_error_msg, "Description not found for state -%04x", state_code);
    return _desc_error_msg;
}
```

### <a name="新增 linkkit_example_solo 中的演示">新增 linkkit_example_solo 中的演示</a>
```c
int dump_every_state_event(const int code, const char * msg) {
    printf("[%s] recv %04x with '%s', means - '%s'\n",
           current_time(),
           code,
           msg,
           IOT_Extension_StateDesc(code));

    return code;
}

int main(int argc, char *argv[]) {
    ...
    IOT_RegisterCallback(ITE_STATE_EVERYTHING, dump_every_state_event);
    ...
}
```

程序运行后, 可出现如下的打印, 看起来像是SDK外挂的一个日志系统

```
[05:01:19] recv -0x0401 with 'TestSSID/TestPwd', means - 'WiFi provision SSID/PWD Got'
[05:01:21] recv -0x0403 with '192.168.1.103', means - 'WiFi hotspot asssociated'
[05:01:22] recv -0x0301 with '', means - 'MQTT cloud connect successfully'
[05:01:23] recv -0x0507 with '4786F0F6E1CC7BF86017FD4D94F', means - 'Bind token reported'
[05:01:24] recv -0x0509 with 'enrollee/match_reply', means - 'Bind token reply received'
[05:01:25] recv -0x050C with '4786F0F6E1CC7BF86017FD4D94F', means - 'Device token notify'
```

# <a name="部分状态码清单">部分状态码清单</a>

## <a name="用户输入相关">用户输入相关</a>

| **#**   | **数值**    | **名字**                            | **解释**
|---------|-------------|-------------------------------------|---------------------------------------------------------------------------------
| **01**  | `-0x0101`   | `STATE_USER_INPUT_NULL_POINTER`     | 用户传递给API的参数中含有不合理的空指针参数
| **02**  | `-0x0102`   | `STATE_USER_INPUT_OUT_RANGE`        | 用户传递给API的参数中含有超出合理范围的取值
| **03**  | `-0x0103`   | `STATE_USER_INPUT_PK`               | 用户传递给API的参数中含有不合法的产品标识(productKey)
| **04**  | `-0x0104`   | `STATE_USER_INPUT_PS`               | 用户传递给API的参数中含有不合法的产品密钥(productSecret)
| **05**  | `-0x0105`   | `STATE_USER_INPUT_DN`               | 用户传递给API的参数中含有不合法的设备名称(deviceName)
| **06**  | `-0x0106`   | `STATE_USER_INPUT_DS`               | 用户传递给API的参数中含有不合法的设备密钥(deviceSecret)
| **07**  | `-0x0107`   | `STATE_USER_INPUT_HTTP_DOMAIN`      | 用户传递给API的参数中含有不合理的域名, SDK无法对其完成HTTP连接
| **08**  | `-0x0108`   | `STATE_USER_INPUT_MQTT_DOMAIN`      | 用户传递给API的参数中含有不合理的域名, SDK无法对其完成MQTT连接
| **09**  | `-0x0109`   | `STATE_USER_INPUT_HTTP_PORT`        | 用户传递给API的参数中含有不合理的HTTP目的端口号
| **10**  | `-0x010A`   | `STATE_USER_INPUT_HTTP_TIMEOUT`     | 用户传递给API的参数中含有不合理的HTTP超时时间
| **11**  | `-0x010B`   | `STATE_USER_INPUT_HTTP_OPTION`      | 用户传递给API的参数中含有不合理的HTTP连接选项
| **12**  | `-0x010C`   | `STATE_USER_INPUT_HTTP_POST_DATA`   | 用户传递给API的参数中, 对HTTP POST含有不合理的报文描述, 如长度为负或空指针等
| **13**  | `-0x010D`   | `STATE_USER_INPUT_HTTP_URL`         | 用户传递给API的参数中含有不合理的HTTP目的URL
| **14**  | `-0x010E`   | `STATE_USER_INPUT_HTTP_PATH`        | 用户传递给API的参数中含有不合理的HTTP资源路径
| **15**  | `-0x010F`   | `STATE_USER_INPUT_META_INFO`        | 用户传递给API的参数中含有不合理的产品标识/密钥或设备名称/密钥
| **16**  | `-0x0110`   | `STATE_USER_INPUT_DEVID`            | 用户传递给API的参数中含有不合理的 deviceID 设备标识符
| **17**  | `-0x0111`   | `STATE_USER_INPUT_DEVICE_TYPE`      | 用户传递给API的参数中含有不合理的设备类型, 既不是master又不是slave
| **18**  | `-0x0112`   | `STATE_USER_INPUT_MSG_TYPE`         | 用户传递给 IOT_Linkkit_Report() 等API的参数中含有不合理的消息类型
| **19**  | `-0x0113`   | `STATE_USER_INPUT_INVALID`          | 用户传递给API的参数中含有其它不合理的取值

## <a name="系统依赖相关">系统依赖相关</a>

| **#**   | **数值**    | **名字**                                | **解释**
|---------|-------------|-----------------------------------------|---------------------------------------------------------------------------------------------
| **01**  | `-0x0201`   | `STATE_SYS_DEPEND_MALLOC`               | SDK调用的系统适配接口 HAL_Malloc() 返回异常, 未能成功分配内存
| **02**  | `-0x0202`   | `STATE_SYS_DEPEND_KV_GET`               | SDK调用的系统适配接口 HAL_Kv_Get() 返回异常, 未能成功根据Key获取Value
| **03**  | `-0x0203`   | `STATE_SYS_DEPEND_KV_SET`               | SDK调用的系统适配接口 HAL_Kv_Set() 返回异常, 未能成功根据Key写入Value
| **04**  | `-0x0204`   | `STATE_SYS_DEPEND_KV_DELETE`            | SDK调用的系统适配接口 HAL_Kv_Del() 返回异常, 未能成功根据Key删除KV对
| **05**  | `-0x0205`   | `STATE_SYS_DEPEND_MUTEX_CREATE`         | SDK调用的系统适配接口 HAL_MutexCreate() 返回异常, 未能创建一个互斥锁
| **06**  | `-0x0206`   | `STATE_SYS_DEPEND_MUTEX_LOCK`           | SDK调用的系统适配接口 HAL_MutexLock() 返回异常, 未能成功申请互斥锁
| **07**  | `-0x0207`   | `STATE_SYS_DEPEND_MUTEX_UNLOCK`         | SDK调用的系统适配接口 HAL_MutexUnlock() 返回异常, 未能成功释放互斥锁
| **08**  | `-0x0208`   | `STATE_SYS_DEPEND_NWK_CLOSE`            | SDK调用的系统适配接口 HAL_TCP_Read
| **09**  | `-0x0209`   | `STATE_SYS_DEPEND_NWK_TIMEOUT`          | SDK调用的系统适配接口 HAL_TCP_Read() 或 HAL_SSL_Read() 未能成功在预期的时间内得到网络回应
| **10**  | `-0x020A`   | `STATE_SYS_DEPEND_NWK_INVALID_HANDLE`   | SDK进行底层网络收发通信时, 看到系统对TCP连接或TLS连接的标识句柄是非法的
| **11**  | `-0x020B`   | `STATE_SYS_DEPEND_NWK_READ_ERROR`       | SDK调用的系统适配接口 HAL_TCP_Read() 或 HAL_SSL_Read() 返回异常, 未能成功读取一段内容
| **12**  | `-0x020C`   | `STATE_SYS_DEPEND_SEMAPHORE_CREATE`     | SDK调用的系统适配接口 HAL_SemaphoreCreate() 返回异常, 未能成功申请信号量
| **13**  | `-0x020D`   | `STATE_SYS_DEPEND_SEMAPHORE_WAIT`       | SDK调用的系统适配接口 HAL_SemaphoreWait() 返回异常, 未能成功在信号量上睡眠
| **14**  | `-0x020E`   | `STATE_SYS_DEPEND_SNPRINTF`             | SDK调用的系统适配接口 HAL_Snprintf() 返回异常, 未能成功拼接格式化字符串
| **15**  | `-0x020F`   | `STATE_SYS_DEPEND_FIRMWAIRE_WIRTE`      | SDK调用的系统适配接口 HAL_Firmware_Persistence_Write() 返回异常, 未能成功将固件写入ROM

## <a name="MQTT通信">MQTT通信</a>

| **#**   | **数值**    | **名字**                                    | **解释**
|---------|-------------|---------------------------------------------|-----------------------------------------------------------------------------------------
| **01**  | `-0x0301`   | `STATE_MQTT_CONNACK_VERSION_UNACCEPT`       | 设备发出MQTT建连请求后, 服务器应答的报文表示不能接受请求中的MQTT协议版本
| **02**  | `-0x0302`   | `STATE_MQTT_CONNACK_IDENT_REJECT`           | 设备发出MQTT建连请求后, 服务器应答的报文表示不能接受请求中的设备标识符
| **03**  | `-0x0303`   | `STATE_MQTT_CONNACK_SERVICE_NA`             | 设备发出MQTT建连请求后, 服务器应答的报文表示当前MQTT服务不可用
| **04**  | `-0x0304`   | `STATE_MQTT_CONNACK_NOT_AUTHORIZED`         | 设备发出MQTT建连请求后, 服务器应答的报文表示当前对请求设备未能完成鉴权
| **05**  | `-0x0305`   | `STATE_MQTT_CONNACK_BAD_USERDATA`           | 设备发出MQTT建连请求后, 服务器应答的报文表示对设备身份鉴权不通过
| **06**  | `-0x0306`   | `STATE_MQTT_WAIT_RECONN_TIMER`              | 跳过当前的重连动作, 因为上一次的重连过去的时间太短, 稍晚再进行连接
| **07**  | `-0x0307`   | `STATE_MQTT_SIGN_HOSTNAME_BUF_SHORT`        | 计算设备签名时, 为域名预留的字符串缓冲区太短, 不足容纳输出内容
| **08**  | `-0x0308`   | `STATE_MQTT_SIGN_USERNAME_BUF_SHORT`        | 计算设备签名时, 为MQTT的用户名预留的字符串缓冲区太短, 不足容纳输出内容
| **09**  | `-0x0309`   | `STATE_MQTT_SIGN_CLIENTID_BUF_SHORT`        | 计算设备签名时, 为MQTT的clientId预留的字符串缓冲区太短, 不足容纳输出内容
| **10**  | `-0x030A`   | `STATE_MQTT_SIGN_SOURCE_BUF_SHORT`          | 计算设备签名时, 为拼接签名来源内容预留的字符串缓冲区太短, 不足容纳输出内容
| **11**  | `-0x030B`   | `STATE_MQTT_WRAPPER_INIT_FAIL`              | SDK调用的MQTT建连底层接口 wrapper_mqtt_init() 异常, 初始化失败
| **12**  | `-0x030C`   | `STATE_MQTT_SERIALIZE_CONN_ERROR`           | 序列化MQTT建连请求报文异常
| **13**  | `-0x030D`   | `STATE_MQTT_SERIALIZE_PUBACK_ERROR`         | 序列化对服务器下推报文的应答消息异常
| **14**  | `-0x030E`   | `STATE_MQTT_SERIALIZE_PINGREQ_ERROR`        | 序列化MQTT心跳请求报文异常
| **15**  | `-0x030F`   | `STATE_MQTT_SERIALIZE_SUB_ERROR`            | 序列化MQTT订阅请求报文异常
| **16**  | `-0x0310`   | `STATE_MQTT_SERIALIZE_UNSUB_ERROR`          | 序列化MQTT取消订阅请求报文异常
| **17**  | `-0x0311`   | `STATE_MQTT_SERIALIZE_PUB_ERROR`            | 序列化MQTT上报消息报文异常
| **18**  | `-0x0312`   | `STATE_MQTT_DESERIALIZE_CONNACK_ERROR`      | 反序列化MQTT服务器返回的建连应答报文异常
| **19**  | `-0x0313`   | `STATE_MQTT_DESERIALIZE_SUBACK_ERROR`       | 反序列化MQTT服务器返回的订阅应答报文异常
| **20**  | `-0x0314`   | `STATE_MQTT_DESERIALIZE_PUB_ERROR`          | 反序列化MQTT服务器下推的消息报文异常
| **21**  | `-0x0315`   | `STATE_MQTT_DESERIALIZE_UNSUBACK_ERROR`     | 反序列化MQTT服务器返回的取消订阅应答报文异常
| **22**  | `-0x0316`   | `STATE_MQTT_PACKET_READ_ERROR`              | 未能成功从网络上读取服务器下发的MQTT报文内容
| **23**  | `-0x0317`   | `STATE_MQTT_CONNACK_UNKNOWN_ERROR`          | 未能成功解析从MQTT服务器返回的建连请求应答
| **24**  | `-0x0318`   | `STATE_MQTT_RX_BUFFER_TOO_SHORT`            | 从网络协议栈读取MQTT下推报文时, 预留的缓冲区太短, 不足容纳待收取内容
| **25**  | `-0x0319`   | `STATE_MQTT_TX_BUFFER_TOO_SHORT`            | 组装MQTT上报消息准备发给网络协议栈时, 为报文内容预留的缓冲区太短, 不足容纳待发送内容
| **26**  | `-0x031A`   | `STATE_MQTT_TOPIC_BUF_TOO_SHORT`            | 组装Topic准备进行MQTT发布或者订阅时, 为Topic字段预留的缓冲区太短, 不足容纳组装内容
| **27**  | `-0x031B`   | `STATE_MQTT_CONN_RETRY_EXCEED_MAX`          | 单独的一次 IOT_MQTT_Construct() 接口工作中, 已多次重试仍未连上服务端, 停止尝试返回
| **28**  | `-0x031C`   | `STATE_MQTT_QOS1_REPUB_EXCEED_MAX`          | 对QoS1类型的MQTT上报消息已多次重试, 仍未得到服务端收到的回应, 放弃重新发送
| **29**  | `-0x031D`   | `STATE_MQTT_PUB_QOS_INVALID`                | 对MQTT消息上报时, 遇到不合理的QoS参数取值
| **30**  | `-0x031E`   | `STATE_MQTT_IN_OFFLINE_STATUS`              | 因为当前MQTT连接已断开, 跳过当前可能的发送或接收动作
| **31**  | `-0x031F`   | `STATE_MQTT_RECV_UNKNOWN_PACKET`            | 从网络上收取到的MQTT报文, 无法解析其报文类型
| **32**  | `-0x0320`   | `STATE_MQTT_CLI_EXCEED_MAX`                 | 当前已创建的MQTT连接数量超出限制, 已无法再新建连接
| **33**  | `-0x0321`   | `STATE_MQTT_SUB_EXCEED_MAX`                 | 当前已订阅的MQTT主题数量超出限制, 已无法再新建订阅
| **34**  | `-0x0322`   | `STATE_MQTT_UNEXP_TOPIC_FORMAT`             | 遇到不合理的MQTT主题格式, 无法进行订阅或发布等动作
| **35**  | `-0x0323`   | `STATE_MQTT_SYNC_SUB_TIMEOUT`               | 进行同步的订阅请求时, 已超出限定时长而未得到服务端回应
| **36**  | `-0x0324`   | `STATE_MQTT_ASYNC_STACK_CONN_IN_PROG`       | 在异步协议栈上工作时, 上次的连接请求尚未得到服务端回应, 跳过当前连接请求
| **37**  | `-0x0325`   | `STATE_MQTT_ASYNC_STACK_NOT_SUPPORT`        | 当前SDK未被配置为支持在异步协议栈上工作
| **38**  | `-0x0326`   | `STATE_MQTT_ASYNC_STACK_UNKNOWN_EVENT`      | SDK在异步协议栈上工作时, 接收到来自协议栈的事件, 但不能理解
| **39**  | `-0x0327`   | `STATE_MQTT_CONN_USER_INFO`                 | MQTT发起连接的过程中, 上报连接相关参数如用户名和密码等
| **40**  | `-0x0328`   | `STATE_MQTT_SUB_INFO`                       | MQTT发起订阅的过程中, 上报订阅相关参数如Topic等
| **41**  | `-0x0329`   | `STATE_MQTT_PUB_INFO`                       | MQTT发布上报的过程中, 上报发布相关参数如Topic等

## <a name="WiFi配网">WiFi配网</a>

| **#**   | **数值**    | **名字**                                    | **解释**
|---------|-------------|---------------------------------------------|---------------------------------------------------------------------
| **01**  | `-0x0401`   | `STATE_WIFI_ENABLE_AWSS`                    | WiFi配网进入使能状态
| **02**  | `-0x0402`   | `STATE_WIFI_PROCESS_FRAME`                  | WiFi配网正在处理1个2层报文
| **03**  | `-0x0403`   | `STATE_WIFI_CHAN_SCAN`                      | WiFi配网正在进行对信道进行切换/锁定等操作
| **04**  | `-0x0404`   | `STATE_WIFI_CONNECT_AP_SUCCESS`             | 已使用WiFi配网接收到的密码, 连接WiFi热点成功
| **05**  | `-0x0405`   | `STATE_WIFI_CONNECT_AP_FAILED`              | 已使用WiFi配网接收到的密码, 连接WiFi热点失败
| **06**  | `-0x0406`   | `STATE_WIFI_IN_PROGRESS`                    | 不应启动新的一轮WiFi配网, 因为上一轮仍未结束
| **07**  | `-0x0407`   | `STATE_WIFI_FORCE_STOPPED`                  | 已按用户调用API的指令, 停止了WiFi配网进程
| **08**  | `-0x0408`   | `STATE_WIFI_DESTROY`                        | 正在释放WiFi配网所占用的内部资源
| **09**  | `-0x0409`   | `STATE_WIFI_GOT_HINT_FRAME`                 | WiFi配网功能已成功监听到关键信息起始标志帧
| **10**  | `-0x040A`   | `STATE_WIFI_ENCRYPT_TYPE`                   | WiFi配网中发现关键信息的加密方式是一型一密或一机一密等
| **11**  | `-0x040B`   | `STATE_WIFI_CRC_ERROR`                      | WiFi配网中发现报文的校验和不符合预期
| **12**  | `-0x040C`   | `STATE_WIFI_PASSWD_DECODE_FAILED`           | WiFi配网中解密WiFi热点的密码失败
| **13**  | `-0x040D`   | `STATE_WIFI_PASSWD_DECODE_SUCCESS`          | WiFi配网中解密WiFi热点的密码成功
| **14**  | `-0x040E`   | `STATE_WIFI_STATISTIC`                      | WiFi配网中的统计信息, 比如收到包的数量, 配网时间, 收包报文的总长度
| **15**  | `-0x040F`   | `STATE_WIFI_UNEXP_PROT_VERSION`             | 手机端发来的配网协议的版本号不符合预期
| **16**  | `-0x0410`   | `STATE_WIFI_MCAST_DEBUG`                    | WiFi组播配网中的调试信息
| **17**  | `-0x0411`   | `STATE_WIFI_BCAST_DEBUG`                    | WiFi包长编码的一键配网中的调试信息
| **18**  | `-0x0412`   | `STATE_WIFI_ZCONFIG_ENROLLEE_DEBUG`         | WiFi零配过程中的待配方调试信息
| **19**  | `-0x0413`   | `STATE_WIFI_ZCONFIG_REGISTAR_PARAMS_ERROR`  | WiFi零配过程中的主配方发来的参数有误
| **20**  | `-0x0414`   | `STATE_WIFI_ZCONFIG_REGISTAR_DEBUG`         | WiFi零配过程中的主配方调试信息
| **21**  | `-0x0415`   | `STATE_WIFI_P2P_DEBUG`                      | WiFi p2p配网过程中的调试信息
| **22**  | `-0x0416`   | `STATE_WIFI_DEV_AP_DEBUG`                   | WiFi 设备热点配网过程中的调试信息
| **23**  | `-0x0417`   | `STATE_WIFI_OTHERS`                         | WiFi配网中的其它过程信息
| **24**  | `-0x0418`   | `STATE_WIFI_MQTT_INIT_FAILED`               | WiFi配网中 MQTT初始化失败, MQTT用于零配主配方
| **25**  | `-0x0419`   | `STATE_WIFI_COAP_INIT_FAILED`               | WiFi配网中 CoAP 初始化失败, CoAP用于所有配网模式
| **26**  | `-0x041A`   | `STATE_WIFI_COAP_UNINIT`                    | WiFi配网中 CoAP Server 未初始化
| **27**  | `-0x041B`   | `STATE_WIFI_COAP_RSP_INVALID`               | WiFi配网中 收到CoAP非法应答消息
| **28**  | `-0x041C`   | `STATE_WIFI_COAP_PKTBLD_FAILED`             | WiFi配网中 CoAP 组包失败
| **29**  | `-0x041D`   | `STATE_WIFI_INVALID_MQTT_EVENT`             | WiFi配网中 收到非法 MQTT 消息
| **30**  | `-0x041E`   | `STATE_WIFI_SENT_DEVINFO_NOTIFY`            | 手机热点配网广播设备信息
| **31**  | `-0x041F`   | `STATE_WIFI_SENT_CONNECTAP_NOTIFY`          | WiFi配网中广播通知连接ap事件
| **32**  | `-0x0420`   | `STATE_WIFI_GOT_CONNECTAP_QUERY`            | WiFi配网中收到查询连接ap状态的包
| **33**  | `-0x0421`   | `STATE_WIFI_GOT_DEVINFO_QUERY`              | WiFi配网中收到查询设备信息的包(手机热点)
| **34**  | `-0x0422`   | `STATE_WIFI_SENT_CONNECTAP_RESP`            | WiFi配网中设备回复手机查询connectap事件
| **35**  | `-0x0423`   | `STATE_WIFI_SENT_DEVINFO_RESP`              | WiFi配网中设备回复手机查询设备信息事件

## <a name="HTTP通信">HTTP通信</a>

| **#**   | **数值**    | **名字**                                | **解释**
|---------|-------------|-----------------------------------------|-------------------------------------------------------------------------------------
| **01**  | `-0x0601`   | `STATE_HTTP_DYNREG_FAIL_RESP`           | 使用动态注册/一型一密功能时, 从服务端返回的报文表示, 申请密钥失败
| **02**  | `-0x0602`   | `STATE_HTTP_DYNREG_INVALID_DS`          | 使用动态注册/一型一密功能时, 从服务端返回的报文中, 解析得到的密钥不合理
| **03**  | `-0x0603`   | `STATE_HTTP_PREAUTH_REQ_BUF_SHORT`      | 组装预认证消息准备发给服务端时, 为HTTP上行报文预留的缓冲区太短, 不足容纳待发送内容
| **04**  | `-0x0604`   | `STATE_HTTP_PREAUTH_INVALID_RESP`       | 收取预认证请求的应答报文时, 为HTTP回应报文预留的缓冲区太短, 不足容纳待接收内容
| **05**  | `-0x0605`   | `STATE_HTTP_RESP_MISSING_CONTENT_LEN`   | 从HTTP服务器返回的报文中, 没有找到 Content-Length 字段
| **06**  | `-0x0606`   | `STATE_HTTP_PREAUTH_REQ`                | 进行HTTP预认证的过程中, 上报预认证请求的相关信息
| **07**  | `-0x0607`   | `STATE_HTTP_PREAUTH_RSP`                | 进行HTTP预认证的过程中, 上报预认证应答的相关信息

## <a name="设备绑定">设备绑定</a>

| **#**   | **数值**    | **名字**                                | **解释**
|---------|-------------|-----------------------------------------|-------------------------------------------------------------------------
| **01**  | `-0x0801`   | `STATE_BIND_SET_APP_TOKEN`              | 绑定功能模块接收到来自外部的Token传入
| **02**  | `-0x0802`   | `STATE_BIND_COAP_INIT_FAIL`             | 绑定功能模块未能成功开启CoAP服务, 该服务用于局域网内的token传递
| **03**  | `-0x0803`   | `STATE_BIND_REPORT_TOKEN`               | 正在将绑定token发送给服务端
| **04**  | `-0x0804`   | `STATE_BIND_REPORT_TOKEN_SUCCESS`       | 绑定token发送给服务端后, 已得到服务端成功接收的回应
| **05**  | `-0x0805`   | `STATE_BIND_NOTIFY_TOKEN_SENT`          | 正在主动将绑定token在局域网内广播
| **06**  | `-0x0806`   | `STATE_BIND_RECV_TOKEN_QUERY`           | 正在接收到同一局域网内来自其它电子设备的token查询请求
| **07**  | `-0x0807`   | `STATE_BIND_SENT_TOKEN_RESP`            | 正在响应局域网内的token查询请求, 将token发送给其它电子设备
| **08**  | `-0x0808`   | `STATE_BIND_MQTT_RSP_INVALID`           | mqtt信息上报给服务端后, 接收到的回应报文不合法
| **09**  | `-0x0809`   | `STATE_BIND_MQTT_MSGID_INVALID`         | 将绑定token上报给服务端后, 接收到的回应报文msgid不是最新的
| **10**  | `-0x080A`   | `STATE_BIND_COAP_RSP_INVALID`           | 将绑定token上报给手机后, 接收到的回应报文不合法
| **11**  | `-0x080B`   | `STATE_BIND_COAP_REQ_INVALID`           | 接收到来自同一局域网内其它电子设备的token查询请求, 但请求报文不合法
| **12**  | `-0x080C`   | `STATE_BIND_ASSEMBLE_APP_TOKEN_FAILED`  | app token 组装失败
| **13**  | `-0x080D`   | `STATE_BIND_COAP_REGISTER_FAILED`       | 注册coap topic失败
| **14**  | `-0x080E`   | `STATE_BIND_TOKEN_EXPIRED`              | token过期, 需要更新
| **15**  | `-0x080F`   | `STATE_BIND_REPORT_RESET_SUCCESS`       | 解绑信息发送给服务端后, 已得到服务端成功接收的回应
| **16**  | `-0x0810`   | `STATE_BIND_RECV_CLOUD_NOTIFY`          | 收到云端推送的事件, 如绑定与解绑

## <a name="物模型/子设备管理">物模型/子设备管理</a>

| **#**   | **数值**    | **名字**                                        | **解释**
|---------|-------------|-------------------------------------------------|-----------------------------------------------------------------------------
| **01**  | `-0x0901`   | `STATE_DEV_MODEL_MASTER_ALREADY_OPEN`           | 主设备已经打开, 不必再次初始化
| **02**  | `-0x0902`   | `STATE_DEV_MODEL_MASTER_ALREADY_CONNECT`        | 主设备已经连接服务端, 不必再次连接
| **03**  | `-0x0903`   | `STATE_DEV_MODEL_MASTER_NOT_OPEN_YET`           | 当前动作需要主设备open成功, 但这尚未完成
| **04**  | `-0x0904`   | `STATE_DEV_MODEL_MASTER_NOT_CONNECT_YET`        | 当前动作需要主设备连接服务端成功, 但这尚未完成
| **05**  | `-0x0905`   | `STATE_DEV_MODEL_DEVICE_ALREADY_EXIST`          | 当前请求创建的设备已经存在, 不必再次创建
| **06**  | `-0x0906`   | `STATE_DEV_MODEL_DEVICE_NOT_FOUND`              | 当前请求对应的设备未找到, 无法对其操作
| **07**  | `-0x0907`   | `STATE_DEV_MODEL_SUBD_NOT_DELETEABLE`           | 当前请求对应的设备不可删除, 无法对其进行删除操作
| **08**  | `-0x0908`   | `STATE_DEV_MODEL_SUBD_NOT_LOGIN`                | 当前请求对应的设备不在登入状态, 无法对其进行登出操作
| **09**  | `-0x0909`   | `STATE_DEV_MODEL_INTERNAL_ERROR`                | 物模型/子设备管理模块发生内部错误
| **10**  | `-0x090A`   | `STATE_DEV_MODEL_INTERNAL_MQTT_DUP_INIT`        | 物模型/子设备管理模块发现MQTT连接已经建立, 跳过再次建连动作
| **11**  | `-0x090B`   | `STATE_DEV_MODEL_INTERNAL_MQTT_NOT_INIT_YET`    | 物模型/子设备管理模块发生MQTT连接未建立的内部错误
| **12**  | `-0x090C`   | `STATE_DEV_MODEL_CM_OPEN_FAILED`                | 物模型模块中发生连接抽象层初始化失败
| **13**  | `-0x090D`   | `STATE_DEV_MODEL_CM_FD_NOT_FOUND`               | 物模型模块中未找到连接抽象层的fd
| **14**  | `-0x090E`   | `STATE_DEV_MODEL_CM_FD_ERROR`                   | 物模型/子设备管理模块发生fd有关的内部错误
| **15**  | `-0x090F`   | `STATE_DEV_MODEL_MQTT_CONNECT_FAILED`           | 物模型模块中发生MQTT连接建立失败
| **16**  | `-0x0910`   | `STATE_DEV_MODEL_RECV_UNEXP_MQTT_PUB`           | 物模型模块中接收到来自服务端的MQTT下推消息, 但报文内容不符合预期
| **17**  | `-0x0911`   | `STATE_DEV_MODEL_WRONG_JSON_FORMAT`             | 物模型模块中接收到来自服务端的MQTT下推消息, 但报文内容不是合理的JSON格式
| **18**  | `-0x0912`   | `STATE_DEV_MODEL_GET_JSON_ITEM_FAILED`          | 物模型模块中从JSON报文解析KV对失败
| **19**  | `-0x0913`   | `STATE_DEV_MODEL_SERVICE_CTX_NOT_EXIST`         | 物模型模块中发送服务回应报文给服务端时, 没有找到对应的服务端请求上下文
| **20**  | `-0x0914`   | `STATE_DEV_MODEL_OTA_NOT_ENABLED`               | 物模型模块中发现OTA功能未开启
| **21**  | `-0x0915`   | `STATE_DEV_MODEL_OTA_NOT_INITED`                | 物模型模块中发现OTA功能未能正确初始化
| **22**  | `-0x0916`   | `STATE_DEV_MODEL_OTA_INIT_FAILED`               | 物模型模块中发现对OTA功能初始化失败
| **23**  | `-0x0917`   | `STATE_DEV_MODEL_OTA_STILL_IN_PROGRESS`         | 子设备OTA时, 由于当前仍有其它子设备OTA尚未完成, 而跳过当前操作
| **24**  | `-0x0918`   | `STATE_DEV_MODEL_OTA_IMAGE_CHECK_FAILED`        | 物模型模块中发现OTA下载的固件, 未能通过完整性校验
| **25**  | `-0x0919`   | `STATE_DEV_MODEL_OTA_TYPE_ERROR`                | 物模型模块中发现OTA操作的类型既不是FOTA, 又不是COTA
| **26**  | `-0x091A`   | `STATE_DEV_MODEL_OTA_FETCH_FAILED`              | 物模型模块中从云端获取下载内容未达预期
| **27**  | `-0x091B`   | `STATE_DEV_MODEL_ALCS_CONSTRUCT_FAILED`         | 本地控制功能初始化失败
| **28**  | `-0x091C`   | `STATE_DEV_MODEL_GATEWAY_NOT_ENABLED`           | SDK当前未被配置为打开子设备管理/网关功能
| **29**  | `-0x091D`   | `STATE_DEV_MODEL_RRPCID_TOO_LONG`               | 物模型模块中发现RRPC请求的ID太长, 超过了可处理的最大长度
| **30**  | `-0x091E`   | `STATE_DEV_MODEL_IN_RAWDATA_SOLO`               | 物模型模块已被配置为单品透传模式, 无法处理JSON报文
| **31**  | `-0x091F`   | `STATE_DEV_MODEL_DUP_UPSTREAM_MSG`              | 物模型模块中发现重复的上行报文请求
| **32**  | `-0x0920`   | `STATE_DEV_MODEL_UPSTREAM_REC_NOT_FOUND`        | 物模型模块处理下行报文时, 未找到与之对应的上行报文记录
| **33**  | `-0x0921`   | `STATE_DEV_MODEL_REFUSED_BY_CLOUD`              | 物模型/子设备管理模块中得到来自云端对请求的拒绝报文
| **34**  | `-0x0922`   | `STATE_DEV_MODEL_INVALID_DM_OPTION`             | 物模型模块中发现用于获取内部信息的option不合理
| **35**  | `-0x0923`   | `STATE_DEV_MODEL_URL_SPLIT_FAILED`              | 物模型模块在处理云端回复报文时, 从中解析URL失败
| **36**  | `-0x0924`   | `STATE_DEV_MODEL_ALINK_MSG_PARSE_FAILED`        | 解析Alink上行报文或下行报文失败
| **37**  | `-0x0925`   | `STATE_DEV_MODEL_LOG_REPORT_ERROR`              | 上传设备日志到云端日志接口失败
| **38**  | `-0x0926`   | `STATE_DEV_MODEL_IN_AUTOSUB_MODE`               | 物模型模块被配置为免订阅模式, 跳过当前的系统Topic订阅
| **39**  | `-0x0927`   | `STATE_DEV_MODEL_RX_CLOUD_MESSAGE`              | 物模型/子设备管理模块中, 接收到来自云端的下推报文
| **40**  | `-0x0928`   | `STATE_DEV_MODEL_TX_CLOUD_MESSAGE`              | 物模型/子设备管理模块中, 正在发送去往云端的上行报文
| **41**  | `-0x0929`   | `STATE_DEV_MODLE_RX_LOCAL_MESSAGE`              | 物模型/子设备管理模块中, 接收到来自本地控制设备的报文
| **42**  | `-0x092A`   | `STATE_DEV_MODLE_TX_LOCAL_MESSAGE`              | 物模型/子设备管理模块中, 正在发送去往本地控制设备的报文
| **43**  | `-0x092B`   | `STATE_DEV_MODLE_ALCS_CONTROL`                  | 物模型/子设备管理模块中, 正在处理本地控制协议中的控制类消息
| **44**  | `-0x092C`   | `STATE_DEV_MODEL_MSGQ_OPERATION`                | 物模型模块中的消息队列正在被插入或删除元素
| **45**  | `-0x092D`   | `STATE_DEV_MODEL_MSGQ_FULL`                     | 物模型模块中的消息队列已满
| **46**  | `-0x092E`   | `STATE_DEV_MODEL_MSGQ_EMPTY`                    | 物模型模块中的消息队列已空
| **47**  | `-0x092F`   | `STATE_DEV_MODEL_CTX_LIST_INSERT`               | 物模型模块中的会话上下文链表正在被插入新元素
| **48**  | `-0x0930`   | `STATE_DEV_MODEL_CTX_LIST_REMOVE`               | 物模型模块中的会话上下文链表正在被删除某元素
| **49**  | `-0x0931`   | `STATE_DEV_MODEL_CTX_LIST_FADEOUT`              | 物模型模块中的会话上下文元素因为闲置时间太长而被老化删除
| **50**  | `-0x0932`   | `STATE_DEV_MODEL_CTX_LIST_FULL`                 | 物模型模块中的会话上下文链表已满
| **51**  | `-0x0933`   | `STATE_DEV_MODEL_CTX_LIST_EMPTY`                | 物模型模块中的会话上下文链表已空
| **52**  | `-0x0934`   | `STATE_DEV_MODLE_LOG_REPORT_STOP`               | 物模型模块中的日志上云已被停止
| **53**  | `-0x0935`   | `STATE_DEV_MODLE_LOG_REPORT_SWITCH`             | 物模型模块中的日志上云已被调节
| **54**  | `-0x0936`   | `STATE_DEV_MODLE_LOG_REPORT_SEND`               | 物模型模块中的日志上云消息已被发送
| **55**  | `-0x0937`   | `STATE_DEV_MODEL_SYNC_REQ_LIST`                 | 物模型模块中的同步请求队列正在被操作
| **56**  | `-0x0938`   | `STATE_DEV_MODEL_ALINK_PROT_EVENT`              | 物模型模块正在处理Alink协议相关的报文
| **57**  | `-0x0939`   | `STATE_DEV_MODEL_ALCS_SEND_FAILED`              | 物模型模块中, 在局域网内发送本地控制相关报文失败
| **58**  | `-0x093A`   | `STATE_DEV_MODEL_INVALID_ALINK_PAYLOAD`         | 接收到非法的alink协议报文
| **59**  | `-0x093B`   | `STATE_DEV_MODEL_INVALID_ALINK_TOPIC`           | 收到不符合当前场景的alink协议topic
| **60**  | `-0x093C`   | `STATE_DEV_MODEL_YIELD_STOPPED`                 | IOT_Linkkit_Yield 已停止运行
| **61**  | `-0x093D`   | `STATE_DEV_MODEL_YIELD_RUNNING`                 | IOT_Linkkit_Yield 仍在运行

