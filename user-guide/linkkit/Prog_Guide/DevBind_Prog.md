# <a name="目录">目录</a>
+ [绑定原理及问题排查](#绑定原理及问题排查)
    * [绑定工作原理](#绑定工作原理)
        - [1.绑定token由设备端生成](#1.绑定token由设备端生成)
        - [2)手机通过CoAP主动向手机查询token](#2)手机通过CoAP主动向手机查询token)
    * [2.绑定token有手机端生成](#2.绑定token有手机端生成)
    * [绑定问题排查](#绑定问题排查)

# <a name="绑定原理及问题排查">绑定原理及问题排查</a>

绑定即将指定IOT终端设备与手机IOT APP已登陆账号产生关联的过程, 过程中需要依赖一个由设备或手机生成的唯一的token完成绑定验证关系


## <a name="绑定工作原理">绑定工作原理</a>
目前支持两大类绑定:1.token由设备端生成. 2.token由手机端生成(待发布). 下面分别介绍:

### <a name="1.绑定token由设备端生成">1.绑定token由设备端生成</a>
这是当前传统方式, 优点是绑定与配网分离,可以工作在不需要配网的场景. 缺点是依赖可靠性不高的局域网内CoAP通信, 流程较为复杂. 具体细节又分为设备主动广播与手机主动查询两种方式, aos2.1/sdk3.0.1及以下版本版本两种方式同时工作. 下面分别介绍


#### <a name="1)设备通过CoAP广播方式主动向手机发送绑定token">1)设备通过CoAP广播方式主动向手机发送绑定token</a>
主要流程:

1.linkkit SDK初始化, mqtt连云成功;

- 需要调用的API

```
int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info).
int IOT_Linkkit_Connect(int devid);
```
- 关键log

```
iotx_mc_connect(2768): mqtt connect success!
```

2.生成16字节随机数作为绑定token;

3.mqtt订阅"/sys/{pk}/{dn}/thing/awss/enrollee/match_reply" topic;

- 关键log

```
mqtt subscribe packet sent,topic = /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply!
sub /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply success
```


4.通过"/sys/{pk}/{dn}/ts/thing/awss/enrollee/match"topic像云端发送token;

- 关键log

```
report token:{"id":"0","version":"1.0","method":"thing.awss.enrollee.match","params":{"token":"4786F0F6E1CC7BF86017FD4D1429EE4F"}}
```


5.等待云端应答, 应答信息通过步骤3中订阅topic下发.

- 关键log

```
Topic Name : /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply
topic be matched
awss_report_token_reply
```

6.如果30秒中内无应答, 则尝试再次上报, 总尝试次数为2.

7.设备收到云端应答信息后, 启动coap 广播, 向局域网内广播设备信息及token. 广播总次数为30次.

- 关键log

```
topic:/sys/device/info/notify
payload:{"id":"7","version":"1.0","method":"device.info.notify","params":{"awssVer":{"smartconfig":"2.0","zconfig":"2.0","router":"2.0","ap":"2.0","softap":"2.0"},"productKey":"a1GQc8iJOWI","deviceName":"gateway_test01","mac":"B0:F8:93:15:BF:21","ip":"10.0.0.59","cipherType":4,"token":"B52C577E55DE6B3CAF5B0C882410D8BF","remainTime":43603,"type":0}}
```


8.手机收到设备广播的设备信息及token后, 使用相关信息去云端发起绑定动作以完成绑定.

9.手机向云端发起绑定时可能失败, 原因可能有:
  - 云端没查询到相关token(设备没上报, 或上报的与手机收到的token不一致)
  - token已经被使用, 如下图所示, phone1, phone2都收到此token,phone2先绑定成功, phone1绑定失败

<img src="https://cdn.nlark.com/yuque/0/2019/png/288886/1556186104142-e746dc0f-6134-4251-b527-805003c26f38.png?x-oss-process=image/resize,w_1098" width="600"  />



### <a name="2)手机通过CoAP主动向手机查询token">2)手机通过CoAP主动向手机查询token</a>
主要流程:

1.linkkit SDK初始化, mqtt连云成功.

- 需要调用的API

```
int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info).
int IOT_Linkkit_Connect(int devid);
```

- 关键log

```
iotx_mc_connect(2768): mqtt connect success!
```

2.生成16字节随机数作为绑定token;

3.mqtt订阅"/sys/{pk}/{dn}/thing/awss/enrollee/match_reply" topic;

- 关键log

```
mqtt subscribe packet sent,topic = /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply!
sub /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply success
```

4.coap server注册相关资源监听手机查询信息: /sys/device/info/get 及/sys/{pk}/{dn}/device/info/get;
- 关键log

```
Register new resource /sys/device/info/get success, count: 9
Register new resource /sys/a1puSwM5TIg/chazuokaiguan/device/info/get success, count: 8
```


5.通过"/sys/{pk}/{dn}/ts/thing/awss/enrollee/match"topic像云端发送token;
- 关键log

```
report token:{"id":"0","version":"1.0","method":"thing.awss.enrollee.match","params":{"token":"4786F0F6E1CC7BF86017FD4D1429EE4F"}}
```

6.等待云端应答, 应答信息通过步骤3中订阅topic下发.
- 关键log

```
Topic Name : /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply
topic be matched
awss_report_token_reply
```

7.如果30秒中内无应答, 则尝试再次上报, 总尝试次数为2.

8.手机通过coap广播或单播(知道对方IP地址前提下)查询设备信息.
- 关键log

```
Request path is /sys/device/info/get
Found the resource: /sys/device/info/get
```

9.设备收到相关查询请求后, 如果已有未过期的并且上报云端成功的token,直接返回此token给手机. 同时更新token并上报云端;
- 关键log

```
sending message to app: {"id":123,"code":200,"data":{"awssVer":{"smartconfig":"2.0","zconfig":"2.0","router":"2.0","ap":"2.0","softap":"2.0"},"productKey":"a1GQc8iJOWI","deviceName":"gateway_test01","mac":"B0:F8:93:15:BF:21","ip":"10.0.0.59","cipherType":4,"token":"B52C577E55DE6B3CAF5B0C882410D8BF","remainTime":40738,"type":0}}
report token:{"id":"1","version":"1.0","method":"thing.awss.enrollee.match","params":{"token":"6CFD937AE0AFF0C7B1044A9AA3B5EF2E"}}
```

10.设备收到相关查询请求后, 如果没有未过期的并且上报云端成功的token,不返回token给手机. 同时更新token并上报云端.
11.手机收到设备返回的设备信息及token后, 使用相关信息去云端发起绑定动作以完成绑定.
12.这种场景可用于多个手机同时发起绑定动作(每个手机拿到的token都是唯一的, 只要产品安全策略允许, 多个手机都可以绑定成功).

<img src="https://cdn.nlark.com/yuque/0/2019/png/288886/1556186286338-484b0c8e-8ec8-4ec2-acce-5ffa53cfcf27.png?x-oss-process=image/resize,w_1088" width="600"  />


## <a name="2.绑定token有手机端生成">2.绑定token有手机端生成</a>
此方式在配网过程中完成绑定token的交互, 过程简单可靠, 提高与配网一起工作的绑定成功率, 同时可以在路由隔离的情况下完成绑定动作. <br />主要流程:

1.设备处于配网模式(支持传输绑定token的配网模式, 如热点配网等);

- 需要调用的API

```
awss_start();//alios 上为 netmgr_start(1);
awss_config_press();
//or
dev_ap_start();//设备热点配网
```

- 关键log

```
IOTX_AWSS_START
zconfig_init
```

2.手机进入配网模式, 生成绑定token, 发起配网动作.

3.设备收到配网及绑定信息.

4.设备连接指定路由器.

-   关键log

```
Got ip : 192.168.1.128, gw : 192.168.1.1, mask : 255.255.255.0
```

5.连接mqtt成功.

- 需要调用的API

```
int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info).
int IOT_Linkkit_Connect(int devid);
```

- 关键log

```
iotx_mc_connect(2768): mqtt connect success!
```

6.设备上报配网过程中得到的token.

7.手机端定期查询云端此token是否存在. 存在则完成绑定动作.

<img src="https://cdn.nlark.com/yuque/0/2019/png/288886/1556186301632-1c54af06-ab2a-45c9-91b9-726760605c99.png?x-oss-process=image/resize,w_1020" width="600"  />


## <a name="绑定问题排查">绑定问题排查</a>
绑定不成功时, 需要将设备的的log等级开成debug级别, 以方便问题排查. <br />

1.绑定失败, 第一步排查mqtt是否连接成功,关键词: mqtt connect success!

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

2.查看token是否上报云端, 关键词: report token

```
[2019-04-24-14:37:26][018912]<D> report token:{"id":"0","version":"1.0","method":"thing.awss.enrollee.match","params":{"token":"4786F0F6E1CC7BF86017FD4D1429EE4F"}}
[2019-04-24-14:37:26]
[2019-04-24-14:37:26][018927]<D> Payload Len : 0
[2019-04-24-14:37:26][018930]<I> report token res:13
```

3.查看上报token是否收到云端应答, 关键词: enrollee/match_reply 和 awss_report_token_reply


```
4-24-14:36:32][020362]<D>         Packet Ident : 00000000
[2019-04-24-14:36:32][020366]<D>         Topic Length : 63
[2019-04-24-14:36:32][020372]<D>           Topic Name : /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply
[2019-04-24-14:36:32][020381]<D>     Payload Len/Room : 104 / 112
[2019-04-24-14:36:32][020385]<D>       Receive Buflen : 180
[2019-04-24-14:36:32][020389]<D> delivering msg ...
[2019-04-24-14:36:32][020392]<D> topic be matched
[2019-04-24-14:36:32][020395]<I> awss_report_token_reply
```
如果有收到这个回包, 但是提示NO matching any topic, call default handle function, 如下, 则说明<br />/sys/{pk}/{dn}/thing/awss/enrollee/match_reply订阅失败, 需要查看订阅失败的原因
```
[2019-04-24-14:37:05][053333]<D>           Topic Name : /sys/a1GQc8iJOWI/gateway_test01/thing/awss/enrollee/match_reply
[2019-04-24-14:37:05][053341]<D>     Payload Len/Room : 127 / 135
[2019-04-24-14:37:05][053345]<D>       Receive Buflen : 191
[2019-04-24-14:37:05][053349]<D> delivering msg ...
[2019-04-24-14:37:05][053354]<D> NO matching any topic, call default handle function
```

4.查看token有无广播给手机, 关键词: device.info.notify

```
[2019-04-24-14:37:27][020561]<I> topic:/sys/device/info/notify
[2019-04-24-14:37:27]
[2019-04-24-14:37:27][020566]<D> *********Message Info**********
[2019-04-24-14:37:27][020581]<D> payload:{"id":"7","version":"1.0","method":"device.info.notify","params":{"awssVer":{"smartconfig":"2.0","zconfig":"2.0","router":"2.0","ap":"2.0","softap":"2.0"},"productKey":"a1GQc8iJOWI","deviceName":"gateway_test01","mac":"B0:F8:93:15:BF:21","ip":"10.0.0.59","cipherType":4,"token":"4786F0F6E1CC7BF86017FD4D1429EE4F","remainTime":43360,"type":0}}
```

5.查看手机是主动过来查询过token,关键词Request path is /sys/device/info/get<br />如果当前有可用token,这会返回给手机, 关键词: sending message to app:<br />否则需要进一步定位为什么没有可用token. 可能能原因:

-  mqtt还没连接成功.
- token上报云端未收到应答.
- coap注册/sys/device/info/get失败

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

