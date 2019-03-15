最新SDK长期分支:
---
git clone git@github.com:aliyun/iotkit-embedded.git -b v3.0.1

最新SDK发布声明:
---
+ 发布日期: 2019/03/15
+ 下载链接: [v3.0.1.zip](https://github.com/aliyun/iotkit-embedded/archive/v3.0.1.zip)
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

SDK手册目录
---

+ [SDK概述](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/SDK_Overview)
+ [快速体验](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Quick_Start)
+ **移植指南**
    * [移植概述](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Porting_Overview)
    * [不使用SDK自带编译系统时的移植示例](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Extract_Example)
    * [基于Make的编译系统说明](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Make_Usage)
    * [使用SDK自带编译系统时的移植示例](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Example)
    * **不直接网络通信的MCU上集成SDK示例**
        - [MCU+支持MQTT的通信模组](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/MCU_MQTT_Example)
        - [MCU+支持TCP的通信模组](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/MCU_TCP_Example)
    * **通信模组上集成SDK示例**
        - [乐鑫ESP8266+AliOS](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_ESP8266)
        - [庆科MK3080+AliOS](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_MK3080)
        - [联发科MTK2503/MTK6261+Nucleus](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_MTK2503)
    * **高级系统集成SDK示例**
        - [目标系统为64位Linux](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Linux64)
        - [目标系统为32位Linux](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Linux32)
        - [目标系统为arm-linux](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_ArmLinux)
        - [目标系统为Windows](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Windows)

+ [编程指南](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/ProgGuide_Home)
    * [设备签名](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Dev_Sign)
    * [一型一密](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Dynamic_Register)
    * [OTA开发](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/OTA_Prog)
    * [MQTT自定义TOPIC通信](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/MQTT_Connect)
    * [异步通知式底层通信模型](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/AsyncMQTT_Prog)
    * **设备多协议连接**
        - [CoAP](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/CoAP_Connect)
        - [HTTP/S](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HTTP_Connect)
    * [物模型编程](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/DeviceModel_Prog)
    * **数据通道编程**
        - [HTTP2文件上传](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/H2_FileUpload)
        - [HTTP2流式传输](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/H2_Stream)
    * [标签](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/DeviceTag_Prog)
    * [子设备管理](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Gateway_Prog)
    * [远程配置](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Cota_Prog)
    * [WiFi配网概述](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/WiFi_Provision)
        - [一键配网](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Awss_Smartconfig)
        - [设备热点配网](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Awss_DevAP)
        - [手机热点配网](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Awss_PhoneAP)
        - [零配](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Awss_ZeroConfig)

    * [设备重置](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/DevReset_Prog)
    * **用户编程接口**
        - [Alink协议相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides)
        - [基础API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Basic_Provides)
        - [MQTT相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides)
        - [CoAP上云相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides)
        - [HTTP上云相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/HTTP_Provides)
        - [设备影子相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Shadow_Provides)
        - [OTA固件下载相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides)
        - [HTTP2流式传输相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/HTTP2_Provides)
        - [配网绑定相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides)
    * [HAL接口清单](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Required_APIs)
        - [基础功能相关HAL](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires)
        - [多线程相关HAL](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires)
        - [MQTT上云相关HAL](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MQTT_Requires)
        - [CoAP上云相关HAL](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires)
        - [OTA固件升级相关HAL](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/OTA_Requires)
        - [HTTP2流式传输相关HAL](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires)
        - [WiFi配网相关HAL](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires)
        - [本地通信相关HAL](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Alcs_Requires)

+ [附录A 用户感知变更说明](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/User_Interface_Changes)
+ [附录B 常见问题](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Typical_Questions)
+ [附录C 云端接口说明](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Cloud_Interface)
+ **历史文档**
    * [V2.3.0版本的历史文档](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Archived/V230_Snapshot_Index)
    * [旧版的物模型API快速开始](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Archived/Deprecated_QuickStart)
    * [TSL静态集成和动态拉取](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Archived/TSL_Operation)
    * [旧版的物模型API编程指南](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Archived/Deprecated_ProgGuide)
+ [最佳实践](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/Practice_Home)
    * [厂商使用自有MQTT连接阿里云IoT平台](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/PrivMQTT_Access)
    * [NBIoT设备连接飞燕平台](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/NBIoT_Access)
    * [使用I-cube-Aliyun连接阿里云IoT平台](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Vendors/Icube)
