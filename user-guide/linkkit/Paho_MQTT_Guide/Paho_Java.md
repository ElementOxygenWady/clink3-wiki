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
本文档演示如何使用Paho MQTT for Java来接入阿里云物联网平台

# <a name="准备开发环境">准备开发环境</a>

+ 操作系统: `macOS`
+ JDK版本: [下载JDK8](https://www.oracle.com/technetwork/pt/java/javase/downloads/jdk8-downloads-2133151.html)
+ 集成开发环境: [下载IntelliJ IDEA社区版](https://www.jetbrains.com/idea/)

按照JDK和IDEA的官方文档进行安装, 安装完毕后开发环境就准备就绪了

# <a name="下载Paho客户端">下载Paho客户端</a>

+ 在Maven工程中添加Paho MQTT for Java依赖

```xml
<dependencies>
  <dependency>
      <groupId>org.eclipse.paho</groupId>
      <artifactId>org.eclipse.paho.client.mqttv3</artifactId>
      <version>1.2.0</version>
    </dependency>
</dependencies>
```

# <a name="改造连接阿里云">改造连接阿里云</a>

## <a name="下载示例">下载示例</a>

[点此下载](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/aiot-java-demo.zip)

## <a name="运行示例">运行示例</a>

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-java-run.png" width="1000">

## <a name="核心源码">核心源码</a>
### <a name="计算登录密码">计算登录密码</a>

`MqttSign.java`源码

```java
package com.aliyun.iot;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import java.math.BigInteger;

class CryptoUtil {
    private static String hmac(String plainText, String key, String algorithm, String format) throws Exception {
        if (plainText == null || key == null) {
            return null;
        }

        byte[] hmacResult = null;

        Mac mac = Mac.getInstance(algorithm);
        SecretKeySpec secretKeySpec = new SecretKeySpec(key.getBytes(), algorithm);
        mac.init(secretKeySpec);
        hmacResult = mac.doFinal(plainText.getBytes());
        return String.format(format, new BigInteger(1, hmacResult));
    }

//    public static String hmacMd5(String plainText, String key) throws Exception {
//        return hmac(plainText,key,"HmacMD5","%032x");
//    }
//
//    public static String hmacSha1(String plainText, String key) throws Exception {
//        return hmac(plainText,key,"HmacSHA1","%040x");
//    }

    public static String hmacSha256(String plainText, String key) throws Exception {
        return hmac(plainText,key,"HmacSHA256","%064x");
    }
}

public class MqttSign {
    private String username = "";
    private String password = "";
    private String clientid = "";
    public String getUsername() { return this.username;}
    public String getPassword() { return this.password;}
    public String getClientid() { return this.clientid;}

    public void calculate(String productKey, String deviceName, String deviceSecret) {
        if (productKey == null || deviceName == null || deviceSecret == null) {
            return;
        }

        try {
            // MQTT用户名
            this.username = deviceName + "&" + productKey;

            // MQTT密码
            String timestamp = Long.toString(System.currentTimeMillis());
            String plainPasswd = "clientId" + productKey + "." + deviceName + "deviceName" +
                    deviceName + "productKey" + productKey + "timestamp" + timestamp;
            this.password = CryptoUtil.hmacSha256(plainPasswd, deviceSecret);

            // MQTT ClientId
            this.clientid = productKey + "." + deviceName + "|" + "timestamp=" + timestamp +
                    ",_v=sdk-java-1.0.0,securemode=2,signmethod=hmacsha256|";
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
```

调用`MqttSign`计算密码

```java
String productKey = "a1X2bEnP82z";
String deviceName = "example1";
String deviceSecret = "ga7XA6KdlEeiPXQPpRbAjOZXwG8ydgSe";

// 计算Mqtt建联参数
MqttSign sign = new MqttSign();
sign.calculate(productKey, deviceName, deviceSecret);

System.out.println("username: " + sign.getUsername());
System.out.println("password: " + sign.getPassword());
System.out.println("clientid: " + sign.getClientid());
```

在调用`MqttSign`计算密码时, 将下图红框中的部分用自己的三元组进行替换

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-java-triple.png" width="1000">

### <a name="连接阿里云IoT">连接阿里云IoT</a>

密码计算完成后, 调用Paho Mqtt客户端连接阿里云IoT

```java
...
// Paho Mqtt 客户端
MqttClient sampleClient = new MqttClient(broker, sign.getClientid(), persistence);

// Paho Mqtt 连接参数
MqttConnectOptions connOpts = new MqttConnectOptions();
connOpts.setCleanSession(true);
connOpts.setKeepAliveInterval(180);
connOpts.setUserName(sign.getUsername());
connOpts.setPassword(sign.getPassword().toCharArray());
sampleClient.connect(connOpts);
System.out.println("Broker: " + broker + " Connected");
...
```

### <a name="发布数据">发布数据</a>

上报物模型的`LightSwitch`属性(需要在阿里云IoT控制台进行物模型功能定义)

```java
...
// Paho Mqtt 消息发布
String topic = "/sys/" + productKey + "/" + deviceName + "/thing/event/property/post";
String content = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":1}}";
MqttMessage message = new MqttMessage(content.getBytes());
message.setQos(0);
sampleClient.publish(topic, message);
...
```

### <a name="订阅主题">订阅主题</a>

物模型属性上报成功后, 阿里云IoT会返回一条应答消息, 订阅应答的topic即可收到此消息

```java
class MqttPostPropertyMessageListener implements IMqttMessageListener {
    @Override
    public void messageArrived(String var1, MqttMessage var2) throws Exception {
        System.out.println("reply topic  : " + var1);
        System.out.println("reply payload: " + var2.toString());
    }
}
...
// Paho Mqtt 消息订阅
String topicReply = "/sys/" + productKey + "/" + deviceName + "/thing/event/property/post_reply";
sampleClient.subscribe(topicReply, new MqttPostPropertyMessageListener());
...
```

## <a name="详细开发过程">详细开发过程</a>

安装 `JDK8` 和 `IntelliJ IDEA` 社区版

使用IDEA导入下载好的示例工程`aiot-java-demo`, 并进行配置

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-java-conf.png" width="1000">

# <a name="确认接入成功">确认接入成功</a>

以下日志说明连云成功
---
```
...
broker: ssl://a1X2bEnP82z.iot-as-mqtt.cn-shanghai.aliyuncs.com:443 Connected
...
```

以下日志说明数据上报成功
---
```
...
publish: {"id":"1","version":"1.0","params":{"LightSwitch":1}}
...
```

以下日志说明订阅成功
---
```
...
subscribe: /sys/a1X2bEnP82z/example1/thing/event/property/post_reply
...
reply topic  : /sys/a1X2bEnP82z/example1/thing/event/property/post_reply
reply payload: {"code":200,"data":{},"id":"1","message":"success","method":"thing.event.property.post","version":"1.0"}
...
```

---
