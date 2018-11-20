# <a name="目录">目录</a>
+ [MQTT站点配置](#MQTT站点配置)
+ [例程讲解](#例程讲解)
+ [MQTT功能API](#MQTT功能API)
+ [需要实现的HAL接口](#需要实现的HAL接口)

+ [MQTT站点配置](#MQTT站点配置)
+ [例程讲解](#例程讲解)
+ [MQTT功能API](#MQTT功能API)
+ [需要实现的HAL接口](#需要实现的HAL接口)

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

+ 首先使用 [IOT_Ioctl](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Basic_Provides#IOT_Ioctl) 的 `IOTX_IOCTL_SET_REGION` 选项, 配合上面的枚举值, 设置要连接的站点
+ 然后使用 [IOT_MQTT_Construct](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Construct) 或者 [IOT_Linkkit_Connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Connect) 来建立设备到阿里云的连接

例如:
---

    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_REGION, (void *)&domain_type);

SDK同时还支持手动配置站点域名, 对于枚举类型`iotx_cloud_region_types_t`中未定义的地区站点, 可通过[IOT_Ioctl](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Basic_Provides#IOT_Ioctl)的其他选项进行设置

+ 使用[IOT_Ioctl](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Basic_Provides#IOT_Ioctl)的`IOTX_IOCTL_SET_MQTT_DOMAIN`选项手动配置MQTT服务器域名
+ 使用[IOT_Ioctl](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Basic_Provides#IOT_Ioctl)的`IOTX_IOCTL_SET_HTTP_DOMAIN`选项手动配置HTTP服务器域名

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
AliOS things物联网操作系统也包含了Linkkit SDK，但不同版本的AOS在连接海外站点的配置上有所不同。在AOS1.3.2版本中连接到新加坡站点的配置方式有所不同, 新加坡站点会自动为设备分配最近的加速点

只需要修改`example/linkkitapp/linkkitapp.mk`文件, 将默认配置:
```
GLOBAL_DEFINES += MQTT_DIRECT ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD HAL_ASYNC_API COAP_USE_PLATFORM_LOG
```
修改为:
```
GLOBAL_DEFINES += SUPPORT_SINGAPORE_DOMAIN ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD HAL_ASYNC_API COAP_USE_PLATFORM_LOG
```

然后重新编译固件烧录即可


# <a name="例程讲解">例程讲解</a>

> 现对照 `examples/mqtt/mqtt-example.c` 例程分步骤讲解如何使用这几个API实现MQTT上云功能

1. 初始化阶段, 如果用户需要配置使用海外站点或者使用动态注册功能, 可先使用`IOT_Ioctl()`完成配置
---
```
    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_DOMAIN_SH;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login  Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
```

2. MQTT会话建立前的准备, 导入设备三元组, 初始化连接信息
---
```
    iotx_conn_info_pt pconn_info;

    HAL_GetProductKey(__product_key);
    HAL_GetDeviceName(__device_name);
    HAL_GetDeviceSecret(__device_secret);

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(__product_key, __device_name, __device_secret, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        rc = -1;
        goto do_exit;
    }
```
3. 配置MQTT参数, 构建MQTT连接会话, 与云端服务器建立连接
---
```
    void *pclient;
    iotx_mqtt_param_t mqtt_params;

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = MQTT_MSGLEN;
    mqtt_params.write_buf_size = MQTT_MSGLEN;

    mqtt_params.handle_event.h_fp = event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        rc = -1;
        goto do_exit;
    }
```

4. MQTT会话建立成功后, 用户便可以根据业务需要对自定义Topic进行数据发布(需事先在https://iot.console.aliyun.com 自定义此topic并赋予其publish权限)
---

```
    #define TOPIC_DATA              "/"PRODUCT_KEY"/"DEVICE_NAME"/data"
    iotx_mqtt_topic_info_t topic_msg;

    /* Initialize topic information */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    strcpy(msg_pub, "update: hello! start!");

    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg_pub;
    topic_msg.payload_len = strlen(msg_pub);

    /* 发布指定的Topic */
    rc = IOT_MQTT_Publish(pclient, TOPIC_UPDATE, &topic_msg);
    if (rc < 0) {
        IOT_MQTT_Destroy(&pclient);
        EXAMPLE_TRACE("error occur when publish");
        rc = -1;
        goto do_exit;
    }

```

5. 同样, 用户也可以根据业务需要对自定义Topic进行订阅或取消订阅(需事先在https://iot.console.aliyun.com 自定义此topic并赋予其subscribe权限)
---

```

    /* 订阅指定的Topic, 同时注册对应回调函数用于处理云端发布的消息 */
    rc = IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
    if (rc < 0) {
        IOT_MQTT_Destroy(&pclient);
        EXAMPLE_TRACE("IOT_MQTT_Subscribe() failed, rc = %d", rc);
        rc = -1;
        goto do_exit;
    }

    /* 取消订阅指定的Topic */
    IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
```

6. 用户会发现每一个向服务器的上行操作(包括发布, 订阅和取消订阅等行为)后都会紧接一个`IOT_MQTT_Yield()`, 此函数主要用于处理服务器下行报文的收取和解析, 同时内含了心跳的维持. 在while()循环中必须包含次函数
---
```
    /* handle the MQTT packet received from TCP or SSL connection */
    IOT_MQTT_Yield(pclient, 200);
```

7. 最终用户可以通过`IOT_MQTT_Destroy()`结束会话, 断开设备与服务端连接
---
```
    IOT_MQTT_Destroy(&pclient);
```

# <a name="MQTT功能API">MQTT功能API</a>

| 函数名                                                                                                                                                      | 说明
|-------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------
| [IOT_SetupConnInfo](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_SetupConnInfo)                    | MQTT连接前的准备, 基于`DeviceName + DeviceSecret + ProductKey`产生MQTT连接的用户名和密码等
| [IOT_MQTT_CheckStateNormal](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_CheckStateNormal)    | MQTT连接后, 调用此函数检查长连接是否正常
| [IOT_MQTT_Construct](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Construct)                  | MQTT实例的构造函数, 入参为`iotx_mqtt_param_t`结构体, 连接MQTT服务器, 并返回被创建句柄
| [IOT_MQTT_Destroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Destroy)                      | MQTT实例的摧毁函数, 入参为 [IOT_MQTT_Construct](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Construct) 创建的句柄
| [IOT_MQTT_Publish](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Publish)                      | MQTT会话阶段, 组织一个完整的`MQTT Publish`报文, 向服务端发送消息发布报文
| [IOT_MQTT_Publish_Simple](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Publish_Simple)        | MQTT会话阶段, 组织一个完整的`MQTT Publish`报文, 向服务端发送消息发布报文,参数中不含结构体等复杂数据类型
| [IOT_MQTT_Subscribe](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Subscribe)                  | MQTT会话阶段, 组织一个完整的`MQTT Subscribe`报文, 向服务端发送订阅请求
| [IOT_MQTT_Subscribe_Sync](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Subscribe_Sync)        | MQTT会话阶段, 组织一个完整的`MQTT Subscribe`报文, 向服务端发送订阅请求,并等待应答
| [IOT_MQTT_Unsubscribe](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Unsubscribe)              | MQTT会话阶段, 组织一个完整的`MQTT UnSubscribe`报文, 向服务端发送取消订阅请求
| [IOT_MQTT_Yield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides#IOT_MQTT_Yield)                          | MQTT会话阶段, MQTT主循环函数, 内含了心跳的维持, 服务器下行报文的收取等



# <a name="需要实现的HAL接口">需要实现的HAL接口</a>
**以下函数为可选实现, 如果希望SDK提供MQTT通道功能, 则需要用户对接**

| 函数名                                                                                                                                      | 说明
|---------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------
| [HAL_SSL_Destroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_SSL_Destroy)        | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能
| [HAL_SSL_Establish](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_SSL_Establish)    | 建立一个TLS连接, 用于MQTT功能, HTTPS功能
| [HAL_SSL_Read](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_SSL_Read)              | 从一个TLS连接中读数据, 用于MQTT功能, HTTPS功能
| [HAL_SSL_Write](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_SSL_Write)            | 向一个TLS连接中写数据, 用于MQTT功能, HTTPS功能
| [HAL_TCP_Destroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_TCP_Destroy)        | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能
| [HAL_TCP_Establish](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_TCP_Establish)    | 建立一个TCP连接, 包含了域名解析的动作和TCP连接的建立
| [HAL_TCP_Read](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_TCP_Read)              | 在指定时间内, 从TCP连接读取流数据, 并返回读到的字节数
| [HAL_TCP_Write](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_TCP_Write)            | 在指定时间内, 向TCP连接发送流数据, 并返回发送的字节数
| [HAL_Random](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_Random)                  | 随机数函数, 接受一个无符号数作为范围, 返回0到该数值范围内的随机无符号数
| [HAL_Srandom](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires#HAL_Srandom)                | 随机数播种函数, 使 [HAL_Random](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Random) 的返回值每个执行序列各不相同, 类似`srand`

