# <a name="目录">目录</a>
+ [功能说明](#功能说明)
+ [例子程序讲解](#例子程序讲解)
+ [功能API接口](#功能API接口)
+ [需要实现的HAL接口](#需要实现的HAL接口)
+ [常见问题](#常见问题)
    * [调节断线重连敏感度](#调节断线重连敏感度)
    * [断线的通知和处理](#断线的通知和处理)


# <a name="功能说明">功能说明</a>

C-SDK提供了一份基于开源方案深度优化的阿里巴巴自有实现MQTT, 并和阿里的云端做了很多性能和稳定性方面的优化, 推荐使用阿里自有的MQTT方案连接云平台

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/brief_mqtt_connect.png" width="800" hegiht="600" align=center />

如上图演示的流程

+ 通过 `IOT_MQTT_Construct()` 接口调用, 成功的话可以建立设备和云平台之间的长连接
    - 这个接口是用来向云平台发起连接请求的, 其参数中需要的"签名信息"可以通过 `dev_sign` 功能中的 `IOT_Sign_MQTT()` 获得
    - 连接成功的话, 会返回一个 `handle` 参数, 这就是连接的句柄, 可用作之后所有MQTT网络接口的入参

+ 通过 `IOT_MQTT_Subscribe()` 接口调用, 可以向云平台表达设备将接受哪些Topic上的报文下发

+ 然后进入业务主循环
    + 通过 `IOT_MQTT_Publish()` 或 `IOT_MQTT_Publish_Simple()`, 可将消息上报到云端
    + 通过 `IOT_MQTT_Yield()`, 可收取云端下发的消息, 这些指令通过 `IOT_MQTT_Subscribe()` 接口订阅的 Topic 到达

# <a name="例子程序讲解">例子程序讲解</a>

MQTT上云的例子程序在 `src/mqtt/examples/mqtt_example.c`, 在"快速体验"一章中已对其进行过逐段的讲解

请参考 [快速体验](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Quick_Start) 一章中的 "以MQTT Topic编程方式接入设备" 一节
---

# <a name="功能API接口">功能API接口</a>

| 函数名                                                                                                                                                          | 说明
|-----------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------
| [IOT_MQTT_CheckStateNormal](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_CheckStateNormal)   | MQTT连接后, 调用此函数检查长连接是否正常
| [IOT_MQTT_Construct](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Construct)                 | MQTT实例的构造函数, 入参为`iotx_mqtt_param_t`结构体, 连接MQTT服务器, 并返回被创建句柄
| [IOT_MQTT_Destroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Destroy)                     | MQTT实例的摧毁函数, 入参为 [IOT_MQTT_Construct](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Construct) 创建的句柄
| [IOT_MQTT_Publish](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Publish)                     | MQTT会话阶段, 组织一个完整的`MQTT Publish`报文, 向服务端发送消息发布报文
| [IOT_MQTT_Publish_Simple](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Publish_Simple)       | MQTT会话阶段, 组织一个完整的`MQTT Publish`报文, 向服务端发送消息发布报文,参数中不含结构体等复杂数据类型
| [IOT_MQTT_Subscribe](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Subscribe)                 | MQTT会话阶段, 组织一个完整的`MQTT Subscribe`报文, 向服务端发送订阅请求
| [IOT_MQTT_Subscribe_Sync](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Subscribe_Sync)       | MQTT会话阶段, 组织一个完整的`MQTT Subscribe`报文, 向服务端发送订阅请求,并等待应答
| [IOT_MQTT_Unsubscribe](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Unsubscribe)             | MQTT会话阶段, 组织一个完整的`MQTT UnSubscribe`报文, 向服务端发送取消订阅请求
| [IOT_MQTT_Yield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Yield)                         | MQTT会话阶段, MQTT主循环函数, 内含了心跳的维持, 服务器下行报文的收取等

# <a name="需要实现的HAL接口">需要实现的HAL接口</a>
**以下函数为可选实现, 如果希望SDK提供MQTT通道功能, 则需要用户对接**

| 函数名                                                                                                                                          | 说明
|-------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------
| [HAL_UptimeMs](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_UptimeMs)            | 返回设备加电以来到当前时间点过去的毫秒数
| [HAL_SleepMs](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_SleepMs)              | 按照入参指定的毫秒数睡眠相应时间
| [HAL_SSL_Destroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_SSL_Destroy)       | 销毁一个TLS连接, 用于MQTT功能, 加密连接的情况
| [HAL_SSL_Establish](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_SSL_Establish)   | 建立一个TLS连接, 用于MQTT功能, 加密连接的情况
| [HAL_SSL_Read](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_SSL_Read)             | 从一个TLS连接中读数据, 用于MQTT功能, 加密连接的情况
| [HAL_SSL_Write](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_SSL_Write)           | 向一个TLS连接中写数据, 用于MQTT功能, 加密连接的情况
| [HAL_TCP_Destroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_TCP_Destroy)       | 销毁一个TLS连接, 用于MQTT功能
| [HAL_TCP_Establish](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_TCP_Establish)   | 建立一个TCP连接, 包含了域名解析的动作和TCP连接的建立
| [HAL_TCP_Read](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_TCP_Read)             | 在指定时间内, 从TCP连接读取流数据, 并返回读到的字节数
| [HAL_TCP_Write](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_TCP_Write)           | 在指定时间内, 向TCP连接发送流数据, 并返回发送的字节数

注: SDK提供的用户API接口都列在 `src/mqtt/mqtt_api.h`, 可能需要对接的HAL函数都列在 `src/mqtt/mqtt_wrapper.h`, 以代码为准
---

# <a name="常见问题">常见问题</a>

## <a name="调节断线重连敏感度">调节断线重连敏感度</a>

SDK的断网探测机制
---
> SDK并未要求用户去发现网络断开的情况, 因为它会通过和服务器之间的心跳来自动探测网络情况并在认为必要时做重连动作

+ MQTT长连接建立之后, SDK会每过一个心跳周期向服务端发送ping报文并等待回应
+ 用户调用 `IOT_MQTT_Yield()`, 或者 `IOT_Linkkit_Yield()` 时, SDK会去网络上收取报文
    + 如果有收到从服务端对上次ping的回应, 则认为网络仍然是通的
    + 如果有收到从服务端下推的其它MQTT数据, 也会认为网络仍然是通的

+ 到了下一个心跳周期, 而上一次的心跳回应仍未收到, 并且这期间也没有收到其它数据时, 则计数器自增
+ 如果 **计数器自增超过了容忍的阈值, 则SDK会认为网络断开**, 它会通过回调函数告知用户, 并自动发起重连

可调节的变量
---
> 依据上面介绍的断网探测机制, SDK对断线的判断敏感度可由以下变量调节

| 变量名                                  | 位置                                | 含义                                                    | 调节说明
|-----------------------------------------|-------------------------------------|---------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------
| `IOTX_DM_CLIENT_KEEPALIVE_INTERVAL_MS`  | `src/dev_model/iotx_dm_config.h`    | 使用 `IOT_Linkkit_XXX()` 接口时SDK实际上的心跳间隔      | 相当于下面的 `CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN`, 只不过影响的接口不同, 默认为`30`, 代表半分钟, 改的越小, 心跳可以越频繁
| `CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN`    | `src/mqtt/mqtt_api.h`               | 使用 `IOT_MQTT_XXX()` 接口时可设置的最小心跳间隔        | 默认为`30`, 代表半分钟, 改的越小, 心跳可以越频繁, 对应在`2.3.0`上是改 `KEEP_ALIVE_INTERVAL_DEFAULT_MIN`
| `IOTX_MC_KEEPALIVE_PROBE_MAX`           | `src/mqtt/impl/iotx_mqtt_config.h`  | 可容忍心跳丢失的次数上限, 也即上文所说的阈值            | 默认值为`1`, 代表最快`2`个心跳周期发现断网, 改的越小, 对断网判定越敏感
| `mqtt_params.keepalive_interval_ms`     | `.../mqtt_example.c` 或业务代码     | 使用 `IOT_MQTT_XXX()` 接口时用户设置的心跳间隔          | 调用 `IOT_MQTT_Construct()` 的时候用户传入的参数结构体中的一个成员, 不传参时, SDK默认用`60`秒

**注意: 对断网的探测不是调节的越敏感越好. 敏感度高了虽然可以缩短重连时间, 但在网络不佳发生抖动时也更容易做出不必要的主动重连动作, 造成掉线次数增多**

## <a name="断线的通知和处理">断线的通知和处理</a>

+ 接口 `IOT_RegisterCallback()` 可让用户向SDK表达自己关心的事件, 并指定事件发生时应该执行的回调函数

        IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);

+ 例如以上代码, 可使SDK在感知断线发生时, 调用用户传入的回调函数 `user_disconnected_event_handler()`
+ **注意: 回调函数只是告诉用户断线了, 用户不必在其中又调用 `IOT_Linkkit_Connect()` 去做主动重连, SDK自己会有重连动作, 在用户回调中主动重连, 反倒会造成循环卡顿**
