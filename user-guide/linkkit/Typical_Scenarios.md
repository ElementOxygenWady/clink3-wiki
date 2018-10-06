# <a name="目录">目录</a>
+ [第五章 典型场景示例](#第五章 典型场景示例)
    * [5.1 MQTT站点配置](#5.1 MQTT站点配置)
        - [基础版](#基础版)
        - [高级版](#高级版)
        - [AOS1.3.2版本连接海外站点](#AOS1.3.2版本连接海外站点)
    * [5.2 TSL静态集成和动态拉取](#5.2 TSL静态集成和动态拉取)
        - [TSL静态集成](#TSL静态集成)
        - [TSL动态拉取](#TSL动态拉取)
    * [5.3 单品动态注册/一型一密](#5.3 单品动态注册/一型一密)
        - [动态注册/一型一密的概念](#动态注册/一型一密的概念)
        - [相关API和例程](#相关API和例程)
        - [完整过程示例](#完整过程示例)


# <a name="第五章 典型场景示例">第五章 典型场景示例</a>
## <a name="5.1 MQTT站点配置">5.1 MQTT站点配置</a>
在使用阿里云物联网套件连接阿里云时, 需要指定MQTT需要连接的站点, 在基础版和高级版中, 站点配置方法如下:

### <a name="基础版">基础版</a>
在 `iotx-sdk-c/include/iot_export.h` 中, 枚举类型 `iotx_cloud_domain_types_t` 定义了当前可连接的MQTT站点:

    /* domain type */
    typedef enum IOTX_CLOUD_DOMAIN_TYPES {
        /* Shanghai */
        IOTX_CLOUD_DOMAIN_SH,

        /* Singapore */
        IOTX_CLOUD_DOMAIN_SG,

        /* Japan */
        IOTX_CLOUD_DOMAIN_JP,

        /* America */
        IOTX_CLOUD_DOMAIN_US,

        /* Germany */
        IOTX_CLOUD_DOMAIN_GER,

        /* Maximum number of domain */
        IOTX_CLOUD_DOMAIN_MAX
    } iotx_cloud_domain_types_t;

在使用基础版MQTT接口连接阿里云时, 使用 [IOT_Ioctl](#IOT_Ioctl) 的 `IOTX_IOCTL_SET_DOMAIN` 选项设置要连接的站点, 然后再使用 [IOT_SetupConnInfo](#IOT_SetupConnInfo) 来建立设备到阿里云的连接

### <a name="高级版">高级版</a>
在 `iotx-sdk-c/include/exports/linkkit_export.h` 中, 枚举类型 `linkkit_cloud_domain_type_t` 定义了当前可连接的MQTT站点:

    typedef enum {
        /* shanghai */
        linkkit_cloud_domain_shanghai,
        /* singapore */
        linkkit_cloud_domain_singapore,
        /* japan */
        linkkit_cloud_domain_japan,
        /* america */
        linkkit_cloud_domain_america,
        /* germany */
        linkkit_cloud_domain_germany,

        linkkit_cloud_domain_max,
    } linkkit_cloud_domain_type_t;

在使用高级版接口连接阿里云时, 在 `linkkit_start` 中传入需要连接的站点即可

**注意事项: 如果在阿里云物联网控制台申请的三元组与连接时使用的域名不符合, 连接站点时会出现认证错误(错误码-35)**

*例如: 在阿里云物联网控制台的华东2站点申请的三元组, 在物联网套件中应该连接华东2(上海)站点*

### <a name="AOS1.3.2版本连接海外站点">AOS1.3.2版本连接海外站点</a>
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

## <a name="5.2 TSL静态集成和动态拉取">5.2 TSL静态集成和动态拉取</a>

物模型指将物理空间中的实体数字化, 并在云端构建该实体的数据模型. 在物联网平台中, 定义物模型即定义产品功能. 完成功能定义后, 系统将自动生成该产品的物模型

物模型描述产品是什么, 能做什么, 可以对外提供哪些服务

物模型以 JSON 格式表述, 称之为 TSL(即 Thing Specification Language). 在产品的功能定义页面, 单击查看物模型即可查看 JSON 格式的 TSL:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E7%89%A9%E6%A8%A1%E5%9E%8B.png)

此时有两种方式在物联网套件中使用TSL:

- 静态集成: 指将TSL直接写到代码中, 这时需要上图中选择导出物模型, 将设备的TSL(JSON格式)导出到文件, 并手动集成到代码中
- 动态拉取: 指在物联网套件运行时去云端拉取TSL

### <a name="TSL静态集成">TSL静态集成</a>
如上所述, 将设备的TSL(JSON格式)导出到文件后(默认文件名为model.json), 由于C语言需要将字符串中的双引号`"`进行转义, 所以需要将导出的TSL文件进行转义

在网上有很多这样的转义工具, 物联网套件中也自带了一个可以转义JSON的小工具

当SDK编译完成后, 使用 `iotx-sdk-c/output/release/bin` 目录下的 `linkkit_tsl_convert` 可以完成该转义任务. 使用方法如下:

     $./linkkit_tsl_convert -i model.json

上述命令执行完成后, 会在调用命令的当前目录生成一个 `conv.txt` 文件, 里面就是转义后的JSON字符串. 目前仅高级版单品支持静态集成TSL, 集成方法如下:

高级版单品使用TSL静态集成
---
默认情况下, 物联网套件编译单品版本的例子程序, 源代码位于 `iotx-sdk-c/examples/linkkit` 目录下的 `linkkit_example_solo.c`

同目录下的 `example_tsl_solo.data` 为存放静态集成TSL的文件

将刚才 `conv.txt` 中的内容复制, 替换 `example_tsl_solo.data` 文件中的 `TSL_STRING` 变量, 重新编译即可

不要忘了将 `linkkit_example_solo.c` 中的三元组替换成该TSL对应产品下设备的三元组

### <a name="TSL动态拉取">TSL动态拉取</a>
如果使用动态拉取TSL, 就不用去替换代码中的静态TSL

需要注意的是, 使用动态拉取TSL时, 在TSL占用空间较大(TSL定义的服务/属性/事件越多, TSL越大)的情况下, 有可能需要更改物联网套件默认的MQTT接收buffer长度

可以按如下两种方法进行配置更改:

- 修改平台配置文件, 在当前使用平台的config.xxx.xxx文件(位于iotx-sdk-c/src/board目录下)中, 修改 `CONFIG_ENV_CFLAGS` 中的 `CONFIG_MQTT_RX_MAXLEN` 编译选项

> 以ubuntu为例, 如下图所示:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/iotkit-MQTT%E9%BB%98%E8%AE%A4%E6%8E%A5%E6%94%B6%E9%95%BF%E5%BA%A6%E5%9C%A8Makefile%E4%B8%AD%E7%9A%84%E9%85%8D%E7%BD%AE.png)

- 在代码中修改默认值, 在 `iotx-sdk-c/include/imports/iot_import_config.h` 文件中修改 `CONFIG_MQTT_RX_MAXLEN` 即可, 如下图所示:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/iotkit-MQTT%E9%BB%98%E8%AE%A4%E6%8E%A5%E6%94%B6%E9%95%BF%E5%BA%A6%E5%9C%A8iot_import_config.h%E4%B8%AD%E7%9A%84%E9%85%8D%E7%BD%AE.png)

*注意: 在Makefile中设置的 `CONFIG_MQTT_RX_MAXLEN` 值会覆盖掉 `iot_import_config.h` 中设置的值*

#### <a name="高级版单品使用TSL动态拉取">高级版单品使用TSL动态拉取</a>
默认情况下, 在高级版单品的example(源代码位于 `iotx-sdk-c/examples/linkkit` 目录下的 `linkkit_example_solo.c` )中, 使用的是TSL静态集成

若要使用TSL动态拉取, 只需要将 `linkkit_start` 的第二个入参 `get_tsl_from_cloud` 设为`1`即可

#### <a name="高级版网关使用TSL动态拉取">高级版网关使用TSL动态拉取</a>
默认情况下, 在高级版网关的example(源代码位于 `iotx-sdk-c/examples/linkkit` 目录下的 `linkkit_example_gateway.c` )中, 使用的总是TSL动态拉取

## <a name="5.3 单品动态注册/一型一密">5.3 单品动态注册/一型一密</a>
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

打开linkkit sdk, 用设备`Example_dyn1`的四元组替换示例代码(examples/mqtt/mqtt-example.c)中的四元组, 并使用`IOT_Ioctl`选择使用一型一密方式:
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

打开linkkit sdk, 用设备`AdvExample_dyn1`的四元组替换单品示例代码(examples/linkkit/deprecated/solo.c)中的四元组, 并使用`IOT_Ioctl`选择使用一型一密方式:

```
int main(int argc, char **argv)
{
    IOT_OpenLog("linkkit");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    EXAMPLE_TRACE("start!\n");

    HAL_SetProductKey("a1csED27mp7");
    HAL_SetProductSecret("VuULINCrtTAzCSzp");
    HAL_SetDeviceName("AdvExample_dyn1");
    /* HAL_SetDeviceSecret("FcYjzB3GiYiRunsBInIB3fms86UgwuCY"); */

    int dynamic_register = 1;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /*
     * linkkit dome
     * please check document: https://help.aliyun.com/document_detail/73708.html
     *         API introduce: https://help.aliyun.com/document_detail/68687.html
     */
    linkkit_example();

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    EXAMPLE_TRACE("out of sample!\n");

    return 0;
}
```
使用以下命令编译示例代码:
```
$make distclean
$make
```
执行示例程序:
```
$./output/release/bin/linkkit-example-solo
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
