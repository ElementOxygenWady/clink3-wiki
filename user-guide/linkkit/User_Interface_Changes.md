# <a name="目录">目录</a>
+ [V3.0.1](#V3.0.1)
    * [新增支持源代码方式移植](#新增支持源代码方式移植)
    * [目录结构](#目录结构)
    * [API接口和HAL接口的呈现方式](#API接口和HAL接口的呈现方式)
    * [make.settings 文件](#make.settings 文件)
    * [MQTT函数接口](#MQTT函数接口)
    * [新增的功能点和接口](#新增的功能点和接口)
    * [去掉的功能](#去掉的功能)
+ [V2.3.0](#V2.3.0)
    * [make.settings 文件](#make.settings 文件)
    * [全局函数接口](#全局函数接口)
    * [MQTT函数接口](#MQTT函数接口)
    * [OTA函数接口](#OTA函数接口)
    * [Linkkit函数接口](#Linkkit函数接口)
        - [新增以下接口用于完成物模型管理功能](#新增以下接口用于完成物模型管理功能)
        - [去掉的接口](#去掉的接口)
    * [HAL函数接口](#HAL函数接口)


> 本页面记录 Link Kit SDK 在__所有用户可感知方面的变化内容__, 包括但不限于函数接口, 目录文件, 以及配置方式等

# <a name="V3.0.1">V3.0.1</a>
> V3.0.1版本在2019年03月发布, 相对2018年11月发布的V2.3.0版本有如下更动

## <a name="新增支持源代码方式移植">新增支持源代码方式移植</a>

V3.0.1版本最大的变化, 在于不强制用户使用SDK自带的编译系统, 而是可以按需抽取部分功能点对应的C文件, 以自己喜欢的任何方式编译成二进制
---

+ 适应用户不论是用安装了 `Ubuntu16.04 64位` 的主机还是用安装了 `Windows XP`, `Windows7`, `Windows10` 的主机进行开发
+ 用户都可以通过 `make menuconfig` 命令(Ubuntu)或者通过点击 `config.bat` 脚本(Windows)来选择功能点
+ 选好功能点之后, 都可以通过 `extract.sh` 命令(Ubuntu)或者是通过点击 `extract.bat` 脚本(Windows) 来抽取需要的源文件
+ 有了这些源文件, 不论用户在什么主机上用什么方式开发, 都可以方便的将这些文件加入自己的工程中, 仍用自己熟悉的源码编译方式, 就可以完成移植了
+ 在这个过程中, **所有需要用户自行实现的函数接口也都已被自动整理到一个单独的文件中, 位于 `output/eng/wrappers/wrapper.c`, 逐个填写即可**

这项增强颠覆性的解决了过去如果用户不不能很顺利的交叉编译SDK, 就会卡住无法继续的问题, 大幅增强SDK的可移植性和易用性
---

由此也带来了一系列SDK文件布局和设计方式的变化, 对移植方式的变化详见文档: [不使用SDK自带编译系统时的移植示例](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Extract_Example)

---
*注: 原有的编译系统和使用方式仍然存在, 详情可见文档: [使用SDK自带编译系统时的移植示例](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Example)*

## <a name="目录结构">目录结构</a>

+ 为增强易用性, V3.0.1版本的SDK采用了扁平化的目录结构摆放源码, 例如 `src/mqtt` 下集中摆放所有MQTT上云功能点的H文件和C文件, 所有的 `src/xxxx` 目录都是内聚的
+ 所有源文件的摆放和SDK的功能点形成一一对应的关系, 除了表示基础的 `src/infra` 目录, 现在所有的 `src/xxx` 都各自对应一个SDK的功能, 有几个 `src/xxx`, 就有几个功能点, 例如

    - `src/dynamic_register`: 摆放一型一密/动态注册相关的所有源文件
    - `src/dev_sign`: 摆放设备签名相关的所有源文件
    - `src/mqtt`: 摆放MQTT连云相关的所有源文件
    - `src/ota`: 摆放OTA固件下载相关的所有源文件
    - `src/dev_model`: 摆放物模型管理/子设备管理相关的所有源文件

---
*对于新版SDK的目录摆放全面介绍, 可在[SDK概述](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/SDK_Overview)的 "主要目录及文件一览" 一节看到*

## <a name="API接口和HAL接口的呈现方式">API接口和HAL接口的呈现方式</a>

> 过去SDK只有2个全局性的头文件 `iot_import.h` 和 `iot_export.h`, 分别列出SDK对外界需要依赖的 `HAL_XXX()` 底层接口和列出SDK对外界提供的 `IOT_XXX` 用户接口
>
> 我们得到部分用户的反馈, `iot_import.h` 中列出的100多个HAL接口使他们误以为全都需要对接, `iot_export.h` 中列出的200多个API接口使他们误以为全部需要学习使用, 感到负担很重

因此, 我们去掉了这种设计
---

+ SDK不再有一个整体性的 `iot_import.h` 和 `iot_export.h`
+ 也不再有顶级目录下的 `include`
+ 而是将源文件严格按所归属的功能点目录分散排布, **我们尽量避免让用户看到他其实不需要关注的部分**

现在
---

+ 使用V3.0.1版本的SDK, 用户首先要了解SDK提供了哪些功能点(例如MQTT连云, OTA下载等), 并确定自己需要这些功能中的哪一些, 并图形化的配置和选中它们

+ 其次, 通过 **抽取工具** (在Ubuntu主机上是 `extract.sh` 脚本, 在Windows主机上是 `extract.bat`) 得到 `output` 目录下这些功能对应的源文件

+ 接着便可以使用, 在使用环节, 我们对SDK所有外部接口的呈现方式遵循如下的设计原则

    - `output/eng/xxx` 就对应名为 `xxx` 的功能点, 它们从 `src/xxx` 整理而来
    - `output/eng/xxx/xxx_api.h` 就对应名为 `xxx` 的功能点提供的API用户接口, 它们从 `src/xxx/xxx_api.h` 整理而来
    - `output/eng/wrappers/wrapper.c` 自动产生并列出所有需要用户根据嵌入式目标平台实现的 `HAL_XXX()` 底层接口

+ 也就是说

    - 只有您需要的功能点, 才需要去看 `src/xxx/xxx_api.h` 或者 `output/eng/xxx/xxx_api.h`, 所以确保用户不会看到多余的API接口声明
    - 所有您需要实现的函数, 都列在 `output/eng/wrappers/wrapper.c` 中, **不需对接的不会包含其中**, 所以确保用户不会看到多余的HAL接口定义

---
举例来说, 在最简单的情形下, 用户只需要SDK中计算MQTT连云签名的能力, 由于这个能力由 `src/dev_sign` 或者 `output/eng/dev_sign` 提供

+ 用户只会看到1个API接口, 列在 `dev_sign_api.h` 中

+ 用户不会看到任何HAL接口, 甚至也看不到 `wrapper.c`, 因为这个功能是零依赖的, 不需要用户实现任何C函数即可使用

## <a name="make.settings 文件">make.settings 文件</a>

* 新增对使用 `Windows XP`, `Windows 7` 或者 `Windows 10` 主机做开发的用户, 也可以用 `config.bat` 工具图形化的配置所需要的功能, 不一定需要 Linux 主机

* `FEATURE_XXX` 系列的选项名字修改

| 曾用名                              | 新版名                          | 说明
|-------------------------------------|---------------------------------|-------------------------------------
| `FEATURE_AWSS_SUPPORT_PHONEASAP`    | `FEATURE_AWSS_SUPPORT_AHA`      | WiFi配网功能中的手机热点配网模式
| `FEATURE_AWSS_SUPPORT_ROUTER`       | `FEATURE_AWSS_SUPPORT_ADHA`     | WiFi配网功能中的路由器配网模式

* 新增以下选项

    + `FEATURE_AWSS_SUPPORT_SMARTCONFIG_WPS`: WiFi配网功能中的点对点配网模式
    + `FEATURE_AWSS_SUPPORT_DEV_AP`: WiFi配网共中的设备热点配网模式

## <a name="MQTT函数接口">MQTT函数接口</a>

+ 不再需要调用 `IOT_SetupConnInfo()`, 用户建立MQTT连接只需要知道1个接口 `IOT_MQTT_Construct()`

+ 不再需要准备入参, 调用 `IOT_MQTT_Construct()` 的时候绝大多数连接参数都可以不填写, SDK会自动补上默认值而不是报错返回, 详见 [快速体验](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Quick_Start)

## <a name="新增的功能点和接口">新增的功能点和接口</a>

+ 新增 **设备签名** 功能点, 是SDK的最简形态

    - 这个功能点只有1个API, 是 `IOT_Sign_MQTT()`
    - 详见文档: [设备签名编程指南](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Dev_Sign)

+ 新增 **一型一密** 功能点, 用于所有设备不必烧录不同三元组的场景

    - 这个功能点只有1个API, 是 `IOT_Dynamic_Register()`
    - 详见文档: [一型一密编程指南](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Dynamic_Register)

+ 新增 **设备重置** 功能点, 可用于设备告知云端解除绑定关系, 解除子设备和网关对应关系的场景

    - 这个功能点只有1个API, 是 `IOT_DevReset_Report()`
    - 详见文档: [设备重置编程指南](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/DevReset_Prog)

+ 新增 **HTTP2文件上传** 功能点, 可用于流式上传大文件到云端的场景

    + 这个功能点只有3个API, 是 `IOT_HTTP2_UploadFile_Connect()`, `IOT_HTTP2_UploadFile_Request()` 和 `IOT_HTTP2_UploadFile_Disconnect()`
    + 详见文档: [HTTP2文件上传编程指南](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/H2_FileUpload)

## <a name="去掉的功能">去掉的功能</a>

+ 由于用户已可使用源文件方式移植或者GNU Make方式移植, 我们去掉了基于 `cmake` 的编译方式支持

# <a name="V2.3.0">V2.3.0</a>
> V2.3.0版本在2018年11月发布, 相对2018年08月31日发布的V2.2.1版本有如下更动

## <a name="make.settings 文件">make.settings 文件</a>

* 对使用 `Ubuntu16.04` 的 `64位` 主机开发的用户, 可以用 `make menuconfig` 命令图形化编辑 `make.settings` 文件
* `FEATURE_XXX` 系列的选项名字修改

| 曾用名                          | 新版名                              | 说明
|---------------------------------|-------------------------------------|-------------------------------------
| `FEATURE_SDK_ENHANCE`           | `FEATURE_DEVICE_MODEL_ENABLED`      | 物模型管理(Alink JSON协议)能力开关
| `FEATURE_ENHANCED_GATEWAY`      | `FEATURE_DEVICE_MODEL_GATEWAY`      | 物模型管理中的网关能力开关
| `FEATURE_WIFI_AWSS_ENABLED`     | `FEATURE_WIFI_PROVISION_ENABLED`    | WiFi配网能力开关

* 新增以下选项

    * `FEATURE_DEVICE_BIND_ENABLED`: 设备绑定功能的开关, 只有连接飞燕平台的客户需要关注并打开这个开关, 用于使能公版app控制设备
    * `FEATURE_ALCS_CLIENT_ENABLED`: 本地通信中的客户端部分功能开关, 只有边缘网关这类需要控制其他IoT设备的设备才需要打开
    * `FEATURE_ALCS_SERVER_ENABLED`: 本地通信中的服务端部分功能开关, 需要接受手机app或者边缘网关局域网控制的设备需要打开
    * `FEATURE_AWSS_SUPPORT_SMARTCONFIG`: WiFi配网中的一键配网模式
    * `FEATURE_AWSS_SUPPORT_ZEROCONFIG`: WiFi配网中的零配模式
    * `FEATURE_AWSS_SUPPORT_ROUTER`: WiFi配网中的路由器配网模式
    * `FEATURE_AWSS_SUPPORT_PHONEASAP`: WiFi配网中的手机热点配网模式
* 删除以下选项
    * `FEATURE_COAP_DTLS_SUPPORT`: CoAP上云是否经过DTLS加密, 由于总是使能, 不再需要用户配置

## <a name="全局函数接口">全局函数接口</a>

* 删除 `IOT_OpenLog()` 和 `IOT_CloseLog()`, 用户可以通过 `IOT_SetLogLevel()` 来控制SDK打印日志的详细程度或者不打印
* 删除 `IOT_LOG_EMERG` 这个日志等级, 改成 `IOT_LOG_NONE`, 用于 `IOT_SetLogLevel(IOT_LOG_NONE)`, 表达日志功能的整体关闭
* 删除 `IOT_Linkkit_Ioctl()`, 原有功能合并到 `IOT_Ioctl()`中, 所有的参数选项可查看 `iotx_ioctl_option_t` 枚举定义
* 增强 `IOT_Ioctl()`, 新选项 `IOTX_IOCTL_SET_MQTT_DOMAIN` 和 `IOTX_IOCTL_SET_HTTP_DOMAIN` 可用于让客户传入自定义连接的MQTT/HTTP域名, 用于海外新站点

## <a name="MQTT函数接口">MQTT函数接口</a>

* 原有基础版接口及新增接口中, 第1个参数`handle`都可以传`NULL`, 表示选取默认参数创建连接或使用默认连接, 包括
    * `IOT_MQTT_Construct`
    * `IOT_MQTT_Destroy`
    * `IOT_MQTT_Yield`
    * `IOT_MQTT_LogPost`
    * `IOT_MQTT_CheckStateNormal`
    * `IOT_MQTT_Subscribe`
    * `IOT_MQTT_Unsubscribe`
    * `IOT_MQTT_Publish`
    * `IOT_MQTT_Subscribe_Sync`
    * `IOT_MQTT_Publish_Simple`
* 删除结构体 `iotx_mqtt_param_t` 中的 `pwrite_buf` 成员和 `pread_buf` 成员
* 新增接口 `IOT_MQTT_Publish_Simple()`, 可以传入字符串类型的参数表达topic和payload

        int IOT_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len);

* 新增接口 `IOT_MQTT_Subscribe_Sync()`, 可以阻塞式的进行同步订阅, 订阅成功之前函数并不返回

        int IOT_MQTT_Subscribe_Sync(void *handle,
                                    const char *topic_filter,
                                    iotx_mqtt_qos_t qos,
                                    iotx_mqtt_event_handle_func_fpt topic_handle_func,
                                    void *pcontext,
                                    int timeout_ms);

* 优化接口 `IOT_MQTT_Subscribe()`, 可以在MQTT连接尚未建立的时候就提前做离线的订阅, C-SDK会在建连成功后立即发送订阅报文

## <a name="OTA函数接口">OTA函数接口</a>

* 由于MQTT函数接口的第1个参数`handle`可以传`NULL`, 也影响到基础版OTA函数接口 `IOT_OTA_Init` 的第3个参数可以接受`NULL`传参, 表示使用默认连接

        void *IOT_OTA_Init(const char *product_key, const char *device_name, void *ch_signal);

## <a name="Linkkit函数接口">Linkkit函数接口</a>

### <a name="新增以下接口用于完成物模型管理功能">新增以下接口用于完成物模型管理功能</a>

| 函数名                                                                                                                                                          | 说明
|-----------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------
| [IOT_Linkkit_Open](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Open)                  | 创建本地资源, 在进行网络报文交互之前, 必须先调用此接口, 得到一个会话的句柄
| [IOT_Linkkit_Connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Connect)            | 对主设备/网关来说, 将会建立设备与云端的通信. 对于子设备来说, 将向云端注册该子设备(若需要), 并添加主子设备拓扑关系
| [IOT_Linkkit_Yield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Yield)                | 若SDK占有独立线程, 该函数内容为空, 否则表示将CPU交给SDK让其接收网络报文并将消息分发到用户的回调函数中
| [IOT_Linkkit_Close](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Close)                | 若入参中的会话句柄为主设备/网关, 则关闭网络连接并释放SDK为该会话所占用的所有资源
| [IOT_Linkkit_TriggerEvent](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_TriggerEvent)  | 向云端发送__事件报文__, 如错误码, 异常告警等
| [IOT_Linkkit_Report](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Report)              | 向云端发送__没有云端业务数据下发的上行报文__, 包括属性值/设备标签/二进制透传数据/子设备管理等各种报文
| [IOT_Linkkit_Query](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Query)                | 向云端发送__存在云端业务数据下发的查询报文__, 包括OTA状态查询/OTA固件下载/子设备拓扑查询/NTP时间查询等各种报文
| [IOT_RegisterCallback](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_RegisterCallback)          | 对SDK注册事件回调函数, 如云端连接成功/失败, 有属性设置/服务请求到达, 子设备管理报文答复等
| [IOT_Ioctl](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Ioctl)                                | 对SDK进行各种参数运行时设置和获取, 以及运行状态的信息获取等, 实参可以是任何数据类型

### <a name="去掉的接口">去掉的接口</a>

+ **旧版的 `linkkit_xxx()` 和 `linkkit_gateway_xxx()` 接口作为编译时可选使能, 缺省不编译**

---
*注1: 旧版的linkkit接口仍然可以通过在 `make.settings` 文件中的如下语句使能, 此时新版接口消失, 旧版接口重新出现*

    FEATURE_DEPRECATED_LINKKIT=y

*注2: 使用新版API上传属性变化通知/事件通知到云端时SDK不再检查数据格式的正确性, 因此用户不再需要从云端导出TSL的json文件转换为C的字符串后放到C代码中*

## <a name="HAL函数接口">HAL函数接口</a>

* 删除了和已有接口 `HAL_Reboot` 重复的 `HAL_Sys_reboot`

