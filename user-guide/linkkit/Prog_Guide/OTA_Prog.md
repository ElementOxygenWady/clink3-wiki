# <a name="目录">目录</a>
+ [功能说明](#功能说明)
+ [OTA例程讲解](#OTA例程讲解)
    * [用基础版接口实现的OTA例程](#用基础版接口实现的OTA例程)
    * [用高级版接口实现的OTA例程](#用高级版接口实现的OTA例程)
+ [OTA功能API](#OTA功能API)
    * [用基础版接口实现OTA功能涉及的API](#用基础版接口实现OTA功能涉及的API)
    * [用高级版接口实现OTA功能涉及的API](#用高级版接口实现OTA功能涉及的API)
+ [需要实现的HAL](#需要实现的HAL)
    * [用基础版接口实现OTA功能需要实现的API](#用基础版接口实现OTA功能需要实现的API)
    * [用高级版接口实现OTA功能需要实现的API](#用高级版接口实现OTA功能需要实现的API)

# <a name="功能说明">功能说明</a>

OTA(Over-the-Air Technology)即空中下载技术. 物联网平台支持通过OTA方式进行设备固件升级

基于MQTT协议下固件升级流程如下
---

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/brief_ota_mqtt.png" width="1000" hegiht="800" align=center />


# <a name="OTA例程讲解">OTA例程讲解</a>

> OTA整体流程请见[OTA服务](https://help.aliyun.com/document_detail/85700.html)

+ 通过OTA的API可以实现设备端固件下载, **但是如何存储/使用下载到的固件, 需要用户实现**
+ 存储固件是指将下载到的固件存储到FLASH等介质上
+ 使用固件, 包括加载新下载的固件, 需要用户根据业务的具体需求(比如需要用户点击升级按钮)来实现

---
下面用两个例子分别说明如何用`基础版`接口和`高级版`接口来实现OTA功能

## <a name="用基础版接口实现的OTA例程">用基础版接口实现的OTA例程</a>
> 现对照 `src/ota/examples/ota_example_mqtt.c` 例程分步骤讲解如何使用基础版的接口实现OTA的功能

1. OTA业务建立前的准备: 导入设备三元组, 初始化连接信息
---
```
int main(int argc, char *argv[]) {
    ...
    /**< get device info*/
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    /**< end*/
    _ota_mqtt_client()
}
```
2. 在_ota_mqtt_client函数完成建连和OTA的主要配置逻辑
---
```
    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(g_product_key, g_device_name, g_device_secret, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        rc = -1;
        goto do_exit;
    }

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
3. 在_ota_mqtt_client函数进行OTA有关的初始化工作(主要是订阅跟这个设备有关的固件升级信息)
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

+ 到IoT控制台的 [OTA服务](https://iot.console.aliyun.com/service/ota) 页面, 点击"新增固件",
+ 点击"创建固件", "验证固件",
+ 点击这个新增固件的"批量升级"按钮, 从中选择设备所属产品为 examples/ota/ota_mqtt-example.c 中三元组对应的产品,
+ 待升级版本号点开下拉框选当前版本号, "升级范围"选"定向升级", 再从"设备范围"中选当前的三元组对应的设备, 点击确定即可

5. 下载OTA内容, 上报下载进度
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
        /* 获取已下载到的数据量, 文件总大小, md5信息, 版本号等信息 */
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

## <a name="用高级版接口实现的OTA例程">用高级版接口实现的OTA例程</a>
> 现对照`src/dev_model/examples/linkkit_example_solo.c`分步骤讲解如何使用高级版的接口实现OTA的功能

1. 初始化主设备，注册FOTA的回调函数，建立与云端的连接
---
```
int res = 0;
int domain_type = 0, dynamic_register = 0, post_reply_need = 0;
iotx_linkkit_dev_meta_info_t master_meta_info;

memset(&g_user_example_ctx, 0, sizeof(user_example_ctx_t));

memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

/* Register Callback */
...
...
IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);

domain_type = IOTX_CLOUD_REGION_SHANGHAI;
IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

/* Choose Login Method */
dynamic_register = 0;
IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

/* post reply doesn't need */
post_reply_need = 1;
IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_reply_need);

/* Create Master Device Resources */
g_user_example_ctx.master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
if (g_user_example_ctx.master_devid < 0) {
    EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
    return -1;
}

/* Start Connect Aliyun Server */
res = IOT_Linkkit_Connect(g_user_example_ctx.master_devid);
if (res < 0) {
    EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
    return -1;
}
```

2. 实现上述代码中的回调函数`user_fota_event_handler`：
---
该回调函数在如下两种情况下会被触发:
+ 直接收到云端下发的新固件通知时
+ 由设备端主动发起新固件查询，云端返回新固件通知时

在收到新固件通知后，可调用`IOT_Linkkit_Query`进行固件下载
```
int user_fota_event_handler(int type, const char *version)
{
    char buffer[128] = {0};
    int buffer_length = 128;

    /* 0 - new firmware exist, query the new firmware */
    if (type == 0) {
        EXAMPLE_TRACE("New Firmware Version: %s", version);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}
```
**注意**：
- 使用`ITM_MSG_QUERY_FOTA_DATA`选项调用`IOT_Linkkit_Query`时，此接口会阻塞直到镜像下载结束
- `buffer_length`指定了镜像下载的分片大小(每个HTTP GET请求的Content-Length大小)，用户应根据设备网络性能合理配置分片大小以提高下载数据。
- 若`IOT_Linkkit_Query`返回`FAIL_RETURN`则固件下载失败；返回`SUCCESS_RETURN`则固件下载成功。


3. 用户主动发起新固件查询：
---
```
IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_REQUEST_FOTA_IMAGE,
                      (unsigned char *)("app-1.0.0-20180101.1001"), 30);
```

4. 固件的存储
---
用户需要实现如下3个HAL接口来实现固件的存储
```
/* SDK在开始下载固件之前进行调用 */
void HAL_Firmware_Persistence_Start(void);

/* SDK在接收到固件数据时进行调用 */
int HAL_Firmware_Persistence_Write(char *buffer, uint32_t length);

/* SDK在固件下载结束时进行调用 */
int HAL_Firmware_Persistence_Stop(void);
```

# <a name="OTA功能API">OTA功能API</a>
## <a name="用基础版接口实现OTA功能涉及的API">用基础版接口实现OTA功能涉及的API</a>
| 函数名                                                                                                                                                  | 说明
|---------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------
| [IOT_OTA_Init](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_Init)                      | OTA实例的构造函数, 创建一个OTA会话的句柄并返回
| [IOT_OTA_Deinit](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_Deinit)                  | OTA实例的摧毁函数, 销毁所有相关的数据结构
| [IOT_OTA_Ioctl](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_Ioctl)                    | OTA实例的输入输出函数, 根据不同的命令字可以设置OTA会话的属性, 或者获取OTA会话的状态
| [IOT_OTA_GetLastError](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_GetLastError)      | OTA会话阶段, 若某个 IOT_OTA_XXX() 函数返回错误, 调用此接口可获得最近一次的详细错误码
| [IOT_OTA_ReportVersion](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_ReportVersion)    | OTA会话阶段, 向服务端汇报当前的固件版本号
| [IOT_OTA_FetchYield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_FetchYield)          | OTA下载阶段, 在指定的`timeout`时间内, 从固件服务器下载一段固件内容, 保存在入参buffer中
| [IOT_OTA_IsFetchFinish](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_IsFetchFinish)    | OTA下载阶段, 判断迭代调用 [IOT_OTA_FetchYield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_FetchYield) 是否已经下载完所有的固件内容
| [IOT_OTA_IsFetching](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_IsFetching)          | OTA下载阶段, 判断固件下载是否仍在进行中, 尚未完成全部固件内容的下载
| [IOT_OTA_ReportProgress](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides#IOT_OTA_ReportProgress)  | 可选API, OTA下载阶段, 调用此函数向服务端汇报已经下载了全部固件内容的百分之多少

## <a name="用高级版接口实现OTA功能涉及的API">用高级版接口实现OTA功能涉及的API</a>

| 函数名                                                  | 说明
|---------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------
| [IOT_Linkkit_Open](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Open)                   | 创建本地资源, 在进行网络报文交互之前, 必须先调用此接口, 得到一个会话的句柄
| [IOT_Linkkit_Connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Connect)             | 对主设备/网关来说, 将会建立设备与云端的通信. 对于子设备来说, 将向云端注册该子设备(若需要), 并添加主子设备拓扑关系
| [IOT_Linkkit_Yield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Yield)                 | 若SDK占有独立线程, 该函数只将接收到的网络报文分发到用户的回调函数中, 否则表示将CPU交给SDK让其接收网络报文并将消息分发到用户的回调函数中
| [IOT_Linkkit_Close](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Close)                 | 若入参中的会话句柄为主设备/网关, 则关闭网络连接并释放SDK为该会话所占用的所有资源
| [IOT_Linkkit_Query](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Query)                 | 向云端发送**存在云端业务数据下发的查询报文**, 包括OTA状态查询/OTA固件下载/子设备拓扑查询/NTP时间查询等各种报文
| [IOT_RegisterCallback](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_RegisterCallback)   | 对SDK注册事件回调函数, 如云端连接成功/失败, 有属性设置/服务请求到达, 子设备管理报文答复等

# <a name="需要实现的HAL">需要实现的HAL</a>
## <a name="用基础版接口实现OTA功能需要实现的API">用基础版接口实现OTA功能需要实现的API</a>
无

## <a name="用高级版接口实现OTA功能需要实现的API">用高级版接口实现OTA功能需要实现的API</a>
| 函数名                                                  | 说明
|---------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------
| [HAL_Firmware_Persistence_Start](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/OTA_Requires#HAL_Firmware_Persistence_Start)                   | 固件持久化功能开始
| [HAL_Firmware_Persistence_Write](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/OTA_Requires#HAL_Firmware_Persistence_Write)             | 固件持久化写入固件
| [HAL_Firmware_Persistence_Stop](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/OTA_Requires#HAL_Firmware_Persistence_Stop)                 | 固件持久化功能结束