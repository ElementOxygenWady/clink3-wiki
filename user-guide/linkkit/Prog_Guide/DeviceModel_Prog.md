# <a name="目录">目录</a>
+ [设备属性](#设备属性)
+ [设备服务](#设备服务)
+ [设备事件](#设备事件)
+ [关于上报消息的格式说明及示例](#关于上报消息的格式说明及示例)
+ [基于MQTT Topic进行数据收发](#基于MQTT Topic进行数据收发)
+ [与物模型功能相关的API列表](#与物模型功能相关的API列表)

> 物模型管理功能是指SDK能够使能IoT设备接受云端控制台或者手机公版app的控制, 进行属性/事件/服务维度的设置和监控, 在本文的其它地方, 有时也称为"高级版"

下面的讲解中使用了示例代码 `./src/dev_model/examples/linkkit_example_solo.c`

**注意**
- 在Linux环境下, 用户可通过修改 `wrappers/os/ubuntu/HAL_OS_linux.c` 文件中的默认三元组来使用自己在云端控制台创建的设备
- 我们在 `src/dev_model/examples` 目录下提供了名为 `model_for_example.json` 的物模型描述文件
    + 用户可以用自己产品的 `productkey` 替换掉该文件中的 `"productKey"` 值, 并将该物模型文件导入到云端产品中
    + 这样便能保证例程物模型与用户自建产品物模型的相互匹配

# <a name="设备属性">设备属性</a>

SDK提供当上报属性或者事件时是否需要云端应答的功能, 通过`IOT_Ioctl`对`IOTX_IOCTL_RECV_EVENT_REPLY`选项进行设备

* 属性上报说明

> 用户可以调用 `IOT_Linkkit_Report()` 函数来上报属性, 属性上报时需要按照云端定义的属性格式使用JSON编码后进行上报

示例中函数 `user_post_property` 展示了如何使用 `IOT_Linkkit_Report` 进行属性上报(对于异常情况的上报, 详见example):

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

> 注: property_payload = "{\"Counter\":1}" 即是将属性编码为JSON对象

* 属性设置说明:

示例在回调函数 `user_property_set_event_handler` 中获取云端设置的属性值, 并原样上报数据到云端

这样可以更新在云端的设备影子中保存的设备属性值, 用户可在此处对收到的属性值进行处理

*注: 该回调函数是在example初始化时使用 `IOT_RegisterCallback` 注册的 `ITE_PROPERTY_SET` 事件对应的回调函数:*

    static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
    {
        int res = 0;
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        EXAMPLE_TRACE("Property Set Received, Devid: %d, Request: %s", devid, request);

        res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                                (unsigned char *)request, request_len);
        EXAMPLE_TRACE("Post Property Message ID: %d", res);

        return 0;
    }

# <a name="设备服务">设备服务</a>

在设备端示例程序中, 当收到服务调用请求时, 会进入如下回调函数, 以下代码使用了开源软件 `cJSON` 来解析服务请求的参数值:

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

# <a name="设备事件">设备事件</a>

示例中使用 `IOT_Linkkit_TriggerEvent` 上报事件. 其中展示了如何使用**IOT_Linkkit_Report**进行事件上报(对于异常情况的上报, 详见example):

    void user_post_event(void)
    {
        int res = 0;
        char *event_id = "HardwareError";
        char *event_payload = "{\"ErrorCode\": 0}";

        res = IOT_Linkkit_TriggerEvent(EXAMPLE_MASTER_DEVID, event_id, strlen(event_id),
                                    event_payload, strlen(event_payload));
        EXAMPLE_TRACE("Post Event Message ID: %d", res);
    }

# <a name="关于上报消息的格式说明及示例">关于上报消息的格式说明及示例</a>

上报属性时, 属性ID和值以JSON格式的形式放在 `IOT_Linkkit_Report()` 的 payload 中, 不同数据类型以及多个属性的格式示例如下:
```
/* 整型数据 */
char *payload = "{\"Brightness\":50}";

/* 浮点型数据上报 */
char *payload = "{\"Temperature\":11.11}";

/* 枚举型数据上报 */
char *payload = "{\"WorkMode\":2}";

/* 布尔型数据上报, 在物模型定义中, 布尔型为整型, 取值为0或1, 与JSON格式的整型不同 */
char *payload = "{\"LightSwitch\":1}";

/* 字符串数据上报 */
char *payload = "{\"Description\":\"Amazing Example\"}";

/* 时间型数据上报, 在物模型定义中, 时间型为字符串 */
char *payload = "{\"Timestamp\":\"1252512000\"}";

/* 复合类型属性上报, 在物模型定义中, 符合类型属性为JSON对象 */
char *payload = "{\"RGBColor:{\"Red\":11,\"Green\":22,\"Blue\":33}\"}";

/* 多属性上报, 如果需要上报以上各种数据类型的所有属性, 将它们放在一个JSON对象中即可 */
char *payload = "{\"Brightness\":50,\"Temperature\":11.11,\"WorkMode\":2,\"LightSwitch\":1,\"Description\":\"Amazing Example\",\"Timestamp\":\"1252512000\",\"RGBColor:{\"Red\":11,\"Green\":22,\"Blue\":33}\"}";

/* 属性payload准备好以后, 就可以使用如下接口进行上报了 */
IOT_Linkkit_Report(devid, ITM_MSG_POST_PROPERTY, payload, strlen(payload));

```

上报事件时, 与上报属性的区别是, 事件ID需要单独拿出来, 放在`IOT_Linkkit_TriggerEvent()`的`eventid`中

而事件的上报内容, 也就是物模型定义中事件的输出参数, 则使用与上报属性相同的格式进行上报, 示例如下:
```
/* 事件ID为Error, 其输出参数ID为ErrorCode, 数据类型为枚举型 */
char *eventid = "Error";
char *payload = "{\"ErrorCode\":0}";

/* 事件ID为HeartbeatNotification, 其输出参数有2个. 第一个是布尔型参数ParkingState, 第二个是浮点型参数VoltageValue */
char *eventid = "HeartbeatNotification";
char *payload = "{\"ParkingState\":1,\"VoltageValue\":3.0}";

/* 事件payload准备好以后, 就可以使用如下接口进行上报了 */
IOT_Linkkit_TriggerEvent(devid, event_id, strlen(event_id), payload, strlen(payload));

/* 从上面的示例可以看出, 当事件的输出参数有多个时, payload的格式与多属性上报是相同的 */
```

# <a name="基于MQTT Topic进行数据收发">基于MQTT Topic进行数据收发</a>

*注意: 虽然物模型编程的 API 并未返回 MQTT 编程接口 `IOT_MQTT_XXX()` 所需要的 `pClient` 参数, 但基于MQTT Topic进行数据收发仍可和物模型编程混用*
---
+ 所有MQTT数据收发的接口, 第1个参数都可接受参数 `0` 作为输入, 表示 **"使用当前唯一的MQTT通道进行数据收发等操作"**, 包括
    - `IOT_MQTT_Construct`
    - `IOT_MQTT_Destroy`
    - `IOT_MQTT_Yield`
    - `IOT_MQTT_CheckStateNormal`
    - `IOT_MQTT_Subscribe`
    - `IOT_MQTT_Unsubscribe`
    - `IOT_MQTT_Publish`
    - `IOT_MQTT_Subscribe_Sync`
    - `IOT_MQTT_Publish_Simple`

+ 比如要在使用物模型编程API的程序代码段落中表示对某个Topic进行订阅, 可以用

        IOT_MQTT_Subscribe(0, topic_request, IOTX_MQTT_QOS0, topic_callback, topic_context);

+ 比如要在使用物模型编程API的程序代码段落中表示在某个Topic进行发布(数据上报), 可以用

        IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, payload_len);

# <a name="与物模型功能相关的API列表">与物模型功能相关的API列表</a>

| 函数名                                                  | 说明
|---------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------
| [IOT_Linkkit_Open](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Open)                   | 创建本地资源, 在进行网络报文交互之前, 必须先调用此接口, 得到一个会话的句柄
| [IOT_Linkkit_Connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Connect)             | 对主设备/网关来说, 将会建立设备与云端的通信. 对于子设备来说, 将向云端注册该子设备(若需要), 并添加主子设备拓扑关系
| [IOT_Linkkit_Yield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Yield)                 | 若SDK占有独立线程, 该函数只将接收到的网络报文分发到用户的回调函数中, 否则表示将CPU交给SDK让其接收网络报文并将消息分发到用户的回调函数中
| [IOT_Linkkit_Close](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Close)                 | 若入参中的会话句柄为主设备/网关, 则关闭网络连接并释放SDK为该会话所占用的所有资源
| [IOT_Linkkit_TriggerEvent](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_TriggerEvent)   | 向云端发送**事件报文**, 如错误码, 异常告警等
| [IOT_Linkkit_Report](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Report)               | 向云端发送**没有云端业务数据下发的上行报文**, 包括属性值/设备标签/二进制透传数据/子设备管理等各种报文
| [IOT_Linkkit_Query](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Query)                 | 向云端发送**存在云端业务数据下发的查询报文**, 包括OTA状态查询/OTA固件下载/子设备拓扑查询/NTP时间查询等各种报文

| 函数名                                          | 说明
|-------------------------------------------------|---------------------------------------------------------------------------------------------
| [IOT_RegisterCallback](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_RegisterCallback)   | 对SDK注册事件回调函数, 如云端连接成功/失败, 有属性设置/服务请求到达, 子设备管理报文答复等
| [IOT_Ioctl](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Ioctl)                         | 对SDK进行各种参数运行时设置和获取, 以及运行状态的信息获取等, 实参可以是任何数据类型
