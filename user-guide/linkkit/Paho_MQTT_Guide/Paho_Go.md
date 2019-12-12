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

# <a name="文档简介">文档简介</a>
本示例文档讲解如何调用Paho的Go版本的MQTT类库连接阿里云物联网平台

# <a name="准备开发环境">准备开发环境</a>

安装Go语言包

+ 苹果电脑

```bash
brew install go
```

+ Ubuntu电脑

```bash
sudo apt-get install golang-go
```

# <a name="下载Go语言版本的Paho库">下载Go语言版本的Paho库</a>
安装go语言的mqtt和相关依赖的库

```bash
go get github.com/eclipse/paho.mqtt.golang
go get github.com/gorilla/websocket
go get golang.org/x/net/proxy
```

# <a name="改造连接阿里云">改造连接阿里云</a>

## <a name="下载示例">下载示例</a>

[下载链接](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/aiot-go-demo.zip)

| 文件         | 说明
|-------------|---------------------------------
| MqttSign.go | 这个文件里面包含了mqtt连云所需的鉴权信息
| iot.go      | 这个文件里面包含了连云的逻辑
| x509        | 这个目录包含了连云需要的证书

## <a name="运行示例">运行示例</a>

运行iot.go文件
---
```bash
go run iot.go MqttSign.go 
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
publish msg: 1
publish msg:  ABC #1
publish msg: 2
publish msg:  ABC #2
publish msg: 3
publish msg:  ABC #3
publish msg: 4
publish msg:  ABC #4
publish msg: 5
publish msg:  ABC #5
```

## <a name="核心源码">核心源码</a>
[MqttSign.go](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/MqttSign.go)

### <a name="计算登录密码">计算登录密码</a>

```go
func calculate_sign(clientId, productKey, deviceName, deviceSecret, timeStamp string) AuthInfo {
    var raw_passwd bytes.Buffer
    raw_passwd.WriteString("clientId" + clientId)
    raw_passwd.WriteString("deviceName")
    raw_passwd.WriteString(deviceName)
    raw_passwd.WriteString("productKey")
    raw_passwd.WriteString(productKey);
    raw_passwd.WriteString("timestamp")
    raw_passwd.WriteString(timeStamp)
    fmt.Println(raw_passwd.String())

    // hmac, use sha1
    mac := hmac.New(sha1.New, []byte(deviceSecret))
    mac.Write([]byte(raw_passwd.String()))
    password:= fmt.Sprintf("%02x", mac.Sum(nil))
    fmt.Println(password)
    username := deviceName + "&" + productKey;

    var MQTTClientId bytes.Buffer
    MQTTClientId.WriteString(clientId)
    // hmac, use sha1; securemode=2 means TLS connection 
    MQTTClientId.WriteString("|securemode=2,_v=paho-go-1.0.0,signmethod=hmacsha1,timestamp=")
    MQTTClientId.WriteString(timeStamp)
    MQTTClientId.WriteString("|")

    auth := AuthInfo{password:password, username:username, mqttClientId:MQTTClientId.String()}
    return auth;
}
```

用户替换三元组的时候, 只要替换下图中的productKey, deviceName, deviceSecret这三个值即可

<img src="http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Paho_MQTT_Guide/imgs/aiot-go-dev-1.png" width="600">

### <a name="连接阿里云IoT">连接阿里云IoT</a>
调用calculate_sign函数，计算出连云需要参数, 包括`username`, `password`, `clientId`等

接着将这些信息都包含在`opts`中, 调用MQTT的Connect()函数连云

```go
    // set the login broker url
    var raw_broker bytes.Buffer
    raw_broker.WriteString("tls://")
    raw_broker.WriteString(productKey)
    raw_broker.WriteString(".iot-as-mqtt.cn-shanghai.aliyuncs.com:1883")
    opts := MQTT.NewClientOptions().AddBroker(raw_broker.String());

    // calculate the login auth info, and set it into the connection options
    auth := calculate_sign(clientId, productKey, deviceName, deviceSecret, timeStamp)
    opts.SetClientID(auth.mqttClientId)
    opts.SetUsername(auth.username)
    opts.SetPassword(auth.password)
    opts.SetKeepAlive(60 * 2 * time.Second)
    opts.SetDefaultPublishHandler(f)
```

### <a name="发布数据">发布数据</a>
指定了拟发布报文的目的topic, 以及相应的payload, 调用`Publish`接口就能实现报文的发送

```go
    // publish 5 messages to pubTopic("/a1Zd7n5yTt8/deng/user/update")
    for i := 0; i < 5; i++ {
        fmt.Println("publish msg:", i)
        text := fmt.Sprintf("ABC #%d", i)
        token := c.Publish(pubTopic, 0, false, text)
        fmt.Println("publish msg: ", text)
        token.Wait()
        time.Sleep(2 * time.Second)
    }
```

### <a name="订阅主题">订阅主题</a>
指定了要订阅的topic, 以及相应的payload, 调用`Subscribe`接口就能实现主题的订阅

```go
  // Subscribe to the subTopic ('/${productkey}/${deviceName}/user/get') 
  if token := c.Subscribe(subTopic, 0, nil); token.Wait() && token.Error() != nil {
    fmt.Println(token.Error())
    os.Exit(1)
  }
  fmt.Print("Subscribe topic " + subTopic + " success\n");
```

## <a name="详细开发过程">详细开发过程</a>

+ 编辑代码: 用vi等工具修改.go文件
+ 编译与运行: 在命令行里面用 `go run iot.go MqttSign.go`

# <a name="确认接入成功">确认接入成功</a>

以下日志说明连云成功
---
```
Connect aliyun IoT Cloud Success
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
