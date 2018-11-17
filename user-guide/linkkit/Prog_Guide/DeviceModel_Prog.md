> 物模型管理功能是指SDK能够使能IoT设备接受云端控制台或者手机公版app的控制, 进行属性/事件/服务维度的设置和监控, 在本文的其它地方, 有时也称为"高级版"

#  物模型功能示例
现对照 `examples/linkkit/linkkit_example_solo.c` 例程分步骤讲解实现物模型管理功能

1. 示例主程序进行初始化/连接云端的说明:
---
    int linkkit_main(void *paras)
    {
        int res = 0;
        uint64_t time_prev_sec = 0, time_now_sec = 0;
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        iotx_linkkit_dev_meta_info_t master_meta_info;
        ...
        ...

设置SDK打印Log的Level

        IOT_SetLogLevel(IOT_LOG_DEBUG);

注册用户回调函数, 当事件触发时, 各事件类型对应的回调函数会被调用

        /* Register Callback */

SDK连接云端成功时, 会触发**ITE_CONNECT_SUCC**事件

        IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);

SDK与云端连接断开时, 会触发**ITE_DISCONNECTED**事件

        IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);

从云端下发的Raw Data透传数据到达时, 会触发**ITE_RAWDATA_ARRIVED**事件

        IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, user_down_raw_data_arrived_event_handler);

从云端进行同步或异步服务调用时, 会触发**ITE_SERVICE_REQUST**事件

        IOT_RegisterCallback(ITE_SERVICE_REQUST, user_service_request_event_handler);

从云端或本地通信进行属性设置时, 会触发**ITE_PROPERTY_SET**事件

        IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);

使用本地通信时, 手机app会来获取设备当前所有属性值, 请务必参考本example的**user_property_get_event_handler**, 将设备的所有属性正确填写, 否则本地通信不能正常工作

        IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);

当使用**IOT_Linkkit_Report**上报属性, 更新设备标签和删除设备标签后收到应答时, 会触发**ITE_REPORT_REPLY**事件

        IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);

当使用**IOT_Linkkit_TriggerEvent**上报设备事件后收到应答时, 会触发**ITE_TRIGGER_EVENT_REPLY**事件

        IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);

当使用**IOT_Linkkit_Query**向云端查询当前timestamp后, 查询结果到达时会触发**ITE_TIMESTAMP_REPLY**事件

        IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);

当设备初始化完成时, 会触发**ITE_INITIALIZE_COMPLETED**事件, 这时可以正常进行与云端的消息交互

        IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);

当有可用的新Firmware时, 会触发**ITE_FOTA**事件, 用户可在此回调函数中调用**IOT_Linkkit_Query**触发固件升级

        IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);

当有可用的Config文件时, 会触发**ITE_COTA**事件, 用户可在次回调函数中调用**IOT_Linkkit_Query**触发Config文件的更新

        IOT_RegisterCallback(ITE_COTA, user_cota_event_handler);

        memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
        memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
        memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
        memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
        memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

使用**IOT_Ioctl**可手动配置SDK中没有的目标服务器域名(比如服务器域名有新增而)

        /* Choose Login Server, domain should be configured before IOT_Linkkit_Open() */
    #if USE_CUSTOME_DOMAIN
        IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)CUSTOME_DOMAIN_MQTT);
        IOT_Ioctl(IOTX_IOCTL_SET_HTTP_DOMAIN, (void *)CUSTOME_DOMAIN_HTTP);
    #else
        int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
        IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
    #endif

选择是否需要使用一型一密

        /* Choose Login Method */
        int dynamic_register = 0;
        IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

选择在发送property post或trigger event消息时, 是否需要应答

        /* Choose Whether You Need Post Property/Event Reply */
        int post_event_reply = 1;
        IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

初始化设备需要的资源, 返回设备描述符

        /* Create Master Device Resources */
        user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
        if (user_example_ctx->master_devid < 0) {
            EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
            return -1;
        }

认证并连接阿里云

        /* Start Connect Aliyun Server */
        res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
        if (res < 0) {
            EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
            return -1;
        }

        while (1) {

从网络上获取报文

            IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);

            time_now_sec = user_update_sec();
            if (time_prev_sec == time_now_sec) {
                continue;
            }

每11秒执行一次属性上报的示例, 在这个**user_post_property**函数中, 会循环上报正确和异常的报文, 产生的错误信息会通过返回值或云端的应答告知用户

            /* Post Proprety Example */
            if (time_now_sec % 11 == 0 && user_master_dev_available()) {
                user_post_property();
            }

每17秒执行一次事件上报的示例, 在这个**user_post_event**函数中, 会循环上报正确和异常的报文, 产生的错误信息会通过返回值或云端的应答告知用户

            /* Post Event Example */
            if (time_now_sec % 17 == 0 && user_master_dev_available()) {
                user_post_event();
            }

            ...
            ...
            ...

需要高级版Linkkit正常退出时可调用**IOT_Linkkit_Close**函数来释放所有资源

        IOT_Linkkit_Close(user_example_ctx->master_devid);
        return 0;
    }

