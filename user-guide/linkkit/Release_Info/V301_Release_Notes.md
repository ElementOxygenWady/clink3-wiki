# <a name="目录">目录</a>


C-SDK的`3.0.1`版本长期分支
---

    git clone git@code.aliyun.com:linkkit/c-sdk.git -b v3.0.1

`3.0.1`版本发布声明:
---
+ 发布日期: 2019/03/15
+ 下载链接: [v3.0.1.zip](https://code.aliyun.com/linkkit/c-sdk/repository/archive.zip?ref=v3.0.1)
+ 更新内容:
    - **新增对异步/通知式的底层TCP/IP协议栈的支持:** 专用于联发科 `MTK2503/MTK6261` 的 Nucleus 系统对接
        + 新增 `FEATURE_ASYNC_PROTOCOL_STACK` 开关支持异步协议栈, 详情介绍请看文档: [异步通知式底层通信模型](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/AsyncMQTT_Prog)
        + 在异步协议栈支持的基础上, 支持与 `MTK2503/MTK6261` 系统对接, 详情介绍请看文档: [移植到联发科MTK2503/MTK6261](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_MTK2503)
    - **新增以源文件方式移植对接:** 用户可以使用任何自己熟悉或者喜欢的方式编译SDK
        + 提供"配置工具", 用于配置选用SDK的哪些功能点
        + 提供"抽取工具", 用于根据功能点抽取需要的源文件
        + 源文件被抽取后, 在 `output` 目录下, 用户可以用任何方式编译它们
        + 详情介绍请看文档: [不使用SDK自带编译系统时的移植示例](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Extract_Example)
    - **新增用户需要实现的HAL接口自动整理:** 用户不再会看到自己不需要实现的HAL接口
        + 上述"抽取工具", 会根据所选功能点自动整理待填充的源文件 `output/eng/wrappers/wrapper.c`
        + 用户需要做的只是把这个文件中留空的函数全部按注释实现即可, 不需要查阅文档, 也不会做多余的实现
        + SDK整体性质的 `include/iot_import.h` 也去掉了, 以避免用户看到他其实不必关注的HAL接口声明
    - **新增每个功能点有独立的和风格统一的API声明头文件:** 都在 `src/xxx/xxx_api.h`
        + 目录扁平化处理, 每一个 `src/xxx` 就对应了SDK的一个名字为 `xxx` 的功能点
        + SDK整体性质的 `include/iot_export.h` 也去掉了, 以避免用户看到他其实不必关注的API接口声明
    - **新增HTTP2文件上传的功能:** 在 `src/http2/http2_upload_api.h`, 基于流式传输实现
    - **新增设备签名功能:** 在 `src/dev_sign`, 可计算设备连云签名, 是SDK的最小形态(ROM: `4.5KB`, RAM: `1KB`)
    - **新增设备重置的功能:** 在 `src/dev_reset`, 可用于子设备重置网关对应关系, 或设备和用户绑定关系等
    - **新增简化版的一型一密功能:** 在 `src/dynamic_register`, 只有1个API, 可帮助用户获取`DeviceSecret`
    - **WiFi配网功能中新增了设备热点配网模式:** 在 `src/wifi_provision/dev_ap`, 通过设备开AP来得到SSID和密码
    - **优化了MQTT的建连接口:** 调用时需要用户传入的参数减少了80%
        + 仍然通过 `IOT_MQTT_Construct()` 发起云端建连, 但不再需要调用 `IOT_SetupConnInfo()`
        + 对 `IOT_MQTT_Construct()` 调用时, 结构体入参中的绝大多数都可以不填写, SDK会自动补充默认值

