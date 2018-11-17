简介
---
以移远BC26 NBIoT开发板为例, 演示如何连接飞燕平台, 进行MQTT Topic订阅, 属性设置/上报/事件上报等操作

环境准备
---
将准备好的可供NBIoT使用的SIM卡插入开发板SIM卡槽

以ubuntu16.04系统为例, 使用USB连接BC26开发板到ubuntu, 在/dev/下可以看到连上来的开发板, 共有两个设备文件: /dev/ttyXRUSB0和/dev/ttyXRUSB1

使用minicom连接开发板, 按下开发板上的reset键后出现如下日志:

    $sudo minicom -D /dev/ttyXRUSB0 -b 115200
    F1: 0000 0000
    V0: 0000 0000 [0001]
    00: 0006 000C
    01: 0000 0000
    U0: 0000 0001 [0000]
    T0: 0000 00B4
    Leaving the BROM
现在便可以使用AT指令与开发板进行通信了

连接飞燕平台
---
在连接开发板后, 防止开发板进入睡眠状态:

    AT+SM=lock
    OK

确认当前是否连上NBIoT基站:

    AT+CGPADDR=1
    +CGPADDR: 1,100.106.219.61

配置MQTT连接参数:

    AT+QMTCFG="keepalive",0,60
    OK

    AT+QMTCFG="session",0,1
    OK

    AT+QMTCFG="timeout",0,10,1,0
    OK

配置三元组:

    AT+QMTCFG="aliauth",0,"a1X2bEnP82z","example_nbiot","DdBF53KirNVdxujCX8JbU4VT9hodSqer"
    OK

检查三元组是否正确配置:

    AT+QMTCFG="aliauth",0
    +QMTCFG: "aliauth","a1X2bEnP82z","example_nbiot","DdBF53KirNVdxujCX8JbU4VT9hodSqer"

连接飞燕平台(当下面第一条命令执行成功后, 需要快速执行第二条指令):

    AT+QMTOPEN=0,"a1X2bEnP82z.iot-as-mqtt.cn-shanghai.aliyuncs.com",1883
    OK
    +QMTOPEN: 0,0

    AT+QMTCONN=0,"example_nbiot"
    OK
    +QMTCONN: 0,0,0

查询当前连接状态:

    AT+QMTOPEN?
    +QMTOPEN: 0,"a1X2bEnP82z.iot-as-mqtt.cn-shanghai.aliyuncs.com",1883
    OK

当连上飞燕平台后, 就可以开始进行属性设置/属性上报和时间上报等操作了

设置属性
---
先订阅设置属性所需的topic:

    AT+QMTSUB=0,1,"/sys/a1X2bEnP82z/example_nbiot/thing/service/property/set",1
    OK
    +QMTSUB: 0,1,0,1

订阅成功后, 在飞燕控制台进行设置属性的操作:
![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/NBIoT-%E8%AE%BE%E7%BD%AE%E5%B1%9E%E6%80%A7.png)

当开发板收到属性设置指令后, 会打印如下日志:

    +QMTRECV: 0,0,"/sys/a1X2bEnP82z/example_nbiot/thing/service/property/set","{"method":"thing.service.property.set","id":"160500006","params":{"LightSwitch":0},"version":"1.0.0"}"

这样, 一次属性设置操作就成功了

上报属性
---
先订阅上报属性的云端应答所需的topic:

    AT+QMTSUB=0,1,"/sys/a1X2bEnP82z/example_nbiot/thing/event/property/post_reply",1
    OK
    +QMTSUB: 0,1,0,1

在本示例中使用的BC26开发板, 其AT指令具有一定的特殊性, 需要将发送内容中的双引号改为单引号进行输入:

    AT+QMTPUB=0,1,1,0,"/sys/a1X2bEnP82z/example_nbiot/thing/event/property/post","{'id':'1','version':'1.0','params':{'LightSwitch':1},'method':'thing.event.property.post'}"
    OK

    +QMTPUB: 0,1,0

    +QMTRECV: 0,0,"/sys/a1X2bEnP82z/example_nbiot/thing/event/property/post_reply","{"code":200,"data":{},"id":"1","message":"success","method":"thing.event.property.post","version":"1.0"}"

从上述日志可以看到, 向云端上报属性后, 收到了上报成功的应答. 这时候在云端获取属性会得到刚才上报的值:
![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/NBIoT-%E4%B8%8A%E6%8A%A5%E5%B1%9E%E6%80%A7.png)

上报事件
---
先订阅上报事件的云端应答所需的topic:

    AT+QMTSUB=0,1,"/sys/a1X2bEnP82z/example_nbiot/thing/event/Error/post_reply",1
    OK
    +QMTSUB: 0,1,0,1

上报一次Error事件:

    AT+QMTPUB=0,1,1,0,"/sys/a1X2bEnP82z/example_nbiot/thing/event/Error/post","{'id':'2','version':'1.0','params':{'ErrorCode':0},'method':'thing.event.Error.post'}"

    +QMTPUB: 0,1,0

    +QMTRECV: 0,0,"/sys/a1X2bEnP82z/example_nbiot/thing/event/Error/post_reply","{"code":200,"data":{},"id":"2","message":"success","method":"thing.event.Error.post","version":"1.0"}"

从上述日志可以看到, 向云端上报事件后, 收到了上报成功的应答. 这时候在云端可以查询到刚才的事件上报:
![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/NBIoT-%E4%B8%8A%E6%8A%A5%E4%BA%8B%E4%BB%B6.png)