2. 属性上报说明:
---
```
示例中使用**user_post_property**作为上报属性的例子. 该示例会循环上报各种情况的payload, 用户可观察在上报错误payload时返回的提示信息:

    void user_post_property(void)
    {
        static int example_index = 0;
        int res = 0;
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        char *property_payload = "NULL";

        if (example_index == 0) {

正常上报属性的情况

            /* Normal Example */
            property_payload = "{\"LightSwitch\":1}";
            example_index++;
        } else if (example_index == 1) {

属性ID写错的情况

            /* Wrong Property ID */
            property_payload = "{\"LightSwitchxxxx\":1}";
            example_index++;
        } else if (example_index == 2) {

属性和其取值数据类型不匹配的情况(这里LightSwitch是布尔型属性)

            /* Wrong Value Format */
            property_payload = "{\"LightSwitch\":\"test\"}";
            example_index++;
        } else if (example_index == 3) {

属性的值超出其取值范围的情况

            /* Wrong Value Range */
            property_payload = "{\"LightSwitch\":10}";
            example_index++;
        } else if (example_index == 4) {

在上报复合属性时, 其成员缺失的情况(这里的**RGBColor**复合属性有3个int型成员: **Red**/**Green**和**Blue**)

            /* Missing Property Item */
            property_payload = "{\"RGBColor\":{\"Red\":45,\"Green\":30}}";
            example_index++;
        } else if (example_index == 5) {

payload格式是json格式, 但不符合要求的情况

            /* Wrong Params Format */
            property_payload = "\"hello world\"";
            example_index++;
        } else if (example_index == 6) {

payload格式不是json格式的情况

            /* Wrong Json Format */
            property_payload = "hello world";
            example_index = 0;
        }

        res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                                (unsigned char *)property_payload, strlen(property_payload));

        EXAMPLE_TRACE("Post Property Message ID: %d", res);
    }
```
3. 属性设置说明:
---
示例在回调函数`user_property_set_event_handler`中获取云端设置的属性值, 并原样上报数据到云端, 这样可以更新在云端的设备影子中保存的设备属性值, 用户可在此处对收到的属性值进行处理:

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

4. 服务调用说明:
---
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

5. 事件上报说明:
---

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
        } else if (example_index == 1) {

上报事件ID错误的情况:

            /* Wrong Property ID */
            event_payload = "{\"ErrorCodexxx\":0}";
            example_index++;
        } else if (example_index == 2) {

上报事件ID和其取值数据类型不同的情况:

            /* Wrong Value Format */
            event_payload = "{\"ErrorCode\":\"test\"}";
            example_index++;
        } else if (example_index == 3) {

上报事件ID的值超出其定义的取值范围的情况:

            /* Wrong Value Range */
            event_payload = "{\"ErrorCode\":10}";
            example_index++;
        } else if (example_index == 4) {

上报的payload是json格式, 但不符合格式要求的情况:

            /* Wrong Value Range */
            event_payload = "\"hello world\"";
            example_index++;
        } else if (example_index == 5) {

上报的payload不是json格式的情况:

            /* Wrong Json Format */
            event_payload = "hello world";
            example_index = 0;
        }

上报事件到云端

        res = IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid, event_id, strlen(event_id),
                                    event_payload, strlen(event_payload));
        EXAMPLE_TRACE("Post Event Message ID: %d", res);
    }

6. 关于上报消息的格式说明及示例
---

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

# 与物模型功能相关的API列表

| 函数名                                                  | 说明
|---------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------
| [IOT_Linkkit_Open](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Open)                   | 创建本地资源, 在进行网络报文交互之前, 必须先调用此接口, 得到一个会话的句柄
| [IOT_Linkkit_Connect](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Connect)             | 对主设备/网关来说, 将会建立设备与云端的通信. 对于子设备来说, 将向云端注册该子设备(若需要), 并添加主子设备拓扑关系
| [IOT_Linkkit_Yield](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Yield)                 | 若SDK占有独立线程, 该函数只将接收到的网络报文分发到用户的回调函数中, 否则表示将CPU交给SDK让其接收网络报文并将消息分发到用户的回调函数中
| [IOT_Linkkit_Close](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Close)                 | 若入参中的会话句柄为主设备/网关, 则关闭网络连接并释放SDK为该会话所占用的所有资源
| [IOT_Linkkit_TriggerEvent](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_TriggerEvent)   | 向云端发送**事件报文**, 如错误码, 异常告警等
| [IOT_Linkkit_Report](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Report)               | 向云端发送**没有云端业务数据下发的上行报文**, 包括属性值/设备标签/二进制透传数据/子设备管理等各种报文
| [IOT_Linkkit_Query](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Query)                 | 向云端发送**存在云端业务数据下发的查询报文**, 包括OTA状态查询/OTA固件下载/子设备拓扑查询/NTP时间查询等各种报文

| 函数名                                          | 说明
|-------------------------------------------------|---------------------------------------------------------------------------------------------
| [IOT_RegisterCallback](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/API/Linkkit_Provides#IOT_RegisterCallback)   | 对SDK注册事件回调函数, 如云端连接成功/失败, 有属性设置/服务请求到达, 子设备管理报文答复等
| [IOT_Ioctl](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/API/Linkkit_Provides#IOT_Ioctl)                         | 对SDK进行各种参数运行时设置和获取, 以及运行状态的信息获取等, 实参可以是任何数据类型
