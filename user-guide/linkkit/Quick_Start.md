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

本章描述如何在Ubuntu上通过MQTT topic和通过物模型的编程方式, 上报和接收业务报文. 这个环节使用Ubuntu主机模拟IoT设备, 体验跟云端的连接和交互

## <a name="准备开发环境">准备开发环境</a>

### <a name="安装本地开发环境">安装本地开发环境</a>

#### <a name="安装Ubuntu16.04">安装Ubuntu16.04</a>

本文编写是对照的编译环境是__64位__主机上的`Ubuntu16.04`, 在其它Linux版本上尚未验证过, 推荐安装与阿里一致的发行版以避免碰到兼容性方面的问题

如果您使用`Windows`操作系统, 可以安装虚拟机软件`Virtualbox`获得Linux开发环境, 下载地址: [https://www.virtualbox.org/wiki/Downloads](https://www.virtualbox.org/wiki/Downloads)

然后安装64位的Desktop版本的`Ubuntu 16.04.x LTS`, 下载地址: [http://releases.ubuntu.com/16.04](http://releases.ubuntu.com/16.04)

#### <a name="安装必备软件">安装必备软件</a>

本SDK的开发编译环境使用如下软件: `make-4.1`, `git-2.7.4`, `gcc-5.4.0`, `gcov-5.4.0`, `lcov-1.12`, `bash-4.3.48`, `tar-1.28`, `mingw-5.3.1`, `gawk-4.1.3`

可使用如下命令行安装必要的软件:
```
$ sudo apt-get install -y build-essential make git gcc gawk
```

### <a name="通过Hacklab Web IDE进行开发">通过Hacklab Web IDE进行开发</a>

如果您不打算在您的PC或者笔记本电脑上安装Ubuntu, 也可通过阿里云IoT提供的Hacklab Web IDE环境直接进行设备开发, Hacklab Web IDE是一个云端的Linux开发环境, 已经将必要的软件安装完毕, 只要您拥有一个阿里云账号, 就可以通过浏览器登录进行开发, 点击此处登录[Hacklab Web IDE](https://hacklab.aliyun.com/)

## <a name="以MQTT Topic编程方式接入设备">以MQTT Topic编程方式接入设备</a>
### <a name="创建产品和设备">创建产品和设备</a>

请登录[阿里云IoT物联网平台](https://iot.console.aliyun.com/product)进行产品创建, 登录时通过您的阿里云账号进行登录. 因为是直接通过MQTT的Topic进行产品功能实现, 所以在创建产品时选择"基础版"即可

创建产品之后可以添加一个具体的设备, 阿里云IoT物联网平台会为设备生成身份信息

如果您对云端如何创建产品不熟悉, 请[点击此处](https://help.aliyun.com/document_detail/73705.html)了解如何在阿里云IoT物联网平台进行产品和设备创建

### <a name="产品功能实现">产品功能实现</a>

#### <a name="了解SDK根目录结构">了解SDK根目录结构</a>

获取Linkkit SDK后, 顶层目录结构如下:
```
$ ls
certs  config.bat  external_libs  extract.bat  extract.sh  LICENSE  makefile  make.settings  model.json  README.md  src  tools  wrappers
```

#### <a name="配置SDK">配置SDK</a>

SDK默认配置打开了物模型选项, 这里仅演示基础版的使用, 先关闭物模型选项

```
$ make menuconfig
```

在弹出的图形化配置界面中, 去掉`FEATURE_DEVICE_MODEL_ENABLED`选项即可

#### <a name="填写设备三元组到例程中">填写设备三元组到例程中</a>

打开文件 `wrappers/os/ubuntu/HAL_OS_linux.c`, 编辑如下代码段, 填入之前在云端创建设备后得到的__设备三元组__:
```
#ifdef DYNAMIC_REGISTER
...
...
#else
#ifdef DEVICE_MODEL_ENABLED
...
...
#else
char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1MZxOdcBnO";
char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "h4I4dneEFp7EImTv";
char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "test_01";
char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "t9GmMf2jb3LgWfXBaZD2r3aJrfVWBv56";
#endif
#endif
```

<em>注: 请在物联网平台的管理控制台将topic  </em><em><code>/${productKey}/${deviceName}/get</code></em><em> 设置为&quot;可订阅可发布&quot;权限, 下面的代码中将会用到</em>

#### <a name="初始化与建立连接">初始化与建立连接</a>

下面的代码片段来自MQTT上云功能的例程 `src/mqtt/examples/mqtt_example.c`, 它简单描述了设备的初始化以及连接过程:

##### <a name="定制化MQTT参数">定制化MQTT参数</a>

```
iotx_mqtt_param_t       mqtt_params;

memset(&mqtt_params, 0x0, sizeof(mqtt_params));

/* mqtt_params.request_timeout_ms = 2000; */
/* mqtt_params.clean_session = 0; */
/* mqtt_params.keepalive_interval_ms = 60000; */
/* mqtt_params.write_buf_size = 1024; */
/* mqtt_params.read_buf_size = 1024; */
mqtt_params.handle_event.h_fp = example_event_handle;
```

> 上面的代码中注释掉的地方是mqtt相关配置的默认数值, 用户可以不用赋值, SDK会自动填写默认值. 如果用户希望调整默认的连接参数, 只需要去掉相应的注释, 并填入数值即可

##### <a name="尝试建立与服务器的MQTT连接">尝试建立与服务器的MQTT连接</a>
```
pclient = IOT_MQTT_Construct(&mqtt_params);
if (NULL == pclient) {
    EXAMPLE_TRACE("MQTT construct failed");
    return -1;
}
```

> 将连接参数结构体传参给 `IOT_MQTT_Construct()` 接口, 即可触发MQTT连接建立的动作
>
> 成功返回非空值作为已建立连接的句柄, 失败则返回空

#### <a name="上报数据到云端">上报数据到云端</a>

在示例文件中定义了如下的topic:
```
/${productKey}/${deviceName}/get
```

下面的代码片段示例了如何向这个Topic发送数据
```
int example_publish(void *handle)
{
    int             res = 0;
    const char     *fmt = "/%s/%s/get";
    char           *topic = NULL;
    int             topic_len = 0;
    char           *payload = "{\"message\":\"hello!\"}";

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
```

> 其中, `IOT_MQTT_Publish_Simple()` 的第1个参数可以填入之前调用 `IOT_MQTT_Construct()` 得到的句柄返回值
>
> 也可以直接填入0, 代表告诉SDK, 使用当前已建立的唯一MQTT连接来发送消息

注: 如何查看消息是否发送成功

+ 对于发送QoS = 1的消息，发送成功后会收到云端ACK并通过回调事件IOTX_MQTT_EVENT_PUBLISH_SUCCESS 告知用户，查看log

```
mqtt_client|238 :: packet-id=306, publish topic msg={"attr_name":"temperature","attr_value":"1"}
event_handle|080 :: publish success, packet-id=306 //消息发送成功，得到云端应答

```

+ 通过物联网平台查看（适应QoS0和QoS1）：

![1](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/mcu_mqtt/1_yun.png)

#### <a name="从云端订阅并处理数据">从云端订阅并处理数据</a>

注: 示例程序为了尽量简单的演示发布/订阅, 代码中对topic `/${productKey}/${deviceName}/get`进行了订阅, 意味着设备发送给物联网平台的数据将会被物联网平台发送回设备

下面的代码订阅指定的topic并指定接收到数据时的处理函数:
```
res = example_subscribe(pclient);
if (res < 0) {
    IOT_MQTT_Destroy(&pclient);
    return -1;
}

...
...

int example_subscribe(void *handle)
{
    ...
    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, example_message_arrive, NULL);
    ...
```

> 其中, `IOT_MQTT_Subscribe()` 的第1个参数可以填入之前调用 `IOT_MQTT_Construct()` 得到的句柄返回值
>
> 也可以直接填入0, 代表告诉SDK, 使用当前已建立的唯一MQTT连接来订阅Topic.

示例程序中收到来自云端消息, 在回调函数中处理时只是把消息打印出来
```
void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            EXAMPLE_TRACE("Message Arrived:");
            EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
            EXAMPLE_TRACE("Payload: %.*s", topic_info->payload_len, topic_info->payload);
            EXAMPLE_TRACE("\n");
            break;
        default:
            break;
    }
}
```

示例代码向该Topic周期性的发送数据, 用户在实现自己的产品逻辑时不需要周期的发送数据, 只是有需要上报的时候再发送数据
```
while (1) {
    if (0 == loop_cnt % 20) {
        example_publish(pclient);
    }

    IOT_MQTT_Yield(pclient, 200);

    loop_cnt += 1;
}
```

#### <a name="编译例子程序">编译例子程序</a>

在SDK顶层目录运行如下命令:
```
make distclean
make
```

编译成功完成后, 生成的样例程序在当前路径的 `output/release/bin` 目录下:
```
$ tree output/release
output/release/
+-- bin
...
...
|   +-- mqtt-example
...
...
```

### <a name="观察数据">观察数据</a>

执行如下命令:
```
$ ./output/release/bin/mqtt-example
```

可以在物联网平台的控制台, 找到指定的产品, 在其日志服务中查看设备上报的消息. 可以[点击此处](https://help.aliyun.com/document_detail/44542.html)了解如何在云端查看设备上报的数据

在Linux的console里面也可以看见示例程序打印的来自云端的数据:
```
example_message_arrive|031 :: Message Arrived:
example_message_arrive|032 :: Topic  : /a1MZxOdcBnO/test_01/get
example_message_arrive|033 :: Payload: {"message":"hello!"}
example_message_arrive|034 ::
```

## <a name="以物模型编程方式接入设备">以物模型编程方式接入设备</a>

### <a name="创建产品和设备">创建产品和设备</a>

可以在阿里云IoT物联网平台以及其上承载的多个行业服务中进行产品的创建, 下面是在阿里云IoT物联网平台创建产品的帮助链接:

* [如何创建支持物模型的产品](https://help.aliyun.com/document_detail/73728.html)
* [如何定义物模型](https://help.aliyun.com/document_detail/73727.html)

若产品需要在生活物联网平台(注: 基于阿里云IoT物联网平台创建的针对生活场景的行业服务)进行创建, 可以登录[生活物联网平台](https://iot.aliyun.com/products/livinglink)创建产品

---

本示例所属产品的物模型描述文件`model_for_examples.JSON`存放在`./src/dev_model/examples/`目录下，用户可将该文件中`productkey`替换为自己创建产品的`productKey`，并在 **产品详情** - **功能定义** 页面点击 **导入物模型** 按钮导入到自己创建的产品中。

定义物模型之后云端会为该物模型生成物模型(TSL)描述文件, 该文件使用JSON格式进行描述

### <a name="产品功能实现">产品功能实现</a>

#### <a name="填写设备三元组到例程中">填写设备三元组到例程中</a>

将 `wrappers/os/ubuntu/HAL_OS_linux.c` 中的三元组替换成云端创建的设备的三元组
```
#ifdef DEVICE_MODEL_ENABLED
char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1RIsMLz2BJ";
char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "fSAF0hle6xL0oRWd";
char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "example1";
char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "RDXf67itLqZCwdMCRrw0N5FHbv5D7jrE";
#else
```

#### <a name="编译与运行程序">编译与运行程序</a>

在SDK顶层目录执行如下命令:
```
$ make distclean
$ make
```

编译成功完成后, 生成的高级版例子程序在当前路径的 `output/release/bin` 目录下, 名为`linkkit-example-solo`

在SDK顶层目录执行如下命令:
```
$ ./output/release/bin/linkkit-example-solo
```

### <a name="观察数据">观察数据</a>

示例程序会定期将`Counter`属性的数值上报云端, 因此可以在云端查看收到的属性。用户可以将该属性配置为可读写属性，并且可以在云端对该属性进行设置, 然后再次查看从设备端上报的`Counter`值。

#### <a name="属性上报">属性上报</a>

示例中使用__user\_post\_property__作为上报属性的例子. 该示例会循环上报各种情况的payload, 用户可观察在上报错误payload时返回的提示信息:

代码中上报属性的代码片段如下:
```
/* Post Proprety Example */
if (time_now_sec % 11 == 0 && user_master_dev_available()) {
    user_post_property();
}
```

观察属性上报示例函数
```
void user_post_property(void)
{
    static int example_index = 0;
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *property_payload = "NULL";

    if (example_index == 0) {
```

正常上报属性的情况
```
void user_post_property(void)
{
    static int cnt = 0;
    int res = 0;

    char property_payload[30] = {0};
    HAL_Snprintf(property_payload, sizeof(property_payload), "{\"Counter\": %d}", cnt++);

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));

    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}
```

下面是上报正常属性时的日志
```
[inf] dm_msg_request(205): DM Send Message, URI: /sys/a1X2bEnP82z/test_06/thing/event/property/post, Payload: {"id":"2","version":"1.0","params":{"LightSwitch":1},"method":"thing.event.property.post"}
[inf] MQTTPublish(2546): Upstream Topic: '/sys/a1X2bEnP82z/test_06/thing/event/property/post'
```

这里是发送给云端的消息
```
> {
>     "id": "2",
>     "version": "1.0",
>     "params": {
>         "Counter": 1
>     },
>     "method": "thing.event.property.post"
> }
```

收到的云端应答
```
< {
<     "code": 200,
<     "data": {
<     },
<     "id": "1",
<     "message": "success",
<     "method": "thing.event.property.post",
<     "version": "1.0"
< }
```

用户回调函数的日志
```
user_report_reply_event_handler.314: Message Post Reply Received, Devid: 0, Message ID: 2, Code: 200, Reply: {}
```

#### <a name="属性设置处理">属性设置处理</a>

收到属性set请求时, 会进入如下回调函数
```
static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Property Set Received, Devid: %d, Request: %s", devid, request);
```

将属性设置的执行结果发回云端, 更新云端设备属性
```
    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                            (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}
```

日志中可以看到从服务端下来的属性设置消息
```
[dbg] iotx_mc_cycle(1774): PUBLISH
[inf] iotx_mc_handle_recv_PUBLISH(1549): Downstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/service/property/set'
[inf] iotx_mc_handle_recv_PUBLISH(1550): Downstream Payload:
```

从云端收到的属性设置报文内容
```
< {
<     "method": "thing.service.property.set",
<     "id": "161430786",
<     "params": {
<         "LightSwitch": 1
<     },
<     "version": "1.0.0"
< }
```

发送回云端的应答消息
```
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
```

`user_property_set_event_handler()` 示例回调函数中收到属性设置的日志
```
user_property_set_event_handler.160: Property Set Received, Devid: 0, Request: {"LightSwitch":1}
```

这样, 一条从服务端设置属性的命令就到达设备端并执行完毕了

最后收到的对属性上报的应答
```
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
```

*注: 实际的产品收到属性设置时, 应该解析属性并进行相应处理而不是仅仅将数值发送回云端*

#### <a name="事件上报">事件上报</a>

示例中使用 `IOT_Linkkit_TriggerEvent` 上报属性. 该示例会循环上报各种情况的payload, 用户可观察在上报错误payload时返回的提示信息:

正常上报事件的情况
```
void user_post_event(void)
{
    int res = 0;
    char *event_id = "HardwareError";
    char *event_payload = "{\"ErrorCode\": 0}";

    res = IOT_Linkkit_TriggerEvent(EXAMPLE_MASTER_DEVID, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    EXAMPLE_TRACE("Post Event Message ID: %d", res);
}
```

示例程序中 `Error` 事件(Event)是约每10s上报一次, 在以上各种情况中循环. 其中正常上报的日志如下:
```
[inf] dm_msg_request(218): DM Send Message, URI: /sys/a1csED27mp7/AdvExample1/thing/event/HardwareError/post, Payload: {"id":"1","version":"1.0","params":{"ErrorCode":0},"method":"thing.event.HardwareError.post"}
[dbg] MQTTPublish(319): ALLOC: (136) / [200] @ 0x1195150
[inf] MQTTPublish(378): Upstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/event/HardwareError/post'
[inf] MQTTPublish(379): Upstream Payload:
```

向云端上报的事件消息内容及日志
```
> {
>     "id": "1",
>     "version": "1.0",
>     "params": {
>         "ErrorCode": 0
>     },
>     "method": "thing.event.HardwareError.post"
> }

[inf] dm_client_publish(106): Publish Result: 0
[dbg] alcs_observe_notify(105): payload:{"id":"1","version":"1.0","params":{"ErrorCode":0},"method":"thing.event.Error.post"}
[inf] dm_server_send(76): Send Observe Notify Result 0
[dbg] dm_msg_cache_insert(79): dmc list size: 0
user_post_event.470: Post Event Message ID: 1
[dbg] iotx_mc_cycle(1774): PUBLISH
[inf] iotx_mc_handle_recv_PUBLISH(1549): Downstream Topic: '/sys/a1csED27mp7/AdvExample1/thing/event/HardwareError/post_reply'
[inf] iotx_mc_handle_recv_PUBLISH(1550): Downstream Payload:
```

从云端收到的应答消息内容及日志
```
< {
<     "code": 200,
<     "data": {
<     },
<     "id": "1",
<     "message": "success",
<     "method": "thing.event.HardwareError.post",
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
```

用户回调函数 `user_trigger_event_reply_event_handler()` 中的日志:
```
user_trigger_event_reply_event_handler.310: Trigger Event Reply Received, Devid: 0, Message ID: 1, Code: 200, EventID: Error, Message: success
```

#### <a name="服务调用">服务调用</a>

注册服务消息的处理函数
```
IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
```

收到服务请求消息时, 会进入下面的回调函数。设备端演示了一个简单的加法运算服务，入参为`NumberA`和`NumberB`，出参为`Result`，例程中使用`cJSON`解析属性的值。
```
static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
                                              const char *request, const int request_len,
                                              char **response, int *response_len)
{
    int add_result = 0;
    cJSON *root = NULL, *item_number_a = NULL, *item_number_b = NULL;
    const char *response_fmt = "{\"Result\": %d}";

    EXAMPLE_TRACE("Service Request Received, Service ID: %.*s, Payload: %s", serviceid_len, serviceid, request);

    /* Parse Root */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }

    if (strlen("Operation_Service") == serviceid_len && memcmp("Operation_Service", serviceid, serviceid_len) == 0) {
        /* Parse NumberA */
        item_number_a = cJSON_GetObjectItem(root, "NumberA");
        if (item_number_a == NULL || !cJSON_IsNumber(item_number_a)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("NumberA = %d", item_number_a->valueint);

        /* Parse NumberB */
        item_number_b = cJSON_GetObjectItem(root, "NumberB");
        if (item_number_b == NULL || !cJSON_IsNumber(item_number_b)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("NumberB = %d", item_number_b->valueint);

        add_result = item_number_a->valueint + item_number_b->valueint;

        /* Send Service Response To Cloud */
        *response_len = strlen(response_fmt) + 10 + 1;
        *response = (char *)HAL_Malloc(*response_len);
        if (*response == NULL) {
            EXAMPLE_TRACE("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        HAL_Snprintf(*response, *response_len, response_fmt, add_result);
        *response_len = strlen(*response);
    }

    cJSON_Delete(root);
    return 0;
}
```

此时在设备端可以看到如下日志

收到的云端的服务调用, 输入参数为`NumberA`(值为1), `NumberB`(值为2)。
```
< {
<     "method": "thing.service.Operation_Service",
<     "id": "280532170",
<     "params": {
<         "NumberB": 2,
<         "NumberA": 1
<     },
<     "version": "1.0.0"
< }
```

在回调函数中将 `NumberA` 和 `NumberB`的值相加后赋值给 `Result` 后, 上报到云端
```
> {
>     "id": "280532170",
>     "code": 200,
>     "data": {
>         "Result":  3
>     }
> }

```

关于高级版单品例程中服务/属性/事件的说明就此结束

