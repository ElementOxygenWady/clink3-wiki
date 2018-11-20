# <a name="目录">目录</a>

+ [SDK概述](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/SDK_Overview)
+ [快速体验](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Quick_Start)
+ **移植指南**
    * [移植概述](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Porting_Overview)
    * [基于Make的编译系统说明](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Make_Usage)
    * [基于Make的交叉编译示例](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Example)
    * **MCU上集成SDK示例**
        - ~~在KEIL上集成SDK~~
        - ~~在IAR上集成SDK~~
    * **模组上集成SDK示例**
        - [庆科MK3060/MK3080](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_MK3060)
        - [乐鑫ESP8266](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_ESP8266)
    * **高级系统集成SDK示例**
        - [目标系统为64位Linux](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Linux64)
        - [目标系统为32位Linux](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Linux32)
        - [目标系统为arm-linux](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_ArmLinux)
        - [目标系统为Windows](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Windows)

+ [编程指南](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/ProgGuide_Home)
    * [设备认证](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Auth_Connect)
    * [MQTT自定义TOPIC通信](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/MQTT_Connect)
    * **设备多协议连接**
        - [CoAP](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/CoAP_Connect)
        - [HTTP/S](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HTTP_Connect)
        - [~~HTTP2~~](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/H2_Connect)
    * [物模型编程](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/DeviceModel_Prog)
    * **数据通道编程**
        - [~~HTTP2~~](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/H2_Stream)
    * [OTA开发](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/OTA_Prog)
    * [标签](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/DeviceTag_Prog)
    * [子设备管理](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Gateway_Prog)
    * [远程配置](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Cota_Prog)
    * [WiFi配网](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/WiFi_Provision)
    * [设备本地控制](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Alcs_Prog)
    * [设备影子](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/DeviceShadow_Prog)
    * **用户编程接口**
        - [Alink协议相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides)
        - [基础API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Basic_Provides)
        - [MQTT相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides)
        - [CoAP上云相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides)
        - [HTTP上云相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/HTTP_Provides)
        - [设备影子相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Shadow_Provides)
        - [OTA固件下载相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/OTA_Provides)
        - [HTTP2流式传输相关API](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/HTTP2_Provides)
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
+ [历史文档](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Archived/Archived_Home)
    * [旧版的快速开始](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Archived/Deprecated_QuickStart)
    * [TSL静态集成和动态拉取](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Archived/TSL_Operation)
    * [旧版的物模型API编程指南](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Archived/Deprecated_ProgGuide)
+ [最佳实践](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/Practice_Home)
    * [厂商使用自有MQTT连接阿里云IoT平台](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/PrivMQTT_Access)
    * [NBIoT设备连接飞燕平台](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Best_Practice/NBIoT_Access)
