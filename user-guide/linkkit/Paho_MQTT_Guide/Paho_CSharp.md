# <a name="目录">目录</a>
+ [文档简介](#文档简介)
+ [准备开发环境](#准备开发环境)
+ [下载Paho客户端](#下载Paho客户端)
+ [改造连接阿里云](#改造连接阿里云)
    * [下载示例](#下载示例)
    * [运行示例](#运行示例)
    * [核心源码](#核心源码)
        - [计算登录密码](#计算登录密码)
        - [连接阿里云IoT](#连接阿里云IoT)
        - [发布数据](#发布数据)
        - [订阅主题](#订阅主题)
    * [详细开发过程](#详细开发过程)
+ [确认接入成功](#确认接入成功)

# <a name="文档简介">文档简介</a>
本文档演示如何使用Paho MQTT for C#来接入阿里云物联网平台.

+ Paho提供的MQTT C#开源代码中已包含`Visual Studio`解决方案工程, 工程中的每个项目针对不同的.NET平台可生成对应的类库
+ 本文在这个解决方案工程中新建一个控制台应用项目, 调用Paho的MQTT类库连接阿里云物联网平台

# <a name="准备开发环境">准备开发环境</a>

+ 操作系统: `Win10`
+ 集成开发环境: [Visual Studio 2019社区版](https://visualstudio.microsoft.com/zh-hans/downloads/)

打开下载好的`Visual Studio Installer`, 选择`.NET桌面开发`, 开始安装

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-install-1.png" width="1200">

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-install-2.png" width="1200">

安装完毕后开发环境就准备就绪了

# <a name="下载Paho客户端">下载Paho客户端</a>

+ 在github上下载Paho MQTT for C#, [点此下载](https://github.com/eclipse/paho.mqtt.m2mqtt/archive/master.zip)

*本文编写时, 使用了master分支, commit id为`b2e64bc4485721a0bd5ae805d9f4917e8d040e81`*

# <a name="改造连接阿里云">改造连接阿里云</a>

## <a name="下载示例">下载示例</a>

[点此下载](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/aiot-csharp-demo.zip)

## <a name="运行示例">运行示例</a>

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-run.png" width="1200">

## <a name="核心源码">核心源码</a>
[MqttSign.cs](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/MqttSign.cs)

原型说明
---
class MqttSign

功能描述
---
MqttSign类用于计算通过MQTT连接阿里云IoT物联网平台时需要的mqtt协议中username/password和clientid

调用者可以通过这些参数使用paho的mqtt接口连接到阿里云IoT物联网平台

成员
---

| **类型定义**    | **方法描述**
|-----------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
| public void     | calculate(String productKey, String deviceName, String deviceSecret) <br><br> 通过输入设备的product key/device name/device secret来计算MQTT连接参数username/password和clientid
| public String   | getUsername() <br><br> 获取mqtt建联参数username
| public String   | getPassword() <br><br> 获取mqtt建联参数password
| public String   | getClientid() <br><br> 获取mqtt建联参数clientid

### <a name="计算登录密码">计算登录密码</a>
调用以上文件中的`MqttSign`, 计算连接阿里云的密码

```csharp
String productKey = "a1X2bEnP82z";
String deviceName = "example1";
String deviceSecret = "ga7XA6KdlEeiPXQPpRbAjOZXwG8ydgSe";

// 计算Mqtt建联参数
MqttSign sign = new MqttSign();
sign.calculate(productKey, deviceName, deviceSecret);

Console.WriteLine("username: " + sign.getUsername());
Console.WriteLine("password: " + sign.getPassword());
Console.WriteLine("clientid: " + sign.getClientid());
```

在调用`MqttSign`计算密码时, 将下图红框中的部分用自己的三元组进行替换

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-triple.png" width="1200">

### <a name="连接阿里云IoT">连接阿里云IoT</a>

密码计算完成后, 调用Paho Mqtt客户端连接阿里云IoT

```csharp
...
// 使用Paho链接阿里云物联网平台
int port = 443;
String broker = productKey + ".iot-as-mqtt.cn-shanghai.aliyuncs.com";

MqttClient mqttClient = new MqttClient(broker, port, true, MqttSslProtocols.TLSv1_2, null, null);
mqttClient.Connect(sign.getClientid(), sign.getUsername(), sign.getPassword());

Console.WriteLine("Broker: " + broker + " Connected");
...
```

### <a name="发布数据">发布数据</a>

上报物模型的`LightSwitch`属性(需要在阿里云IoT控制台进行物模型功能定义)

```csharp
...
// Paho Mqtt 消息发布
String topic = "/sys/" + productKey + "/" + deviceName + "/thing/event/property/post";
String message = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";
mqttClient.Publish(topic, Encoding.UTF8.GetBytes(message));
...
```

### <a name="订阅主题">订阅主题</a>

物模型属性上报成功后, 阿里云IoT会返回一条应答消息, 订阅应答的topic即可收到此消息

```csharp
...
// Paho Mqtt 消息订阅
String topicReply = "/sys/" + productKey + "/" + deviceName + "/thing/event/property/post_reply";

mqttClient.MqttMsgPublishReceived += MqttPostProperty_MqttMsgPublishReceived;
mqttClient.Subscribe(new string[] { topicReply }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_MOST_ONCE });
...
private static void MqttPostProperty_MqttMsgPublishReceived(object sender, uPLibrary.Networking.M2Mqtt.Messages.MqttMsgPublishEventArgs e)
{
    Console.WriteLine("reply topic  :" + e.Topic);
    Console.WriteLine("reply payload:" + e.Message.ToString());
}
...
```

## <a name="详细开发过程">详细开发过程</a>

打开 `Visual Studio 2019` 社区版, 选择打开项目或解决方案

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-1.png" width="1200">

Paho提供的源代码中已包含Visual Studio工程, 打开Paho源代码中的Visual Studio解决方案文件`M2MMqtt.sln`

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-2.png" width="1200">

此时会提示解决方案中有一些项目文件不兼容, 忽略即可. 然后按照提示安装.NET 可移植库目标包

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-3.png" width="1200">

该工程中所有项目的输出目标均为类库, 此时选择`M2Mqtt.Net`项目进行生成, 此时应该可以生成Paho MQTT for C#的动态链接库文件

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-4.png" width="1200">

为了使用该类库进行演示, 在该解决方案中新建一个控制台应用. 在解决方案资源管理器中右键单击解决方案`M2Mqtt`, 选择`添加->新建项目`, 然后选择`控制台应用`

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-5.png" width="1200">

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-6.png" width="1200">

配置项目名为aiot-csharp-demo, 并创建该项目

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-7.png" width="1200">

在解决方案"M2Mqtt"->属性中, 更改启动项目及项目依赖项

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-8.png" width="1200">

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-9.png" width="1200">

在项目aiot-csharp-demo->添加->引用中, 为该项目添加引用, 选择M2Mqtt.Net

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-csharp-dev-10.png" width="1200">

至此, Visual Studio项目解决方案准备就绪, 在aiot-csharp-demo中引入`MqttSign.cs`即可进行登录密码的计算, 在`Program.cs`中即可进行密码计算并调用Paho Mqtt类库连接阿里云IoT

# <a name="确认接入成功">确认接入成功</a>

以下日志说明连云成功
---
```
...
broker: a1X2bEnP82z.iot-as-mqtt.cn-shanghai.aliyuncs.com Connected
...
```

以下日志说明连数据上报成功
---
```
...
publish: {"id":"1","version":"1.0","params":{"LightSwitch":0}}
...
```

以下日志说明订阅成功
---
```
...
subscribe: /sys/a1X2bEnP82z/example1/thing/event/property/post_reply
...
```
