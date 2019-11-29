# <a name="目录">目录</a>
+ [基础背景](#基础背景)
+ [准备工作](#准备工作)
    * [第一步 注册或登录阿里云网站](#第一步 注册或登录阿里云网站)
    * [第二步 开通阿里云IoT服务](#第二步 开通阿里云IoT服务)
    * [第三步 从控制台获取设备三元组](#第三步 从控制台获取设备三元组)
+ [选择适合你的编程语言](#选择适合你的编程语言)
+ [控制台确认设备上线成功](#控制台确认设备上线成功)

> 阿里云IoT平台基于标准的MQTT协议搭建, 因此它支持用户直接使用第三方的开源MQTT客户端接入平台, 而不一定要使用阿里提供的官方SDK

本文介绍如何基于各种编程语言的第三方MQTT实现, 接入阿里云IoT

# <a name="基础背景">基础背景</a>
**Eclipse Paho**项目是开源社区提供的MQTT协议客户端实现, 涵盖了多种编程语言和功能特性, [点此进入它的最新下载页面](https://www.eclipse.org/paho/downloads.php)

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/paho_clients_comp.png" width="800">

阿里云IoT平台和其它的MQTT服务端一样, 对请求接入的设备, 基于以下3个字符串进行身份认证. 只有通过了服务端身份认证的设备, 才能进行后续的数据收发.

+ `ClientId`: 这是客户端的自定义描述, 用户可在其中表达设备的生产批次, 部署位置, 功能范围等任意信息
+ `Username`: 这是客户端登录MQTT服务器的用户名
+ `Password`: 这是客户端登录MQTT服务器的密码, 也是服务端验证客户端是否合法接入的最终判断凭据

---
与其它云平台不同的是, 阿里云IoT平台对设备颁发以下的3个字符串进行身份管理, 这也叫**设备的三元组**

+ `productKey`: 这是设备所属的品类标识符, 品类就是设备的类型, 例如: 灯/打印机/自行车, 等等
+ `deviceName`: 这是设备不同个体的标识符, 一般是产品序列号(Serial Number)
+ `deviceSecret`: 这是设备和阿里云IoT平台之间的共享密钥, 用于各种身份认证, 报文加密的安全场景

---
**使用第三方MQTT实现接入阿里云IoT, 就是把阿里定义的三元组, 转换成MQTT协议定义的用户名和密码的过程**

# <a name="准备工作">准备工作</a>
MQTT客户端能够连接到阿里云IoT平台的前提, 是用户在平台侧开通服务, 并获取阿里为它颁发的三元组

## <a name="第一步 注册或登录阿里云网站">第一步 注册或登录阿里云网站</a>
使用淘宝账号或者阿里云账号等[登录阿里云](https://account.aliyun.com), 尚无阿里云账号的用户, 可以在[注册页面](https://account.aliyun.com/register/register.htm)以手机号注册得到账号

## <a name="第二步 开通阿里云IoT服务">第二步 开通阿里云IoT服务</a>
登录[阿里云IoT服务页面](https://www.aliyun.com/product/iot), 点击开通按钮, 免费开通服务

## <a name="第三步 从控制台获取设备三元组">第三步 从控制台获取设备三元组</a>
+ 服务成功开通后, 登录[控制台页面](https://iot.console.aliyun.com), 在左侧导航栏的"设备管理"->"产品"标签下, 创建产品(品类)
+ 创建产品成功后, 在左侧导航栏的"设备管理"标签下, 点击"设备", 然后到右侧页面, 点击"添加设备", 创建完整的三元组
+ 在以下页面部分中, 就能看到设备的三元组字符串了

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/device_summary.png" width="800">

# <a name="选择适合你的编程语言">选择适合你的编程语言</a>
以下子章节中, 分不同的编程语言, 详细演示了如何接入阿里云IoT平台

+ [Paho-MQTT Go](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/Paho_Go)
+ [Paho-MQTT C#](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/Paho_CSharp)
+ [Paho-MQTT C](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/Paho_C)
+ [Paho-MQTT Java](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/Paho_Java)
+ [Paho-MQTT Android](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/Paho_Android)

# <a name="控制台确认设备上线成功">控制台确认设备上线成功</a>
除了客户端的运行日志, 从阿里云IoT的控制台, 以下页面部分也可以看到设备上下线和上报数据的状态

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/console_online_log.png" width="800">
