# <a name="目录">目录</a>
+ [1.概述](#1.概述)
+ [2.前提](#2.前提)
+ [3.基本概念](#3.基本概念)
+ [4.飞燕控制台操作](#4.飞燕控制台操作)
    * [4.1 创建网关产品](#4.1 创建网关产品)
    * [4.2 创建子设备产品](#4.2 创建子设备产品)
+ [5.SDK配置](#5.SDK配置)
    * [5.1 menuconfig](#5.1 menuconfig)
    * [5.2 编译，运行例程](#5.2 编译，运行例程)
+ [6. 使用飞燕App交互式添加网关和子设备](#6. 使用飞燕App交互式添加网关和子设备)
    * [6.1 添加网关](#6.1 添加网关)
    * [6.1 交互式添加子设备：](#6.1 交互式添加子设备：)
+ [7.子设备接入逻辑](#7.子设备接入逻辑)
    * [7.1 网关与云端建连](#7.1 网关与云端建连)
    * [7.2 将网关添加到飞燕App中](#7.2 将网关添加到飞燕App中)
    * [7.3 子设备添加](#7.3 子设备添加)
    * [7.4 permit join交互逻辑](#7.4 permit join交互逻辑)
+ [8. Reset的实现](#8. Reset的实现)
+ [9. 注意事项](#9. 注意事项)

# <a name="1.概述">1.概述</a>

本文主要讲解如何使用LinkKit SDK 3.0.1的网关例程连上飞燕平台，并与飞燕公版App实现简单交互，如使用App完成子设备的添加等操作。本文会顺带介绍SDK的配置和代码抽取功能，但不会涉及跨平台移植的内容。

# <a name="2.前提">2.前提</a>

1. 本文假设用户熟悉[飞燕平台](https://living.aliyun.com/#/)的相关操作，能熟练地在平台上创建产品，新建设备，获取设备三元组(用户可先访问[飞燕在线文档](https://living.aliyun.com/doc?spm=a2c9o.12549863.0.0.4e9d7946DDvtTP#index.html)做进一步了解)；
2. 用户已安装飞燕公版App**开发版本**，未安装App的设备可访问[飞燕App说明页面](https://living.aliyun.com/doc?spm=a2c9o.12549863.0.0.4e9d7946DDvtTP#muti-app.html)扫描**开发版本**的App二维码完成下载。
3. 本文使用Ubuntu作为演示平台，Ubuntu与手机必须在同一局域网下，否则无法在App上发现设备；用户也可使用移植了Linkkit SDK的嵌入式开发板来进行演示。

# <a name="3.基本概念">3.基本概念</a>

| 名词      | 说明
|-----------|-------------------------------------------------------------------------------
| 网关      | 能够直接连接物联网平台, 且具有子设备管理能力的设备, 能够代理子设备连接云端
| 子设备	| 子设备一般不能直接连接物联网平台, 只能通过网关连接，比如ZigBee，蓝牙设备。
| 设备ID    | Device ID, 也就是设备句柄, 在网关场景中用于标识一个具体的设备, 调用`IOT_Linkkit_Open`时返回。引入设备ID目的是简化设备标识
| 拓扑关系   | 子设备和网关的关联关系为拓扑关系, 子设备与网关建立拓扑关系后, 便可以复用网关的物理通道进行数据通信
| 子设备动态注册   | 子设备在注册时只需将productKey和deviceName上报给网关, 网关代替子设备向云端发起身份认证并获取云端返回的deviceSecret用之后的上线操作
| 设备绑定  | 绑定即将指定IoT设备与手机APP已登陆账号产生关联的过程，这种设备授权机制依赖一个由设备或手机生成的唯一的token完成绑定验证

Linkkit SDK提供了提供网关主设备上云，代理子设备上云的能力，主要包括了网关和子设备的身份认证，子设备拓扑同步，数据交互，账号绑定等。SDK不提供网关和实际子设备间的通信链路管理，用户需要自己完成网关与子设备间通信协议部分（如BLE，ZigBee，Modbus协议）的开发设计。飞燕平台为每个设备赋予了独立的身份标识(即ProductKey+DeviceName)以及应用数据的格式规范(即ICA标准数据格式Alink Json)，子设备和网关主设备在与云端的数据交互上并无太大差异，区别只是子设备无直接的数据上云能力，需要通过网关中转。

# <a name="4.飞燕控制台操作">4.飞燕控制台操作</a>

为了完成本文演示，需要在飞燕平台创建网关类型的产品和支持接入网关的子设备产品。

## <a name="4.1 创建网关产品">4.1 创建网关产品</a>

用户创建产品时，指定产品的节点类型为**网关**，如下图所示
<div align="left">
<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/pictures/create_gw.jpg" width="500">
</div>


接着，进入产品页面，点击设备调试进入设备页面，点击**新增测试设备**创建新的设备。

## <a name="4.2 创建子设备产品">4.2 创建子设备产品</a>

用户创建产品是，是定产品的节点类型为**设备**，并且可接入网关，为了易于演示我们选择产品类型为**电工照明/灯**，如下图所示
<div align="left">
<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/pictures/create_sub.jpg" width="500">
</div>

接着，进入产品页面，点击设备调试进入设备页面，点击**新增测试设备**创建新的设备。

# <a name="5.SDK配置">5.SDK配置</a>

我们已经在上一节创建了网关和子设备产品并添加了相应的设备，接下来我们将对应的三元组信息设置到SDK中，

配置网关的三元组：`wrappers/os/ubuntu/HAL_OS_linux.c`
```
    #ifdef DEVICE_MODEL_ENABLED
        char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1RIsMLz2BJ";
        char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "fSAF0hle6xL0oRWd";
        char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "example123";
        char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "RDXf67itLqZCwdMCRrw0N5FHbv5D7jrE";
    #else
```
配置子设备的三元组：`src/dev_model/examples/linkkit_example_gateway.c`
```
    #define EXAMPLE_SUBDEV_ADD_NUM          3
    #define EXAMPLE_SUBDEV_MAX_NUM          20
    const iotx_linkkit_dev_meta_info_t subdevArr[EXAMPLE_SUBDEV_MAX_NUM] = {
        {
            "a13Npv1vjZ4",
            "PKbZL7baK8pBso94",
            "example_sub1",
            "eglNFNJiRuR0yncB9RP05sSTY4FrUIoe"
        },
        {
            "a1YRfb9bepk",
            "PKbZL7baK8pBso94",
            "test_02",
            "jFsErM3uA7UfbS6J0hm0QaEXsQbmO6Pa"
        },
```

## <a name="5.1 menuconfig">5.1 menuconfig</a>

运行`make menuconfig`，进入`Device Model Configurations`选中`FEATURE_DEVICE_MODEL_GATEWAY`，即打开了网关功能。接着选中`FEATURE_DEV_BIND_ENABLED`，打开绑定功能。保存后退出。

## <a name="5.2 编译，运行例程">5.2 编译，运行例程</a>

运行`make`编译SDK，运行`./output/release/bin/linkkit-example-gateway`执行网关例程。

# <a name="6. 使用飞燕App交互式添加网关和子设备">6. 使用飞燕App交互式添加网关和子设备</a>

## <a name="6.1 添加网关">6.1 添加网关</a>

确保上节配置的例程已正常运行，本次演示的Ubuntu系统与手机在同一局域网中，因此打开App的添加设备页面即可看到此网关，点击即可完成添加。
<div align="left">
<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/pictures/add_gateway.jpg" width="300">
</div>

## <a name="6.1 交互式添加子设备：">6.1 交互式添加子设备：</a>

飞燕平台建议子设备通过手机扫描二维码交互的方式登陆云端，主要步骤如下：

1. 在子设备的产品页面按照下图获取子设备的配网二维码
<div align="left">
<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/pictures/subdev_barcode.jpg" width="600">
</div>

2. 打开飞燕公版App，在**添加设备**页面，点击扫码，扫码刚刚获取的子设备二维码，扫码成功后App会让用户选择加入哪个已有的网关设备
<div align="left">
<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/pictures/add_to_gw.jpg" width="300">
</div>

3. 添加子设备成功，子设备便会与对应网关建立拓扑关系，并绑定到用户账户。

# <a name="7.子设备接入逻辑">7.子设备接入逻辑</a>

本节主要讲解网关和子设备上云的几个关键步骤以及代码实现。

## <a name="7.1 网关与云端建连">7.1 网关与云端建连</a>

显而易见，网关与云端建连是一个网关能正常工作的首要条件。当网关设备连上以太网或者WiFi路由器并正常获取到IP地址后，即可调用以下例程代码连接云端。Linkkit SDK提供的WiFi配网方案可以很好的支持网络接口类型为WiFi的网关设备，详情可以查看配网的相关文档。
`IOT_Linkkit_Open`是用于创建设备(包括网关和子设备)资源的标准接口；
`IOT_Linkkit_Connect`则是用于设备与云端建立连接的标准接口。

```
    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_PERMIT_JOIN, user_permit_join_event_handler);

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

    /* Choose Login Server */
    domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Choose Whether You Need Post Property/Event Reply */
    post_event_reply = 0;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }
```

`IOT_Linkkit_Yield`是用于读取网络报文并做解析处理分发的标准接口，在网关产品编程中必须为其建立独立线程。
```
    void *user_dispatch_yield(void *args)
    {
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();

        while (user_example_ctx->g_user_dispatch_thread_running) {
            IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
        }

        return NULL;
    }

    user_example_ctx->g_user_dispatch_thread_running = 1;
    res = HAL_ThreadCreate(&user_example_ctx->g_user_dispatch_thread, user_dispatch_yield, NULL, NULL, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("HAL_ThreadCreate Failed\n");
        IOT_Linkkit_Close(user_example_ctx->master_devid);
        return -1;
    }
```

## <a name="7.2 将网关添加到飞燕App中">7.2 将网关添加到飞燕App中</a>

1. 如果网关设备是通过飞燕配网方案上云的，那么在配网成功网关会自动与用户账户建立绑定。具体可以查看配网的相关文章。
2. 如果网关为以太网设备，那还需要手动添加到飞燕App中，SDK必须使能bind功能(在`make menuconfig`中打开`FEATURE_DEV_BIND_ENABLED`)。本次演示的Ubuntu与手机在同一局域网中，因此打开App的添加设备页面即可看到此网关，点击即可完成添加。

## <a name="7.3 子设备添加">7.3 子设备添加</a>

网关通过自身的通信协议(如BLE，Zigbee)发现子设备后，可以将设备信息添加到一个设备列表中(如例程中的`subdevArr[EXAMPLE_SUBDEV_MAX_NUM]`数组)，设备列表中应该包含子设备的三元组信息，完整的三元组信息是子设备上云的必要条件。
+ 对于一机一密的子设备，这个三元组信息应该包括Productkey，DeviceName，DeviceSecret；
+ 对于一型一密的子设备，这个三元组信息应该包括Productkey，DeviceName，ProductSecret；

子设备上云主要使用到`IOT_Linkkit_Open`，`IOT_Linkkit_Connect`，`IOT_Linkkit_Report`这三个API
`IOT_Linkkit_Report`使用`ITM_MSG_LOGIN`参数选项用于登陆子设备。子设备通过`IOT_Linkkit_Connect`连接云端后还需要Login才能开始数据交互。

```
static int example_add_subdev(iotx_linkkit_dev_meta_info_t *meta_info)
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
## <a name="7.4 permit join交互逻辑">7.4 permit join交互逻辑</a>

飞燕平台建议子设备通过手机扫描二维码交互的方式登陆云端，用户扫描子设备的配网二维码，并选择所要加入的网关设备后，云端会下发permit_join事件给对应的网关，事件Payload包含了运行添加的子设备信息ProductKey和窗口事件
```
{
    "method": "thing.gateway.permit",
    "id": "696271662",
    "params": {
        "time": 60,
        "productKey": "a1YRfb9bepk"
    },
    "version": "1.0.0"
}
```
网关在收到permit_join事件后应触发对应子设备的添加，例程代码实现如下：
```
int user_permit_join_event_handler(const char *product_key, const int time)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Product Key: %s, Time: %d", product_key, time);

    user_example_ctx->permit_join = 1;  /* 触发子设备添加 */

    return 0;
}
```

# <a name="8. Reset的实现">8. Reset的实现</a>

当用户希望解除设备与账户的绑定关系是，既可以在App上的设备设置页面解除绑定关系，也可以在设备端发送解除绑定的报文到云端完成解绑。
`IOT_DevReset_Report`便是用于解绑的API，
```
    void example_devrst_evt_handle(iotx_devrst_evt_type_t evt, void *msg)
    {
        switch (evt)
        {
            case IOTX_DEVRST_EVT_RECEIVED: {
                iotx_devrst_evt_recv_msg_t *recv_msg = (iotx_devrst_evt_recv_msg_t *)msg;
                if (recv_msg->msgid != reset_mqtt_packet_id) {
                    return;
                }
                EXAMPLE_TRACE("Receive Reset Responst");
                EXAMPLE_TRACE("Msg ID: %d", recv_msg->msgid);
                EXAMPLE_TRACE("Payload: %.*s", recv_msg->payload_len, recv_msg->payload);
                reset_reply_received = 1;
            }
            break;

            default:
                break;
        }
    }

    int main(int argc, char *argv[]) {
        ......

        res = IOT_DevReset_Report(&meta_info, example_devrst_evt_handle, NULL);
        if (res < 0) {
            return -1;
        }
        ......
    }
```

# <a name="9. 注意事项">9. 注意事项</a>

1. 当子设备处于离线状态时，无法与云端进行数据交互
2. 在网关离线后，网关拓扑下的所有子设备都会离线；在网关重新上线后，用户需要自行处理子设备的Login/Logout操作

