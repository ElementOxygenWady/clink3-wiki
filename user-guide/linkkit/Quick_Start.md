# <a name="目录">目录</a>
+ [快速体验](#快速体验)
    * [准备开发环境](#准备开发环境)
        - [安装本地开发环境](#安装本地开发环境)
        - [通过Hacklab Web IDE进行开发](#通过Hacklab Web IDE进行开发)
    * [以MQTT Topic编程方式接入设备](#以MQTT Topic编程方式接入设备)
        - [创建产品和设备](#创建产品和设备)
        - [产品功能实现](#产品功能实现)
        - [观察数据](#观察数据)
    * [以物模型编程方式接入设备](#以物模型编程方式接入设备)
        - [创建产品和设备](#创建产品和设备)
        - [产品功能实现](#产品功能实现)
        - [观察数据](#观察数据)

# <a name="快速体验">快速体验</a>

本章描述如何创建产品和设备, 并在Ubuntu上快速体验通过MQTT topic和通过物模型的编程方式, 上报和接收业务报文.

## <a name="准备开发环境">准备开发环境</a>

### <a name="安装本地开发环境">安装本地开发环境</a>

#### <a name="安装Ubuntu16.04">安装Ubuntu16.04</a>

本快速体验的编译环境是**64位**主机上的`Ubuntu16.04`, 在其它Linux版本上尚未验证过, 推荐安装与阿里一致的发行版以避免碰到兼容性方面的问题

如果您使用`Windows`操作系统, 建议安装虚拟机软件`Virtualbox`, 下载地址: [https://www.virtualbox.org/wiki/Downloads](https://www.virtualbox.org/wiki/Downloads)

然后安装64位的Desktop版本的`Ubuntu 16.04.x LTS`, 下载地址: [http://releases.ubuntu.com/16.04](http://releases.ubuntu.com/16.04)

#### <a name="安装必备软件">安装必备软件</a>

本SDK的开发编译环境使用如下软件: `make-4.1`, `git-2.7.4`, `gcc-5.4.0`, `gcov-5.4.0`, `lcov-1.12`, `bash-4.3.48`, `tar-1.28`, `mingw-5.3.1`

可使用如下命令行安装必要的软件:

    apt-get install -y build-essential make git gcc

### <a name="通过Hacklab Web IDE进行开发">通过Hacklab Web IDE进行开发</a>

如果您不打算在您的PC或者笔记本电脑上安装Ubuntu, 也可通过阿里云IoT提供的Hacklab Web IDE环境直接进行设备开发, Hacklab Web IDE是一个云端的Linux开发环境, 已经将必要的软件安装完毕, 只要您拥有一个阿里云账号, 就可以通过浏览器登录进行开发, 点击此处登录[Hacklab Web IDE](https://hacklab.aliyun.com/)

## <a name="以MQTT Topic编程方式接入设备">以MQTT Topic编程方式接入设备</a>
### <a name="创建产品和设备">创建产品和设备</a>

请登录[阿里云IoT物联网平台](https://iot.aliyun.com/products/linkKits)进行产品创建, 登录时通过您的阿里云账号进行登录. 因为是直接通过MQTT的Topic进行产品功能实现, 所以在创建产品时选择"基础版"即可
创建产品之后可以添加一个具体的设备, 阿里云IoT物联网平台会为设备生成身份信息
如果您对云端如何创建产品不熟悉, 请[点击此处](https://help.aliyun.com/document_detail/68946.html)了解如何在阿里云IoT物联网平台进行产品和设备创建

### <a name="产品功能实现">产品功能实现</a>

#### <a name="了解SDK根目录结构">了解SDK根目录结构</a>

*注：本文档以2.3.0版本为例，不同的版本内容可能会有不同*

获取Linkkit SDK后，顶层目录结构如下：

    cd iotx-sdk-c/
    ls
    build-rules  Config.in  examples  LICENSE   make.settings  project.mk  sdk-c.mk  win_board_conf.mk CMakeLists.txt  Config.linkkit include   makefile  prebuilt  README.txt  src  win.makefile

#### <a name="填写设备三元组到例程中">填写设备三元组到例程中</a>

打开文件 `iotx-sdk-c/examples/mqtt/mqtt-example.c`, 编辑如下代码段, 填入之前在云端创建设备后得到的**设备三元组**:

<pre>
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
</pre>

*注: 在Ubuntu的获取ProductKey、DeviceName、DeviceSecret的HAL实现中读取了上面定义的设备三元组信息*

#### <a name="初始化与建立连接">初始化与建立连接</a>

下面的代码片段简单描述了设备的初始化以及连接过程:

<pre>
int mqtt_client(void)
{
    ...
    ...


/*在与服务器尝试建立MQTT连接前, 填入设备身份认证信息:*/

    if (0 != IOT_SetupConnInfo(__product_key, __device_name, __device_secret, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        rc = -1;
        goto do_exit;
    }

    /* Initialize MQTT parameter */
    ...
    ...
</pre>

尝试建立与服务器的MQTT连接
<pre>
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        rc = -1;
        goto do_exit;
    }
</pre>

#### <a name="上报数据到云端">上报数据到云端</a>

在示例文件中定义了如下的topic:

<pre>
/* These are pre-defined topics */
#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/get"
#define TOPIC_DATA              "/"PRODUCT_KEY"/"DEVICE_NAME"/data"

/* These are pre-defined topics format*/
#define TOPIC_UPDATE_FMT            "/%s/%s/update"
#define TOPIC_ERROR_FMT             "/%s/%s/update/error"
#define TOPIC_GET_FMT               "/%s/%s/get"
#define TOPIC_DATA_FMT              "/%s/%s/data"
</pre>

下面的代码片段示例了如何向topic发送数据:

		/* Initialize topic information */
		memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
		strcpy(msg_pub, "update: hello! start!");

		topic_msg.qos = IOTX_MQTT_QOS1;
		topic_msg.retain = 0;
		topic_msg.dup = 0;
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

#### <a name="从云端订阅并处理数据">从云端订阅并处理数据</a>

示例程序向一个topic发送数据, 并自己订阅该topic将其从云端将数据接收回来. 下面的代码订阅指定的topic并指定接收到数据时的处理函数:

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

示例程序中收到来自云端数据时只是把数据打印出来:

<pre>
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
</pre>

下面的代码片段尝试向该topic发送数据:

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

#### <a name="编译例子程序">编译例子程序</a>

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

### <a name="观察数据">观察数据</a>

执行如下命令:

    cd ~/srcs/iotx-sdk-c
    ./output/release/bin/mqtt-example

可以在物联网平台的控制台, 找到指定的产品, 在其日志服务中查看设备上报的消息. 可以[点击此处](https://help.aliyun.com/document_detail/44542.html)了解如何在云端查看设备上报的数据

在Linux的console里面也可以看见示例程序打印的来自云端的数据:

<pre>
_demo_message_arrive|166 :: ----
_demo_message_arrive|167 :: packetId: 35641
_demo_message_arrive|171 :: Topic: '/a1ExpAkj9Hi/Example1/data' (Length: 26)
_demo_message_arrive|175 :: Payload: '{"attr_name":"temperature", "attr_value":"1"}' (Length: 45)
_demo_message_arrive|176 :: ----
</pre>

## <a name="以物模型编程方式接入设备">以物模型编程方式接入设备</a>

### <a name="创建产品和设备">创建产品和设备</a>

可以在阿里云IoT物联网平台以及其上承载的多个行业服务中进行产品的创建, 下面是在阿里云IoT物联网平台创建产品的帮助链接:

+ [如何创建支持物模型的产品](https://help.aliyun.com/document_detail/73728.html)
+ [如何定义物模型](https://help.aliyun.com/document_detail/88241.html)

若产品需要在生活物联网平台(注: 基于阿里云IoT物联网平台创建的针对生活场景的行业服务)进行创建, 可以登录[生活物联网平台](https://iot.aliyun.com/products/livinglink)创建产品

在本示例中, 需要创建包含如下物模型的产品:

+ 属性LightSwitch
    - 标识符: LightSwitch
    - 数据类型: enum,
    - 数据长度: 1,
    - 读写类型: 读写
+ 服务Custom
    - 标识符: Custom
    - 调用方式: 异步
    - 输入参数: {标识符: transparency, 数据类型: int32}
    - 输出参数: {标识符: Contrastratio, 数据类型: int32}
+ 事件
    - 标识符: Error
    - 事件类型: 信息
    - 输出参数: {标识符: ErrorCode, 数据类型: enum, 取值范围: 0, 1}

定义物模型之后云端会为该物模型生成物模型(TSL)描述文件, 该文件使用JSON格式进行描述

### <a name="产品功能实现">产品功能实现</a>

#### <a name="填写设备三元组到例程中">填写设备三元组到例程中</a>

将 `iotx-sdk-c/examples/linkkit/linkkit_example_solo.c` 中的三元组替换成云端创建的设备的三元组

    #define PRODUCT_KEY      "a1csED27mp7"
    #define PRODUCT_SECRET   "VuULINCrtTAzCSzp"
    #define DEVICE_NAME      "AdvExample1"
    #define DEVICE_SECRET    "3xGoxtWRscxPAoMxnJnjocZbNfmCTRi0"

#### <a name="编译与运行程序">编译与运行程序</a>

运行如下命令:

    $ make distclean
    $ make

编译成功完成后, 生成的高级版例子程序在当前路径的 `output/release/bin` 目录下, 名为`linkkit-example-solo`

执行如下命令:

    cd ~/srcs/iotx-sdk-c
    ./output/release/bin/linkkit-example-solo

### <a name="观察数据">观察数据</a>

示例程序会定期将`LightSwitch`属性的数值上报云端, 因此可以在云端查看收到的属性, 并且可以在云端对该属性进行设置, 然后再次查看从设备端上报的`LightSwitch`已经修改为用户设置的数值

#### <a name="属性上报">属性上报</a>

示例中使用**user_post_property**作为上报属性的例子. 该示例会循环上报各种情况的payload, 用户可观察在上报错误payload时返回的提示信息:

    void user_post_property(void)
    {
        static int example_index = 0;
        int res = 0;
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        char *property_payload = "NULL";

        if (example_index == 0) {

> 正常上报属性的情况

            /* Normal Example */
            property_payload = "{\"LightSwitch\":1}";
            example_index++;
        }else if(...) {
            ...
            ...
        }

        res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                                (unsigned char *)property_payload, strlen(property_payload));

        EXAMPLE_TRACE("Post Property Message ID: %d", res);
    }

下面是上报正常属性时的日志:

    [inf] dm_msg_request(218): DM Send Message, URI: /sys/a1csED27mp7/AdvExample1/thing/event/property/post, Payload: {"id":"1","version":"1.0","params":{"LightSwitch":1},"method":"thing.event.property.post"}
    [dbg] MQTTPublish(319): ALLOC: (160) / [224] @ 0x1da41a0
    [inf] MQTTPublish(378): Upstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/event/property/post'
    [inf] MQTTPublish(379): Upstream Payload:

这里是发送给云端的消息

    > {
    >     "id": "1",
    >     "version": "1.0",
    >     "params": {
    >         "LightSwitch": 1
    >     },
    >     "method": "thing.event.property.post"
    > }

    [inf] dm_client_publish(106): Publish Result: 0
    [dbg] alcs_observe_notify(105): payload:{"id":"1","version":"1.0","params":{"LightSwitch":1},"method":"thing.event.property.post"}
    [dbg] CoAPResourceByPath_get(176): Found the resource: /sys/a1csED27mp7/AdvExample1/thing/event/property/post
    [inf] dm_server_send(76): Send Observe Notify Result 0
    [dbg] dm_msg_cache_insert(79): dmc list size: 0
    user_post_property.431: Post Property Message ID: 1
    [dbg] iotx_mc_cycle(1774): PUBLISH
    [inf] iotx_mc_handle_recv_PUBLISH(1549): Downstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/event/property/post_reply'
    [inf] iotx_mc_handle_recv_PUBLISH(1550): Downstream Payload:

收到的云端应答

    < {
    <     "code": 200,
    <     "data": {
    <     },
    <     "id": "1",
    <     "message": "success",
    <     "method": "thing.event.property.post",
    <     "version": "1.0"
    < }

    [dbg] iotx_mc_handle_recv_PUBLISH(1555):         Packet Ident : 00000000
    [dbg] iotx_mc_handle_recv_PUBLISH(1556):         Topic Length : 60
    [dbg] iotx_mc_handle_recv_PUBLISH(1560):           Topic Name : /sys/a1csED27mp7/AdvExample1/thing/event/property/post_reply
    [dbg] iotx_mc_handle_recv_PUBLISH(1563):     Payload Len/Room : 104 / 4935
    [dbg] iotx_mc_handle_recv_PUBLISH(1564):       Receive Buflen : 5000
    [dbg] iotx_mc_handle_recv_PUBLISH(1575): delivering msg ...
    [dbg] iotx_mc_deliver_message(1291): topic be matched
    [inf] dm_msg_proc_thing_event_post_reply(258): Event Id: property
    [dbg] dm_msg_response_parse(167): Current Request Message ID: 1
    [dbg] dm_msg_response_parse(168): Current Request Message Code: 200
    [dbg] dm_msg_response_parse(169): Current Request Message Data: {}
    [dbg] dm_msg_response_parse(174): Current Request Message Desc: success
    [dbg] dm_ipc_msg_insert(87): dm msg list size: 0, max size: 50
    [dbg] dm_msg_cache_remove(142): Remove Message ID: 1
    [inf] _iotx_linkkit_event_callback(219): Receive Message Type: 30
    [inf] _iotx_linkkit_event_callback(221): Receive Message: {"id":1,"code":200,"devid":0,"payload":{}}
    [dbg] _iotx_linkkit_event_callback(476): Current Id: 1
    [dbg] _iotx_linkkit_event_callback(477): Current Code: 200
    [dbg] _iotx_linkkit_event_callback(478): Current Devid: 0

用户回调函数的日志

    user_report_reply_event_handler.300: Message Post Reply Received, Devid: 0, Message ID: 1, Code: 200, Reply: {}

#### <a name="属性设置处理">属性设置处理</a>

在示例代码中当收到属性set请求时, 会进入如下回调函数:

    static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
    {
        int res = 0;
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        EXAMPLE_TRACE("Property Set Received, Devid: %d, Request: %s", devid, request);

这里将收到的属性设置消息发送给云端, 更新云端设备属性

        res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                                (unsigned char *)request, request_len);
        EXAMPLE_TRACE("Post Property Message ID: %d", res);

        return 0;
    }


此时在设备端的日志中可以看到从服务端set下来的值:

    [dbg] iotx_mc_cycle(1774): PUBLISH
    [inf] iotx_mc_handle_recv_PUBLISH(1549): Downstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/service/property/set'
    [inf] iotx_mc_handle_recv_PUBLISH(1550): Downstream Payload:

从云端收到的属性设置请求:

    < {
    <     "method": "thing.service.property.set",
    <     "id": "161430786",
    <     "params": {
    <         "LightSwitch": 1
    <     },
    <     "version": "1.0.0"
    < }

    [dbg] iotx_mc_handle_recv_PUBLISH(1555):         Packet Ident : 00000000
    [dbg] iotx_mc_handle_recv_PUBLISH(1556):         Topic Length : 55
    [dbg] iotx_mc_handle_recv_PUBLISH(1560):           Topic Name : /sys/a1csED27mp7/AdvExample1/thing/service/property/set
    [dbg] iotx_mc_handle_recv_PUBLISH(1563):     Payload Len/Room : 113 / 4940
    [dbg] iotx_mc_handle_recv_PUBLISH(1564):       Receive Buflen : 5000
    [dbg] iotx_mc_handle_recv_PUBLISH(1575): delivering msg ...
    [dbg] iotx_mc_deliver_message(1291): topic be matched
    [inf] dm_msg_proc_thing_service_property_set(93): thing/service/property/set
    [dbg] dm_msg_request_parse(142): Current Request Message ID: 161430786
    [dbg] dm_msg_request_parse(143): Current Request Message Version: 1.0.0
    [dbg] dm_msg_request_parse(144): Current Request Message Method: thing.service.property.set
    [dbg] dm_msg_request_parse(145): Current Request Message Params: {"LightSwitch":1}
    [dbg] dm_ipc_msg_insert(87): dm msg list size: 0, max size: 50
    [inf] dm_msg_response(274): Send URI: /sys/a1csED27mp7/AdvExample1/thing/service/property/set_reply, Payload: {"id":"161430786","code":200,"data":{}}
    [dbg] MQTTPublish(319): ALLOC: (100) / [164] @ 0x154f1f0
    [inf] MQTTPublish(378): Upstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/service/property/set_reply'
    [inf] MQTTPublish(379): Upstream Payload:

发送回云端的应答消息:

    > {
    >     "id": "161430786",
    >     "code": 200,
    >     "data": {
    >     }
    > }

    [inf] dm_client_publish(106): Publish Result: 0
    [inf] _iotx_linkkit_event_callback(219): Receive Message Type: 15
    [inf] _iotx_linkkit_event_callback(221): Receive Message: {"devid":0,"payload":{"LightSwitch":1}}
    [dbg] _iotx_linkkit_event_callback(339): Current Devid: 0
    [dbg] _iotx_linkkit_event_callback(340): Current Payload: {"LightSwitch":1}

**user_property_set_event_handler**示例函数中收到属性设置的日志:

    user_property_set_event_handler.160: Property Set Received, Devid: 0, Request: {"LightSwitch":1}

这样, 一条从服务端设置属性的命令就到达设备端了

又由于在例程里, 收到这条属性的设置指令之后, 会调用 `IOT_Linkkit_Report` 将该属性值上报给服务端, 所以有如下的日志打印:

    [inf] dm_msg_request(218): DM Send Message, URI: /sys/a1csED27mp7/AdvExample1/thing/event/property/post, Payload: {"id":"2","version":"1.0","params":{"LightSwitch":1},"method":"thing.event.property.post"}
    [dbg] MQTTPublish(319): ALLOC: (156) / [220] @ 0x154f280
    [inf] MQTTPublish(378): Upstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/event/property/post'
    [inf] MQTTPublish(379): Upstream Payload:

**user_property_set_event_handler**示例函数中发送给云端的属性值, 更新云端属性:

    > {
    >     "id": "2",
    >     "version": "1.0",
    >     "params": {
    >         "LightSwitch": 1
    >     },
    >     "method": "thing.event.property.post"
    > }

    [inf] dm_client_publish(106): Publish Result: 0
    [dbg] alcs_observe_notify(105): payload:{"id":"2","version":"1.0","params":{"LightSwitch":1},"method":"thing.event.property.post"}
    [dbg] CoAPResourceByPath_get(176): Found the resource: /sys/a1csED27mp7/AdvExample1/thing/event/property/post
    [inf] dm_server_send(76): Send Observe Notify Result 0
    [dbg] dm_msg_cache_insert(79): dmc list size: 0
    user_property_set_event_handler.164: Post Property Message ID: 2
    [dbg] iotx_mc_cycle(1774): PUBLISH
    [inf] iotx_mc_handle_recv_PUBLISH(1549): Downstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/event/property/post_reply'
    [inf] iotx_mc_handle_recv_PUBLISH(1550): Downstream Payload:

收到的上报属性的应答:

    < {
    <     "code": 200,
    <     "data": {
    <     },
    <     "id": "2",
    <     "message": "success",
    <     "method": "thing.event.property.post",
    <     "version": "1.0"
    < }

    [dbg] iotx_mc_handle_recv_PUBLISH(1555):         Packet Ident : 00000000
    [dbg] iotx_mc_handle_recv_PUBLISH(1556):         Topic Length : 60
    [dbg] iotx_mc_handle_recv_PUBLISH(1560):           Topic Name : /sys/a1csED27mp7/AdvExample1/thing/event/property/post_reply
    [dbg] iotx_mc_handle_recv_PUBLISH(1563):     Payload Len/Room : 104 / 4935
    [dbg] iotx_mc_handle_recv_PUBLISH(1564):       Receive Buflen : 5000
    [dbg] iotx_mc_handle_recv_PUBLISH(1575): delivering msg ...
    [dbg] iotx_mc_deliver_message(1291): topic be matched
    [inf] dm_msg_proc_thing_event_post_reply(258): Event Id: property
    [dbg] dm_msg_response_parse(167): Current Request Message ID: 2
    [dbg] dm_msg_response_parse(168): Current Request Message Code: 200
    [dbg] dm_msg_response_parse(169): Current Request Message Data: {}
    [dbg] dm_msg_response_parse(174): Current Request Message Desc: success
    [dbg] dm_ipc_msg_insert(87): dm msg list size: 0, max size: 50
    [dbg] dm_msg_cache_remove(142): Remove Message ID: 2
    [inf] _iotx_linkkit_event_callback(219): Receive Message Type: 30
    [inf] _iotx_linkkit_event_callback(221): Receive Message: {"id":2,"code":200,"devid":0,"payload":{}}
    [dbg] _iotx_linkkit_event_callback(476): Current Id: 2
    [dbg] _iotx_linkkit_event_callback(477): Current Code: 200
    [dbg] _iotx_linkkit_event_callback(478): Current Devid: 0
    user_report_reply_event_handler.300: Message Post Reply Received, Devid: 0, Message ID: 2, Code: 200, Reply: {}

注: 实际的产品收到属性设置时, 应该解析属性并进行相应处理而不是仅仅将数值发送回云端

#### <a name="事件上报">事件上报</a>

示例中使用 `IOT_Linkkit_TriggerEvent` 上报属性. 该示例会循环上报各种情况的payload, 用户可观察在上报错误payload时返回的提示信息:

    void user_post_event(void)
    {
        static int example_index = 0;
        int res = 0;
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        char *event_id = "Error";
        char *event_payload = "NULL";

        if (example_index == 0) {

正常上报事件的情况:

            /* Normal Example */
            event_payload = "{\"ErrorCode\":0}";
            example_index++;
        } else if (...) {
            ...
            ...
        }

上报事件到云端

        res = IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid, event_id, strlen(event_id),
                                    event_payload, strlen(event_payload));
        EXAMPLE_TRACE("Post Event Message ID: %d", res);
    }

示例程序中 `Error` 事件(Event)是每17s上报一次, 在以上各种情况中循环. 其中正常上报的日志如下:

    [inf] dm_msg_request(218): DM Send Message, URI: /sys/a1csED27mp7/AdvExample1/thing/event/Error/post, Payload: {"id":"1","version":"1.0","params":{"ErrorCode":0},"method":"thing.event.Error.post"}
    [dbg] MQTTPublish(319): ALLOC: (136) / [200] @ 0x1195150
    [inf] MQTTPublish(378): Upstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/event/Error/post'
    [inf] MQTTPublish(379): Upstream Payload:

向云端上报的事件:

    > {
    >     "id": "1",
    >     "version": "1.0",
    >     "params": {
    >         "ErrorCode": 0
    >     },
    >     "method": "thing.event.Error.post"
    > }

    [inf] dm_client_publish(106): Publish Result: 0
    [dbg] alcs_observe_notify(105): payload:{"id":"1","version":"1.0","params":{"ErrorCode":0},"method":"thing.event.Error.post"}
    [inf] dm_server_send(76): Send Observe Notify Result 0
    [dbg] dm_msg_cache_insert(79): dmc list size: 0
    user_post_event.470: Post Event Message ID: 1
    [dbg] iotx_mc_cycle(1774): PUBLISH
    [inf] iotx_mc_handle_recv_PUBLISH(1549): Downstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/event/Error/post_reply'
    [inf] iotx_mc_handle_recv_PUBLISH(1550): Downstream Payload:

从云端收到的应答消息:

    < {
    <     "code": 200,
    <     "data": {
    <     },
    <     "id": "1",
    <     "message": "success",
    <     "method": "thing.event.Error.post",
    <     "version": "1.0"
    < }

    [dbg] iotx_mc_handle_recv_PUBLISH(1555):         Packet Ident : 00000000
    [dbg] iotx_mc_handle_recv_PUBLISH(1556):         Topic Length : 57
    [dbg] iotx_mc_handle_recv_PUBLISH(1560):           Topic Name : /sys/a1csED27mp7/AdvExample1/thing/event/Error/post_reply
    [dbg] iotx_mc_handle_recv_PUBLISH(1563):     Payload Len/Room : 101 / 4938
    [dbg] iotx_mc_handle_recv_PUBLISH(1564):       Receive Buflen : 5000
    [dbg] iotx_mc_handle_recv_PUBLISH(1575): delivering msg ...
    [dbg] iotx_mc_deliver_message(1291): topic be matched
    [inf] dm_msg_proc_thing_event_post_reply(258): Event Id: Error
    [dbg] dm_msg_response_parse(167): Current Request Message ID: 1
    [dbg] dm_msg_response_parse(168): Current Request Message Code: 200
    [dbg] dm_msg_response_parse(169): Current Request Message Data: {}
    [dbg] dm_msg_response_parse(174): Current Request Message Desc: success
    [dbg] dm_ipc_msg_insert(87): dm msg list size: 0, max size: 50
    [dbg] dm_msg_cache_remove(142): Remove Message ID: 1
    [inf] _iotx_linkkit_event_callback(219): Receive Message Type: 31
    [inf] _iotx_linkkit_event_callback(221): Receive Message: {"id":1,"code":200,"devid":0,"eventid":"Error","payload":"success"}
    [dbg] _iotx_linkkit_event_callback(513): Current Id: 1
    [dbg] _iotx_linkkit_event_callback(514): Current Code: 200
    [dbg] _iotx_linkkit_event_callback(515): Current Devid: 0
    [dbg] _iotx_linkkit_event_callback(516): Current EventID: Error
    [dbg] _iotx_linkkit_event_callback(517): Current Message: success

用户回调函数**user_trigger_event_reply_event_handler**中的日志:

    user_trigger_event_reply_event_handler.310: Trigger Event Reply Received, Devid: 0, Message ID: 1, Code: 200, EventID: Error, Message: success

#### <a name="服务调用">服务调用</a>

在设备端示例程序中, 当收到服务调用请求时, 会进入如下回调函数:

    static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
    {
        int contrastratio = 0, to_cloud = 0;
        cJSON *root = NULL, *item_transparency = NULL, *item_from_cloud = NULL;
        EXAMPLE_TRACE("Service Request Received, Devid: %d, Service ID: %.*s, Payload: %s", devid, serviceid_len,
                    serviceid,
                    request);

        /* Parse Root */
        root = cJSON_Parse(request);
        if (root == NULL || !cJSON_IsObject(root)) {
            EXAMPLE_TRACE("JSON Parse Error");
            return -1;
        }

处理收到的Service ID为**Custom**的服务, 将该服务的输入参数+1赋值给输出参数并返回给云端:

        if (strlen("Custom") == serviceid_len && memcmp("Custom", serviceid, serviceid_len) == 0) {
            ...
            contrastratio = item_transparency->valueint + 1;

            ...
            HAL_Snprintf(*response, *response_len, response_fmt, contrastratio);
            *response_len = strlen(*response);
        } else if (strlen("SyncService") == serviceid_len && memcmp("SyncService", serviceid, serviceid_len) == 0) {
            ...
            ...
        }
        cJSON_Delete(root);

        return 0;
    }

此时在设备端可以看到如下日志:

    [dbg] iotx_mc_cycle(1774): PUBLISH
    [inf] iotx_mc_handle_recv_PUBLISH(1549): Downstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/service/Custom'
    [inf] iotx_mc_handle_recv_PUBLISH(1550): Downstream Payload:

收到的云端的服务调用, 输入参数为**transparency**:

    < {
    <     "method": "thing.service.Custom",
    <     "id": "161445548",
    <     "params": {
    <         "transparency": 5
    <     },
    <     "version": "1.0.0"
    < }

    [dbg] iotx_mc_handle_recv_PUBLISH(1555):         Packet Ident : 00000000
    [dbg] iotx_mc_handle_recv_PUBLISH(1556):         Topic Length : 49
    [dbg] iotx_mc_handle_recv_PUBLISH(1560):           Topic Name : /sys/a1csED27mp7/AdvExample1/thing/service/Custom
    [dbg] iotx_mc_handle_recv_PUBLISH(1563):     Payload Len/Room : 96 / 4946
    [dbg] iotx_mc_handle_recv_PUBLISH(1564):       Receive Buflen : 5000
    [dbg] iotx_mc_handle_recv_PUBLISH(1575): delivering msg ...
    [dbg] iotx_mc_deliver_message(1291): topic be matched
    [inf] dm_msg_proc_thing_service_request(224): Service Identifier: Custom
    [dbg] dm_msg_request_parse(142): Current Request Message ID: 161445548
    [dbg] dm_msg_request_parse(143): Current Request Message Version: 1.0.0
    [dbg] dm_msg_request_parse(144): Current Request Message Method: thing.service.Custom
    [dbg] dm_msg_request_parse(145): Current Request Message Params: {"transparency":5}
    [dbg] dm_ipc_msg_insert(87): dm msg list size: 0, max size: 50
    [inf] _iotx_linkkit_event_callback(219): Receive Message Type: 18
    [inf] _iotx_linkkit_event_callback(221): Receive Message: {"id":"161445548","devid":0,"serviceid":"Custom","payload":{"transparency":5}}
    [dbg] _iotx_linkkit_event_callback(300): Current Id: 161445548
    [dbg] _iotx_linkkit_event_callback(301): Current Devid: 0
    [dbg] _iotx_linkkit_event_callback(302): Current ServiceID: Custom
    [dbg] _iotx_linkkit_event_callback(303): Current Payload: {"transparency":5}

用户回调函数**user_service_request_event_handler**被调用:

    user_service_request_event_handler.99: Service Request Received, Devid: 0, Service ID: Custom, Payload: {"transparency":5}

    user_service_request_event_handler.116: transparency: 5
    [dbg] iotx_dm_send_service_response(280): Current Service Response Payload, Length: 19, Payload: {"Contrastratio":6}
    [dbg] dm_mgr_upstream_thing_service_response(1275): Current Service Name: thing/service/Custom_reply
    [inf] dm_msg_response(274): Send URI: /sys/a1csED27mp7/AdvExample1/thing/service/Custom_reply, Payload: {"id":"161445548","code":200,"data":{"Contrastratio":6}}
    [dbg] MQTTPublish(319): ALLOC: (111) / [175] @ 0x85a1a0
    [inf] MQTTPublish(378): Upstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/service/Custom_reply'
    [inf] MQTTPublish(379): Upstream Payload:

在回调函数中将**transparency**的值+1后赋值给**Contrastratio**后, 上报到云端:

    > {
    >     "id": "161445548",
    >     "code": 200,
    >     "data": {
    >         "Contrastratio": 6
    >     }
    > }

    [inf] dm_client_publish(106): Publish Result: 0
    [dbg] alcs_observe_notify(105): payload:{"id":"161445548","code":200,"data":{"Contrastratio":6}}
    [inf] dm_server_send(76): Send Observe Notify Result 0

关于高级版单品例程中服务/属性/事件的说明就此结束

[回到页头](#快速体验)