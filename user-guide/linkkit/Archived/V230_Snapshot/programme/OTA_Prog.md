# <a name="目录">目录</a>
+ [OTA例程讲解](#OTA例程讲解)
    * [用高级版接口实现的OTA例程](#用高级版接口实现的OTA例程)
    * [用基础版接口实现的OTA例程](#用基础版接口实现的OTA例程)
+ [OTA功能API](#OTA功能API)
    * [用基础版接口实现OTA功能涉及的API](#用基础版接口实现OTA功能涉及的API)
    * [用高级版接口实现OTA功能涉及的API](#用高级版接口实现OTA功能涉及的API)
+ [需要实现的HAL](#需要实现的HAL)
    * [用基础版接口实现OTA功能需要实现的API](#用基础版接口实现OTA功能需要实现的API)
    * [用高级版接口实现OTA功能需要实现的API](#用高级版接口实现OTA功能需要实现的API)
+ [HAL的参考实现](#HAL的参考实现)


# <a name="OTA例程讲解">OTA例程讲解</a>

OTA整体流程请见[OTA服务](
https://help.aliyun.com/document_detail/85700.html)。
通过OTA的API可以实现设备端固件下载， `但是如何存储/使用下载到的固件, 需要用户实现`. 存储固件是指将下载到的固件存储到FLASH等介质上. 使用固件, 包括加载新下载的固件, 需要用户根据业务的具体需求(比如需要用户点击升级按钮)来实现.
下面用两个例子分别说明如何用`高级版`和`基础版`接口来实现OTA功能

## <a name="用高级版接口实现的OTA例程">用高级版接口实现的OTA例程</a>

> 现对照 `examples/linkkit/linkkit_example_solo.c` 例程分步骤讲解通过高级版接口实现OTA固件升级的功能

1. 定义和注册用户收到固件升级信息后的回调函数
---
```
/* 定义回调函数 */
static int user_fota_event_handler(int type, const char *version)
{
    char buffer[128] = {0};
    int buffer_length = 128;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (type == 0) {
        EXAMPLE_TRACE("New Firmware Version: %s", version);

        IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

/* 注册回调 */
int linkkit_main(void *paras)
{
...
IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);
...
}
```

2. 设置三元组
---
```
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));
```

3. 设置与云端建立连接的配置, 调用IOT_Ioctl()进行相关配置, 详细情况可查看对应API说明
---
```
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
```
4. 建立连接
---
```
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }
```
IOT_Linkkit_Connect订阅了固件升级有关的topic, 因此服务端在发布固件升级信息的时候, 设备端可以接收到. 从而实现了OTA的功能

5. 等待服务端publish固件升级的信息. 获取到固件升级信息后, 回调函数user_fota_event_handler会被触发, 可以观察该函数中打印出来的日志
---
```
    while (1) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
        ...
    }

```
6. 固件的存储
---
开始下载前, linkkit会调用HAL_Firmware_Persistence_Start去打开固件文件, 获取到一个可以写入的fd. 开始下载后, linkkit会调用HAL_Firmware_Persistence_Write去存储固件文件. 下载结束后, linkkit会调用HAL_Firmware_Persistence_Stop去关闭固件文件. 高级版的接口实现OTA的功能的过程中, 用户只要需要实现`这三个HAL函数`就能实现固件的存储. 但是在存储完成后, 如何使用新的固件(比如选择时机重启以加载新固件), 用户还需要自己另外实现

## <a name="用基础版接口实现的OTA例程">用基础版接口实现的OTA例程</a>
> 现对照 `examples/ota/ota_mqtt-example.c` 例程分步骤讲解如何使用基础版的接口实现OTA的功能
> 基础版接口实现OTA较为复杂，推荐使用高级版接口进行OTA实现

1. OTA业务建立前的准备: 导入设备三元组, 初始化连接信息
---
```
    /**< get device info*/
    HAL_GetProductKey(g_product_key);
    HAL_GetDeviceName(g_device_name);
    HAL_GetDeviceSecret(g_device_secret);
    /**< end*/

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(g_product_key, g_device_name, g_device_secret, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        rc = -1;
        goto do_exit;
    }
```

2. 配置MQTT参数, 构建MQTT连接会话, 与云端服务器建立连接
---
```
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
    mqtt_params.read_buf_size = OTA_MQTT_MSGLEN;
    mqtt_params.write_buf_size = OTA_MQTT_MSGLEN;

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
3. 进行OTA有关的初始化工作(主要是订阅跟这个设备有关的固件升级信息)
---
```
    h_ota = IOT_OTA_Init(PRODUCT_KEY, DEVICE_NAME, pclient);
    if (NULL == h_ota) {
        rc = -1;
        EXAMPLE_TRACE("initialize OTA failed");
        goto do_exit;
    }
```
4. 建立一个循环, 一直去尝试接收OTA升级的消息
---
```
    int ota_over = 0;
    do {
        uint32_t firmware_valid;

        EXAMPLE_TRACE("wait ota upgrade command....");

        /* 接收MQTT消息 */
        IOT_MQTT_Yield(pclient, 200);

        /* 判断接收到的消息中是否有固件升级的消息 */
        if (IOT_OTA_IsFetching(h_ota)) {
         /* 下载OTA内容, 上报下载进度,见章节 "5. 下载OTA内容, 上报下载进度" */
         /* 校验固件的md5, 见章节 "6. 校验md5的值" */
        }

        } while (!ota_over);
```
需要到服务端推送一个固件升级事件下去, IOT_OTA_IsFetching返回才能结果为1, 才能走入固件升级的逻辑. 推送固件升级事件的具体步骤如下:
到[OTA服务](https://iot.console.aliyun.com/service/ota), 点击"新增固件",
点击"创建固件", "验证固件",
点击这个新增固件的"批量升级"按钮, 从中选择设备所属产品为 examples/ota/ota_mqtt-example.c 中三元组对应的产品,
待升级版本号点开下拉框选当前版本号, "升级范围"选"定向升级", 再从"设备范围"中选当前的三元组对应的设备, 点击确定即可

5. 下载OTA内容, 上报下载进度
---
```
    do {
        /* 下载OTA固件 */
        len = IOT_OTA_FetchYield(h_ota, buf_ota, OTA_BUF_LEN, 1);
        if (len > 0) {
            if (1 != fwrite(buf_ota, len, 1, fp)) {
                EXAMPLE_TRACE("write data to file failed");
                rc = -1;
                break;
            }
        } else {
            /* 上报已下载进度 */
            IOT_OTA_ReportProgress(h_ota, IOT_OTAP_FETCH_FAILED, NULL);
            EXAMPLE_TRACE("ota fetch fail");
        }

        /* get OTA information */
        /* 获取已下载到的数据量, 文件总大小, md5信息, 版本号等信息 */
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_FETCHED_SIZE, &size_downloaded, 4);
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &size_file, 4);
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_MD5SUM, md5sum, 33);
        IOT_OTA_Ioctl(h_ota, IOT_OTAG_VERSION, version, 128);

        last_percent = percent;
        percent = (size_downloaded * 100) / size_file;
        if (percent - last_percent > 0) {
            /* 上报已下载进度 */
            IOT_OTA_ReportProgress(h_ota, percent, NULL);
            IOT_OTA_ReportProgress(h_ota, percent, "hello");
        }
        IOT_MQTT_Yield(pclient, 100);
        /* 判断下载是否结束 */
    } while (!IOT_OTA_IsFetchFinish(h_ota));
```
6. 校验md5的值
---
```
    IOT_OTA_Ioctl(h_ota, IOT_OTAG_CHECK_FIRMWARE, &firmware_valid, 4);
    if (0 == firmware_valid) {
        EXAMPLE_TRACE("The firmware is invalid");
    } else {
        EXAMPLE_TRACE("The firmware is valid");
    }

    ota_over = 1;
```
7. 用户通过IOT_OTA_Deinit释放所有资源
---
```
    if (NULL != h_ota) {
        IOT_OTA_Deinit(h_ota);
    }

    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }

    if (NULL != msg_buf) {
        HAL_Free(msg_buf);
    }

    if (NULL != msg_readbuf) {
        HAL_Free(msg_readbuf);
    }

    if (NULL != fp) {
        fclose(fp);
    }

    return rc;

```
8. 固件的存储
---
在_ota_mqtt_client函数中通过下述方式打开, 写入和关闭一个文件
```
fp = fopen("ota.bin", "wb+")
...
if (1 != fwrite(buf_ota, len, 1, fp)) {
    EXAMPLE_TRACE("write data to file failed");
    rc = -1;
    break;
}
...
if (NULL != fp) {
    fclose(fp);
}
```




# <a name="OTA功能API">OTA功能API</a>
## <a name="用基础版接口实现OTA功能涉及的API">用基础版接口实现OTA功能涉及的API</a>
| 函数名                                                                                                                                              | 说明
|-----------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------
| [IOT_OTA_Init](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_Init)                        | OTA实例的构造函数, 创建一个OTA会话的句柄并返回
| [IOT_OTA_Deinit](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_Deinit)                    | OTA实例的摧毁函数, 销毁所有相关的数据结构
| [IOT_OTA_Ioctl](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_Ioctl)                      | OTA实例的输入输出函数, 根据不同的命令字可以设置OTA会话的属性, 或者获取OTA会话的状态
| [IOT_OTA_GetLastError](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_GetLastError)        | OTA会话阶段, 若某个 IOT_OTA_XXX() 函数返回错误, 调用此接口可获得最近一次的详细错误码
| [IOT_OTA_ReportVersion](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_ReportVersion)      | OTA会话阶段, 向服务端汇报当前的固件版本号
| [IOT_OTA_FetchYield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_FetchYield)            | OTA下载阶段, 在指定的`timeout`时间内, 从固件服务器下载一段固件内容, 保存在入参buffer中
| [IOT_OTA_IsFetchFinish](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_IsFetchFinish)      | OTA下载阶段, 判断迭代调用 [IOT_OTA_FetchYield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_FetchYield) 是否已经下载完所有的固件内容
| [IOT_OTA_IsFetching](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_IsFetching)            | OTA下载阶段, 判断固件下载是否仍在进行中, 尚未完成全部固件内容的下载
| [IOT_OTA_ReportProgress](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_ReportProgress)    | 可选API, OTA下载阶段, 调用此函数向服务端汇报已经下载了全部固件内容的百分之多少

## <a name="用高级版接口实现OTA功能涉及的API">用高级版接口实现OTA功能涉及的API</a>

| 函数名                                                  | 说明
|---------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------
| [IOT_Linkkit_Open](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Open)                   | 创建本地资源, 在进行网络报文交互之前, 必须先调用此接口, 得到一个会话的句柄
| [IOT_Linkkit_Connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Connect)             | 对主设备/网关来说, 将会建立设备与云端的通信. 对于子设备来说, 将向云端注册该子设备(若需要), 并添加主子设备拓扑关系
| [IOT_Linkkit_Yield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Yield)                 | 若SDK占有独立线程, 该函数只将接收到的网络报文分发到用户的回调函数中, 否则表示将CPU交给SDK让其接收网络报文并将消息分发到用户的回调函数中
| [IOT_Linkkit_Close](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Close)                 | 若入参中的会话句柄为主设备/网关, 则关闭网络连接并释放SDK为该会话所占用的所有资源
| [IOT_Linkkit_TriggerEvent](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_TriggerEvent)   | 向云端发送**事件报文**, 如错误码, 异常告警等
| [IOT_Linkkit_Report](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Report)               | 向云端发送**没有云端业务数据下发的上行报文**, 包括属性值/设备标签/二进制透传数据/子设备管理等各种报文
| [IOT_Linkkit_Query](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Query)                 | 向云端发送**存在云端业务数据下发的查询报文**, 包括OTA状态查询/OTA固件下载/子设备拓扑查询/NTP时间查询等各种报文


# <a name="需要实现的HAL">需要实现的HAL</a>
## <a name="用基础版接口实现OTA功能需要实现的API">用基础版接口实现OTA功能需要实现的API</a>
无
## <a name="用高级版接口实现OTA功能需要实现的API">用高级版接口实现OTA功能需要实现的API</a>
| 函数名                          | 说明
|---------------------------------|-----------------------------------------
| HAL_Firmware_Persistence_Start  | 固件持久化开始, 包含OTA功能时必须实现
| HAL_Firmware_Persistence_Stop   | 固件持久化结束, 包含OTA功能时必须实现
| HAL_Firmware_Persistence_Write  | 固件持久化写入, 包含OTA功能时必须实现


# <a name="HAL的参考实现">HAL的参考实现</a>
---
```
static FILE *fp;
#define otafilename "/tmp/alinkota.bin"
void HAL_Firmware_Persistence_Start(void)
{
#ifdef __DEMO__
    fp = fopen(otafilename, "w");
    //    assert(fp);
#endif
    return;
}

int HAL_Firmware_Persistence_Stop(void)
{
#ifdef __DEMO__
    if (fp != NULL) {
        fclose(fp);
    }
#endif

    /* check file md5, and burning it to flash ... finally reboot system */

    return 0;
}

int HAL_Firmware_Persistence_Write(_IN_ char *buffer, _IN_ uint32_t length)
{
#ifdef __DEMO__
    unsigned int written_len = 0;
    written_len = fwrite(buffer, 1, length, fp);

    if (written_len != length) {
        return -1;
    }
#endif
    return 0;
}
```
