# <a name="目录">目录</a>
+ [例程讲解](#例程讲解)
    * [名词说明](#名词说明)
    * [网关与云端建连](#网关与云端建连)
    * [添加子设备](#添加子设备)
    * [子设备管理相关](#子设备管理相关)
    * [子设备数据交互](#子设备数据交互)
    * [注意事项](#注意事项)
+ [网关相关API](#网关相关API)

# <a name="例程讲解">例程讲解</a>

本节内容以`example/linkkit_gateway/linkkit_example_gateway.c`为例讲解网关产品的编程方法

## <a name="名词说明">名词说明</a>

| 名词      | 说明
|-----------|-----------------------------------------------------------------
| 网关      | 能够直接连接物联网平台的设备, 且具有子设备管理功能, 能够代理子设备连接云端
| 子设备	| 本质上也是设备. 子设备不能直接连接物联网平台, 只能通过网关连接
| 设备ID    | Device ID, 也就是设备句柄, 在网关场景中用于标识一个具体的设备, 调用`IOT_Linkkit_Open`时返回
| 拓扑关系   | 子设备和网关的关联关系为拓扑关系, 子设备与网关建立拓扑关系后, 便可以复用网关的物理通道进行数据通信
| 子设备动态注册   | 子设备在注册时只需将productKey和deviceName上报给网关, 网关代替子设备向云端发起身份认证并获取云端返回的deviceSecret用之后的上线操作

## <a name="网关与云端建连">网关与云端建连</a>

网关的建连过程与单品直连设备的建连过程完全一致

1. 调用`IOT_RegisterCallback`注册必要的回调处理函数, 如连接事件处理, 设备连云初始化完成处理, 属性设置事件处理等回调函数。子设备和网关共用一组回调处理函数, 以参数DeviceID来区分不同的设备
```
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_PERMIT_JOIN, user_permit_join_event_handler);
```
2. 调用`IOT_Ioctl`进行必要的配置, 如选择服务器站点, 选择是否使用一型一密等等
```
    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
```
3. 使用`IOTX_LINKKIT_DEV_TYPE_MASTER`参数调用`IOT_Linkkit_Open`初始化主设备资源
```
    iotx_linkkit_dev_meta_info_t master_meta_info;

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }
```
4. 同样使用`IOTX_LINKKIT_DEV_TYPE_MASTER`参数调用`IOT_Linkkit_Connect`与云端建立连接
```
    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }
```
5. 在`ITE_INITIALIZE_COMPLETED`事件处理函数中确认网关连云初始化完成后, 便可以进行下一步添加子设备的操作

## <a name="添加子设备">添加子设备</a>

添加子设备主要由3个步骤完成,

1. 使用`IOTX_LINKKIT_DEV_TYPE_SLAVE`参数调用`IOT_Linkkit_Open`初始化子设备资源
    > 如果需要使用动态注册, 只需要将设备信息参数的`device_secret`配置为空字符串即可. 启用动态注册功能需要把子设备的DeviceName事先在物联网控制台预注册
2. 调用`IOT_Linkkit_Connect`将子设备连上云端, 这个接口为同步接口, 会自动完成子设备注册和拓扑关系的添加
3. 使用`ITM_MSG_LOGIN`参数调用`IOT_Linkkit_Report`完成子设备上线操作
4. 在`ITE_INITIALIZE_COMPLETED`事件处理函数中确认对应的子设备连云初始化完成后, 便可以进行子设备与云端的数据交互了
```
int example_add_subdev(iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0, devid = -1;
    devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_SLAVE, meta_info);
    if (devid == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev open Failed\n");
        return FAIL_RETURN;
    }
    EXAMPLE_TRACE("subdev open susseed, devid = %d\n", devid);

    res = IOT_Linkkit_Connect(devid);
    if (res == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev connect Failed\n");
        return res;
    }
    EXAMPLE_TRACE("subdev connect success: devid = %d\n", devid);

    res = IOT_Linkkit_Report(devid, ITM_MSG_LOGIN, NULL, 0);
    if (res == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev login Failed\n");
        return res;
    }
    EXAMPLE_TRACE("subdev login success: devid = %d\n", devid);
    return res;
}
```

## <a name="子设备管理相关">子设备管理相关</a>

+ 子设备登出: 使用`ITM_MSG_LOGOUT`选项调用`IOT_Linkkit_Report`即可完成子设备登出. 子设备登出功能主要用于通知云端控制台设备处于离线状态
```
    res = IOT_Linkkit_Report(devid, ITM_MSG_LOGOUT, NULL, 0);
    if (res == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev logout Failed\n");
        return res;
    }
    EXAMPLE_TRACE("subdev logout success: devid = %d\n", devid);
```

+ 获取子设备列表: 网关可以用`ITM_MSG_QUERY_TOPOLIST`选项来调用`IOT_Linkkit_Query`以获取与其存在拓扑关系的所有子设备信息. 列表信息将在`ITE_TOPOLIST_REPLY`事件回调中返回

+ 注销子设备: SDK不提供注销子设备的API, 防止因用户错误调用导致子设备被意外删除

+ 删除子设备拓扑关系: SDK不提供删除拓扑关系的API

## <a name="子设备数据交互">子设备数据交互</a>

子设备与云端的数据交互方法与单品产品完全一致. 比如:

+ 调用`IOT_Linkkit_Report`上报属性, 上报透传数据, 更新设备标签信息, 删除设备标签信息

+ 调用`IOT_Linkkit_TriggerEvent`进行Event的主动上报

+ 在`ITE_RAWDATA_ARRIVED`事件回调中接收云端下发的透传数据

+ 在`ITE_SERVICE_REQUST`事件回调中接收服务请求(同步服务和异步服务)

+ 在`ITE_PROPERTY_SET`事件回调中处理云端下发的属性设置

+ 在`ITE_PROPERTY_GET`事件回调中处理本地通信下发的属性获取

**详情可查看物模型编程章节**

## <a name="注意事项">注意事项</a>

1. 网关设备必须支持多线程, 并使用独立线程用于执行`IOT_Linkkit_Yield`
```
    void *user_dispatch_yield(void *args)
    {
        while (1) {
            IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
        }

        return NULL;
    }

    res = HAL_ThreadCreate(&g_user_dispatch_thread, user_dispatch_yield, NULL, NULL, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("HAL_ThreadCreate Failed\n");
        IOT_Linkkit_Close(user_example_ctx->master_devid);
        return -1;
    }
```
2. 对接**智能生活开放平台**时, 只有在收到云端下发的`ITE_PERMIT_JOIN`事件后, 才可以执行子设备添加流程
    > 当用户通过通过App扫码发起子设备添加时, App将会向云端发送PermitJoin命令, 之后云端会将该命令转发给网关. `ITE_PERMIT_JOIN`事件会下发子设备的的`productKey`和允许子设备接入的时间窗口`timeoutSec`(一般为60秒), 厂商可在此窗口时间内去执行子设备的发现和绑定, 并执行添加子设备流程上报云端, 上报成功后便可以在App界面查看到添加的子设备. 此功能让子设备的添加被有效的管控起来, 只有在窗口时间内才可以添加子设备
```
int user_permit_join_event_handler(const char *product_key, const int time)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Product Key: %s, Time: %d", product_key, time);

    user_example_ctx->permit_join = 1;

    return 0;
}
```
3. 在SDK主目录的`make.setting`文件中添加`FEATURE_DEVICE_MODEL_GATEWAY=y`, 再运行`make`命令即可编译出网关例程


# <a name="网关相关API">网关相关API</a>

| 函数名                                                  | 说明
|---------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------
| [IOT_Linkkit_Open](#IOT_Linkkit_Open)                   | 创建本地资源, 在进行网络报文交互之前, 必须先调用此接口, 得到一个会话的句柄
| [IOT_Linkkit_Connect](#IOT_Linkkit_Connect)             | 对主设备/网关来说, 将会建立设备与云端的通信. 对于子设备来说, 将向云端注册该子设备(若需要), 并添加主子设备拓扑关系
| [IOT_Linkkit_Yield](#IOT_Linkkit_Yield)                 | 若SDK占有独立线程, 该函数内容为空, 否则表示将CPU交给SDK让其接收网络报文并将消息分发到用户的回调函数中
| [IOT_Linkkit_Close](#IOT_Linkkit_Close)                 | 若入参中的会话句柄为主设备/网关, 则关闭网络连接并释放SDK为该会话所占用的所有资源
| [IOT_Linkkit_Report](#IOT_Linkkit_Report)               | 向云端发送**没有云端业务数据下发的上行报文**, 包括属性值/设备标签/二进制透传数据/子设备管理等各种报文
| [IOT_Linkkit_Query](#IOT_Linkkit_Query)                 | 向云端发送**存在云端业务数据下发的查询报文**, 包括OTA状态查询/OTA固件下载/子设备拓扑查询/NTP时间查询等各种报文
| [IOT_Linkkit_TriggerEvent](#IOT_Linkkit_TriggerEvent)   | 向云端发送**事件报文**, 如错误码, 异常告警等


| 其他通用函数名                                           | 说明
|---------------------------------------------------------|-------------------------------------
| [IOT_Ioctl](#IOT_Ioctl)                                 | 设置SDK运行时的可配置选项
| [IOT_RegisterCallback](#IOT_RegisterCallback)           | 注册事件回调函数
