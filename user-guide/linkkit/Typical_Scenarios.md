# <a name="目录">目录</a>
+ [第五章 典型场景示例](#第五章 典型场景示例)
    * [5.1 MQTT站点配置](#5.1 MQTT站点配置)
    * [5.2 单品动态注册/一型一密](#5.2 单品动态注册/一型一密)
        - [动态注册/一型一密的概念](#动态注册/一型一密的概念)
        - [相关API和例程](#相关API和例程)
        - [完整过程示例](#完整过程示例)
            + [基础版示例](#基础版示例)
            + [高级版示例](#高级版示例)
    * [5.3 物模型管理功能](#5.3 物模型管理功能)


# <a name="第五章 典型场景示例">第五章 典型场景示例</a>
## <a name="5.1 MQTT站点配置">5.1 MQTT站点配置</a>
在使用阿里云物联网套件连接阿里云时, 可指定MQTT连接的服务器站点, 配置方法如下:

---

在 `iotx-sdk-c/include/iot_export.h` 中, 枚举类型 `iotx_cloud_region_types_t` 定义了当前可连接的MQTT站点:

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

+ 首先使用 [IOT_Ioctl](#IOT_Ioctl) 的 `IOTX_IOCTL_SET_DOMAIN` 选项, 配合上面的枚举值, 设置要连接的站点
+ 然后使用 [IOT_MQTT_Construct](#IOT_MQTT_Construct) 或者 [IOT_Linkkit_Connect](#IOT_Linkkit_Connect) 来建立设备到阿里云的连接

例如:
---

    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

**注意事项: 如果在阿里云物联网控制台申请的三元组与连接时使用的域名不符合, 连接站点时会出现认证错误(错误码-35)**

*例如: 在阿里云物联网控制台的华东2站点申请的三元组, 在物联网套件中应该连接华东2(上海)站点*

*AOS1.3.2版本连接海外站点*
---
在AOS1.3.2版本中连接到新加坡站点的配置方式有所不同, 新加坡站点会自动为设备分配最近的加速点

只需要修改`example/linkkitapp/linkkitapp.mk`文件, 将默认配置:
```
GLOBAL_DEFINES += MQTT_DIRECT ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD HAL_ASYNC_API COAP_USE_PLATFORM_LOG
```
修改为:
```
GLOBAL_DEFINES += SUPPORT_SINGAPORE_DOMAIN ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD HAL_ASYNC_API COAP_USE_PLATFORM_LOG
```

然后重新编译固件烧录即可

## <a name="5.2 单品动态注册/一型一密">5.2 单品动态注册/一型一密</a>
### <a name="动态注册/一型一密的概念">动态注册/一型一密的概念</a>
> 我们知道设备三元组包含productKey, deviceName和deviceSecret
>
> 每个设备有自己的设备密钥(deviceSecret), 在生产设备时, 需要将设备三元组烧录进设备中, 由于每台设备的设备密钥不同, 所以烧录时需要单独进行配置

为了简化此流程, 引入产品密钥(productSecret)

将原来需要烧录的每个设备唯一的设备密钥换成每个产品唯一的产品密钥, 在设备连网认证时, 再用产品证书(productKey和productSecret)向云端动态获取设备密钥(deviceSecret)

**需要注意的是, 使用一型一密获取设备密钥只能获取一次, 当设备端尝试再次使用一型一密时, 云端会拒绝设备端的请求**

### <a name="相关API和例程">相关API和例程</a>

无论使用高级版还是基础版, 在linkkit启动(高级版为`linkkit_start`, `linkkit_gateway_start`或`IOT_Linkkit_Connect`, 基础版为`IOT_SetupConnInfo`)之前, 使用以下接口配置是否需要使用一型一密:
```
/* Choose Login Method */
int dynamic_register = 1; /* 0: 不使用一型一密, 1: 使用一型一密 */
IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
```

### <a name="完整过程示例">完整过程示例</a>

#### <a name="基础版示例">基础版示例</a>

访问`物联网套件控制台`, 选择要打开一型一密功能的产品, 进入`产品详情`, 如下图所示:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E4%B8%80%E5%9E%8B%E4%B8%80%E5%AF%86.png)

如上图所示, 打开`动态注册`的开关, 即可开启一型一密功能

现在在该产品下新建一个设备`Example_dyn1`:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E8%AE%BE%E5%A4%87%E7%AE%A1%E7%90%86-%E5%88%9B%E5%BB%BA%E4%B8%80%E5%9E%8B%E4%B8%80%E5%AF%86%E8%AE%BE%E5%A4%87.png)

打开linkkit sdk, 用设备`Example_dyn1`的四元组替换示例代码(`examples/mqtt/mqtt-example.c`)中的四元组, 并使用`IOT_Ioctl`选择使用一型一密方式:
```
#if defined(SUPPORT_ITLS)

    ...
    ...
    #else
        #define PRODUCT_KEY             "a1ExpAkj9Hi"
        #define PRODUCT_SECRET          "ffFnFlKQW3HYjjPR"
        #define DEVICE_NAME             "Example_dyn1"
     /* #define DEVICE_SECRET           "ik0qF60vcdvStygvKOTs3xEUbVj6BbSR" */
    #endif

#endif
...
...
int main(int argc, char **argv)
{
    IOT_OpenLog("mqtt");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    user_argc = argc;
    user_argv = argv;
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetProductSecret(PRODUCT_SECRET);
    HAL_SetDeviceName(DEVICE_NAME);
    /* HAL_SetDeviceSecret(DEVICE_SECRET); */

    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_DOMAIN_SH;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login  Method */
    int dynamic_register = 1;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    mqtt_client();
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    EXAMPLE_TRACE("out of sample!");

    return 0;
}
```
使用以下命令编译示例代码:
```
$ make distclean
$ make
```
执行示例程序:
```
$./output/release/bin/mqtt-example
[inf] IOT_SetupConnInfo(114): DeviceSecret KV not exist, Now We Need Dynamic Register...
[inf] _calc_dynreg_sign(61): Random Key: 7y4Jg5xdKCy9W2i
[inf] _calc_dynreg_sign(75): Sign: d3b560d5be0c9c19749470e85d912b65685fa4b20edcbd179ccfe98fcca23d5e
[inf] httpclient_common(794): host: 'iot-auth.cn-shanghai.aliyuncs.com', port: 443
...
...
[dbg] httpclient_send_header(326): REQUEST (Length: 211 Bytes)
> POST /auth/register/device HTTP/1.1
> Host: iot-auth.cn-shanghai.aliyuncs.com
> Accept: text/xml,text/javascript,text/html,application/json
> Content-Length: 161
> Content-Type: application/x-www-form-urlencoded
>
[dbg] httpclient_send_header(331): Written 211 bytes
[dbg] httpclient_send_userdata(348): client_data->post_buf: productKey=a1ExpAkj9Hi&deviceName=Example_dyn1&random=7y4Jg5xdKCy9W2i&sign=d3b560d5be0c9c19749470e85d912b65685fa4b20edcbd179ccfe98fcca23d5e&signMethod=hmacsha256
[dbg] httpclient_send_userdata(353): Written 161 bytes
[dbg] httpclient_recv(393): 32 bytes has been read
[dbg] httpclient_recv_response(769): RESPONSE (Length: 32 Bytes)
< HTTP/1.1 200 OK
< Server: Tengine
...
...
[inf] _fetch_dynreg_http_resp(110): Http Response Payload: {"code":200,"data":{"deviceName":"Example_dyn1","deviceSecret":"KGQQFFlGinIipW9Xn7xQ5U6d6MokPZD4","productKey":"a1ExpAkj9Hi"},"message":"success"}
[inf] _fetch_dynreg_http_resp(127): Dynamic Register Code: 200
[inf] _fetch_dynreg_http_resp(148): Dynamic Register Device Secret: KGQQFFlGinIipW9Xn7xQ5U6d6MokPZD4
[inf] iotx_device_info_init(39): device_info created successfully!
[dbg] iotx_device_info_set(49): start to set device info!
[dbg] iotx_device_info_set(63): device_info set successfully!
[inf] guider_print_dev_guider_info(279): ....................................................
[inf] guider_print_dev_guider_info(280):           ProductKey : a1ExpAkj9Hi
[inf] guider_print_dev_guider_info(281):           DeviceName : Example_dyn1
[inf] guider_print_dev_guider_info(282):             DeviceID : a1ExpAkj9Hi.Example_dyn1
[inf] guider_print_dev_guider_info(284): ....................................................
[inf] guider_print_dev_guider_info(285):        PartnerID Buf : ,partner_id=example.demo.partner-id
[inf] guider_print_dev_guider_info(286):         ModuleID Buf : ,module_id=example.demo.module-id
[inf] guider_print_dev_guider_info(287):           Guider URL :
[inf] guider_print_dev_guider_info(289):       Guider SecMode : 2 (TLS + Direct)
[inf] guider_print_dev_guider_info(291):     Guider Timestamp : 2524608000000
[inf] guider_print_dev_guider_info(292): ....................................................
[inf] guider_print_dev_guider_info(298): ....................................................
[inf] guider_print_conn_info(256): -----------------------------------------
...
...
[inf] iotx_mc_connect(2502): mqtt connect success!
```

从上面的运行结果可以看出, 设备已使用一型一密的方式获得了设备密钥

```
(Device Secret): "KGQQFFlGinIipW9Xn7xQ5U6d6MokPZD4"
```
SDK会调用`HAL_Kv_Get`将之持久化. 若用户尝试对同一设备第二次使用一型一密功能, 则云端会返回以下错误:

```
[inf] _fetch_dynreg_http_resp(110): Http Response Payload: {"code":6289,"message":"device is already active"}
```

#### <a name="高级版示例">高级版示例</a>

访问`物联网套件控制台`, 选择要打开一型一密功能的产品, 进入`产品详情`, 如下图所示:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E4%B8%80%E5%9E%8B%E4%B8%80%E5%AF%86.png)

如上图所示, 打开`动态注册`的开关, 即可开启一型一密功能

现在在该产品下新建一个设备`AdvExample_dyn1`:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E8%AE%BE%E5%A4%87%E7%AE%A1%E7%90%86-%E5%88%9B%E5%BB%BA%E4%B8%80%E5%9E%8B%E4%B8%80%E5%AF%86%E8%AE%BE%E5%A4%87.png)

打开linkkit sdk, 用设备`AdvExample_dyn1`的四元组替换单品示例代码(`examples/linkkit/linkkit_example_solo.c`)中的四元组

    // for demo only
    #define PRODUCT_KEY      "a1csED27mp7"
    #define PRODUCT_SECRET   "VuULINCrtTAzCSzp"
    #define DEVICE_NAME      "AdvExample_dyn1"
    #define DEVICE_SECRET    "FcYjzB3GiYiRunsBInIB3fms86UgwuCY"

并使用`IOT_Ioctl`选择使用一型一密方式:

    int main(int argc, char **argv)
    {

        ...
        ...

        /* Choose Login Method */
        int dynamic_register = 0;
        IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

        ...
        ...

        /* Start Connect Aliyun Server */
        res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
        if (res < 0) {
            EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
            return -1;
        }

        ...
        ...
    }

使用以下命令编译示例代码:
```
make distclean
make
```
执行示例程序:
```
./output/release/bin/linkkit-example-solo

main|712 :: start!

linkkit_example|613 :: linkkit start
...
...
[inf] IOT_SetupConnInfo(114): DeviceSecret KV not exist, Now We Need Dynamic Register...
...
...
[dbg] httpclient_send_header(326): REQUEST (Length: 211 Bytes)
> POST /auth/register/device HTTP/1.1
> Host: iot-auth.cn-shanghai.aliyuncs.com
> Accept: text/xml,text/javascript,text/html,application/json
> Content-Length: 164
> Content-Type: application/x-www-form-urlencoded
>
[dbg] httpclient_send_header(331): Written 211 bytes
[dbg] httpclient_send_userdata(348): client_data->post_buf: productKey=a1csED27mp7&deviceName=AdvExample_dyn1&random=2HyP9O70q5t9VuX&sign=9bba3312eb8d62ab11baaf910ad617ac052b31defc35a10446277a9e0aab2556&signMethod=hmacsha256
[dbg] httpclient_send_userdata(353): Written 164 bytes
...
...
[dbg] httpclient_retrieve_content(437): Current data: {"code":200,"data"
[dbg] httpclient_retrieve_content(543): Total-Payload: 149 Bytes; Read: 18 Bytes
[dbg] httpclient_recv(393): 131 bytes has been read
[dbg] httpclient_retrieve_content(598): no more (content-length)
[inf] httpclient_common(833): close http channel
[inf] _network_ssl_disconnect(488): ssl_disconnect
[inf] httpclient_close(783): client disconnected
[inf] _fetch_dynreg_http_resp(110): Http Response Payload: {"code":200,"data":{"deviceName":"AdvExample_dyn1","deviceSecret":"ximd4xyeAjtHp5lsz3A5okefnJMYtQTP","productKey":"a1csED27mp7"},"message":"success"}
[inf] _fetch_dynreg_http_resp(127): Dynamic Register Code: 200
[inf] _fetch_dynreg_http_resp(148): Dynamic Register Device Secret: ximd4xyeAjtHp5lsz3A5okefnJMYtQTP
[inf] iotx_device_info_init(39): device_info created successfully!
[dbg] iotx_device_info_set(49): start to set device info!
[dbg] iotx_device_info_set(63): device_info set successfully!
[inf] guider_print_dev_guider_info(279): ....................................................
[inf] guider_print_dev_guider_info(280):           ProductKey : a1csED27mp7
[inf] guider_print_dev_guider_info(281):           DeviceName : AdvExample_dyn1
[inf] guider_print_dev_guider_info(282):             DeviceID : a1csED27mp7.AdvExample_dyn1
[inf] guider_print_dev_guider_info(284): ....................................................
[inf] guider_print_dev_guider_info(285):        PartnerID Buf : ,partner_id=example.demo.partner-id
[inf] guider_print_dev_guider_info(286):         ModuleID Buf : ,module_id=example.demo.module-id
[inf] guider_print_dev_guider_info(287):           Guider URL :
[inf] guider_print_dev_guider_info(289):       Guider SecMode : 2 (TLS + Direct)
[inf] guider_print_dev_guider_info(291):     Guider Timestamp : 2524608000000
[inf] guider_print_dev_guider_info(292): ....................................................
[inf] guider_print_dev_guider_info(298): ....................................................
[inf] guider_print_conn_info(256): -----------------------------------------
[inf] guider_print_conn_info(257):             Host : a1csED27mp7.iot-as-mqtt.cn-shanghai.aliyuncs.com
[inf] guider_print_conn_info(258):             Port : 1883
[inf] guider_print_conn_info(261):         ClientID : a1csED27mp7.AdvExample_dyn1|securemode=2,timestamp=2524608000000,signmethod=hmacsha1,gw=0,ext=0,partner_id=example.demo.partner-id,module_id=example.demo.module-id|
[inf] guider_print_conn_info(263):       TLS PubKey : 0x47e936 ('-----BEGIN CERTI ...')
[inf] guider_print_conn_info(266): -----------------------------------------
...
...
[inf] iotx_mc_connect(2502): mqtt connect success!
```

从上面的运行结果可以看出, 设备已使用一型一密的方式获得了设备密钥

```
(Device Secret): "ximd4xyeAjtHp5lsz3A5okefnJMYtQTP"
```

SDK会调用`HAL_Kv_Get`将之持久化. 若用户尝试对同一设备第二次使用一型一密功能, 则云端会返回以下错误:

```
[inf] _fetch_dynreg_http_resp(110): Http Response Payload: {"code":6289,"message":"device is already active"}
```
## <a name="5.3 物模型管理功能">5.3 物模型管理功能</a>
> 物模型管理功能是指SDK能够使能IoT设备接受云端控制台或者手机公版app的控制, 进行属性/事件/服务维度的设置和监控, 在本文的其它地方, 有时也称为"高级版"

与物模型功能相关的API有如下这些
---

| 函数名                                                  | 说明
|---------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------
| [IOT_Linkkit_Open](#IOT_Linkkit_Open)                   | 创建本地资源, 在进行网络报文交互之前, 必须先调用此接口, 得到一个会话的句柄
| [IOT_Linkkit_Connect](#IOT_Linkkit_Connect)             | 对主设备/网关来说, 将会建立设备与云端的通信. 对于子设备来说, 将向云端注册该子设备(若需要), 并添加主子设备拓扑关系
| [IOT_Linkkit_Yield](#IOT_Linkkit_Yield)                 | 若SDK占有独立线程, 该函数内容为空, 否则表示将CPU交给SDK让其接收网络报文并将消息分发到用户的回调函数中
| [IOT_Linkkit_Close](#IOT_Linkkit_Close)                 | 若入参中的会话句柄为主设备/网关, 则关闭网络连接并释放SDK为该会话所占用的所有资源
| [IOT_Linkkit_TriggerEvent](#IOT_Linkkit_TriggerEvent)   | 向云端发送**事件报文**, 如错误码, 异常告警等
| [IOT_Linkkit_Report](#IOT_Linkkit_Report)               | 向云端发送**没有云端业务数据下发的上行报文**, 包括属性值/设备标签/二进制透传数据/子设备管理等各种报文
| [IOT_Linkkit_Query](#IOT_Linkkit_Query)                 | 向云端发送**存在云端业务数据下发的查询报文**, 包括OTA状态查询/OTA固件下载/子设备拓扑查询/NTP时间查询等各种报文

| 函数名                                          | 说明
|-------------------------------------------------|---------------------------------------------------------------------------------------------
| [IOT_RegisterCallback](#IOT_RegisterCallback)   | 对SDK注册事件回调函数, 如云端连接成功/失败, 有属性设置/服务请求到达, 子设备管理报文答复等
| [IOT_Ioctl](#IOT_Ioctl)                         | 对SDK进行各种参数运行时设置和获取, 以及运行状态的信息获取等, 实参可以是任何数据类型

> 现对照 `examples/linkkit/linkkit_example_solo.c` 例程分步骤讲解如何使用这几个API实现物模型管理功能

1. 初始化阶段, 调用`IOT_Linkkit_Open()`导入三元组, 获取一个设备标识
---
```
    iotx_linkkit_dev_meta_info_t master_meta_info;

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }
```

2. 与云端建立的配置, 调用`IOT_Ioctl()`(通用接口)与`IOT_Linkkit_Ioctl()`(linkkit专用接口)进行相关配置, 详细情况可查看对应API说明
---
```
    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_DOMAIN_SH;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Choose Whether You Need Post Property Reply */
    int post_property_reply = 0;
    IOT_Linkkit_Ioctl(user_example_ctx->master_devid, IOTX_LINKKIT_CMD_OPTION_PROPERTY_POST_REPLY,
                      (void *)&post_property_reply);

    /* Choose Whether You Need Post Event Reply */
    int post_event_reply = 0;
    IOT_Linkkit_Ioctl(user_example_ctx->master_devid, IOTX_LINKKIT_CMD_OPTION_EVENT_POST_REPLY, (void *)&post_event_reply);
```

3. 主设备建立连接
---
```
    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }
```

4. 进入while()循环, 循环中包含`IOT_Linkkit_Yield()`用于接收网络报文并将消息分发到用户的回调函数中
---
```
    while (1) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
        ...
    }
```

5. 属性上报, 拓展信息上报和裸数据上都通过对`IOT_Linkkit_Post()`的封装实现, 对于JSON格式数据, 用户需自行进行组包处理
---
```
/* 属性上报 */
void user_post_property(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *property_payload = "{\"LightSwitch\":1}";

    res = IOT_Linkkit_Post(user_example_ctx->master_devid, IOTX_LINKKIT_MSG_POST_PROPERTY,
                           (unsigned char *)property_payload, strlen(property_payload));
    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}

/* 拓展信息添加 */
void user_deviceinfo_update(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *device_info_update = "[{\"attrKey\":\"abc\",\"attrValue\":\"hello,world\"}]";

    res = IOT_Linkkit_Post(user_example_ctx->master_devid, IOTX_LINKKIT_MSG_DEVICEINFO_UPDATE,
                           (unsigned char *)device_info_update, strlen(device_info_update));
    EXAMPLE_TRACE("Device Info Update Message ID: %d", res);
}

/* 拓展信息删除 */
void user_deviceinfo_delete(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *device_info_delete = "[{\"attrKey\":\"abc\"}]";

    res = IOT_Linkkit_Post(user_example_ctx->master_devid, IOTX_LINKKIT_MSG_DEVICEINFO_DELETE,
                           (unsigned char *)device_info_delete, strlen(device_info_delete));
    EXAMPLE_TRACE("Device Info Delete Message ID: %d", res);
}

/* 裸数据上报 */
void user_post_raw_data(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    unsigned char raw_data[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

    res = IOT_Linkkit_Post(user_example_ctx->master_devid, IOTX_LINKKIT_MSG_POST_RAW_DATA,
                           raw_data, 7);
    EXAMPLE_TRACE("Post Raw Data Message ID: %d", res);
}
```

5. 事件上报需调用`IOT_Linkkit_TriggerEvent()`接口, 数据格式为JSON
---
```
void user_post_event(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *event_id = "Error";
    char *event_payload = "{\"ErrorCode\":0}";

    res = IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    EXAMPLE_TRACE("Post Event Message ID: %d", res);
}
```