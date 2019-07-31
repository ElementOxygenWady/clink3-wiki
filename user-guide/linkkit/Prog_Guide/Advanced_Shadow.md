# <a name="目录">目录</a>
+ [功能说明 ](#功能说明 )
+ [例程讲解](#例程讲解)
    * [例程讲解1: 设备端向云端查询desire值](#例程讲解1: 设备端向云端查询desire值)
    * [例程讲解2: 设备端删除云端的desire值](#例程讲解2: 设备端删除云端的desire值)
+ [设备影子的API](#设备影子的API)
+ [需要实现的HAL](#需要实现的HAL)


# <a name="功能说明 ">功能说明 </a>

低功耗设备(比如井盖等)通常休眠时间较长. 设备不在线时, 云端想要更新设备状态, 则无法下发消息到设备

为解决这个问题, 云端提供了设备属性缓存的能力. 相应地, C-SDK中的物模型中的设备影子模块, 使得设备端具有主动获取和删除云端缓存属性的能力


# <a name="例程讲解">例程讲解</a>

配置使能方式
---

    make menuconfig

在 `FEATURE_DEVICE_MODEL_ENABLED` 的目录下 `Device Model Configurations` 子目录, 选中 `FEATURE_DEVICE_MODEL_SHADOW`, 保存设置

    make clean
    make

使用方式
---

+ 在物联网平台控制台, 创建一个高级版产品, 使它具有物模型
+ 在自定义功能一栏中, 增加一个属性, 比如 `PowerSwitch`
+ 输入栏写入以下内容

```
{
    "iotId": "",
    "productKey": "a1ybTpus98a",
    "class": "com.aliyun.iotx.devicecenter.device.service.dto.ThingIdDTO",
    "deviceName": "testdevshadow1228"
},

{
    "PowerSwitch":1
}
```

点击"测试"按钮, 正常工作的话结果会返回一个`200`的code, 详情如下:

```
object      {5}
code    :   200
data        [1]
message :   success
```

## <a name="例程讲解1: 设备端向云端查询desire值">例程讲解1: 设备端向云端查询desire值</a>

注册设备影子的回调函数
---

    static int user_property_desired_get_reply_event_handler(const char *request, const int request_len)
    {
        EXAMPLE_TRACE("get desired reply Received, Request: %s", request);
        return 0;
    }
    
    IOT_RegisterCallback(ITE_PROPERTY_DESIRED_GET_REPLY, user_property_desired_get_reply_event_handler);

通过该回调, 用户注册了

    /sys/{productKey}/{deviceName}/thing/property/desired/get_reply

这个topic下行时候触发的回调函数

设备向云端获取设备影子
---

    static int user_property_desired_get_trigger()
    {
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        const char *request = "[\"PowerSwitch\"]";
        unsigned int request_len = strlen(request);
        EXAMPLE_TRACE("send request, Request: %s", request);
        IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_PROPERTY_DESIRED_GET,
                                 (unsigned char *)request, request_len);
        return 0;
    }

在连云后, 每次调用这个函数, 云端都会下行一个消息触发之前注册的 `user_property_desired_get_reply_event_handler`

## <a name="例程讲解2: 设备端删除云端的desire值">例程讲解2: 设备端删除云端的desire值</a>

    static int user_property_desired_delete_trigger()
    {
        user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        const char *request = "{\"PowerSwitch\":{}}";
        unsigned int request_len = strlen(request);
        EXAMPLE_TRACE("user desired delete send request, Request: %s", request);
        IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_PROPERTY_DESIRED_DELETE,
                                 (unsigned char *)request, request_len);
        return 0;
    }

调用这个函数后, 云端设置的设备影子的值就会被清空


# <a name="设备影子的API">设备影子的API</a>

| 函数名                                                                                                                                              | 说明
|-----------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------
| [IOT_Linkkit_Report](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides#IOT_Linkkit_Report)  | 通过 `ITM_MSG_PROPERTY_DESIRED_GET` 这个选项获取云端的desire值, 通过 `ITM_MSG_PROPERTY_DESIRED_DELETE` 选项删除云端的desire值


# <a name="需要实现的HAL">需要实现的HAL</a>

无

---

