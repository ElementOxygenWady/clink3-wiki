# <a name="目录">目录</a>
+ [文档简介](#文档简介)
+ [准备开发环境](#准备开发环境)
+ [下载Paho客户端](#下载Paho客户端)
+ [改造连接阿里云](#改造连接阿里云)
    * [下载示例](#下载示例)
    * [运行示例](#运行示例)
        - [编译](#编译)
        - [运行日志](#运行日志)
    * [核心源码](#核心源码)
        - [计算登录密码](#计算登录密码)
        - [连接阿里云IoT](#连接阿里云IoT)
        - [发布数据](#发布数据)
        - [订阅主题](#订阅主题)
    * [详细开发过程](#详细开发过程)

# <a name="文档简介">文档简介</a>
Paho提供了Embedded-C的MQTT开源实现，我们将介绍如何使用Paho嵌入式C开源库连上阿里云物联网平台，并提供相关工具代码和Demo程序。

# <a name="准备开发环境">准备开发环境</a>

我们使用了Ubuntu 16.04TLS作为开发环境进行演示，用户可运行以下命令准备好开发环境
```
    sudo apt-get update
    sudo apt-get install build-essential git sed cmake
```

# <a name="下载Paho客户端">下载Paho客户端</a>

准备好环境后，执行`git clone git@github.com:eclipse/paho.mqtt.embedded-c.git`克隆官方仓库。
我们使用了master分支，commit id为`29ab2aa29c5e47794284376d7f8386cfd54c3eed`。

# <a name="改造连接阿里云">改造连接阿里云</a>

## <a name="下载示例">下载示例</a>

我们提供了用于连接阿里云物联网平台的demo，下载链接为：
http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/aiot_c_demo.zip

zip包解压后可以看到下面两个文件：
```
.
├── aiot_mqtt_sign.c
└── aiot_c_demo.c
```
其中`aiot_mqtt_sign.c`实现了生成MQTT建连参数的官方API，`aiot_c_demo.c`是用于演示连接阿里云物联网平台的demo。

我们将这2个文件放到paho工程的以下目录中
```
../paho.mqtt.embedded-c/MQTTClient-C/samples/linux
```

## <a name="运行示例">运行示例</a>

### <a name="编译">编译</a>
首先需要对工程进行编译，有2种方法可以完成编译任务

**方法1 使用CMAKE**

Paho支持`cmake`编译系统，我们需要修改`/paho.mqtt.embedded-c/MQTTClient-C/samples/linux/CMakeLists.txt`文件，使新增的文件加入编译并生成`aiot_c_demo`可执行文件，修改后的文件如下:
```
add_executable(
  stdoutsubc
  stdoutsub.c
)

add_executable(
  aiot_c_demo
  aiot_c_demo.c
  aiot_mqtt_sign.c
)

target_link_libraries(stdoutsubc paho-embed-mqtt3cc paho-embed-mqtt3c)
target_include_directories(stdoutsubc PRIVATE "../../src" "../../src/linux")
target_compile_definitions(stdoutsubc PRIVATE MQTTCLIENT_PLATFORM_HEADER=MQTTLinux.h)

target_link_libraries(aiot_c_demo paho-embed-mqtt3cc paho-embed-mqtt3c)
target_include_directories(aiot_c_demo PRIVATE "../../src" "../../src/linux")
target_compile_definitions(aiot_c_demo PRIVATE MQTTCLIENT_PLATFORM_HEADER=MQTTLinux.h)
```
修改完成后回到`/paho.mqtt.embedded-c/`目录执行以下命令即可完成编译
```
    mkdir build.paho
    cd build.paho
    cmake ..
    make
```
编译完成后在`/paho.mqtt.embedded-c/build.paho`目录下运行
`./MQTTClient-C/samples/linux/aiot_c_demo`

**方法2 使用编译脚本**

进入`/paho.mqtt.embedded-c/MQTTClient-C/samples/linux/`目录，修改`build.sh`文件为
```
cp ../../src/MQTTClient.c .
sed -e 's/""/"MQTTLinux.h"/g' ../../src/MQTTClient.h > MQTTClient.h
gcc aiot_mqtt_sign.c aiot_c_demo.c -I ../../src -I ../../src/linux -I ../../../MQTTPacket/src MQTTClient.c ../../src/linux/MQTTLinux.c ../../../MQTTPacket/src/MQTTFormat.c  ../../../MQTTPacket/src/MQTTPacket.c ../../../MQTTPacket/src/MQTTDeserializePublish.c ../../../MQTTPacket/src/MQTTConnectClient.c ../../../MQTTPacket/src/MQTTSubscribeClient.c ../../../MQTTPacket/src/MQTTSerializePublish.c -o aiot_c_demo ../../../MQTTPacket/src/MQTTConnectServer.c ../../../MQTTPacket/src/MQTTSubscribeServer.c ../../../MQTTPacket/src/MQTTUnsubscribeServer.c ../../../MQTTPacket/src/MQTTUnsubscribeClient.c -DMQTTCLIENT_PLATFORM_HEADER=MQTTLinux.h
```
修改完成后运行`./build.sh`即可完成编译，这时在本目录下就会生成`aiot_c_demo`可执行文件，运行
`./aiot_c_demo`

### <a name="运行日志">运行日志</a>
运行我们可以看到以下log
```
    clientid: paho_mqtt&a11xsrWmW14|timestamp=2524608000000,_v=sdk-c-1.0.0,securemode=3,signmethod=hmacsha256,lan=C|
    username: paho_mqtt&a11xsrWmW14
    password: 36E955DC3D9D012EF62C80657A29328B1CFAE6186C611A17DC7939FAB6378ED9
    NetworkConnect 0
    Connecting to a11xsrWmW14.iot-as-mqtt.cn-shanghai.aliyuncs.com 443
    MQTTConnect 0, Connect aliyun IoT Cloud Success!
    Subscribing to /a11xsrWmW14/paho_mqtt/user/get
    MQTTSubscribe 0
    MQTTPublish 0, msgid 1
    MQTTPublish 0, msgid 2
    MQTTPublish 0, msgid 3
    MQTTPublish 0, msgid 4
    MQTTPublish 0, msgid 5
    ...
```

## <a name="核心源码">核心源码</a>
### <a name="计算登录密码">计算登录密码</a>

以下代码使用`aiotMqttSign()`接口生成了MQTT的5个关键的建连参数`clientid`，`username`，`password`。由于`aiotMqttSign()`是在`aiot_mqtt_sign.c`文件中定义，需要先用`extern`声明此外部函数。函数的输入参数为设备三元组`productKey`，`deviceName`，`deviceSecret`；输出参数为`clientId`，`username`，`password`，用户必需按例程所示将这3个参数定义为指定长度的数组。
```
    extern int aiotMqttSign(const char *productKey, const char *deviceName, const char *deviceSecret, 
                            char clientId[150], char username[65], char password[65]);

	/* invoke aiotMqttSign to generate mqtt connect parameters */
	char clientId[150] = {0};
	char username[65] = {0};
	char password[65] = {0};

	if ((rc = aiotMqttSign(EXAMPLE_PRODUCT_KEY, EXAMPLE_DEVICE_NAME, EXAMPLE_DEVICE_SECRET, clientId, username, password) < 0)) {
		printf("aiotMqttSign -%0x4x\n", -rc);
		return -1;
	}
	printf("clientid: %s\n", clientId);
	printf("username: %s\n", username);
	printf("password: %s\n", password);
```

**注意**：设备的三元组信息通过宏定义的指定，**用户应该再此替换自己的三元组**
```
    #define EXAMPLE_PRODUCT_KEY			"a11xsrWmW14"
    #define EXAMPLE_DEVICE_NAME			"paho_mqtt"
    #define EXAMPLE_DEVICE_SECRET       "Y877Bgo8X5owd3lcB5wWDjryNPoBUdiH"
```

### <a name="连接阿里云IoT">连接阿里云IoT</a>
在创建MQTT连接前，应该先调用`NetworkInit`和`NetworkConnect`建立TCP连接，TCP建连成功后我们可以看到以下log:
```
NetworkConnect 0
```
TCP建连成功后，我们调用`MQTTClientInit`初始化mqtt client，随后我们配置mqtt建连参数结构体`MQTTPacket_connectData`。其中的`clientID`、`username`、`password`三个参数都是由`aiotMqttSign()`这个API生成的，建连成功后会打印以下log:
```
MQTTConnect 0, Connect aliyun IoT Cloud Success!
```
以下为建连的例程代码
```
    /* network init and establish network to aliyun IoT platform */
	NetworkInit(&n);
	rc = NetworkConnect(&n, host, port);
	printf("NetworkConnect %d\n", rc);

	/* init mqtt client */
	MQTTClientInit(&c, &n, 1000, buf, sizeof(buf), readbuf, sizeof(readbuf));
 
	/* set the default message handler */
	c.defaultMessageHandler = messageArrived;

	/* set mqtt connect parameter */
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = clientId;
	data.username.cstring = username;
	data.password.cstring = password;
	data.keepAliveInterval = 60;
	data.cleansession = 1;
	printf("Connecting to %s %d\n", host, port);

	rc = MQTTConnect(&c, &data);
	printf("MQTTConnect %d, Connect aliyun IoT Cloud Success!\n", rc);
```
这时查看云端控制台，我们可以看到设备已在线，同时我们可以查看下`Topic列表`标签页，稍后我们会使用到`/user/update`和`/user/get`这个两个topic。
<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot_c_online.png" width="800">

### <a name="发布数据">发布数据</a>
以下代码演示了在`while`循环中向`/user/update`这个topic发布固件的消息`Hello world`。
```
    char *pubTopic = "/"EXAMPLE_PRODUCT_KEY"/"EXAMPLE_DEVICE_NAME"/user/update";
	int cnt = 0;
    unsigned int msgid = 0;
	while (!toStop)
	{
		MQTTYield(&c, 1000);	

		if (++cnt % 5 == 0) {
			MQTTMessage msg = {
				QOS1, 
				0,
				0,
				0,
				"Hello world",
				strlen("Hello world"),
			};
            msg.id = ++msgid;
			rc = MQTTPublish(&c, pubTopic, &msg);
			printf("MQTTPublish %d, msgid %d\n", rc, msgid);
		}
	}
```
设备端log如下
```
MQTTPublish 0, msgid 1
```
云端控制台log如图所示

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot_c_check_log.png" width="800">

### <a name="订阅主题">订阅主题</a>
以下代码演示了订阅`/user/get`这个topic，同时注册消息到达回调函数。
```
    void messageArrived(MessageData* md)
    {
        MQTTMessage* message = md->message;

        printf("%.*s\t", md->topicName->lenstring.len, md->topicName->lenstring.data);
        printf("%.*s\n", (int)message->payloadlen, (char*)message->payload);
    }

	char *subTopic = "/"EXAMPLE_PRODUCT_KEY"/"EXAMPLE_DEVICE_NAME"/user/get";

    printf("Subscribing to %s\n", subTopic);
	rc = MQTTSubscribe(&c, subTopic, 1, messageArrived);
	printf("MQTTSubscribe %d\n", rc);    
```
订阅成功的设备端log如下
```
Subscribing to /a11xsrWmW14/paho_mqtt/user/get
MQTTSubscribe 0
```
如下图所示，我们可以在云端控制台向`/user/get`这个topic发布消息，发布成功后便可以在设备端console中查看到对应的下行消息了。

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot_c_pub_msg.png" width="800">

## <a name="详细开发过程">详细开发过程</a>

Paho嵌入式C工程提供了3个子项目：
+ MQTTPacket - 提供了MQTT数据包的序列化与反序列化，以及部分辅助函数
+ MQTTClient - 通过封装MQTTPacket形成的高级别C++客户端程序
+ MQTTClient-C - 通过封装MQTTPacket形成的高级别C客户端程序

以我们关系的MQTTClient-C为例：
```
    ├── CMakeLists.txt
    ├── samples
    │   ├── CMakeLists.txt
    │   ├── FreeRTOS
    │   └── linux
    ├── src
    │   ├── CMakeLists.txt
    │   ├── FreeRTOS
    │   ├── MQTTClient.c
    │   ├── MQTTClient.h
    │   ├── cc3200
    │   └── linux
    └── test
        ├── CMakeLists.txt
        └── test1.c
```
`samples`目录提供了分别支持`FreeRTOS`和`linux`的两个例程。

`src`目录提供了MQTTClient的代码实现，以及用于移植到`FreeRTOS`，`cc3200`和`linux`的网络驱动。

针对linux平台，我们只需要在`samples/linux`目录导入我们提供的5个demo文件，修改编译脚本，即可编译出可以连上阿里云物联网平台的demo程序。

如果想了解paho mqtt的更多API细节，可以查看`MQTTClient.h`。