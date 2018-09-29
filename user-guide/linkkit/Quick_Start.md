# <a name="目录">目录</a>
+ [第二章 快速开始](#第二章 快速开始)
    * [2.1 准备本地开发环境](#2.1 准备本地开发环境)
        - [安装Ubuntu16.04](#安装Ubuntu16.04)
        - [安装必备软件](#安装必备软件)
    * [2.2 准备云端控制台环境](#2.2 准备云端控制台环境)
        - [注册/登录阿里云账号](#注册/登录阿里云账号)
        - [访问物联网套件控制台](#访问物联网套件控制台)
    * [2.3 体验MQTT Topic编程方式接入设备](#2.3 体验MQTT Topic编程方式接入设备)
        - [创建基础版产品和设备](#创建基础版产品和设备)
        - [编译运行基础版例程](#编译运行基础版例程)
            + [下载解压物联网平台设备端C-SDK](#下载解压物联网平台设备端C-SDK)
            + [填写设备三元组到例程中](#填写设备三元组到例程中)
            + [编译基础版例程](#编译基础版例程)
            + [运行基础版例程](#运行基础版例程)
        - [观察基础版例程](#观察基础版例程)
            + [基础版初始化说明](#基础版初始化说明)
            + [观察消息上报](#观察消息上报)
            + [观察消息下推](#观察消息下推)
            + [观察控制台日志](#观察控制台日志)
    * [2.4 体验物模型编程方式接入设备](#2.4 体验物模型编程方式接入设备)
        - [创建高级版产品和设备](#创建高级版产品和设备)
        - [为高级版设备创建物模型](#为高级版设备创建物模型)
        - [编译运行高级版例程](#编译运行高级版例程)
            + [下载设备物模型](#下载设备物模型)
            + [填写设备物模型到例程中](#填写设备物模型到例程中)
            + [填写设备三元组到例程中](#填写设备三元组到例程中)
            + [编译高级版例程](#编译高级版例程)
            + [运行高级版例程](#运行高级版例程)
        - [观察高级版例程](#观察高级版例程)
            + [高级版初始化说明](#高级版初始化说明)
            + [观察属性的上报](#观察属性的上报)
            + [观察属性的设置](#观察属性的设置)
            + [观察事件的上报](#观察事件的上报)
            + [观察服务的下发](#观察服务的下发)


# <a name="第二章 快速开始">第二章 快速开始</a>

本章描述如何申请自己的设备, 并结合C-SDK快速体验该设备通过`MQTT`协议以topic的方式和通过物模型的编程方式, 上报和接收业务报文.

## <a name="2.1 准备本地开发环境">2.1 准备本地开发环境</a>

### <a name="安装Ubuntu16.04">安装Ubuntu16.04</a>

本SDK的编译环境是**64位**主机上的`Ubuntu16.04`, 在其它Linux上尚未测试过, 所以推荐安装与阿里一致的发行版

如果您使用`Windows`操作系统, 建议安装虚拟机软件`Virtualbox`, 下载地址: [https://www.virtualbox.org/wiki/Downloads](https://www.virtualbox.org/wiki/Downloads)

然后安装64位的Desktop版本的`Ubuntu 16.04.x LTS`, 下载地址: [http://releases.ubuntu.com/16.04](http://releases.ubuntu.com/16.04)

### <a name="安装必备软件">安装必备软件</a>

本SDK的开发编译环境使用如下软件: `make-4.1`, `git-2.7.4`, `gcc-5.4.0`, `gcov-5.4.0`, `lcov-1.12`, `bash-4.3.48`, `tar-1.28`, `mingw-5.3.1`

可使用如下命令行安装必要的软件:

    apt-get install -y build-essential make git gcc

## <a name="2.2 准备云端控制台环境">2.2 准备云端控制台环境</a>
### <a name="注册/登录阿里云账号">注册/登录阿里云账号</a>

访问阿里云[登录页面](https://account.aliyun.com/login/login.htm), 点击[免费注册](https://account.aliyun.com/register/register.htm), 免费获得一个阿里云账号. 若您已有账号, 可直接登录

### <a name="访问物联网套件控制台">访问物联网套件控制台</a>

登入之后, 鼠标悬停在**产品**上, 弹出层叠菜单, 并单击**物联网平台**

> *注: 本处以在物联网平台创建产品作为示例, 用户也可以在阿里云IoT的其它物联网平台, 比如[生活物联网平台](https://living.aliyun.com), 上进行产品创建*

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/%E4%BA%A7%E5%93%81-%E7%89%A9%E8%81%94%E7%BD%91%E5%B9%B3%E5%8F%B0.png)

然后, 点击**立即开通**, 即可进入IoT控制台

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/%E4%BA%A7%E5%93%81-%E9%98%BF%E9%87%8C%E4%BA%91Link%20Platform-%E7%AB%8B%E5%8D%B3%E5%BC%80%E9%80%9A.png)

## <a name="2.3 体验MQTT Topic编程方式接入设备">2.3 体验MQTT Topic编程方式接入设备</a>

> MQTT协议是IBM开发的一个即时通讯协议, 是为大量计算能力有限, 且工作在低带宽, 不可靠的网络的远程传感器和控制设备通讯而设计的协议
>
> 利用MQTT协议是一种基于二进制消息的发布/订阅编程模式的消息协议
>
> 本节将要介绍的例子程序先在阿里云IoT平台订阅(`Subscribe`)一个`Topic`成功, 然后自己向该`Topic`做发布(`Publish`)动作
>
> 阿里云IoT平台收到被发布的消息之后, 就会将该消息原样推送回给例子程序
>
> 因为该程序之前已经通过订阅(`Subscribe`)动作成为该`Topic`的一个接收者, 发布到这个`Topic`上的任何消息, 都会被推送到已订阅该`Topic`的所有终端上

### <a name="创建基础版产品和设备">创建基础版产品和设备</a>

进入IoT控制台后, 点击页面左侧导航栏的**产品管理**, 再点击右侧的**创建产品**, 如下图所示:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E5%88%9B%E5%BB%BA%E4%BA%A7%E5%93%81.png)

在弹出的创建产品中, 选择基础版(也即直接对topic进行消息发布或者订阅), 填写产品信息:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E5%88%9B%E5%BB%BA%E4%BA%A7%E5%93%81-%E8%AF%A6%E7%BB%86%E5%8F%82%E6%95%B0.png)

填写**产品名称**, 选择**节点类型**.

填写好产品信息后, 点击**确认**即可生成该产品:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E5%88%9B%E5%BB%BA%E4%BA%A7%E5%93%81-%E7%A1%AE%E8%AE%A4.png)

点击产品右侧的**查看**, 可跳转到产品详情页面:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85.png)

在该页面中, 有四个主要的选项卡:

- 产品信息: 展示产品相关信息, 其中ProductKey用于标示产品的品类, 该产品下所有设备的ProductKey均一致
- 消息通信: 展示产品用于上下行数据的主要Topic
- 服务端订阅: 在这里可以选择设备消息类型并推送到MNS队列中, 用户服务端从队列里获得设备数据. 这样简化服务端订阅设备数据的流程, 让客户的服务端能够简单方便并高可靠的获得设备数据.
- 日志服务: 此处可浏览设备的历史上下行消息

产品创建好后, 接下来可以在该产品/品类下创建设备实例了

点击**产品详情**页面中**设备数**旁的**前往管理**, 即可看到当前产品下的设备列表, 目前为空:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E7%AE%A1%E7%90%86.png)

点击上图右侧的**添加设备**, 开始创建设备:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E7%AE%A1%E7%90%86-%E6%B7%BB%E5%8A%A0%E8%AE%BE%E5%A4%87.png)

在填写好**DeviceName**后, 点击确认即可创建该设备, 生成设备的**三元组**:

- `ProductKey`: 标识产品的品类, 相同产品的所有设备ProductKey均相同
- `DeviceName`: 标识产品下的每个设备, 相同产品的所有设备DeviceName均不相同
- `DeviceSecret`: 设备密钥, 每个设备均不相同

*三元组用于标识阿里云上的每个设备, 用于连接阿里云服务器时完成设备认证*

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E7%AE%A1%E7%90%86-%E6%B7%BB%E5%8A%A0%E8%AE%BE%E5%A4%87-%E7%94%9F%E6%88%90%E4%B8%89%E5%85%83%E7%BB%84.png)

至此, 基础版产品与设备创建完成.

此外, 为了example演示, 我们在产品的消息通信中定义一个自定义的topic, 用于向服务端发送数据和从服务端接收数据:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81-%E6%B6%88%E6%81%AF%E9%80%9A%E4%BF%A1-%E8%87%AA%E5%AE%9A%E4%B9%89topic-data.png)

### <a name="编译运行基础版例程">编译运行基础版例程</a>

#### <a name="下载解压物联网平台设备端C-SDK">下载解压物联网平台设备端C-SDK</a>

以获取 `V2.2.0` 版本SDK为例, 可用如下命令行获取:

    cd ${HOME}
    mkdir srcs
    cd srcs
    wget https://linkkit-sdk-download.oss-cn-shanghai.aliyuncs.com/linkkit2.2.tar.gz
    tar xvf linkkit2.2.tar.gz

如下则在 `~/srcs/iotx-sdk-c` 目录得到了解压之后的C-SDK

    cd iotx-sdk-c/
    ls
    aos.makefile  build-rules  CMakeLists.txt  doc  examples  include  LICENSE  makefile  make.settings  prebuilt  project.mk  README.md  src  tests

#### <a name="填写设备三元组到例程中">填写设备三元组到例程中</a>

打开文件 `iotx-sdk-c/examples/mqtt/mqtt-example.c`, 编辑如下代码段, 填入之前[创建基础版产品和设备](#创建基础版产品和设备)步骤中得到的**设备三元组**:

```
#if defined(SUPPORT_ITLS)

...
...

#else

    #if defined(ON_DAILY)
        ...
        ...
    #else
        #define PRODUCT_KEY             "a1ExpAkj9Hi"
        #define DEVICE_NAME             "Example1"
        #define DEVICE_SECRET           "cNzcn2Lbqzh4UiXKLwW77hxI9GFmcRgb"
    #endif

#endif
```

#### <a name="编译基础版例程">编译基础版例程</a>

运行如下命令:

    cd ~/srcs/iotx-sdk-c
    make distclean
    make

编译成功完成后, 生成的样例程序在当前路径的 `output/release/bin` 目录下:

    $ tree output/release
    output/release/
    +-- bin
    ...
    ...
    |   +-- mqtt-example
    ...
    ...

#### <a name="运行基础版例程">运行基础版例程</a>

执行如下命令:

    cd ~/srcs/iotx-sdk-c
    ./output/release/bin/mqtt-example

得到运行结果如下:

    [inf] iotx_device_info_init(39): device_info created successfully!
    [dbg] iotx_device_info_set(49): start to set device info!
    [dbg] iotx_device_info_set(63): device_info set successfully!
    [inf] guider_print_dev_guider_info(279): ....................................................
    [inf] guider_print_dev_guider_info(280):           ProductKey : a1ExpAkj9Hi
    [inf] guider_print_dev_guider_info(281):           DeviceName : Example1
    [inf] guider_print_dev_guider_info(282):             DeviceID : a1ExpAkj9Hi.Example1
    [inf] guider_print_dev_guider_info(284): ....................................................
    [inf] guider_print_dev_guider_info(285):        PartnerID Buf : ,partner_id=example.demo.partner-id
    [inf] guider_print_dev_guider_info(286):         ModuleID Buf : ,module_id=example.demo.module-id
    [inf] guider_print_dev_guider_info(287):           Guider URL :
    [inf] guider_print_dev_guider_info(289):       Guider SecMode : 2 (TLS + Direct)
    [inf] guider_print_dev_guider_info(291):     Guider Timestamp : 2524608000000
    [inf] guider_print_dev_guider_info(292): ....................................................
    [inf] guider_print_dev_guider_info(298): ....................................................
    [inf] guider_print_conn_info(256): -----------------------------------------
    [inf] guider_print_conn_info(257):             Host : a1ExpAkj9Hi.iot-as-mqtt.cn-shanghai.aliyuncs.com
    [inf] guider_print_conn_info(258):             Port : 1883
    [inf] guider_print_conn_info(261):         ClientID : a1ExpAkj9Hi.Example1|securemode=2,timestamp=2524608000000,signmethod=hmacsha1 ...
    [inf] guider_print_conn_info(263):       TLS PubKey : 0x437636 ('-----BEGIN CERTI ...')
    [inf] guider_print_conn_info(266): -----------------------------------------
    [inf] IOT_MQTT_Construct(3005):      CONFIG_MQTT_SUBTOPIC_MAXNUM : 65535
    [dbg] IOT_MQTT_Construct(3007): sizeof(iotx_mc_client_t) = 1573144!
    [inf] iotx_mc_init(2098): MQTT init success!
    [inf] _ssl_client_init(142): Loading the CA root certificate ...
    [inf] _ssl_client_init(149):  ok (0 skipped)
    [inf] _TLSConnectNetwork(315): Connecting to /a1ExpAkj9Hi.iot-as-mqtt.cn-shanghai.aliyuncs.com/1883...
    [inf] mbedtls_net_connect_timeout(257): setsockopt SO_SNDTIMEO timeout: 10s
    [inf] mbedtls_net_connect_timeout(260): connecting IP_ADDRESS: 106.15.100.2
    [inf] _TLSConnectNetwork(328):  ok
    [inf] _TLSConnectNetwork(333):   . Setting up the SSL/TLS structure...
    [inf] _TLSConnectNetwork(343):  ok
    [inf] _TLSConnectNetwork(382): Performing the SSL/TLS handshake...
    [inf] _TLSConnectNetwork(390):  ok
    [inf] _TLSConnectNetwork(394):   . Verifying peer X.509 certificate..
    [inf] _real_confirm(90): certificate verification result: 0x00
    [wrn] MQTTConnect(204): NOT USING pre-malloced buf 0xc4a010, malloc per packet
    [dbg] MQTTConnect(204): ALLOC: curr buf = 0xc56890, curr buf_size = 320, required payload_len = 256
    [dbg] MQTTConnect(224): FREED: curr buf = (nil), curr buf_size = 0
    [inf] iotx_mc_connect(2449): mqtt connect success!
    ...
    ...
    mqtt_client|309 :: packet-id=7, publish topic msg={"attr_name":"temperature", "attr_value":"1"}
    [dbg] iotx_mc_cycle(1591): PUBACK
    event_handle|132 :: publish success, packet-id=7
    [dbg] iotx_mc_cycle(1608): PUBLISH
    [dbg] iotx_mc_handle_recv_PUBLISH(1412):         Packet Ident : 00035641
    [dbg] iotx_mc_handle_recv_PUBLISH(1413):         Topic Length : 26
    [dbg] iotx_mc_handle_recv_PUBLISH(1417):           Topic Name : /a1ExpAkj9Hi/Example1/data
    [dbg] iotx_mc_handle_recv_PUBLISH(1420):     Payload Len/Room : 45 / 992
    [dbg] iotx_mc_handle_recv_PUBLISH(1421):       Receive Buflen : 1024
    [dbg] iotx_mc_handle_recv_PUBLISH(1432): delivering msg ...
    [dbg] iotx_mc_deliver_message(1170): topic be matched
    _demo_message_arrive|166 :: ----
    _demo_message_arrive|167 :: packetId: 35641
    _demo_message_arrive|171 :: Topic: '/a1ExpAkj9Hi/Example1/data' (Length: 26)
    _demo_message_arrive|175 :: Payload: '{"attr_name":"temperature", "attr_value":"1"}' (Length: 45)
    _demo_message_arrive|176 :: ----
    ...
    ...
    main|361 :: out of sample!

### <a name="观察基础版例程">观察基础版例程</a>

#### <a name="基础版初始化说明">基础版初始化说明</a>
基础版示例代码位于 `examples/mqtt/mqtt-example.c`

    int mqtt_client(void)
    {
        ...
        ...

在与服务器尝试建立MQTT连接前, 需要进行设备认证

        if (0 != IOT_SetupConnInfo(__product_key, __device_name, __device_secret, (void **)&pconn_info)) {
            EXAMPLE_TRACE("AUTH request failed!");
            rc = -1;
            goto do_exit;
        }

        /* Initialize MQTT parameter */
        ...
        ...

尝试建立与服务器的MQTT连接

        pclient = IOT_MQTT_Construct(&mqtt_params);
        if (NULL == pclient) {
            EXAMPLE_TRACE("MQTT construct failed");
            rc = -1;
            goto do_exit;
        }

尝试向"/a1ExpAkj9Hi/Example1/update"这个topic上报消息

        ...
        ...
        topic_msg.payload = (void *)msg_pub;
        topic_msg.payload_len = strlen(msg_pub);

        rc = IOT_MQTT_Publish(pclient, TOPIC_UPDATE, &topic_msg);
        if (rc < 0) {
            IOT_MQTT_Destroy(&pclient);
            EXAMPLE_TRACE("error occur when publish");
            rc = -1;
            goto do_exit;
        }

        EXAMPLE_TRACE("\n publish message: \n topic: %s\n payload: \%s\n rc = %d", TOPIC_UPDATE, topic_msg.payload, rc);

订阅我们刚才在云端控制台上定义的"/a1ExpAkj9Hi/Example1/data"这个topic

        rc = IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
        if (rc < 0) {
            IOT_MQTT_Destroy(&pclient);
            EXAMPLE_TRACE("IOT_MQTT_Subscribe() failed, rc = %d", rc);
            rc = -1;
            goto do_exit;
        }

        IOT_MQTT_Yield(pclient, 200);

        HAL_SleepMs(2000);

        ...
        ...

尝试向"/a1ExpAkj9Hi/Example1/data"这个topic上报消息

        do {
            cnt++;
            msg_len = snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
            ...
            ...

            rc = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
            if (rc < 0) {
                EXAMPLE_TRACE("error occur when publish");
                rc = -1;
                break;
            }
            EXAMPLE_TRACE("packet-id=%u, publish topic msg=%s", (uint32_t)rc, msg_pub);

处理收到的MQTT消息并调用订阅时注册的回调函数

            IOT_MQTT_Yield(pclient, 200);

            /* infinite loop if running with 'loop' argument */
            if (user_argc >= 2 && !strcmp("loop", user_argv[1])) {
                HAL_SleepMs(2000);
                cnt = 0;
            }

        } while (cnt < 1);

        ...
        ...

        return rc;
    }

初始化日志系统, 将SDK日志级别设置为Debug

    int main(int argc, char **argv)
    {
        IOT_OpenLog("mqtt");
        IOT_SetLogLevel(IOT_LOG_DEBUG);

        ...
        ...

设置设备三元组到HAL中

        HAL_SetProductKey(PRODUCT_KEY);
        HAL_SetDeviceName(DEVICE_NAME);
        HAL_SetDeviceSecret(DEVICE_SECRET);

        ...

选择要登录的服务器

        int domain_type = IOTX_CLOUD_DOMAIN_SH;
        IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

选择直连设备是否需要使用一型一密

        int dynamic_register = 0;
        IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

开始例程

        mqtt_client();

打印SDK内存占用信息, 仅在Linux平台上当WITH_MEM_STATS=1时有效

        IOT_DumpMemoryStats(IOT_LOG_DEBUG);

关闭日志系统

        IOT_CloseLog();

        EXAMPLE_TRACE("out of sample!");

        return 0;
    }

#### <a name="观察消息上报">观察消息上报</a>

在执行`mqtt-example`示例程序时, 如果需要改变上报值, 只需修改如下代码片段即可:
```
examples/mqtt/mqtt-example.c
...
...
    do {
        /* Generate topic message */
        cnt++;
        msg_len = snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            EXAMPLE_TRACE("Error occur! Exit program");
            rc = -1;
            break;
        }

        topic_msg.payload = (void *)msg_pub;
        topic_msg.payload_len = msg_len;

        rc = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
        if (rc < 0) {
            EXAMPLE_TRACE("error occur when publish");
            rc = -1;
            break;
        }
...
...
上述代码中, msg_pub的值即为上报到topic: `/a1ExpAkj9Hi/Example1/data` 的值
```

如下日志信息显示样例程序正在通过`MQTT`的`Publish`类型消息, 上报业务数据到`/${prodcutKey}/${deviceName}/data`

    mqtt_client|309 :: packet-id=7, publish topic msg={"attr_name":"temperature", "attr_value":"1"}

#### <a name="观察消息下推">观察消息下推</a>

当SDK收到服务器的下推信息时, 会进入如下回调函数:
```
static void _demo_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    /* print topic name and topic message */
    EXAMPLE_TRACE("----");
    EXAMPLE_TRACE("packetId: %d", ptopic_info->packet_id);
    EXAMPLE_TRACE("Topic: '%.*s' (Length: %d)",
                  ptopic_info->topic_len,
                  ptopic_info->ptopic,
                  ptopic_info->topic_len);
    EXAMPLE_TRACE("Payload: '%.*s' (Length: %d)",
                  ptopic_info->payload_len,
                  ptopic_info->payload,
                  ptopic_info->payload_len);
    EXAMPLE_TRACE("----");
}
```

如下日志信息显示该消息因为是到达已被订阅的`Topic`, 所以又被服务器原样推送到基础版例程, 并进入相应的回调函数

    _demo_message_arrive|166 :: ----
    _demo_message_arrive|167 :: packetId: 35641
    _demo_message_arrive|171 :: Topic: '/a1ExpAkj9Hi/Example1/data' (Length: 26)
    _demo_message_arrive|175 :: Payload: '{"attr_name":"temperature", "attr_value":"1"}' (Length: 45)
    _demo_message_arrive|176 :: ----

#### <a name="观察控制台日志">观察控制台日志</a>

可以登录物联网套件控制台, 到**产品管理**, 找到刚才创建的产品, 点击**查看**, 选择**日志服务**选项卡, 可以看到刚才上报的消息(Message ID为7)

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E6%97%A5%E5%BF%97%E6%9C%8D%E5%8A%A1.png)

## <a name="2.4 体验物模型编程方式接入设备">2.4 体验物模型编程方式接入设备</a>

### <a name="创建高级版产品和设备">创建高级版产品和设备</a>
进入IoT控制台后, 点击页面左侧导航栏的**产品管理**, 再点击右侧的**创建产品**, 如下图所示:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E5%88%9B%E5%BB%BA%E4%BA%A7%E5%93%81.png)

在弹出的创建产品中, 选择高级版(即物模型), 填写产品信息:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E5%88%9B%E5%BB%BA%E4%BA%A7%E5%93%81-%E8%AF%A6%E7%BB%86%E5%8F%82%E6%95%B0.png)

在此需要填写以下信息:

- 填写**产品名称**
- 选择**节点类型**
- 选择**设备类型**, 如果没有需要的设备类型, 可以填空, 稍后在产品**功能定义**时再定义具体的功能
- 选择**数据格式**, 目前有**Alink JSON**和**透传/自定义**两种格式可选(在这里我们使用Alink JSON格式作为示例)
    + **Alink JSON**: 使用标准的Alink协议格式来进行物联网套件与云端的服务/属性/事件的数据交换
    + **透传/自定义**: 使用物联网套件的透传接口来进行与云端的数据交换, 在云端需要用户完成透传数据与Alink格式的转换脚本
- **ID2**: 是否使用ID2认证

填写好产品信息后, 点击**确认**即可生成该产品:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E5%88%9B%E5%BB%BA%E4%BA%A7%E5%93%81-%E7%A1%AE%E8%AE%A4.png)

点击产品右侧的**查看**, 可跳转到产品详情页面:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85.png)

在该页面中, 有五个主要的选项卡:

- 产品信息: 展示产品相关信息, 其中ProductKey用于标示产品的品类, 该产品下所有设备的ProductKey均一致
- 消息通信: 展示产品用于上下行数据的主要Topic
- 功能定义: 在这里可以定义设备的物模型, 定义物的服务/属性/事件
- 日志服务: 此处可浏览设备的历史上下行消息
- 在线调试: 此处可对该产品下的设备进行在线调试, 如下发服务到设备, 设置设备的属性, 观察设备的事件上报

产品创建好后, 接下来可以创建设备了, 点击**产品详情**页面中**设备数**旁的**前往管理**, 即可看到当前产品下的设备列表, 目前为空:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E7%AE%A1%E7%90%86.png)

点击上图右侧的**添加设备**, 开始创建设备:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E7%AE%A1%E7%90%86-%E6%B7%BB%E5%8A%A0%E8%AE%BE%E5%A4%87.png)

在填写好**DeviceName**后, 点击确认即可创建该设备, 生成设备的**三元组**:

- `ProductKey`: 标识产品的品类, 相同产品的所有设备ProductKey均相同
- `DeviceName`: 标识产品下的每个设备, 相同产品的所有设备DeviceName均不相同
- `DeviceSecret`: 设备密钥, 每个设备均不相同

*三元组用于标识阿里云上的每个设备, 用于连接阿里云服务器时完成设备认证*

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E7%AE%A1%E7%90%86-%E6%B7%BB%E5%8A%A0%E8%AE%BE%E5%A4%87-%E7%94%9F%E6%88%90%E4%B8%89%E5%85%83%E7%BB%84.png)

至此, 高级版产品与设备创建完成.

### <a name="为高级版设备创建物模型">为高级版设备创建物模型</a>

> 在高级版物联网平台中, 用户可以将物理空间中的实体, 例如各类传感器, 或者由传感器组成的设备等进行数字化, 并在云端构建该实体的数据模型
>
> 通过定义物模型(TSL)来对设备是什么, 能做什么, 可以对外提供哪些服务进行描述, 定义好物模型(TSL)也就定义好了产品功能

稍后会展示高级版服务/属性/事件的示例程序, 所以在这里我们回到**产品管理**, 选择我们刚才创建的产品, 进入**产品详情**页, 选择**功能定义**选项卡:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E5%8A%9F%E8%83%BD%E5%AE%9A%E4%B9%89.png)

选择右侧的**新增**, 我们先创建一个属性:

该属性是一个字符串属性, 最大长度为2048个字节

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E5%8A%9F%E8%83%BD%E5%AE%9A%E4%B9%89-%E6%96%B0%E5%A2%9E%E5%AD%97%E7%AC%A6%E4%B8%B2%E5%B1%9E%E6%80%A7.png)

点击**确认**, 这样一个属性就创建好了

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E5%8A%9F%E8%83%BD%E5%AE%9A%E4%B9%89-%E6%96%B0%E5%A2%9E%E5%AD%97%E7%AC%A6%E4%B8%B2%E5%B1%9E%E6%80%A7-%E5%88%9B%E5%BB%BA%E6%88%90%E5%8A%9F.png)

接下来我们创建一个服务:

关于服务的创建要注意的是, 服务只能由服务端下发, 设备端被动接收, 每个服务有自己的输入参数和输出参数, 说明如下:

- 服务的**输入参数**: 指的是当从云端下发服务时, 云端向设备端发送的数据内容
- 服务的**输出参数**: 指的是当设备端收到从云端下发的服务时, 如果有需要对该服务返回一些业务数据, 那么就是输出参数的内容

在这个服务中我们创建一个输入参数和一个输出参数

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E5%8A%9F%E8%83%BD%E5%AE%9A%E4%B9%89-%E6%96%B0%E5%A2%9E%E6%9C%8D%E5%8A%A1.png)

最后, 再创建一个事件:

> 关于事件要注意的是, 事件只能由设备端主动上报给服务端, 每个事件只有输出参数, 表示从设备端需要上报的数据内容

在这个事件中我们创建一个输出参数

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E5%8A%9F%E8%83%BD%E5%AE%9A%E4%B9%89-%E6%96%B0%E5%A2%9E%E4%BA%8B%E4%BB%B6.png)

至此, 我们创建的产品中服务/属性/事件各有一个:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E5%8A%9F%E8%83%BD%E5%AE%9A%E4%B9%89-%E5%AE%8C%E6%88%90.png)

### <a name="编译运行高级版例程">编译运行高级版例程</a>

高级版以单品API为例演示设备服务/属性/事件的上报和下行

首先参考[下载解压物联网平台设备端C-SDK](#下载解压物联网平台设备端C-SDK)一节, 下载及解压得到C-SDK的源码, 然后再执行以下环节

#### <a name="下载设备物模型">下载设备物模型</a>
高级版单品的exmaple位于 `iotx-sdk-c/examples/linkkit/linkkit_deprecated_example_solo.c`, 该example用到的TSL位于同目录下的`example_tsl_solo.data`.

接下来我们需要将SDK默认的设备信息更换成我们在前一章节中创建的高级版设备, 需要替换设备的三元组以及设备的物模型描述(TSL)

进入**产品管理**, 选择我们刚才创建的设备`AdvUserExample`, 进入`产品详情`, 选择`功能定义`选项卡, 这里可以看到之前定义的服务/属性和事件.

点击右侧的`查看物模型`, 如下图所示:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E7%89%A9%E6%A8%A1%E5%9E%8B.png)

点击`导出模型文件`将物模型导出, 则可以下载到JSON格式的, 设备的物模型(TSL)描述文件, **model.json**

#### <a name="填写设备物模型到例程中">填写设备物模型到例程中</a>

运行如下命令:

    make distclean
    make

在 `iotx-sdk-c/output/release/bin` 目录下会产生一个转义工具 `linkkit_tsl_convert`, 将刚才下载的物模型文件**model.json**拷贝到这里, 执行如下命令:

    $ ./linkkit_tsl_convert -i model.json

命令执行成功后会在当前目录下产生一个`conv.txt`文件, 这个就是转义好的物模型字符串了. (转义工具网上很多, 也可自行寻找)

用`conv.txt`文件中的字符串替换`iotx-sdk-c/examples/linkkit/example_tsl_solo.data`中变量`TSL_STRING`的字符串即可.

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E5%8D%95%E5%93%81%E6%9B%BF%E6%8D%A2TSL.png)

#### <a name="填写设备三元组到例程中">填写设备三元组到例程中</a>
将 `iotx-sdk-c/examples/linkkit/linkkit_deprecated_example_solo.c` 中的三元组替换成刚才创建的设备

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E5%8D%95%E5%93%81%E6%9B%BF%E6%8D%A2%E4%B8%89%E5%85%83%E7%BB%84.png)

这样三元组和TSL都已换成我们刚才创建的设备.

之前我们在[为高级版设备创建物模型](#为高级版设备创建物模型)一节中, 定义了由一个服务, 一个属性和一个事件组成的物模型(TSL)

这些与当前 `linkkit_deprecated_example_solo.c` 中, 编写回调函数时对应的物模型(TSL)相同, 所以不需要对回调结构体 `linkkit_ops`, 及其成员回调函数做出修改.

#### <a name="编译高级版例程">编译高级版例程</a>

运行如下命令:

    $ make distclean
    $ make

编译成功完成后, 生成的高级版例子程序在当前路径的 `output/release/bin` 目录下, 名为`linkkit-example-solo`

#### <a name="运行高级版例程">运行高级版例程</a>
执行如下命令:

    cd ~/srcs/iotx-sdk-c
    ./output/release/bin/linkkit-example-solo

得到运行结果如下:

    linkkit_example|587 :: linkkit start
    [dbg] _dm_mgr_search_dev_by_devid(52): Device Not Found, devid: 0
    [inf] iotx_cm_init(77): cm verstion 0.3
    [inf] iotx_device_info_init(39): device_info created successfully!
    [dbg] iotx_device_info_set(49): start to set device info!
    [dbg] iotx_device_info_set(63): device_info set successfully!
    [inf] guider_print_dev_guider_info(279): ....................................................
    [inf] guider_print_dev_guider_info(280):           ProductKey : a1csED27mp7
    [inf] guider_print_dev_guider_info(281):           DeviceName : AdvExample1
    [inf] guider_print_dev_guider_info(282):             DeviceID : a1csED27mp7.AdvExample1
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
    [inf] guider_print_conn_info(261):         ClientID : a1csED27mp7.AdvExample1|securemode=2,timestamp=2524608000000,signmethod=hmacsha1 ...
    [inf] guider_print_conn_info(263):       TLS PubKey : 0x47ffd6 ('-----BEGIN CERTI ...')
    [inf] guider_print_conn_info(266): -----------------------------------------
    [inf] iotx_cm_init(126): cm context initialize
    [inf] linked_list_insert(120): linked list(cm event_cb list) insert node@0x18a2760,size:1
    [inf] linked_list_insert(120): linked list(cm connectivity list) insert node@0x18a28c0,size:1
    [err] iotx_cm_add_connectivity(113): Add Connectivity Success, Type: 1
    [inf] iotx_cm_conn_mqtt_init(358):            CONFIG_MQTT_TX_MAXLEN : 1024
    [inf] iotx_cm_conn_mqtt_init(359):            CONFIG_MQTT_RX_MAXLEN : 5000
    [wrn] iotx_cm_conn_mqtt_init(369): WITH_MQTT_DYNBUF = 1, skipping malloc sendbuf of 1024 bytes
    [inf] IOT_MQTT_Construct(3005):      CONFIG_MQTT_SUBTOPIC_MAXNUM : 65535
    [dbg] IOT_MQTT_Construct(3007): sizeof(iotx_mc_client_t) = 1573144!
    [inf] iotx_mc_init(2098): MQTT init success!
    [inf] _ssl_client_init(142): Loading the CA root certificate ...
    [inf] _ssl_client_init(149):  ok (0 skipped)
    [inf] _TLSConnectNetwork(315): Connecting to /a1csED27mp7.iot-as-mqtt.cn-shanghai.aliyuncs.com/1883...
    [inf] mbedtls_net_connect_timeout(257): setsockopt SO_SNDTIMEO timeout: 10s
    [inf] mbedtls_net_connect_timeout(260): connecting IP_ADDRESS: 106.15.100.2
    [inf] _TLSConnectNetwork(328):  ok
    [inf] _TLSConnectNetwork(333):   . Setting up the SSL/TLS structure...
    [inf] _TLSConnectNetwork(343):  ok
    [inf] _TLSConnectNetwork(382): Performing the SSL/TLS handshake...
    [inf] _TLSConnectNetwork(390):  ok
    [inf] _TLSConnectNetwork(394):   . Verifying peer X.509 certificate..
    [inf] _real_confirm(90): certificate verification result: 0x00
    [dbg] MQTTConnect(204): ALLOC: curr buf = 0x18af8f0, curr buf_size = 320, required payload_len = 256
    [dbg] MQTTConnect(224): FREED: curr buf = (nil), curr buf_size = 0
    [inf] iotx_mc_connect(2449): mqtt connect success!
    ...
    ...

### <a name="观察高级版例程">观察高级版例程</a>

#### <a name="高级版初始化说明">高级版初始化说明</a>

int linkkit_example()
{
    ...
    ...

用户回调函数, 高级版产生事件时, 会自动调用用户的回调函数

    linkkit_ops_t linkkit_ops = {
        .on_connect           = on_connect,            /* connect handler */
        .on_disconnect        = on_disconnect,         /* disconnect handler */
        .raw_data_arrived     = raw_data_arrived,      /* receive raw data handler */
        .thing_create         = thing_create,          /* thing created handler */
        .thing_enable         = thing_enable,          /* thing enabled handler */
        .thing_disable        = thing_disable,         /* thing disabled handler */
        .thing_call_service   = thing_call_service,    /* self-defined service handler */
        .thing_prop_changed   = thing_prop_changed,    /* property set handler */
        .linkit_data_arrived  = linkit_data_arrived,   /* transparent transmission data handler */
    };

    EXAMPLE_TRACE("linkkit start");

    /*
     * linkkit start
     * max_buffered_msg = 16, set the handle msg max numbers.
     *     if it is enough memory, this number can be set bigger.
     * if get_tsl_from_cloud = 0, it will use the default tsl [TSL_STRING]; if get_tsl_from_cloud =1, it will get tsl from cloud.
     */

启动Linkkit, 开始尝试与云端建立连接

    if (-1 == linkkit_start(16, get_tsl_from_cloud, linkkit_loglevel_debug, &linkkit_ops, linkkit_cloud_domain_shanghai,
                            &sample_ctx)) {
        EXAMPLE_TRACE("linkkit start fail");
        return -1;
    }

如果没有选择从云端动态拉取TSL, 那么在这里本地静态设置TSL

    if (!get_tsl_from_cloud) {
        /*
         * if get_tsl_from_cloud = 0, set default tsl [TSL_STRING]
         * please modify TSL_STRING by the TSL's defined.
         */
        linkkit_set_tsl(TSL_STRING, strlen(TSL_STRING));
    }

初始化Config OTA的回调函数

    linkkit_cota_init(linkkit_cota_callback);

初始化Firmware OTA的回调函数

    linkkit_fota_init(linkkit_fota_callback);

    EXAMPLE_TRACE("linkkit enter loop");

此循环中尝试对设备的属性和事件进行上报

    while (1) {
        ...
        ...
    }

高级版SDK正常退出

        linkkit_end();

        return 0;
    }

    int main(int argc, char **argv)
    {

初始化日志系统, 将SDK日志级别设置为Debug

        IOT_OpenLog("linkkit");
        IOT_SetLogLevel(IOT_LOG_DEBUG);

        EXAMPLE_TRACE("start!\n");

设置设备三元组到HAL中

        HAL_SetProductKey("a13Npv1vjZ4");
        HAL_SetDeviceName("example_zc");
        HAL_SetDeviceSecret("ZlexLJ4G0aXiSwkGmUFWuZBLLySKcG8h");

        /*
         * linkkit dome
         * please check document: https://help.aliyun.com/document_detail/73708.html
         *         API introduce: https://help.aliyun.com/document_detail/68687.html
         */
        linkkit_example();

打印SDK内存占用信息, 仅在Linux平台上当编译宏开关WITH_MEM_STATS=1时有效

        IOT_DumpMemoryStats(IOT_LOG_DEBUG);

关闭日志系统

        IOT_CloseLog();

        EXAMPLE_TRACE("out of sample!\n");

        return 0;
    }


#### <a name="观察属性的上报">观察属性的上报</a>

示例中使用`linkkit_post_property`上报属性:
```
/* 此函数在示例代码中每30s被调用一次 */
int post_all_prop(sample_context_t *sample)
{
    /* 这里第二个参数填NULL表示上报所有属性 */
    return linkkit_post_property(sample->thing, NULL, post_property_cb);
}
```

对于属性(Property), 示例程序会每隔30s上报(Post)一次所有属性, 因为我们定义了一个属性 `DeviceStatus`, 所以应该看到如下日志:

    ```
    [dbg] iotx_dm_post_property_end(450): Current Property Post Payload, Length: 19, Payload: {"DeviceStatus":""}
    [dbg] _dm_mgr_search_dev_by_devid(46): Device Found, devid: 0
    [inf] dm_msg_request_all(265): DM Send Message, URI: /sys/a1csED27mp7/AdvExample1/thing/event/property/post, Payload: {"id":"1","version":"1.0","params":{"DeviceStatus":""},"method":"thing.event.property.post"}
    [inf] iotx_cm_conn_mqtt_publish(531): mqtt publish: topic=/sys/a1csED27mp7/AdvExample1/thing/event/property/post, topic_msg={"id":"4","version":"1.0","params":{"DeviceStatus":""},"method":"thing.event.property.post"}
    ```

可以看出, 由于我们尚未设置该属性的值, 所以默认该值为空字符串, 此时在物联网控制台上可以查询到刚才的上报记录:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E6%97%A5%E5%BF%97%E6%9C%8D%E5%8A%A1-%E5%B1%9E%E6%80%A7%E4%B8%8A%E6%8A%A5.png)

从上图可以看出, 一条 `Property Post` 消息已上报至服务端

#### <a name="观察属性的设置">观察属性的设置</a>

在高级版中, 也可从服务端主动向这个属性(Property)设置(Set)一个值

打开`产品管理`->`产品详情`->`在线调试选项卡`, 选择我们要调试的设备:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E8%B0%83%E8%AF%95-%E8%AE%BE%E7%BD%AE%E5%B1%9E%E6%80%A7.png)

如上图所示, 选择 `DeviceStatus` 这个属性, 然后选择**设置**, 在下方的输入框中将"Hello World"填入该属性的值, 然后点击发送指令,

在示例代码中当收到属性set请求时, 会进入如下回调函数:
```
static int thing_prop_changed(const void *thing_id, const char *property, void *ctx) {
    ...
    ...

    /* 这里将被设置的属性值发送到云端, 这样在云端控制台才能看到属性的新值 */
    response_id = linkkit_post_property(thing_id, property, post_property_cb);

    EXAMPLE_TRACE("post property(%s) response id: %d\n", property, response_id);
}
```

此时在设备端的日志中可以看到从服务端set下来的值:

    [dbg] iotx_mc_cycle(1608): PUBLISH
    [dbg] iotx_mc_handle_recv_PUBLISH(1412):         Packet Ident : 00000000
    [dbg] iotx_mc_handle_recv_PUBLISH(1413):         Topic Length : 55
    [dbg] iotx_mc_handle_recv_PUBLISH(1417):           Topic Name : /sys/a1csED27mp7/AdvExample1/thing/service/property/set
    [dbg] iotx_mc_handle_recv_PUBLISH(1420):     Payload Len/Room : 112 / 4940
    [dbg] iotx_mc_handle_recv_PUBLISH(1421):       Receive Buflen : 5000
    [dbg] iotx_mc_handle_recv_PUBLISH(1432): delivering msg ...
    [dbg] iotx_mc_deliver_message(1170): topic be matched
    [inf] iotx_cloud_conn_mqtt_event_handle(180): event_type 12
    [inf] iotx_cloud_conn_mqtt_event_handle(325): mqtt received: topic=/sys/a1csED27mp7/AdvExample1/thing/service/property/set, topic_msg={"method":"thing.service.property.set","id":"65822254","params":{"DeviceStatus":"HelloWorld"},"version":"1.0.0"}

这样, 一条从服务端设置属性的命令就到达设备端了

又由于在例程里, 收到这条属性的设置指令之后, 会调用 `linkkit_post_property` 将该属性值上报给服务端, 所以有如下的日志打印:

    [dbg] iotx_dm_post_property_end(450): Current Property Post Payload, Length: 29, Payload: {"DeviceStatus":"HelloWorld"}
    [dbg] _dm_mgr_search_dev_by_devid(46): Device Found, devid: 0
    [inf] dm_msg_request_all(265): DM Send Message, URI: /sys/a1csED27mp7/AdvExample1/thing/event/property/post, Payload: {"id":"34","version":"1.0","params":{"DeviceStatus":"HelloWorld"},"method":"thing.event.property.post"}
    [inf] iotx_cm_conn_mqtt_publish(531): mqtt publish: topic=/sys/a1csED27mp7/AdvExample1/thing/event/property/post, topic_msg={"id":"34","version":"1.0","params":{"DeviceStatus":"HelloWorld"},"method":"thing.event.property.post"}

> 注意: 只有当设备端主动上报某个属性的值到服务端后, 才能在服务端查询到该属性的值.

在**设备调试**选项卡中, 选择 `DeviceStatus` 这个属性, 然后选择**获取**, 点击**发送指令**, 稍后在下方的输入框中就可以看到刚才设置的属性了:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E8%B0%83%E8%AF%95-%E8%8E%B7%E5%8F%96%E5%B1%9E%E6%80%A7.png)

#### <a name="观察事件的上报">观察事件的上报</a>

示例中使用`linkkit_post_property`上报属性:
```
/* 此函数在示例代码中每45s被调用一次 */
int trigger_event(sample_context_t *sample)
{
    char event_output_identifier[64];
    snprintf(event_output_identifier, sizeof(event_output_identifier), "%s.%s", EVENT_ERROR_IDENTIFIER,
             EVENT_ERROR_OUTPUT_INFO_IDENTIFIER);

    /* 设置Error事件的输出参数值 */
    int errorCode = 0;
    linkkit_set_value(linkkit_method_set_event_output_value,
                      sample->thing,
                      event_output_identifier,
                      &errorCode, NULL);

    /* 上报Error事件到云端 */
    return linkkit_trigger_event(sample->thing, EVENT_ERROR_IDENTIFIER, post_property_cb);
}
```

示例程序中 `Error` 事件(Event)是每45s上报一次, 日志如下:

    [inf] dm_msg_request_all(265): DM Send Message, URI: /sys/a1csED27mp7/AdvExample1/thing/event/Error/post, Payload: {"id":"36","version":"1.0","params":{"ErrorCode":0},"method":"thing.event.Error.post"}
    [inf] iotx_cm_conn_mqtt_publish(531): mqtt publish: topic=/sys/a1csED27mp7/AdvExample1/thing/event/Error/post, topic_msg={"id":"36","version":"1.0","params":{"ErrorCode":0},"method":"thing.event.Error.post"}

相应地, 在物联网控制台的**日志服务**选项卡中可以看到对应的信息:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E8%B0%83%E8%AF%95-%E4%BA%8B%E4%BB%B6%E4%B8%8A%E6%8A%A5.png)

在物联网控制台的**在线调试**选项卡中可以看到对应的信息:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E8%B0%83%E8%AF%95-%E8%8E%B7%E5%8F%96%E4%BA%8B%E4%BB%B6.png)

#### <a name="观察服务的下发">观察服务的下发</a>

在物联网控制台中打开**设备调试**选项卡, 选择我们创建的服务`Custom`

由于该服务的输入参数数据类型为int型, 标识为 `transparency`, 所以在下方的输入框中填入参数, 并点击**发送指令**:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-ADV-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E8%AE%BE%E5%A4%87%E8%B0%83%E8%AF%95-%E6%9C%8D%E5%8A%A1%E4%B8%8B%E5%8F%91.png)

在设备端示例程序中, 当收到服务调用请求时, 会进入如下回调函数:
```
#ifdef RRPC_ENABLED
    static int thing_call_service(const void *thing_id, const char *service, int request_id, int rrpc, void *ctx)
#else
    static int thing_call_service(const void *thing_id, const char *service, int request_id, void *ctx)
#endif /* RRPC_ENABLED */
{
    sample_context_t *sample_ctx = ctx;

    EXAMPLE_TRACE("service(%s) requested, id: thing@%p, request id:%d\n",
                  service, thing_id, request_id);

    /* 这里判断收到的服务调用请求的服务ID是否是Custom, 然后执行相关逻辑 */
    if (strcmp(service, "Custom") == 0) {
#ifdef RRPC_ENABLED
        handle_service_custom(sample_ctx, thing_id, service, request_id, rrpc);
#else
        /* 在此函数中, 我们获取了服务调用的输入参数transparency的值, 将之加1赋值给输出参数Contrastratio, 然后上报给云端 */
        handle_service_custom(sample_ctx, thing_id, service, request_id);
#endif /* RRPC_ENABLED */
    }

    return 0;
}
```

此时在设备端可以看到如下日志:

    [dbg] iotx_mc_cycle(1608): PUBLISH
    [dbg] iotx_mc_handle_recv_PUBLISH(1412):         Packet Ident : 00000000
    [dbg] iotx_mc_handle_recv_PUBLISH(1413):         Topic Length : 49
    [dbg] iotx_mc_handle_recv_PUBLISH(1417):           Topic Name : /sys/a1csED27mp7/AdvExample1/thing/service/Custom
    [dbg] iotx_mc_handle_recv_PUBLISH(1420):     Payload Len/Room : 95 / 4946
    [dbg] iotx_mc_handle_recv_PUBLISH(1421):       Receive Buflen : 5000
    [dbg] iotx_mc_handle_recv_PUBLISH(1432): delivering msg ...

我们可以看到, 设备端example已经收到从云端下发的服务 `Custom`, 其中服务的输入参数 `transparency` 的值为5

    [dbg] iotx_mc_deliver_message(1170): topic be matched
    [inf] iotx_cloud_conn_mqtt_event_handle(180): event_type 12
    [inf] iotx_cloud_conn_mqtt_event_handle(325): mqtt received: topic=/sys/a1csED27mp7/AdvExample1/thing/service/Custom, topic_msg={"method":"thing.service.Custom","id":"65850626","params":{"transparency":5},"version":"1.0.0"}
    [inf] iotx_cm_cloud_conn_response_callback(121): rsp_type 11
    [inf] iotx_cm_cloud_conn_response_handler(525): URI = /sys/a1csED27mp7/AdvExample1/thing/service/Custom{"method":"thing.service.Custom","id":"65850626","params":{"transparency":5},"version":"1.0.0"}
    [inf] dm_disp_event_new_data_received_handler(1289): IOTX_CM_EVENT_NEW_DATA_RECEIVED

而在example中, 当收到服务 `Custom` 的请求后, 进入相应的处理回调函数 `handle_service_custom()` 中

接着会将该输入参数的值+1赋给输出参数 `Contrastratio`, 从上面的日志中可以看到`Contrastratio`的值被设成了6, 并被上报给服务端.

    [inf] dm_cmw_topic_callback(13): DMGR TOPIC CALLBACK
    [inf] dm_cmw_topic_callback(20): DMGR Receive Message: /sys/a1csED27mp7/AdvExample1/thing/service/Custom{"method":"thing.service.Custom","id":"65850626","params":{"transparency":5},"version":"1.0.0"}
    ...
    ...
    [dbg] iotx_dm_send_service_response(597): Current Service Response Payload, Length: 19, Payload: {"Contrastratio":6}
    [dbg] _dm_mgr_search_dev_by_devid(46): Device Found, devid: 0
    [dbg] dm_mgr_upstream_thing_service_response(2098): Current Service Name: thing/service/Custom_reply
    [dbg] dm_msg_response_with_data(384): Send URI: /sys/a1csED27mp7/AdvExample1/thing/service/Custom_reply, Payload: {"id":"65850626","code":200,"data":{"Contrastratio":6}}
    [inf] iotx_cm_conn_mqtt_publish(531): mqtt publish: topic=/sys/a1csED27mp7/AdvExample1/thing/service/Custom_reply, topic_msg={"id":"65850626","code":200,"data":{"Contrastratio":6}}

关于高级版单品例程中服务/属性/事件的说明就此结束

