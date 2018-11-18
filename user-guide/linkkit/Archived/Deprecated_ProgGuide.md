# <a name="目录">目录</a>
+ [物模型管理旧版API](#物模型管理旧版API)

# <a name="物模型管理旧版API">物模型管理旧版API</a>

| 函数名                                                                              | 说明
|-------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------
| [linkkit_start](#linkkit_start)                                                     | 启动 linkkit 服务, 与云端建立连接并安装回调函数
| [linkkit_end](#linkkit_end)                                                         | 停止 linkkit 服务, 与云端断开连接并回收资源
| [linkkit_dispatch](#linkkit_dispatch)                                               | 事件分发函数, 触发 [linkkit_start](#linkkit_start) 安装的回调
| [linkkit_yield](#linkkit_yield)                                                     | linkkit 主循环函数, 内含了心跳的维持, 服务器下行报文的收取等; 如果允许多线程, 请不要调用此函数
| [linkkit_set_value](#linkkit_set_value)                                             | 根据identifier设置物对象的 TSL 属性
| [linkkit_get_value](#linkkit_get_value)                                             | 根据identifier获取物对象的 TSL 属性
| [linkkit_set_tsl](#linkkit_set_tsl)                                                 | 从本地读取 TSL 文件,生成物的对象并添加到 linkkit 中
| [linkkit_answer_service](#linkkit_answer_service)                                   | 对云端服务请求进行回应
| [linkkit_invoke_raw_service](#linkkit_invoke_raw_service)                           | 向云端发送裸数据
| [linkkit_trigger_event](#linkkit_trigger_event)                                     | 上报设备事件到云端
| [linkkit_fota_init](#linkkit_fota_init)                                             | 初始化 OTA-fota 服务, 并安装回调函数(需编译设置宏 OTA_ENABLED)
| [linkkit_invoke_fota_service](#linkkit_invoke_fota_service)                         | 执行fota服务
| [linkkit_cota_init](#linkkit_cota_init)                                             | 初始化 OTA-cota 服务, 并安装回调函数(需编译设置宏 OTA_ENABLED)
| [linkkit_invoke_cota_get_config](#linkkit_invoke_cota_get_config)                   | 设备请求远程配置
| [linkkit_invoke_cota_service](#linkkit_invoke_cota_service)                         | 执行cota服务
| [linkkit_post_property](#linkkit_post_property)                                     | 上报设备属性到云端
| [linkkit_set_opt](#linkkit_set_opt)                                                 | 设置设备属性和服务上报参数
| [linkkit_try_leave](#linkkit_try_leave)                                             | 设置linkkit离开标志
| [linkkit_is_try_leave](#linkkit_is_try_leave)                                       | 获取linkkit离开标志
| [linkkit_is_end](#linkkit_is_end)                                                   | 获取linkkit结束标志
| [linkkit_trigger_extended_info_operate](#linkkit_trigger_extended_info_operate)     | 设备拓展信息上报或删除
| [linkkit_ntp_time_request](#linkkit_ntp_time_request)                               | 设备通过NTP协议从云端获取NTP时间

> 现对照 `examples/linkkit/deprecated/solo.c` 例程分步骤讲解如何使用这几个API实现MQTT上云功能

1. 用户只需要调用`linkkit_start()`便可以完成linkkit的初始化, 并与云端建立连接
---
用户可配置是否从云端拉取TSL, 将 `get_tsl_from_cloud` 设置为 `1` 即可

```
    int get_tsl_from_cloud = 0;                        /* the param of whether it is get tsl from cloud */
    linkkit_ops_t linkkit_ops = {
        .on_connect           = on_connect,            /* connect handler */
        .on_disconnect        = on_disconnect,         /* disconnect handler */
        .raw_data_arrived     = raw_data_arrived,      /* receive raw data handler */
        .thing_create         = thing_create,          /* thing created handler */
        .thing_enable         = thing_enable,          /* thing enabled handler */
        .thing_disable        = thing_disable,         /* thing disabled handler */
        .thing_call_service   = thing_call_service,    /* self-defined service handler */
        .thing_prop_changed   = thing_prop_changed,    /* property set handler */
        .linkit_data_arrived  = linkit_data_arrived,   /* transparent transmission data handler */
    };

    EXAMPLE_TRACE("linkkit start");

    if (-1 == linkkit_start(16, get_tsl_from_cloud, linkkit_loglevel_debug, &linkkit_ops, linkkit_cloud_domain_shanghai,
                            &sample_ctx)) {
        EXAMPLE_TRACE("linkkit start fail");
        return -1;
    }
```

2. 对于本地预置TSL的情况, 需要调用`linkkit_set_tsl()`导入TSL文件
---
```
    if (!get_tsl_from_cloud) {
        /*
         * if get_tsl_from_cloud = 0, set default tsl [TSL_STRING]
         * please modify TSL_STRING by the TSL's defined.
         */
        linkkit_set_tsl(TSL_STRING, strlen(TSL_STRING));
    }
```

3. 若需要使用FOTA和COTA功能, 可紧接在`linkkit_start()`后进行初始化
---
```
    linkkit_cota_init(linkkit_cota_callback);
    linkkit_fota_init(linkkit_fota_callback);
```

4. 在单线程场景下, while循环中会包含`linkkit_dispatch()`和`linkkit_yield()`两个函数. 分别用于消息的处理分发和服务器下行数据的收取, 心跳的维持
---
```
    while (1) {
        /*
         * if linkkit is support Multi-thread, the linkkit_dispatch and linkkit_yield with callback by linkkit,
         * else it need user to call these function to received data.
         */
#if (CONFIG_SDK_THREAD_COST == 0)
        linkkit_dispatch();
#endif
        now = uptime_sec();
        if (prev_sec == now) {
#if (CONFIG_SDK_THREAD_COST == 0)
            linkkit_yield(100);
#else
            HAL_SleepMs(100);
#endif /* CONFIG_SDK_THREAD_COST */
            continue;
        }
        ...
    }
```

5. 如下代码段, 属性的上报可以通过`linkkit_set_value()`, `linkkit_post_property()`这两个函数的组合来完成, 云端返回的应答可在`post_property_cb()`中得到处理
---
```
    linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing, "WIFI_Band", band, NULL);
    linkkit_post_property(sample_ctx->thing,"WIFI_Band",post_property_cb);

    linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing, "WIFI_Channel", &channel, NULL);
    linkkit_post_property(sample_ctx->thing,"WIFI_Channel",post_property_cb);

    linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing, "WiFI_RSSI", &rssi, NULL);
    linkkit_post_property(sample_ctx->thing,"WiFI_RSSI",post_property_cb);

    linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing, "WiFI_SNR", &snr, NULL);
    linkkit_post_property(sample_ctx->thing,"WiFI_SNR",post_property_cb);
```

6. 事件的上报可以通过`linkkit_set_value()`, `linkkit_trigger_event()`这两个函数的组合来完成, 云端返回的应答可在`post_property_cb()`中得到处理
---
```
int trigger_event(sample_context_t *sample)
{
    char event_output_identifier[64];
    snprintf(event_output_identifier, sizeof(event_output_identifier), "%s.%s", EVENT_ERROR_IDENTIFIER, EVENT_ERROR_OUTPUT_INFO_IDENTIFIER);

    int errorCode = 0;
    linkkit_set_value(linkkit_method_set_event_output_value,
                      sample->thing,
                      event_output_identifier,
                      &errorCode, NULL);

    return linkkit_trigger_event(sample->thing, EVENT_ERROR_IDENTIFIER, post_property_cb);
}
```

7. 设备可以通过`linkkit_ntp_time_request`从云端获取NTP时间, 该操作是一个异步操作, 用户通过该API告知Linkkit从云端获取NTP时间, 该API就会返回了. 等到Linkkit成功从云端获取NTP时间后, 再通过回调函数告知用户. 另外, 该API只有在设备和云端成功连接连接后才可以使用.
---
```
int linkkit_ntp_time_request(void (*ntp_reply)(const char *ntp_offset_time_ms));

void ntp_reply(const char *ntp_offset_time_ms)
{
    printf("ntp offset time:%s\n", ntp_offset_time_ms);  //相对于1900年1月1日 00:00:00的偏移时间, 以ms为单位
}
```
