# <a name="目录">目录</a>
+ [设备身份认证](#设备身份认证)
    * [相关API](#相关API)
    * [一机一密编程](#一机一密编程)
    * [一型一密编程](#一型一密编程)
        - [基础版一型一密示例](#基础版一型一密示例)
+ [支持智能生活开放平台动态连云](#支持智能生活开放平台动态连云)

# <a name="设备身份认证">设备身份认证</a>

设备的身份认证分为一机一密以及一型一密两种：
+ 一机一密：
在设备上烧写设备的ProductKey、DeviceName、DeviceSecret，然后适配相应的HAL并调用SDK提供的连接云端的函数即可，这种方式要求对设备的产线工具进行一定的修改，需要对每个设备烧写不同的DeviceName和DeviceSecret；

+ 一型一密：
为了简化一机一密的生成例程，引入了产品密钥(productSecret)。设备上烧写设备的ProductKey、ProductSecret，每个设备需要具备自己的唯一标识并将该标识预先上传到阿里云IoT物联网平台，然后调用SDK提供的函数连接云端。这种方式每个设备上烧写的信息是固定的ProductKey、ProductSecret

**需要注意的是, 使用一型一密获取设备密钥只能获取一次, 当设备端尝试再次使用一型一密时, 云端会拒绝设备端的请求**

## <a name="相关API">相关API</a>

无论使用高级版还是基础版, 在linkkit启动(高级版为`linkkit_start`, `linkkit_gateway_start`或`IOT_Linkkit_Connect`, 基础版为`IOT_SetupConnInfo`)之前, 使用以下接口配置是否需要使用一型一密:
```
/* Choose Login Method */
int dynamic_register = 1; /* 0: 不使用一型一密, 1: 使用一型一密 */
IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
```

## <a name="一机一密编程">一机一密编程</a>

在linkkit启动之前调用以下接口配置为不使用动态注册，以`linkkit_example_solo.c`为例：
```
    // for demo only
    #define PRODUCT_KEY      "a16UKrlKekO"
    #define PRODUCT_SECRET   "RDluqbn3LQazrdqM"
    #define DEVICE_NAME      "gateway_test01"
    #define DEVICE_SECRET    "AT2XFOPOIbJaKfXsKeaEhabJ8TLhMQYp"

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
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
#endif

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* 关闭动态注册功能 */
    int post_event_reply = 0;   /* 0: 不使用一型一密 */
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }
```

## <a name="一型一密编程">一型一密编程</a>

如果要使用一型一密，只要调用以下接口打开动态注册功能即可。
```
/* 打开动态注册功能 */
int dynamic_register = 1; /* 1: 使用一型一密 */
IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
```

### <a name="基础版一型一密示例">基础版一型一密示例</a>

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

编译后执行示例程序:
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

SDK会自动调用`HAL_Kv_Set`将之持久化. 若用户尝试对同一设备第二次使用一型一密功能, 则云端会返回以下错误:

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