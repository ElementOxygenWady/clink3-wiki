# <a name="目录">目录</a>
+ [例程讲解](#例程讲解)
+ [HTTP功能API](#HTTP功能API)
+ [需要实现的HAL](#需要实现的HAL)

# <a name="例程讲解">例程讲解</a>
HTTP只适用于设备端向上报属性和事件的场景, 如果产品需要支持服务, 那么不应该选择HTTP
当前SDK中默认都是开启了TLS功能, 因此下文的部分说明中用到了HTTPS来表示HTTP+TLS

> 现对照 `examples/http/http_example.c` 例程分步骤讲解如何使用这几个API往云端上报数据

1. 导入三元组信息, 创建一个用于HTTP会话的句柄
---
```
    iotx_device_info_t      device_info;
    iotx_http_param_t       http_param;
    memset(&http_param, 0, sizeof(http_param));

    strncpy(device_info.product_key,  IOTX_PRODUCT_KEY, IOTX_PRODUCT_KEY_LEN);
    strncpy(device_info.device_secret, IOTX_DEVICE_SECRET, IOTX_DEVICE_SECRET_LEN);
    strncpy(device_info.device_name,  IOTX_DEVICE_NAME, IOTX_DEVICE_NAME_LEN);
    strncpy(device_info.device_id,  IOTX_DEVICE_ID, IOTX_DEVICE_ID_LEN);

    http_param.device_info = &device_info;
    http_param.timeout_ms = DEFAULT_TIMEOUT_MS;
    void                   *handle = NULL;

    handle = IOT_HTTP_Init(&http_param);
```
2. 进行设备认证
---
```
    if (NULL != handle) {
        IOT_HTTP_DeviceNameAuth(handle);
        HAL_Printf("IoTx HTTP Message Sent\r\n");
    } else {
        HAL_Printf("IoTx HTTP init failed\r\n");
        return 0;
    }
```
3. 往HTTP服务器发送消息, 并判断返回值
---
```
    iotx_http_message_param_t msg;
    char path[IOTX_URI_MAX_LEN + 1] = { 0 };
    int loop_cnt = 100;
    int success_cnt = 0;
    int cnt = 0;

    if (NULL == handle) {
        return;
    }

    HAL_Snprintf(request_buf, MAX_BUF_LEN, "{\"name\":\"hello world\"}");
    memset(response_buf, 0x00, MAX_BUF_LEN);
    HAL_Snprintf(path, IOTX_URI_MAX_LEN, "/topic/%s/%s/data",
                 IOTX_PRODUCT_KEY,
                 IOTX_DEVICE_NAME);
    msg.request_payload = request_buf;
    msg.response_payload = response_buf;
    msg.timeout_ms = 5000;
    msg.request_payload_len = strlen(msg.request_payload) + 1;
    msg.response_payload_len = MAX_BUF_LEN;
    msg.topic_path = path;

    while (cnt++ < loop_cnt) {
        if (0 == IOT_HTTP_SendMessage(handle, &msg)) {
            success_cnt++;
        }
    }
    EXAMPLE_TRACE("loop cnt is %d, success_cnt is %d\n", loop_cnt, success_cnt);
```
4. 断开HTTP连接, 销毁句柄
---
```
    IOT_HTTP_Disconnect(handle);
    IOT_HTTP_DeInit(&handle);
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();
 ```

# <a name="HTTP功能API">HTTP功能API</a>
---
| 函数名                                                  | 说明
|---------------------------------------------------------|-------------------------------------------------------------
| [IOT_HTTP_Init](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/Provided_APIs#IOT_HTTP_Init)                         | Https实例的构造函数, 创建一个HTTP会话的句柄并返回
| [IOT_HTTP_DeInit](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/Provided_APIs#IOT_HTTP_DeInit)                     | Https实例的摧毁函数, 销毁所有相关的数据结构
| [IOT_HTTP_DeviceNameAuth](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/Provided_APIs#IOT_HTTP_DeviceNameAuth)     | 基于控制台申请的`DeviceName`, `DeviceSecret`, `ProductKey`做设备认证
| [IOT_HTTP_SendMessage](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/Provided_APIs#IOT_HTTP_SendMessage)           | Https会话阶段, 组织一个完整的HTTP报文向服务器发送,并同步获取HTTP回复报文
| [IOT_HTTP_Disconnect](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Prog_Guide/Provided_APIs#IOT_HTTP_Disconnect)             | Https会话阶段, 关闭HTTP层面的连接, 但是仍然保持TLS层面的连接


# <a name="需要实现的HAL">需要实现的HAL</a>
| 函数名                                      | 说明
|---------------------------------------------|-------------------------------------------------------------------------
| HAL_SSL_Destroy        | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能
| HAL_SSL_Establish     | 建立一个TLS连接, 用于MQTT功能, HTTPS功能
| HAL_SSL_Read               | 从一个TLS连接中读数据, 用于MQTT功能, HTTPS功能
| HAL_SSL_Write             | 向一个TLS连接中写数据, 用于MQTT功能, HTTPS功能
| HAL_TCP_Destroy        | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能
| HAL_TCP_Establish     | 建立一个TCP连接, 包含了域名解析的动作和TCP连接的建立
| HAL_TCP_Read               | 在指定时间内, 从TCP连接读取流数据, 并返回读到的字节数
| HAL_TCP_Write             | 在指定时间内, 向TCP连接发送流数据, 并返回发送的字节数
| HAL_Random                   | 随机数函数, 接受一个无符号数作为范围, 返回0到该数值范围内的随机无符号数
| HAL_Srandom                 | 随机数播种函数, 使 [HAL_Random](#HAL_Random) 的返回值每个执行序列各不相同, 类似`srand`
