# <a name="目录">目录</a>
+ [MQTT站点配置](#MQTT站点配置)
+ [单品动态注册/一型一密](#单品动态注册/一型一密)
    * [动态注册/一型一密的概念](#动态注册/一型一密的概念)
    * [相关API和例程](#相关API和例程)
    * [完整过程示例](#完整过程示例)
        - [基础版示例](#基础版示例)
        - [高级版示例](#高级版示例)
+ [支持智能生活开放平台动态连云](#支持智能生活开放平台动态连云)

# <a name="MQTT站点配置">MQTT站点配置</a>
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

        /* America(sillicon valley) */
        IOTX_CLOUD_REGION_USA_WEST,

        /* Germany */
        IOTX_CLOUD_REGION_GERMANY,

        /* Custom setting */
        IOTX_CLOUD_REGION_CUSTOM,

        /* Maximum number of domain */
        IOTX_CLOUD_DOMAIN_MAX
    } iotx_cloud_region_types_t;

+ 首先使用 [IOT_Ioctl](#IOT_Ioctl) 的 `IOTX_IOCTL_SET_REGION` 选项, 配合上面的枚举值, 设置要连接的站点
+ 然后使用 [IOT_MQTT_Construct](#IOT_MQTT_Construct) 或者 [IOT_Linkkit_Connect](#IOT_Linkkit_Connect) 来建立设备到阿里云的连接

例如:
---

    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_REGION, (void *)&domain_type);

SDK同时还支持手动配置站点域名, 对于枚举类型`iotx_cloud_region_types_t`中未定义的地区站点, 可通过[IOT_Ioctl](#IOT_Ioctl)的其他选项进行设置

+ 使用[IOT_Ioctl](#IOT_Ioctl)的`IOTX_IOCTL_SET_MQTT_DOMAIN`选项手动配置MQTT服务器域名
+ 使用[IOT_Ioctl](#IOT_Ioctl)的`IOTX_IOCTL_SET_HTTP_DOMAIN`选项手动配置HTTP服务器域名

例如, 通过以下方法将连接站点配置为美国(弗吉尼亚):
---

    #define USA_EAST_DOMAIN_MQTT     "iot-as-mqtt.us-east-1.aliyuncs.com"
    #define USA_EAST_DOMAIN_HTTP     "iot-auth.us-east-1.aliyuncs.com"

    IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)USA_EAST_DOMAIN_MQTT);
    IOT_Ioctl(IOTX_IOCTL_SET_HTTP_DOMAIN, (void *)USA_EAST_DOMAIN_HTTP);


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

# <a name="单品动态注册/一型一密">单品动态注册/一型一密</a>
## <a name="动态注册/一型一密的概念">动态注册/一型一密的概念</a>
> 我们知道设备三元组包含productKey, deviceName和deviceSecret
>
> 每个设备有自己的设备密钥(deviceSecret), 在生产设备时, 需要将设备三元组烧录进设备中, 由于每台设备的设备密钥不同, 所以烧录时需要单独进行配置

为了简化此流程, 引入产品密钥(productSecret)

将原来需要烧录的每个设备唯一的设备密钥(deviceSecret)换成每个产品唯一的产品密钥, 在设备连网认证时, 再用产品证书(productKey和productSecret)向云端动态获取设备密钥(deviceSecret)

**需要注意的是, 使用一型一密获取设备密钥只能获取一次, 当设备端尝试再次使用一型一密时, 云端会拒绝设备端的请求**

## <a name="相关API和例程">相关API和例程</a>

无论使用高级版还是基础版, 在linkkit启动(高级版为`linkkit_start`, `linkkit_gateway_start`或`IOT_Linkkit_Connect`, 基础版为`IOT_SetupConnInfo`)之前, 使用以下接口配置是否需要使用一型一密:
```
/* Choose Login Method */
int dynamic_register = 1; /* 0: 不使用一型一密, 1: 使用一型一密 */
IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
```

## <a name="完整过程示例">完整过程示例</a>

### <a name="基础版示例">基础版示例</a>

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

### <a name="高级版示例">高级版示例</a>

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

# <a name="支持智能生活开放平台动态连云">支持智能生活开放平台动态连云</a>

在**智能生活开放平台**海外设备激活联网时, 将统一连接到新加坡激活中心, 平台会将设备自动分配到就近的数据节点, 如: 在美国激活的设备, 会自动连接美国的服务器

SDK只要做下面的2个配置就可以支持**智能生活开放平台**动态连云模式:
1. 修改`make.setting`的`FEATURE_MQTT_DIRECT`配置为`n`, 这样便启用了https认证模式
2. 将连接站点配置为新加坡站点:
```
    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SINGAPORE;
    IOT_Ioctl(IOTX_IOCTL_SET_REGION, (void *)&domain_type);
```