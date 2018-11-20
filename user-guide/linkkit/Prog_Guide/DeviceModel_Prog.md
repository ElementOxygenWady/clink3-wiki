# <a name="目录">目录</a>
+ [设备属性](#设备属性)
+ [设备服务](#设备服务)
+ [设备事件](#设备事件)
+ [关于上报消息的格式说明及示例](#关于上报消息的格式说明及示例)
+ [与物模型功能相关的API列表](#与物模型功能相关的API列表)

> 物模型管理功能是指SDK能够使能IoT设备接受云端控制台或者手机公版app的控制, 进行属性/事件/服务维度的设置和监控, 在本文的其它地方, 有时也称为"高级版"

# <a name="设备属性">设备属性</a>

SDK提供当上报属性或者事件时是否需要云端应答的功能, 通过`IOT_Ioctl`对`IOTX_IOCTL_RECV_EVENT_REPLY`选项进行设备

* 属性上报说明

示例中使用`user_post_property`作为上报属性的例子, 其中展示了如何使用`IOT_Linkkit_Report`进行属性上报（对于异常情况的上报，详见example）:

    void user_post_property(void)
    {
        static int example_index = 0;
        int res = 0;
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        char *property_payload = "NULL";

        ...
        ...
            /* Normal Example */
            property_payload = "{\"LightSwitch\":1}";
            example_index++;
        ...
        ...

        res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                                (unsigned char *)property_payload, strlen(property_payload));

        EXAMPLE_TRACE("Post Property Message ID: %d", res);
    }

* 属性设置说明:

示例在回调函数`user_property_set_event_handler`中获取云端设置的属性值, 并原样上报数据到云端, 这样可以更新在云端的设备影子中保存的设备属性值, 用户可在此处对收到的属性值进行处理（该回调函数是在example初始化时使用`IOT_RegisterCallback`注册的`ITE_SERVICE_REQUST`事件对应的回调函数）:

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
            /* Parse Item */
            const char *response_fmt = "{\"Contrastratio\":%d}";
            item_transparency = cJSON_GetObjectItem(root, "transparency");
            if (item_transparency == NULL || !cJSON_IsNumber(item_transparency)) {
                cJSON_Delete(root);
                return -1;
            }
            EXAMPLE_TRACE("transparency: %d", item_transparency->valueint);
            contrastratio = item_transparency->valueint + 1;

            /* Send Service Response To Cloud */
            *response_len = strlen(response_fmt) + 10 + 1;
            *response = HAL_Malloc(*response_len);
            if (*response == NULL) {
                EXAMPLE_TRACE("Memory Not Enough");
                return -1;
            }
            memset(*response, 0, *response_len);
            HAL_Snprintf(*response, *response_len, response_fmt, contrastratio);
            *response_len = strlen(*response);
        } else if (strlen("SyncService") == serviceid_len && memcmp("SyncService", serviceid, serviceid_len) == 0) {
            ...
            ...
        }
        cJSON_Delete(root);

        return 0;
    }

# <a name="设备事件">设备事件</a>

示例中使用 `IOT_Linkkit_TriggerEvent` 上报事件. 其中展示了如何使用**IOT_Linkkit_Report**进行事件上报（对于异常情况的上报，详见example）:

    void user_post_event(void)
    {
        static int example_index = 0;
        int res = 0;
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        char *event_id = "Error";
        char *event_payload = "NULL";

        ...
        ...
            /* Normal Example */
            event_payload = "{\"ErrorCode\":0}";
            example_index++;
        ...
        ...

        res = IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid, event_id, strlen(event_id),
                                    event_payload, strlen(event_payload));
        EXAMPLE_TRACE("Post Event Message ID: %d", res);
    }

# <a name="关于上报消息的格式说明及示例">关于上报消息的格式说明及示例</a>

上报属性时, 属性ID和值以JSON格式的形式放在`IOT_Linkkit_Report()`的`payload`中, 不同数据类型以及多个属性的格式示例如下:
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

上报事件时, 与上报属性的区别是, 事件ID需要单独拿出来, 放在`IOT_Linkkit_TriggerEvent()`的`eventid`中, 而事件的上报内容, 也就是物模型定义中事件的输出参数, 则使用与上报属性相同的格式进行上报, 示例如下:
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
