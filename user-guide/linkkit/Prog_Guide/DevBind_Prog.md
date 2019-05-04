# <a name="目录">目录</a>
+ [功能概述](#功能概述)
+ [流程详细说明](#流程详细说明)
    * [Token由设备端生成](#Token由设备端生成)
        - [设备主动发送Token](#设备主动发送Token)
        - [设备被动应答Token](#设备被动应答Token)
    * [Token由手机端生成](#Token由手机端生成)
+ [问题排查步骤](#问题排查步骤)
+ [附加说明](#附加说明)

# <a name="功能概述">功能概述</a>

> 当IoT设备可以被人通过智能手机观察和操作时, 这条通信路径的合法性需要由IoT云进行保障
>
> 也就是说, **云需要确保使用手机的用户的确拥有设备的访问权, 这个建立信任的过程, 就称为绑定**

---
绑定的原理十分简单, 和我们日常生活中经常用到的"验证码"是一样的, 只不过SDK的绑定场景下, IoT云是接收"验证码"的一方, 而设备或者手机, 是发出"验证码"的一方

<br>

+ "验证码"其实只是个一次性使用的字符串, 由英文字母和阿拉伯数字组成, 它由设备或者手机产生, 在下文中以Token称呼它
+ Token的生成者会将它先行发送给IoT云, 使云建立了Token和发送者的对应关系, 生成Token的可能是设备也可能是手机
+ Token的接收者接着也会将它发送给IoT云, 使云建立Token和接收者的对应关系, 接收Token的可能是手机也可能是设备
+ 若IoT云发现自己先后接收到的Token内容匹配, 则它认为Token的生成者/接收者确在局域网内进行了可信的信息交换, 允许二者中的手机控制设备是合法, 安全的, 绑定完成

<br>
<div align=center>
<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/brief_devbind.png" width="700" height="600">
</div>

> 如上图所示, 绑定可能通过两种路径完成, 桔红色流程中, Token由设备产生, 天蓝色流程中, Token由手机产生, 都可以完成绑定

---
另外, 绑定功能被使用的场景是较为集中的

+ *单纯的IoT设备集成SDK后与云端两者之间的通信, 不需要绑定功能, 此时数据和控制流只出现在"物联网"的物与云之间, 没有第三者的参与*
+ *通过蜂窝网络连云的广域网设备集成SDK后, 与云端之间通信, 并由此受手机端操纵或者将设备的状态显示到手机上, 也不需要绑定功能*
+ *只有通过WiFi或者以太网连接到路由器/热点进而连接到云的设备, 集成SDK后可以和手机进行局域网内通信时, 才使用绑定功能, 以提高安全性*

# <a name="流程详细说明">流程详细说明</a>

以下将结合SDK的日志打印, 详细说明绑定流程中顺序发生的各个环节

## <a name="Token由设备端生成">Token由设备端生成</a>

Token由设备端生成是当前绑定的传统方式, 优点是可以在没有WiFi配网的场景工作. 缺点是依赖可靠性不高的局域网内CoAP通信, 流程较复杂

这种绑定路径下, 又可能发生设备主动广播Token和手机查询Token设备被动应答两种交互, 它们在SDK中同时存在, 先后工作

### <a name="设备主动发送Token">设备主动发送Token</a>

集成SDK的设备将通过在局域网内发送CoAP广播报文的方式, 主动向手机发送绑定Token, 详细流程如下

1. SDK初始化, MQTT连云成功
---

> 需要用户调用如下API, 调用条件是设备已具备连接因特网的网络环境

    int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info)
    int IOT_Linkkit_Connect(int devid)

> 标志性日志

    iotx_mc_connect(2768): mqtt connect success!

2. SDK生成16字节随机数作为绑定Token
---

3. SDK订阅 `/sys/${pk}/${dn}/thing/awss/enrollee/match_reply` 以接收云端应答
---

> 标志性日志

    mqtt subscribe packet sent, topic = /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply!
    sub /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply success

4. SDK向 `/sys/${pk}/${dn}/thing/awss/enrollee/match` 发布报文, 将步骤2中产生的Token告知云端
---

> 标志性日志

    report token:{"id":"0","version":"1.0","method":"thing.awss.enrollee.match","params":{"token":"4786F0F6E1CC7BF86017FD4D1429EE4F"}}

5. SDK等待云端对Token的应答, 应答由步骤3中订阅的Topic下发
---

> 标志性日志

    Topic Name : /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply
    topic be matched
    awss_report_token_reply

6. 若超过`30秒`SDK未得到云端应答, 则再次上报, 一共将尝试`2`次
---

7. 若SDK收到云端应答, 则开始局域网内的`CoAP`广播, 发送设备信息及Token, 发送总次数为`30次`
---

> 标志性日志 (此处为易读添加了换行, SDK实际日志是不换行的)

    topic:/sys/device/info/notify
    payload:{
        "id":"7",
        "version":"1.0",
        "method":"device.info.notify",
        "params":{
            "awssVer":{
                "smartconfig":"2.0",
                "zconfig":"2.0",
                "router":"2.0",
                "ap":"2.0",
                "softap":"2.0"
            },
            "productKey":"a1GQc8iJOWI",
            "deviceName":"gateway_test01",
            "mac":"B0:F8:93:15:BF:21",
            "ip":"10.0.0.59",
            "cipherType":4,
            "token":"B52C577E55DE6B3CAF5B0C882410D8BF",
            "remainTime":43603,
            "type":0
        }
    }

8. 手机收到设备广播的设备信息及Token后, 用其中信息向云端发起绑定请求
---

手机向云端请求的绑定也可能失败, 可能原因有:

+ 云端没查到绑定请求中包含的Token (设备没上报, 或上报的Token与手机收到的Token不一致)
+ Token已被使用, 比如下图所示, 如果phone1, phone2都收到了Token, 而phone2先绑定成功, 则phone1绑定将会失败

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/bind_2phones.png" width="600">

### <a name="设备被动应答Token">设备被动应答Token</a>

由于设备主动在局域网内广播的Token, 手机不一定能够收到, 所以手机也可以主动向设备查询Token, 查询的方式是CoAP的广播/单播

> 这种绑定路径下, 前3个环节和依靠设备主动广播时而绑定成功的路径是相同的

1. SDK初始化, MQTT连云成功
---

2. SDK生成16字节随机数作为绑定Token
---

3. SDK订阅 `/sys/${pk}/${dn}/thing/awss/enrollee/match_reply` 以接收云端应答
---

> 第4个环节是不同的, SDK在设备侧准备了应答Token的接口


4. SDK注册CoAP服务端资源监听手机查询
---
+ `/sys/device/info/get`
+ `/sys/${pk}/${dn}/device/info/get`

> 标志性日志

    Register new resource /sys/device/info/get success, count: 9
    Register new resource /sys/a1puSwM5TIg/chazuokaiguan/device/info/get success, count: 8

> 接下来的4个环节和依靠设备主动广播时而绑定成功的路径又是相同的

5. SDK向 `/sys/${pk}/${dn}/thing/awss/enrollee/match` 发布报文, 将步骤2中产生的Token告知云端
---

6. SDK等待云端对Token的应答, 应答由步骤3中订阅的Topic下发
---

7. 若超过`30秒`SDK未得到云端应答, 则再次上报, 一共将尝试`2`次
---

8. 若SDK收到云端应答, 则开始局域网内的`CoAP`广播, 发送设备信息及Token, 发送总次数为`30次`
---

9. 手机错过了上述广播, 通过CoAP广播或单播(知道设备IP地址前提下)查询设备信息
---

> 标志性日志

    Request path is /sys/device/info/get
    Found the resource: /sys/device/info/get

10. 设备收到手机查询, 用CoAP单播回复Token给手机
---

若已有上报云端成功的Token且尚未过期失效(为保证安全, Token每过一段时间定期刷新), 则将此Token在局域网内应答给手机, 同时更新Token并上报云端

> 标志性日志 (此处为易读添加了换行, SDK实际日志是不换行的)

局域网应答日志

    sending message to app: {
        "id":123,
        "code":200,
        "data":{
            "awssVer":{
                "smartconfig":"2.0",
                "zconfig":"2.0",
                "router":"2.0",
                "ap":"2.0",
                "softap":"2.0"
            },
            "productKey":"a1GQc8iJOWI",
            "deviceName":"gateway_test01",
            "mac":"B0:F8:93:15:BF:21",
            "ip":"10.0.0.59",
            "cipherType":4,
            "token":"B52C577E55DE6B3CAF5B0C882410D8BF",
            "remainTime":40738,
            "type":0
        }
    }

以及新Token上报云端日志
    
    report token:{
        "id":"1",
        "version":"1.0",
        "method":"thing.awss.enrollee.match",
        "params":{
            "token":"6CFD937AE0AFF0C7B1044A9AA3B5EF2E"
        }
    }


若没有上报云端成功且尚未过期失效的Token, 则不作应答, 只是更新token并上报云端, 等待手机的下一次查询

11. 手机收到设备应答的设备信息及Token后, 用其中信息向云端发起绑定请求
---

*这种路径可支持多个手机绑定同个设备, 因为每个手机拿到的token都是唯一的, 只要产品安全策略允许, 每个手机都可以绑定成功, 流程图如下*

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/bind_token_from_device.png" width="600">

## <a name="Token由手机端生成">Token由手机端生成</a>

如果集成SDK的IoT设备是WiFi设备, 并使用了SDK的WiFi配网功能, 则Token也可能由手机产生, 通过另一种路径绑定成功

*这种方式的优点是在配网环节完成绑定Token的交互, 过程简单可靠, 提高了成功率, 并且可在路由隔离(访客模式)的情况下绑定成功*

---
详细流程如下

1. 设备进入待配网模式
---
该配网模式需要支持传输绑定Token, 如热点配网模式等

> 需要用户调用的API

    awss_start();       /* 一键配网模式, 在AliOS Things上对应是 netmgr_start(1); */
    awss_config_press();

或者
    
    dev_ap_start();     /* 设备热点配网模式 */

> 标志性日志

    IOTX_AWSS_START
    zconfig_init

2. 手机进入配网app, 生成绑定Token, 开始对设备配网
---

3. 设备收到配网及绑定信息
---
配网信息主要指可连接因特网的热点SSID和密码, 绑定信息主要指手机产生的绑定Token

4. 设备连接指定路由器成功
---

> 标志性日志

    Got ip : 192.168.1.128, gw : 192.168.1.1, mask : 255.255.255.0

5. 设备通过MQTT连云成功
---

> 需要用户调用的API

    int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info).
    int IOT_Linkkit_Connect(int devid);

> 标志性日志

    iotx_mc_connect(2768): mqtt connect success!

6. 设备向云端上报从手机得到的绑定Token
---

7. 手机周期性查询云端此前由自己产生, 并在配网中传递给设备的Token是否已到达云端, 若到达且匹配则完成绑定
---

---
以上流程可总结为下图所示

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/bind_token_from_phone.png" width="600">

# <a name="问题排查步骤">问题排查步骤</a>

绑定不成功时, 首先需要将设备的的 log 等级开成 debug 级别, 然后循以下环节逐个检查关键点日志

1. 检查设备是否由MQTT连云成功, 这是Token上报到云端的前提
---
关键词: `mqtt connect success!`

> 成功时的示例日志

```
[wrn] __alcs_localsetup_kv_get(45): HAL_Kv_Get('4bc1847870c88646d2bddd5a7afe6f5c') = -1 (!= 0), return 1
[wrn] alcs_localsetup_ac_as_load(147): ALCS KV Get local Prefix And Secret Fail
[inf] alcs_add_svr_key(338): alcs_add_svr_key, priority=0
[inf] add_svr_key(295): add_svr_key
[inf] dm_client_open(37): CM Fd: 0
[inf] guider_print_dev_guider_info(320): ....................................................
[inf] guider_print_dev_guider_info(321):           ProductKey : a1x89u9s4YT
[inf] guider_print_dev_guider_info(322):           DeviceName : A09DC140827C
[inf] guider_print_dev_guider_info(323):             DeviceID : a1x89u9s4YT.A09DC140827C
[inf] guider_print_dev_guider_info(327): ....................................................
[inf] guider_print_dev_guider_info(328):        PartnerID Buf : ,partner_id=example.demo.partner-id
[inf] guider_print_dev_guider_info(329):         ModuleID Buf : ,module_id=example.demo.module-id
[inf] guider_print_dev_guider_info(330):           Guider URL :
[inf] guider_print_dev_guider_info(332):       Guider SecMode : 2 (TLS + Direct)
[inf] guider_print_dev_guider_info(334):     Guider Timestamp : 2524608000000
[inf] guider_print_dev_guider_info(338): ....................................................
[dbg] CoAPServer_yield(83): Enter to CoAP daemon task
00 ---app:: pwm disable result 0
[inf] guider_print_conn_info(297): -----------------------------------------
[inf] guider_print_conn_info(298):             Host : a1x89u9s4YT.iot-as-mqtt.cn-shanghai.aliyuncs.com
[inf] guider_print_conn_info(299):             Port : 1883
[inf] guider_print_conn_info(304):         ClientID : a1x89u9s4YT.A09DC140827C|securemode=2,timestamp=2524608000000,signmethod=hmacsha1,gw=0,ext=0,partner_id=example.demo.partner-id,module_id=example.demo.module-id|
[inf] guider_print_conn_info(306):       TLS PubKey : 0x56e1d0 ('-----BEGIN CERTI ...')
[inf] guider_print_conn_info(309): -----------------------------------------
[inf] iotx_mc_init(2378): MQTT init success!
[prt] Loading the CA root certificate ...
[prt]  ok (0 skipped)
[prt] Connecting to /a1x89u9s4YT.iot-as-mqtt.cn-shanghai.aliyuncs.com/1883...
[prt] setsockopt SO_SNDTIMEO timeout: 10s
[prt] connecting IP_ADDRESS: 139.196.135.135
ThreadVoice() g_bThreadVoiceRun loop
[prt]  ok
[prt]   . Setting up the SSL/TLS structure...
[prt]  ok
[prt] Performing the SSL/TLS handshake...
[prt]  ok
[prt]   . Verifying peer X.509 certificate..
[prt] certificate verification result: 0x00
[inf] iotx_mc_connect(2768): mqtt connect success!
```

2. 检查Token是否被上报给云端
---
关键词: `report token`

> 成功时的示例日志

```
[2019-04-24-14:37:26][018912]<D> report token:{"id":"0","version":"1.0","method":"thing.awss.enrollee.match","params":{"token":"4786F0F6E1CC7BF86017FD4D1429EE4F"}}
[2019-04-24-14:37:26]
[2019-04-24-14:37:26][018927]<D> Payload Len : 0
[2019-04-24-14:37:26][018930]<I> report token res:13
```

3. 检查上报Token后是否收到云端应答
---
关键词: `enrollee/match_reply` 以及 ` awss_report_token_reply`

> 成功时的示例日志

```
[2019-04-24-14:36:32][020362]<D>         Packet Ident : 00000000
[2019-04-24-14:36:32][020366]<D>         Topic Length : 63
[2019-04-24-14:36:32][020372]<D>           Topic Name : /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply
[2019-04-24-14:36:32][020381]<D>     Payload Len/Room : 104 / 112
[2019-04-24-14:36:32][020385]<D>       Receive Buflen : 180
[2019-04-24-14:36:32][020389]<D> delivering msg ...
[2019-04-24-14:36:32][020392]<D> topic be matched
[2019-04-24-14:36:32][020395]<I> awss_report_token_reply
```

如果有收到云端的应答, 但提示

    NO matching any topic, call default handle function

则表明 `/sys/${pk}/${dn}/thing/awss/enrollee/match_reply` 订阅失败而导致绑定流程异常, 需调查订阅失败的原因, 例如

```
[2019-04-24-14:37:05][053333]<D>           Topic Name : /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply
[2019-04-24-14:37:05][053341]<D>     Payload Len/Room : 127 / 135
[2019-04-24-14:37:05][053345]<D>       Receive Buflen : 191
[2019-04-24-14:37:05][053349]<D> delivering msg ...
[2019-04-24-14:37:05][053354]<D> NO matching any topic, call default handle function
```

4. 检查Token是否有主动广播给手机
---
关键词: `device.info.notify`

> 成功时的示例日志

```
[2019-04-24-14:37:27][020561]<I> topic:/sys/device/info/notify
[2019-04-24-14:37:27]
[2019-04-24-14:37:27][020566]<D> *********Message Info**********
[2019-04-24-14:37:27][020581]<D> payload:{"id":"7","version":"1.0","method":"device.info.notify","params":{"awssVer":{"smartconfig":"2.0","zconfig":"2.0","router":"2.0","ap":"2.0","softap":"2.0"},"productKey":"a1GQc8iJOWI","deviceName":"gateway_test01","mac":"B0:F8:93:15:BF:21","ip":"10.0.0.59","cipherType":4,"token":"4786F0F6E1CC7BF86017FD4D1429EE4F","remainTime":43360,"type":0}}
```

5. 检查是否收到手机查询Token的请求
---
关键词: `Request path is /sys/device/info/get`

> 成功时的示例日志

```
[2019-04-24-14:36:35][023371]<D> Request path is /sys/device/info/get
[2019-04-24-14:36:35][023376]<D> CoAPResourceByPath_get, context:0x100220f0
[2019-04-24-14:36:35]
[2019-04-24-14:36:35][023382]<D> Found the resource: /sys/device/info/get
[2019-04-24-14:36:35][023389]<D> no rst
[2019-04-24-14:36:35]
[2019-04-24-14:36:35][023392]<I> sending message to app: {"id":123,"code":200,"data":{"awssVer":{"smartconfig":"2.0","zconfig":"2.0","router":"2.0","ap":"2.0","softap":"2.0"},"productKey":"a1GQc8iJOWI","deviceName":"gateway_test01","mac":"B0:F8:93:15:BF:21","ip":"10.0.0.59","cipherType":4,"token":"B52C577E55DE6B3CAF5B0C882410D8BF","remainTime":40738,"type":0}}
[2019-04-24-14:36:35][023423]<I> update token
```

如果收到请求后有可用token, SDK会将它应答给手机, 关键词: `sending message to app`

---
而如果没有应答, 可能需要进一步定位为什么没有可用token

可能的原因:

+ Token上报云端后, 未收到云端应答
+  MQTT连云没有成功
+ 在 CoAP Server 注册资源 `/sys/device/info/get` 失败

# <a name="附加说明">附加说明</a>

+ 设备端连接到云端之后会立即发送一个 Token 到云端, 并记录这个 Token 的时间
    - 设备端不会周期的发送 Token 到云端, 即使 Token 已超时
    - 设备端收到来自手机的 Token 查询时, 如果发现 Token 已超时, 将会立即发送 Token 到云端, 然后再响应手机端的查询
    - 设备端收到来自手机的 Token 查询时, 如果发现 Token 未超时, 将立即应答未超时的 Token . 然后再生成新的 Token 上报云端, 更新 Token 超时时间
    - 云端收到设备端的 Token 时会一直保存 Token , 直到绑定成功或者 Token 超时才会删除, 因此如果设备端在短时间内上报多个 Token , 云端都会进行记录
    - 配网成功时设备端会主动通告 Token , 然后手机端可以收到 Token 之后到云端去进行设备绑定, 然后云端将会删除该 Token 的记录
        + 但是设备端并不知道上报云端的 Token 已经被删除了
        + 所以如果手机立即再去设备端查询 Token, 设备端还是会返回一个在云端已经无效的 Token, 此时手机端去绑定设备就会收到无效 Token 的错误
