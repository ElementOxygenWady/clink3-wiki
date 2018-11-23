# <a name="目录">目录</a>
+ [目录](#目录)
+ [V2.3.0](#V2.3.0)
    * [make.settings 文件](#make.settings 文件)
    * [全局函数接口](#全局函数接口)
    * [MQTT函数接口](#MQTT函数接口)
    * [OTA函数接口](#OTA函数接口)
    * [Linkkit函数接口](#Linkkit函数接口)
        - [新增以下接口用于完成物模型管理功能](#新增以下接口用于完成物模型管理功能)
        - [去掉的接口](#去掉的接口)
    * [HAL函数接口](#HAL函数接口)

# <a name="目录">目录</a>

* [V2.3.0](#V2.3.0)
    * [make.settings 文件](#make.settings 文件)
    * [全局函数接口](#全局函数接口)
    * [MQTT函数接口](#MQTT函数接口)
    * [OTA函数接口](#OTA函数接口)
    * [Linkkit函数接口](#Linkkit函数接口)
    * [HAL函数接口](#HAL函数接口)

> 本页面记录 Link Kit SDK 在__所有用户可感知方面的变化内容__, 包括但不限于函数接口, 目录文件, 以及配置方式等

# <a name="V2.3.0">V2.3.0</a>
> V2.3.0版本在2018年11月发布, 相对2018年8月31日发布的V2.2.1版本有如下更动

## <a name="make.settings 文件">make.settings 文件</a>

* 对使用 `Ubuntu16.04` 的 `64位` 主机开发的用户, 可以用 `make menuconfig` 命令图形化编辑 `make.settings` 文件
* `FEATURE_XXX` 系列的选项名字修改

| 曾用名 | 新版名 | 说明 |
| :--- | :--- | :--- |
| `FEATURE_SDK_ENHANCE` | `FEATURE_DEVICE_MODEL_ENABLED` | 物模型管理(Alink JSON协议)能力开关 |
| `FEATURE_ENHANCED_GATEWAY` | `FEATURE_DEVICE_MODEL_GATEWAY` | 物模型管理中的网关能力开关 |
| `FEATURE_WIFI_AWSS_ENABLED` | `FEATURE_WIFI_PROVISION_ENABLED` | WiFi配网能力开关 |

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
    ```
    int IOT_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len);
    ```
* 新增接口 `IOT_MQTT_Subscribe_Sync()`, 可以阻塞式的进行同步订阅, 订阅成功之前函数并不返回
    ```
    int IOT_MQTT_Subscribe_Sync(void *handle,
                                const char *topic_filter,
                                iotx_mqtt_qos_t qos,
                                iotx_mqtt_event_handle_func_fpt topic_handle_func,
                                void *pcontext,
                                int timeout_ms);
    ```
* 优化接口 `IOT_MQTT_Subscribe()`, 可以在MQTT连接尚未建立的时候就提前做离线的订阅, C-SDK会在建连成功后立即发送订阅报文

## <a name="OTA函数接口">OTA函数接口</a>

* 由于MQTT函数接口的第1个参数`handle`可以传`NULL`, 也影响到基础版OTA函数接口 `IOT_OTA_Init` 的第3个参数可以接受`NULL`传参, 表示使用默认连接
    ```
    void *IOT_OTA_Init(const char *product_key, const char *device_name, void *ch_signal);
    ```

## <a name="Linkkit函数接口">Linkkit函数接口</a>

### <a name="新增以下接口用于完成物模型管理功能">新增以下接口用于完成物模型管理功能</a>

| 函数名 | 说明 |
| :--- | :--- |
| [IOT\_Linkkit\_Open](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Open) | 创建本地资源, 在进行网络报文交互之前, 必须先调用此接口, 得到一个会话的句柄 |
| [IOT\_Linkkit\_Connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Connect) | 对主设备/网关来说, 将会建立设备与云端的通信. 对于子设备来说, 将向云端注册该子设备(若需要), 并添加主子设备拓扑关系 |
| [IOT\_Linkkit\_Yield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Yield) | 若SDK占有独立线程, 该函数内容为空, 否则表示将CPU交给SDK让其接收网络报文并将消息分发到用户的回调函数中 |
| [IOT\_Linkkit\_Close](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Close) | 若入参中的会话句柄为主设备/网关, 则关闭网络连接并释放SDK为该会话所占用的所有资源 |
| [IOT\_Linkkit\_TriggerEvent](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_TriggerEvent) | 向云端发送__事件报文__, 如错误码, 异常告警等 |
| [IOT\_Linkkit\_Report](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Report) | 向云端发送__没有云端业务数据下发的上行报文__, 包括属性值/设备标签/二进制透传数据/子设备管理等各种报文 |
| [IOT\_Linkkit\_Query](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Query) | 向云端发送__存在云端业务数据下发的查询报文__, 包括OTA状态查询/OTA固件下载/子设备拓扑查询/NTP时间查询等各种报文 |
| [IOT\_RegisterCallback](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_RegisterCallback) | 对SDK注册事件回调函数, 如云端连接成功/失败, 有属性设置/服务请求到达, 子设备管理报文答复等 |
| [IOT\_Ioctl](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Ioctl) | 对SDK进行各种参数运行时设置和获取, 以及运行状态的信息获取等, 实参可以是任何数据类型 |

### <a name="去掉的接口">去掉的接口</a>

<font color="#FF0000">**旧版的 `linkkit_xxx()` 和 `linkkit_gateway_xxx()` 接口作为编译时可选使能, 缺省不编译**</font>

<br>
<em>注1: 旧版的linkkit接口仍然可以通过在 </em><em><code>make.settings</code></em><em> 文件中的如下语句使能, 此时新版接口消失, 旧版接口重新出现</em>

```
FEATURE_DEPRECATED_LINKKIT=y
```

<em>注2: 为了减少设备端内存消耗，当用户调用SDK的API上传属性变化通知、事件通知到云端时SDK不再检查数据格式的正确性，因此用户不再需要从云端导出TSL的json文件转换为C的字符串后放到C代码中</em>

## <a name="HAL函数接口">HAL函数接口</a>

* 删除了和已有接口 `HAL_Reboot` 重复的 `HAL_Sys_reboot`

