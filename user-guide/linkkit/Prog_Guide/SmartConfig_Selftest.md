# <a name="目录">目录</a>
+ [知识背景](#知识背景)
    * [监听模式](#监听模式)
    * [底层原理](#底层原理)
+ [用手机app发送一键配网信息流](#用手机app发送一键配网信息流)
    * [在手机上安装配网app](#在手机上安装配网app)
    * [操作配网app发送报文](#操作配网app发送报文)
+ [用自测工具和设备接收信息流](#用自测工具和设备接收信息流)

# <a name="知识背景">知识背景</a>

一键配网的原理十分简单, 由于WiFi报文传播的物理介质是无处不在的空气, 所以

+ 手机app向空中加密发送热点的SSID和密码
+ 设备在监听模式即可接收和上报给SDK
+ SDK解密到SSID和密码后, 作为入参调用 `HAL_Awss_Connect_Ap()` 接口, 设备就能连接上热点

## <a name="监听模式">监听模式</a>

所谓监听模式, 就是SDK希望通过 `HAL_Awss_Open_Monitor(cb)` 调用, 使设备WiFi芯片进入如下的工作模式

+ 对于组播(Multicast)的WiFi报文, 不论其是否属于当前设备加入的组播组, 都会接收并通过回调函数 `cb` 传递给SDK
+ 对于广播(Broadcast)的WiFi报文, 不论来源如何, 都会接收并通过回调函数 `cb` 传递给SDK
+ 对于单播(Unicast)的WiFi报文, 不论其目的 mac 地址是否为自己的 mac 地址, 都会接收并通过回调函数 `cb` 传递给SDK

## <a name="底层原理">底层原理</a>

在监听模式下, WiFi报文的长度及其携带的组播地址都可以用来承载SSID和密码这样的信息

包长编码
---
> 无论是控制帧, 数据帧还是管理帧, 所有的WiFi报文都符合如下的帧格式

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/80211_generic_frame.png" width="1000" height="200" />

*注意: 802.11的MAC帧并未包含Ethernet帧的某些典型功能, 最明显的是Type/Length字段, 这并不像Ethernet帧一般出现在802.11的MAC帧头中*

---
+ 虽然空中传播的WiFi帧并不含有1个实际的长度字段, 但WiFi网卡接收WiFi帧时知道它的长度, 可以传递给SDK
+ 一个WiFi帧的长度不超过 `2312` 字节, 可用 `11` 个比特位表示绝大部分长度, **所以可使用这11个比特位编码和传递连接信息**

组播地址编码
---
> 组播IP地址和目的 mac 地址存在如下的对应关系

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mcast_addr_mapping.jpg" width="900" height="240" />

---
+ 监听模式下设备底层应将感知到的所有组播报文, 无论如何传递给SDK
+ 目的 mac 地址的 `48` 个比特位中, 后 `23` 个是手机发送时可控制, 设备也可接收到的, **所以可使用这23个比特位编码和传递连接信息**

# <a name="用手机app发送一键配网信息流">用手机app发送一键配网信息流</a>
为确认一键配网的相关HAL接口实现正确, 需用手机app向空中发送配网信息流, 通过设备是否可从空中正常接收到相应信息来判断

## <a name="在手机上安装配网app">在手机上安装配网app</a>

+ 公版app的下载说明: https://living.aliyun.com/doc#muti-app.html
+ 也可以通过扫描下面的二维码下载安装
+ 如果是已发布的产品, 且使用的是设备厂商开发的APP, 那么请向设备厂商索取APP

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/download_ilop_app.png" width="700" height="400" />

## <a name="操作配网app发送报文">操作配网app发送报文</a>

+ 手机连接上期望设备将要连接到的热点, 确保热点和设备距离足够近并无阻挡
+ 用二维码生成工具, 将以下文本转换成二维码

        http://www.taobao.com?pk=a1X2bEnP82z&dn=test_01

+ 打开配网app, 扫描该二维码后, 输入热点连接密码, 点击**"下一步"**按钮和**"我确认在闪烁"**按钮, 发起配网

<br>
<div align=center>
<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/app_awss_steps.gif" width="280" height="500" align="center" />
</div>

# <a name="用自测工具和设备接收信息流">用自测工具和设备接收信息流</a>
