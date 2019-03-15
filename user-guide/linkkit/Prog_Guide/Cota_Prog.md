# <a name="目录">目录</a>
+ [例程讲解](#例程讲解)
    * [例程讲解1: 服务端向设备端主动下推远程配置信息](#例程讲解1: 服务端向设备端主动下推远程配置信息)
    * [例程讲解2: 设备端向服务端主动查询远程配置信息](#例程讲解2: 设备端向服务端主动查询远程配置信息)
+ [远程配置功能API](#远程配置功能API)
+ [需要实现的HAL](#需要实现的HAL)

+ [例程讲解](#例程讲解)
+ [远程配置功能API](#远程配置功能API)
+ [需要实现的HAL](#需要实现的HAL)


# <a name="例程讲解">例程讲解</a>

服务端可以向设备端主动下推远程配置信息, 设备端也可以向服务端主动查询远程配置信息。设备离线期间服务端如果发布过远程配置信息, 设备端可以通过后者这种方式来获取到
更多COTA相关功能说明可查看[远程配置](https://help.aliyun.com/document_detail/84891.html)
本节将先介绍前者, 再介绍后者

## <a name="例程讲解1: 服务端向设备端主动下推远程配置信息">例程讲解1: 服务端向设备端主动下推远程配置信息</a>

```
/* 1.定义回调函数 */
static int user_cota_event_handler(int type, const char *config_id, int config_size, const char *get_type,
                                   const char *sign, const char *sign_method, const char *url)
{
    char buffer[128] = {0};
    int buffer_length = 128;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (type == 0) {
        EXAMPLE_TRACE("New Config ID: %s", config_id);
        EXAMPLE_TRACE("New Config Size: %d", config_size);
        EXAMPLE_TRACE("New Config Type: %s", get_type);
        EXAMPLE_TRACE("New Config Sign: %s", sign);
        EXAMPLE_TRACE("New Config Sign Method: %s", sign_method);
        EXAMPLE_TRACE("New Config URL: %s", url);

        IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_QUERY_COTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}


int main(int argc, char **argv)
{
/* 2.注册COTA回调 */
...
IOT_RegisterCallback(ITE_COTA, user_cota_event_handler);
...
/* 3.设置三元组 */
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));
...
/* 4.与云端建立连接的配置, 调用IOT_Ioctl()进行相关配置, 详细情况可查看对应API说明 */
   user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }
...
/* 5.等待服务端publish远程配置的信息. 获取到远程的配置信息后, 回调函数user_cota_event_handler会被触发, 可以观察该函数中打印出来的日志 */
    while (1) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
        EXAMPLE_TRACE("yield loop\n");
    }
}
```
## <a name="例程讲解2: 设备端向服务端主动查询远程配置信息">例程讲解2: 设备端向服务端主动查询远程配置信息</a>


```
/* 1.定义回调函数 */
static int user_cota_event_handler(int type, const char *config_id, int config_size, const char *get_type,
                                   const char *sign, const char *sign_method, const char *url)
{
    char buffer[128] = {0};
    int buffer_length = 128;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (type == 0) {
        EXAMPLE_TRACE("New Config ID: %s", config_id);
        EXAMPLE_TRACE("New Config Size: %d", config_size);
        EXAMPLE_TRACE("New Config Type: %s", get_type);
        EXAMPLE_TRACE("New Config Sign: %s", sign);
        EXAMPLE_TRACE("New Config Sign Method: %s", sign_method);
        EXAMPLE_TRACE("New Config URL: %s", url);

        IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_QUERY_COTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}


int main(int argc, char **argv)
{
/* 2.注册COTA回调 */
...
IOT_RegisterCallback(ITE_COTA, user_cota_event_handler);
...
/* 3.设置三元组 */
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));
...
/* 4.与云端建立连接的配置, 调用IOT_Ioctl()进行相关配置, 详细情况可查看对应API说明 */
   user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }
...
/* 5.用户通过IOT_Linkkit_Query向服务端查询远程配置信息, 如果服务端有这个信息, 则会下发给该设备, 触发设备端的user_cota_event_handler函数被调用.  */
       IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_REQUEST_COTA, NULL, 0);
}
```

# <a name="远程配置功能API">远程配置功能API</a>

| 函数名                                                  | 说明
|---------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------
| [IOT_Linkkit_Open](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Open)                   | 创建本地资源, 在进行网络报文交互之前, 必须先调用此接口, 得到一个会话的句柄
| [IOT_Linkkit_Connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Connect)             | 对主设备/网关来说, 将会建立设备与云端的通信. 对于子设备来说, 将向云端注册该子设备(若需要), 并添加主子设备拓扑关系
| [IOT_Linkkit_Yield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Yield)                 | 若SDK占有独立线程, 该函数只将接收到的网络报文分发到用户的回调函数中, 否则表示将CPU交给SDK让其接收网络报文并将消息分发到用户的回调函数中
| [IOT_Linkkit_Close](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Close)                 | 若入参中的会话句柄为主设备/网关, 则关闭网络连接并释放SDK为该会话所占用的所有资源
| [IOT_Linkkit_Query](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Query)                 | 向云端发送**存在云端业务数据下发的查询报文**, 包括OTA状态查询/OTA固件下载/子设备拓扑查询/NTP时间查询等各种报文


# <a name="需要实现的HAL">需要实现的HAL</a>
无

---



