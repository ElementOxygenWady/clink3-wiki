# <a name="目录">目录</a>
+ [文档简介](#文档简介)
+ [准备开发环境](#准备开发环境)
+ [改造连接阿里云](#改造连接阿里云)
    * [下载示例](#下载示例)
    * [运行示例](#运行示例)
    * [核心源码](#核心源码)
        - [计算登录密码](#计算登录密码)
        - [连接阿里云IoT](#连接阿里云IoT)
        - [发布数据](#发布数据)
        - [订阅主题](#订阅主题)
+ [确认接入成功](#确认接入成功)

# <a name="文档简介">文档简介</a>

本文将介绍如何使用`Paho Android Service`连上阿里云物联网平台并进行简单的数据收发.

+ `Paho android service`是一个基于`paho java mqtt client`库开发的Mqtt客户端服务包
+ 它将MQTT连接封装到一个运行在后台android服务中, 并维护着MQTT心跳, 网络连接等

# <a name="准备开发环境">准备开发环境</a>

+ 本演示使用的`Android Studio`版本为`3.5.1`, `gradle`版本为`3.5.1`.
+ 用户可访问[Android Studio官网](https://developer.android.com/studio)下载最新的`Android Studio`, 安装完成后安装`Android SDK`
+ Android开发的相关教程可以查看官方文档


具体过程
---
+ 首先创建一个新的Android工程, 然后在gradle文件中添加`Paho Android Client`依赖, 我们使用1.1.1版本的PahoAndroidClient进行演示

```xml
// 在工程build.gradle中添加paho仓库地址, 我们使用release仓库
repositories {
    maven {
    	url "https://repo.eclipse.org/content/repositories/paho-releases/"
    }
}

// 在应用build.gradle中添加paho android service, 我们使用1.1.1release版本的paho服务, 底层基于paho.client.mqttv3-1.1.0版本
dependencies {
    implementation 'org.eclipse.paho:org.eclipse.paho.client.mqttv3:1.1.0'
    implementation 'org.eclipse.paho:org.eclipse.paho.android.service:1.1.1'
}
```

+ 为了使App能够创建绑定到`paho android service`, 需要在`AndroidManifest.xml`中添加声明以下服务

```html
<!-- Mqtt Service -->
<service android:name="org.eclipse.paho.android.service.MqttService">
</service>
```

+ 还需要在`AndroidManifest.xml`中添加paho mqtt service所需的权限

```html
<uses-permission android:name="android.permission.WAKE_LOCK" />
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.READ_PHONE_STATE" />
```

# <a name="改造连接阿里云">改造连接阿里云</a>

## <a name="下载示例">下载示例</a>

[下载链接](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/aiot-android-demo.zip)


## <a name="运行示例">运行示例</a>

用于应修改以下三元组定义, 使用自己申请的设备三元组替换
```
/* 设备三元组信息 */
final private String PRODUCTKEY = "a11xsrWmW14";
final private String DEVICENAME = "paho_android";
final private String DEVICESECRET = "tLMT9QWD36U2SArglGqcHCDK9rK9nOrA";
```

运行demo app, 可以在Logcat中看到以下日志:
```
2019-12-04 19:44:01.824 5952-5987/com.linkkit.aiot_android_demo W/OpenGLRenderer: Failed to choose config with EGL_SWAP_BEHAVIOR_PRESERVED, retrying without...
2019-12-04 19:44:01.829 5952-5987/com.linkkit.aiot_android_demo D/EGL_emulation: eglCreateContext: 0xec073240: maj 3 min 0 rcv 3
2019-12-04 19:44:01.830 5952-5987/com.linkkit.aiot_android_demo D/EGL_emulation: eglMakeCurrent: 0xec073240: ver 3 0 (tinfo 0xec09b470)
2019-12-04 19:44:01.852 5952-5987/com.linkkit.aiot_android_demo W/Gralloc3: mapper 3.x is not supported
2019-12-04 19:44:01.854 5952-5987/com.linkkit.aiot_android_demo D/HostConnection: createUnique: call
...
...
2019-12-04 19:44:01.860 5952-5987/com.linkkit.aiot_android_demo D/eglCodecCommon: allocate: Ask for block of size 0x1000
2019-12-04 19:44:01.861 5952-5987/com.linkkit.aiot_android_demo D/eglCodecCommon: allocate: ioctl allocate returned offset 0x3ff706000 size 0x2000
2019-12-04 19:44:01.897 5952-5987/com.linkkit.aiot_android_demo D/EGL_emulation: eglMakeCurrent: 0xec073240: ver 3 0 (tinfo 0xec09b470)
2019-12-04 19:44:02.245 5952-6023/com.linkkit.aiot_android_demo D/AlarmPingSender: Register alarmreceiver to MqttServiceMqttService.pingSender.a11xsrWmW14.paho_android|timestamp=1575459841629,_v=sdk-android-1.0.0,securemode=2,signmethod=hmacsha256|
2019-12-04 19:44:02.256 5952-6023/com.linkkit.aiot_android_demo D/AlarmPingSender: Schedule next alarm at 1575459902256
2019-12-04 19:44:02.256 5952-6023/com.linkkit.aiot_android_demo D/AlarmPingSender: Alarm scheule using setExactAndAllowWhileIdle, next: 60000
2019-12-04 19:44:02.272 5952-5952/com.linkkit.aiot_android_demo I/AiotMqtt: connect succeed
2019-12-04 19:44:02.301 5952-5952/com.linkkit.aiot_android_demo I/AiotMqtt: subscribed succeed
```

## <a name="核心源码">核心源码</a>
> 我们提供`AiotMqttOption`这个class
>
> 用于生成MQTT Client连接阿里云物联网平台时所需的认证信息(包括了`clientId`, `username`和`password`)

[AiotMqttOption.java](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/AiotMqttOption.java)


### <a name="计算登录密码">计算登录密码</a>

使用以下代码段获取clientId, username和password, 并将username和password设置到MqttConnectOptions对象中

```java
/* 获取Mqtt建连信息clientId, username, password */
AiotMqttOption aiotMqttOption = new AiotMqttOption().getMqttOption(PRODUCTKEY, DEVICENAME, DEVICESECRET);
if (aiotMqttOption == null) {
    Log.e(TAG, "device info error");
} else {
    clientId = aiotMqttOption.getClientId();
    userName = aiotMqttOption.getUsername();
    passWord = aiotMqttOption.getPassword();
}

/* 创建MqttConnectOptions对象并配置username和password */
MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
mqttConnectOptions.setUserName(userName);
mqttConnectOptions.setPassword(passWord.toCharArray());
```

### <a name="连接阿里云IoT">连接阿里云IoT</a>

使用`paho android service`, 我们并不需要直接去绑定`MqttService`服务

因为Paho使用了`MqttAndroidClient`类对`MattService`进行了封装, 并提供了一套完整的异步接口

以下代码段创建了一个`MqttAndroidClient`对象, 设置回调接口, 然后使用`mqttConnectOptions`参数调用`connect`方法建立连接
```java
/* 创建MqttAndroidClient对象, 并设置回调接口 */
mqttAndroidClient = new MqttAndroidClient(getApplicationContext(), host, clientId);
mqttAndroidClient.setCallback(new MqttCallback() {
    @Override
    public void connectionLost(Throwable cause) {
        Log.i(TAG, "connection lost");
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        Log.i(TAG, "topic: " + topic + ", msg: " + new String(message.getPayload()));
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        Log.i(TAG, "msg delivered");
    }
});

/* Mqtt建连 */
try {
    mqttAndroidClient.connect(mqttConnectOptions, null, new IMqttActionListener() {
        @Override
        public void onSuccess(IMqttToken asyncActionToken) {
            Log.i(TAG, "connect succeed");

            subscribeTopic(SUB_TOPIC);
        }

        @Override
        public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
            Log.i(TAG, "connect failed");
        }
    });

} catch (MqttException e) {
    e.printStackTrace();
}
```

### <a name="发布数据">发布数据</a>

我们对`publish`方法进行简单的封装用于向/${prodcutKey}/${deviceName}/user/update这个topic发布指定payload的消息
```java
/**
 * 向默认的主题/user/update发布消息
 * @param payload 消息载荷
 */
public void publishMessage(String payload) {
    try {
        if (mqttAndroidClient.isConnected() == false) {
            mqttAndroidClient.connect();
        }

        MqttMessage message = new MqttMessage();
        message.setPayload(payload.getBytes());
        message.setQos(0);
        mqttAndroidClient.publish(PUB_TOPIC, message,null, new IMqttActionListener() {
            @Override
            public void onSuccess(IMqttToken asyncActionToken) {
                Log.i(TAG, "publish succeed!");
            }

            @Override
            public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                Log.i(TAG, "publish failed!");
            }
        });
    } catch (MqttException e) {
        Log.e(TAG, e.toString());
        e.printStackTrace();
    }
}
```

### <a name="订阅主题">订阅主题</a>

我们对`subscribe`方法进行简单的封装用于实现对指定topic的订阅
```java
/**
 * 订阅特定的主题
 * @param topic mqtt主题
 */
public void subscribeTopic(String topic) {
    try {
        mqttAndroidClient.subscribe(topic, 0, null, new IMqttActionListener() {
            @Override
            public void onSuccess(IMqttToken asyncActionToken) {
                Log.i(TAG, "subscribed succeed");
            }

            @Override
            public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                Log.i(TAG, "subscribed failed");
            }
        });

    } catch (MqttException e) {
        e.printStackTrace();
    }
}
```

更多`paho.android.service`包的接口说明请查看官方文档: https://www.eclipse.org/paho/files/android-javadoc/index.html

# <a name="确认接入成功">确认接入成功</a>

以下日志显示连接成功
---
```
2019-12-04 19:44:02.272 5952-5952/com.linkkit.aiot_android_demo I/AiotMqtt: connect succeed
```

以下日志显示数据上报成功
---
```
2019-12-04 20:24:51.084 5952-5952/com.linkkit.aiot_android_demo I/AiotMqtt: publish succeed!
2019-12-04 20:24:51.084 5952-5952/com.linkkit.aiot_android_demo I/AiotMqtt: msg delivered
```

以下日志显示订阅成功
---
```
2019-12-04 19:44:02.301 5952-5952/com.linkkit.aiot_android_demo I/AiotMqtt: subscribed succeed
```

以下日志显示60秒间隔的心跳包
---
```
2019-12-04 19:58:02.349 5952-5952/com.linkkit.aiot_android_demo D/AlarmPingSender: Sending Ping at:1575460682349
2019-12-04 19:58:02.350 5952-5952/com.linkkit.aiot_android_demo D/AlarmPingSender: Schedule next alarm at 1575460742350
2019-12-04 19:58:02.350 5952-5952/com.linkkit.aiot_android_demo D/AlarmPingSender: Alarm scheule using setExactAndAllowWhileIdle, next: 60000
2019-12-04 19:58:02.368 5952-6025/com.linkkit.aiot_android_demo D/AlarmPingSender: Success. Release lock(MqttService.client.a11xsrWmW14.paho_android|timestamp=1575459841629,_v=sdk-android-1.0.0,securemode=2,signmethod=hmacsha256|):1575460682368
```

