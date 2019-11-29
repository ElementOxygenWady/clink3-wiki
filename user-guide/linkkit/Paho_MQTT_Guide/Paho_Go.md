# <a name="目录">目录</a>
+ [文档简介](#文档简介)
+ [准备开发环境](#准备开发环境)
+ [下载Go语言版本的Paho库](#下载Go语言版本的Paho库)
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


---
# <a name="文档简介">文档简介</a>
Paho提供的MQTT Go开源代码中已包含基于Go语言的连云解决方案，本示例文档讲解如何调用Paho的Go版本的MQTT类库连接阿里云物联网平台。


# <a name="准备开发环境">准备开发环境</a>

安装Go语言包


苹果电脑：brew install go


ubuntu电脑：sudo apt-get install golang-go

# <a name="下载Go语言版本的Paho库">下载Go语言版本的Paho库</a>
安装go语言的mqtt和相关依赖的库


go get github.com/eclipse/paho.mqtt.golang


go get github.com/gorilla/websocket


go get golang.org/x/net/proxy

# <a name="改造连接阿里云">改造连接阿里云</a>

## <a name="下载示例">下载示例</a>

[下载链接](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/aiot_go_demo.zip)

说明：

iot.go文件，里面包含了连云的逻辑

x509文件夹包含了连云需要的证书

## <a name="运行示例">运行示例</a>


运行iot.go文件
---
```
go run iot.go
```

运行效果
---
```
clientId192.168.56.1deviceNamedengproductKeya1Zd7n5yTt8timestamp1528018257135
1b865320fc183cc747041c9faffc9055fc45bbab
Connect aliyun IoT Cloud Sucess
Subscribe topic /a1Zd7n5yTt8/deng/user/get success
publish msg: 0
publish msg:  ABC #0
publish msg: 0
publish msg:  ABC #0
publish msg: 1
publish msg:  ABC #1
publish msg: 2
publish msg:  ABC #2
publish msg: 3
publish msg:  ABC #3
publish msg: 4
publish msg:  ABC #4
```

## <a name="核心源码">核心源码</a>
### <a name="计算登录密码">计算登录密码</a>
```
  //mandatory input data
  var productKey string = "a1Zd7n5yTt8"
  var deviceName string = "deng"
  var deviceSecrete string = "UrwclDV33NaFSmk0JaBxNTqgSrJWXIkN"

  //optional input
  var timeStamp string = "1528018257135"
  var clientId string =  "192.168.56.1"
  var subTopic string = "/" + productKey + "/" + deviceName + "/user/get";
  var pubTopic string = "/" + productKey + "/" + deviceName + "/user/update";

  var raw_passwd bytes.Buffer
  raw_passwd.WriteString("clientId" + clientId)
  raw_passwd.WriteString("deviceName")
  raw_passwd.WriteString(deviceName)
  raw_passwd.WriteString("productKey")
  raw_passwd.WriteString(productKey);
  raw_passwd.WriteString("timestamp")
  raw_passwd.WriteString(timeStamp)
  fmt.Println(raw_passwd.String())
  //hmac ,use sha1
  mac := hmac.New(sha1.New, []byte(deviceSecrete))
  mac.Write([]byte(raw_passwd.String()))
  password := fmt.Sprintf("%02x", mac.Sum(nil))
  fmt.Println(password)

  var raw_broker bytes.Buffer
  raw_broker.WriteString("tls://")
  raw_broker.WriteString(productKey)
  raw_broker.WriteString(".iot-as-mqtt.cn-shanghai.aliyuncs.com:1883")
  opts := MQTT.NewClientOptions().AddBroker(raw_broker.String())

  var MQTTClientId bytes.Buffer
  MQTTClientId.WriteString(clientId)
  //hmac ,use sha1
  MQTTClientId.WriteString("|securemode=2,_v=sdk-go-1.0.0,signmethod=hmacsha1,timestamp=") //TLS
  MQTTClientId.WriteString(timeStamp)
  MQTTClientId.WriteString("|")
  opts.SetClientID(MQTTClientId.String())

  opts.SetUsername(deviceName + "&" + productKey)   // device name and product key
  opts.SetPassword(password)

  opts.SetKeepAlive(60 * 2 * time.Second)
  opts.SetDefaultPublishHandler(f)

  tlsconfig := NewTLSConfig()
  opts.SetTLSConfig(tlsconfig)

```

用户替换三元组的时候，只要替换下图中的productKey, deviceName, deviceSecrete这三个值即可。


<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-go-dev-1.png" width="600">


### <a name="连接阿里云IoT">连接阿里云IoT</a>
用上一节中计算出了连云需要参数，包括broker, username, password, clientId等。将这些信息都包含在opts中，调用MQTT的Connect()函数连云。

```
  //create and start a client using the above ClientOptions
  c := MQTT.NewClient(opts)

  if token := c.Connect(); token.Wait() && token.Error() != nil {
    panic(token.Error())
  }
  fmt.Print("Connect aliyun IoT Cloud Sucess\n");
```

### <a name="发布数据">发布数据</a>
指定了拟发布报文的目的topic，以及相应的payload，调用Publish接口就能实现报文的发送。
```
  //Publish 5 messages to /go-mqtt/sample at qos 1 and wait for the receipt
  //from the server after sending each message
  for i := 0; i < 10; i++ {
    fmt.Println("publish msg:", i)
    text := fmt.Sprintf("ABC #%d", i)
    //    token := c.Publish("go-mqtt/sample", 0, false, text)
    token := c.Publish(pubTopic, 0, false, text)
    fmt.Println("publish msg: ", text)
    token.Wait()

    time.Sleep(20 * time.Second)
  }
```

### <a name="订阅主题">订阅主题</a>
指定了要订阅的topic，以及相应的payload，调用Subscribe接口就能实现报文的发送。
```
  //subscribe to the topic /go-mqtt/sample and request messages to be delivered
  //at a maximum qos of zero, wait for the receipt to confirm the subscription
  if token := c.Subscribe(subTopic, 0, nil); token.Wait() && token.Error() != nil {
    fmt.Println(token.Error())
    os.Exit(1)
  }
  fmt.Print("Subscribe topic " + subTopic + " success\n");
```

## <a name="详细开发过程">详细开发过程</a>
编辑代码：用vi等工具修改.go文件


编译与运行：在命令行里面用go run xxx.go

# <a name="确认接入成功">确认接入成功</a>

以下日志说明连云成功
---
```
Connect aliyun IoT Cloud Sucess
```

以下日志说明订阅成功
---
```
Subscribe topic /a1Zd7n5yTt8/deng/user/get success
```

以下日志说明数据上报成功
---
```
publish msg
```


