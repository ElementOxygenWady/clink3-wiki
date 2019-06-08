
> 下表列出的是C-SDK的长期维护版本相关信息, 其中
>
> + **长期维护下载链接**: 指向的是一份被**永久性维护**的代码, 所有Bug都会得到及时的修复并包含在其中, 推荐您**始终使用此链接下载SDK源码**
> + **固定快照下载链接**: 指向的则是发布后的固定快照, 无论是否有Bug, 它的内容都不会有任何变化, 提供给希望代码保持不动的用户
>
> <br>
>
> *重要补丁链接则指向的是所有重要问题修复的表格, 即使您已经移植完C-SDK, 完成了产品开发, 我们仍然建议您时常关注这个表格, 对补丁选择使用*


| **C-SDK版本**   | **发布日期**    | **重要补丁**                                                                                                            | **发布声明**                                                                                                                    | **长期维护下载链接**                                                                    | **固定快照下载链接**
|-----------------|-----------------|-------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------
| **`3.0.1`**     | `2019/03/15`    | [v3.0.1 Bug Fixes](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Release_Info/V301_BugFix_List)   | [v3.0.1 Release Notes](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Release_Info/V301_Release_Notes)     | [v3.0.1.zip](https://code.aliyun.com/linkkit/c-sdk/repository/archive.zip?ref=v3.0.1)   | [v3.0.1-snapshot.zip](https://code.aliyun.com/linkkit/c-sdk/repository/archive.zip?ref=tag-v3.0.1)
| **`2.3.0`**     | `2018/11/19`    | [v2.3.0 Bug Fixes](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Release_Info/V230_BugFix_List)   | [v2.3.0 Release Notes](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Release_Info/V230_Release_Notes)     | [v2.3.0.zip](https://code.aliyun.com/linkkit/c-sdk/repository/archive.zip?ref=v2.3.0)   | [v2.3.0-snapshot.zip](https://code.aliyun.com/linkkit/c-sdk/repository/archive.zip?ref=tag-v2.3.0)


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

    * [设备绑定](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/DevBind_Prog)
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
+ **最佳实践**
    * [移植WiFi配网中的一键配网到嵌入式Linux](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/Smartconfig_on_Linux)
    * [移植WiFi配网中的热点配网到嵌入式Linux](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/Softap_on_Linux)
    * [网关设备连接飞燕平台](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/Gateway_Access)
    * [NBIoT设备连接飞燕平台](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/NBIoT_Access)
    * [使用I-cube-Aliyun连接阿里云IoT平台](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Vendors/Icube)
