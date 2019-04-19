# <a name="目录">目录</a>
+ [方案说明](#方案说明)
+ [如何配置使用这种配网模式](#如何配置使用这种配网模式)
+ [API使用范例](#API使用范例)
+ [API接口说明](#API接口说明)
    * [应用场景](#应用场景)
+ [需要对接的HAL接口](#需要对接的HAL接口)
+ [零配模式WiFi配网的对接自查](#零配模式WiFi配网的对接自查)
    * [知识背景](#知识背景)
        - [WiFi帧通用格式](#WiFi帧通用格式)
        - [WiFi管理帧格式](#WiFi管理帧格式)
    * [Wireshark 抓包方法](#Wireshark 抓包方法)
    * [零配请求帧发送自查](#零配请求帧发送自查)
    * [零配应答帧接收自查](#零配应答帧接收自查)

# <a name="方案说明">方案说明</a>

> 一键配网和手机热点配网都需要用户输入一到两个热点的SSID/密码, 输入时容易出错

零配是一个不需要用户输入热点信息的配网方案, 它是让一个已连接到上网热点的设备将热点的SSID/密码发送给待配网的设备

工作原理如下所示
---

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/brief_awss_zeroconfig.png" width="800" height="540" />

从上面的步骤可以看出, 用户不需要输入上网热点的SSID/密码, 只是选择需要配网的设备并确定即可, 因此用户体验会比较好, 但需要网络中存在一个支持零配的已配网的联网设备

*注: 天猫精灵音箱支持通过零配对WiFi设备进行配网, 因此如果希望设备可以被天猫精灵配网, 设备上需要集成零配方案*

# <a name="如何配置使用这种配网模式">如何配置使用这种配网模式</a>


通过menuconfig选择配网模块并选则零配:
```
make distclean

make menuconfig
```

使能配网:

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/6.png" width="600"  />

选择零配:

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/10.png"  width="800" />

注:
---
+ 零配一般作为辅助配网, 即同时还需要选择一种其他配网方式如一键配

# <a name="API使用范例">API使用范例</a>

下面是设备上电后的配网代码示例:

```
uint8_t bssid[ETH_ALEN] = {0};
char ssid[HAL_MAX_SSID_LEN] = {0};
char passwd[HAL_MAX_PASSWORD_LEN] = {0};
// Users need to get the stord ssid/passwd, to decide whether to start wifi provisioning service
if (INVALID_SSID(ssid) || INVALID_BSSID(bssid) ) {
    awss_start();
} else {
    HAL_Awss_Connect_Ap(TIMEOUT_MS, ssid, passwd, 0, 0, bssid, 0);
}
// Note:
//   1. Device wouldn't parse awss packet until application calls awss_config_press
//   2. Application can detect butturn press or the others method of user touch
//      to call the operation of calling awss_config_press.
```

上面代码演示了上电后根据设备内是否存储了ssid/password来判断是否需要启动配网. 启动配网后出于安全考虑并不会直接解awss包进行配网, 当真正需要配网时, 需要调用awss_config_press接口使能配网:
```
    extern int awss_config_press();
    awss_config_press();

```

配网过程状态监控
如果应用程序需要感知配网过程中的状态变化, 可以注册事件回调函数, 当相关事件发生时, 配网模块会调用回调函数告知具体的事件, 设备商可以在相关事件中操作LED指示灯的亮或灭, 或者通过快闪/慢闪来告知用户状态的变化, 也可以播放声音来告知用户状态

参考示例如下:

```
// 注册事件回调函数, 注册配网事件回调一定要需在awss_start之前调用
iotx_event_regist_cb(linkkit_event_monitor);
// linkkit_event_monitor是事件回调函数参考事件:
static void linkkit_event_monitor(int event)
{
    switch (event) {
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
             // operate led to indicate user
            LOG("IOTX_AWSS_START");
            break;
        case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
            LOG("IOTX_AWSS_ENABLE");
            // operate led to indicate user
            break;
        case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
            LOG("IOTX_AWSS_LOCK_CHAN");
            // operate led to indicate user
            break;
        case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
            LOG("IOTX_AWSS_PASSWD_ERR");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_SSID_PASSWD:
            LOG("IOTX_AWSS_GOT_SSID_PASSWD");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
                                     // discover, router solution)
            LOG("IOTX_AWSS_CONNECT_ADHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
            LOG("IOTX_AWSS_CONNECT_ADHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
            LOG("IOTX_AWSS_CONNECT_AHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
            LOG("IOTX_AWSS_CONNECT_AHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
                                     // (AP and router solution)
            LOG("IOTX_AWSS_SETUP_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
            LOG("IOTX_AWSS_CONNECT_ROUTER");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
                                            // router.
            LOG("IOTX_AWSS_CONNECT_ROUTER_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
                               // ip address
            LOG("IOTX_AWSS_GOT_IP");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
                                   // sucess)
            LOG("IOTX_AWSS_SUC_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
                                    // support bind between user and device
            LOG("IOTX_AWSS_BIND_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
                                       // user needs to enable awss again to support get ssid & passwd of router
            LOG("IOTX_AWSS_ENALBE_TIMEOUT");
            // operate led to indicate user
            break;
         case IOTX_CONN_CLOUD: // Device try to connect cloud
            LOG("IOTX_CONN_CLOUD");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
                                   // net_sockets.h for error code
            LOG("IOTX_CONN_CLOUD_FAIL");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            LOG("IOTX_CONN_CLOUD_SUC");
            // operate led to indicate user
            break;
        case IOTX_RESET: // Linkkit reset success (just got reset response from
                         // cloud without any other operation)
            LOG("IOTX_RESET");
            // operate led to indicate user
            break;
        default:
            break;
    }
}

```

# <a name="API接口说明">API接口说明</a>

| 序号    | 函数名                                                                                                                                          | 说明
|---------|-------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------
| 1       | [awss_start](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_start)                 | 开启配网服务(设备热点配网除外)
| 2       | [awss_config_press](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_config_press)   | 使能配网, 开始解包
| 3       | [awss_stop](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_stop)                   | 关闭配网服务(设备热点配网除外)

## <a name="应用场景">应用场景</a>

int awss_start()
---
调用该API用于启动配网服务. 启动配网服务的场景包括:

设备首次上电, 设备上不存在热点SSID/密码.
用户希望设备重新进行配网
调用该配网服务后, 设备进入配网发现状态, 用于让网络中的其它设备发现自己的存在. 但是在这个状态中, 设备并不会接受来自手机或者其它设备推送的SSID/密码

设备上电时若已有SSID/密码, 直接调用HAL_Awss_Connect_Ap()去连接WiFi热点

int awss_config_press()
---

为了安全考虑, 设备上通常具备一个配网按键, 当该按键被按下时设备才会接受来自手机或者其它设备推送的SSID/密码, 设备商需要实现代码来感知配网按键被按下, 并调用本函数让设备接收手机或者其它设备推送的SSID/密码信息

该函数被调用之后, 只有在HAL_Awss_Get_Timeout_Interval_Ms()指定的时间间隔内会接收手机或者设备推送的SSID/密码

说明:
---

+ 若设备无法设计一个专门的配网按键, 可以通过长按(比如3秒)设备上的某个按键来表示允许接收WiFi热点信息.
+ 若设备希望设备上电就可以接受手机或者其它设备推送的WiFi热点信息, 那么可以调用awss_start()之后直接调用awss_config_press()

int awss_stop()
---

调用该API用于停止当前配网. 停止服务的场景包括:
+ 当前不需要配网, 希望停止配网服务
+ 希望通过其他途径配网, 切换配网(如设备热点配网, 第三方配网)

# <a name="需要对接的HAL接口">需要对接的HAL接口</a>
除wifi配网概述里面列出的通用HAL API需要对接外, 还需要对接以下HAL:

| 序号    | 函数名                                                                                                                                                                  | 说明
|---------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------
| 1       | [HAL_Wifi_Send_80211_Raw_Frame](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Wifi_Send_80211_Raw_Frame)   | 在当前信道(channel)上以基本数据速率(1Mbps)发送裸的802.11帧(raw 802.11 frame)
| 2       | [HAL_Awss_Open_Monitor](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Open_Monitor)                   | 设置Wi-Fi网卡工作在监听(Monitor)模式, 并在收到802.11帧的时候调用被传入的回调函数

# <a name="零配模式WiFi配网的对接自查">零配模式WiFi配网的对接自查</a>

+ 在零配中设备可能充当**主配设备**, 在自身已连接 Internet 的情况下, 帮助尚未获得 SSID 和 WiFi 密码的其它设备接入网络
+ 在零配中设备也可能充当**待配设备**, 由于不知道当前可以连接的WiFI热点SSID和密码, 需要其它设备告知才可连接网络
+ 大多数集成 `Link Kit C-SDK` 的设备都是为了自身可以充当**待配设备**. 例如, 希望能通过天猫精灵找队友的功能连接上网, 此时天猫精灵是主配设备, 设备自身是待配设备

---
下面介绍作为**待配设备**需要对接的HAL函数接口, 以及对应的背景知识和对接自查方法

## <a name="知识背景">知识背景</a>

WiFi帧可以分为3种

+ 控制帧通常与数据帧搭配使用, 负责区域清空, 信道获得, 载波侦听, 肯定确认等
+ 数据帧负责搬运数据
+ 管理帧负责监督, 如加入和退出无线网络以及处理接入点之间关联的转移等

---
*零配中的待配网设备只需实现**1种管理帧(Probe Request)**的发送, 和监听WiFi报文的能力, 即可对接完成*

### <a name="WiFi帧通用格式">WiFi帧通用格式</a>

无论是控制帧, 数据帧还是管理帧, 所有的WiFi报文都符合如下的帧格式

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/80211_generic_frame.png" width="1000" height="200" />

其中前2个字节的 `Frame Control` 字段区分了帧的类型

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/80211_frame_control.png" width="1000" height="300" />

`Frame Control` 字段下的子字段中
---
+ **Protocol子字段**: 2bit, 表示帧的MAC版本. 目前, 802.11 MAC只有1个版本, 其协议编号为0
+ **Type+SubType子字段**: 2bit+4bit, 详见下表, 注意表格以MSB描述, Type子字段的值依次对应b3, b2, 而Subtype子字段为b7, b6, b5, b4, 所以和上图正好顺序相反

| SubType子字段   | Type子字段  | 帧类型                  | 中文名
|-----------------|-------------|-------------------------|-----------------
| `0000`          | `00`        | Association Request     | 关联请求
| `0001`          | `00`        | Association Response    | 关联应答
| **`0100`**      | **`00`**    | **Probe Request**       | **探测请求**
| **`0101`**      | **`00`**    | **Probe Resonse**       | **探测响应**
| `1000`          | `00`        | Beacon                  | 信标
| `1010`          | `00`        | Disassociation          | 断开关联

> 所以对接零配只需要用到表格中的 `Probe Request`和`Probe Respose`帧即可, 即探测请求帧和探测响应帧, 既用来探测WiFi连接信息, 也用来获取主配设备传递过来的WiFi连接信息

### <a name="WiFi管理帧格式">WiFi管理帧格式</a>

零配用到的都是WiFi管理帧, 所有的WiFi管理帧都符合如下的帧格式

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/80211_mgmt_frame.png" width="800" height="200" />

探测请求帧格式
---
探测请求帧即 `Probe Request` 帧, 在零配中

+ 它由待配网设备广播发送, 已入网的主配设备接收时, **待配网设备用这种方式向周围的设备广播表达自己需要被配网**

| 字段名                          | 描述                                                                | 长度(字节)
|---------------------------------|---------------------------------------------------------------------|-------------
| **Frame Control(帧控制)字段**   | **描述与控制MAC帧相关信息**                                         | 2
| Duration字段                    | 计算帧持续时间的作用                                                | 2
| Destination Address             | MAC帧的目的地址                                                     | 6
| Source Address                  | MAC帧的源地址                                                       | 6
| BSSID(基本服务集ID)             | 用于过滤收到的MAC帧(在基础型网络里为工作站所关联的接入点的MAC地址)  | 6
| Sequence Control(顺序控制字段)  | 用来重组帧片段以及丢弃重复帧                                        | 2
| SSID                            | SSID                                                                | 2-34
| Supported Rates                 | Supported rates                                                     | 3-10
| Extended Supported Rates        | Extended Supported Rates                                            | 3-257
| **Vendor Specific**             | **用户定义的信息元素**                                              | 自定义
| FCS(帧校验序列)                 | 验证传来的帧是否有误                                                | 4

表中4个字段都属于上图中的 `Information Element` 信息元素

+ `Supported Rates`
+ `Extended Supported Rates`
+ `SSID`
+ `Vendor Specific`

---
配网的信息将在 `Vendor Specific` 这个字段里面传播, 其内部构造如下, **注意: 该字段包含设备名称等信息, 因此不能硬编码实现**

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/80211_vendor_ie.png" width="800" height="200" />

---
探测响应帧即 `Probe Response` 帧, 在零配中

+ 它由已入网的主配设备单播发送, 待配网设备接收, **已入网设备用这种方式向待配网设备单播表达当前SSID和密码这样的连接信息**
+ 它的帧格式与 `Probe Request` 非常相似, 也包含Vendor Specific字段, 这里不再展开

## <a name="Wireshark 抓包方法">Wireshark 抓包方法</a>

> **注: Windows系统会过滤一些 WiFi 报文不传递给自己的应用程序, 导致 WireShark 软件抓不到空中包, 所以一般以 mac 系统抓包**

可以使用 Wireshark 软件对空气中的WiFi帧进行抓取来帮助自查, 以验证待配网设备是否发出正确的探测请求帧, 以及主配设备是否进行过回复, 步骤如下

在mac电脑安装 Wireshark 软件
---

点击齿轮状的 Capture Options (图中的红框)进入设置, 参考下图

<br>
<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/zero_config/wireshark_setting_1.jpg" width="800" height="400" />

配置 Wireshark 软件为抓包模式
---

反向选择其他所有Interface, 仅仅保留wifi interface, 并且勾选其中的 Promiscuous 和 Monitor 两种抓包模式, 具体见下图

<br>
<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/80211_wireshark_config.png" width="800" height="500" />

## <a name="零配请求帧发送自查">零配请求帧发送自查</a>

SDK需要用户自行实现 [HAL_Wifi_Send_80211_Raw_Frame](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Wifi_Send_80211_Raw_Frame) 接口来进行零配请求帧的发送, **SDK通过发送这种帧寻找主配设备**

> `HAL_Wifi_Send_80211_Raw_Frame()` 被调用时, 应当向空气中广播发出探测请求帧, 用作零配请求

可检查它组装和发送的的报文二进制打印符合如下内容:
---
```
0x40, 0x00, /* management type, in frame control */
0x00, 0x00, /* duration */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* DA */
0xb0, 0xf8, 0x93, 0x10, 0x58, 0x1f, /* SA, to be replaced with wifi mac */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* BSSID */
0xc0, 0x79, /* seq */
0x00, 0x00, /* hidden ssid */
0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x8c, 0x92, 0x98, 0xa4, /* supported rates */
0x32, 0x04, 0xb0, 0x48, 0x60, 0x6c, /* extended supported rates */
0xdd, 0x45, 0xd8, 0x96, 0xe0, 0xaa, /* vendor specific start - 这一行除了第二个字节(表示长度), 其他几个字段要和本例子相同 */
0x01, 0x0a, 0x74, 0x74, 0x5f, 0x74, 0x65, /* vendor specific continue - 从这里开始内容是变化的, 不必和示例相同 */
0x73, 0x74, 0x5f, 0x30, 0x31, 0x00, 0x0b, 0x61, 0x31, 0x58, 0x32, 0x62, 0x45, 0x6e, 0x50, 0x38,
0x32, 0x7a, 0x10, 0xf4, 0xe5, 0x2c, 0x43, 0x31, 0x62, 0xfc, 0x21, 0x74, 0x2f, 0x32, 0x04, 0xb0,
0xb3, 0xf5, 0xda, 0x04, 0x00, 0x14, 0x9f, 0xc5, 0x25, 0x7f, 0xf6, 0xec, 0xe0, 0xa3, 0xfe, 0xbc,
0x0d, 0x3e, 0x26, 0x9e, 0x61, 0x42, 0xb2, 0x05, 0xc5, 0x34, /* vendor specific end */
0x3f, 0x84, 0x10, 0x9e /* FCS */
```

可通过 WireShark 软件抓包检查设备发送的内容符合以下样本:
---
[下载正确发送的 Probe Request 帧样本](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/zero_config/mk3060_tt_test_01_send_probe_request.pcapng)

筛选这种帧的过滤条件为

    (wlan.da == ff:ff:ff:ff:ff:ff) && (wlan.tag.vendor.oui.type == 170)

得到样本展示的截图如下

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/zero_config/demo_tx.jpg" width="1000" height="500" />

可通过测试工具程序自查
---
为了验证当前当前待配网设备是否有能力发出零配请求所需的探测请求帧, 可依次执行以下步骤

+ 实现 `HAL_Wifi_Send_80211_Raw_Frame()`, 检查其报文打印是否符合上面的二进制数组
+ 下载测试工具源码: [TestProbeTx.c](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/TestProbeTx.c)
+ 根据实际情况将程序中的 `local_mac` 替换为自己板子的 mac 地址
+ 周期性地调用测试程序中的 `verify_raw_frame` 函数
+ 通过 WireShark 软件抓包看是否抓到探测请求帧, 以及抓到的报文按上述说明过滤后, 是否符合上文中的样本截图

## <a name="零配应答帧接收自查">零配应答帧接收自查</a>

SDK需要用户自行实现 [HAL_Awss_Open_Monitor](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Open_Monitor) 接口来进行零配应答帧的接收, **SDK通过回调函数得到的报文解析SSID和密码**

> `HAL_Awss_Open_Monitor()` 的参数是一个函数指针, 该指针指向的函数应当在 WiFi 网卡嗅探到 WiFi 帧时被调用到, SDK在其中解析应答类型的 `Probe Response` 报文得到SSID和密码

---
待配网设备收到的探测应答帧的格式与探测请求帧的帧格式类似, 它属于 `Probe Response` 的管理帧, 其中 `Vendor Specific` 字段内容也有区别

+ 在这个字段的第6个字节, 零配应答帧的数据为 `0xab`,  而零配请求帧中为 `0xaa`
+ 探测应答帧的 `Vendor Specific` 字段包含了 SSID 和密码

可检查回调函数被调用时的二进制报文打印符合如下内容:
---

```
0x50, 0x00, /* management type, in frame control */
0x3a, 0x01, /* duration */
0xb0, 0xf8, 0x93, 0x10, 0x58, 0x24, /* DA */
0x78, 0xda, 0x07, 0x6d, 0x05, 0xe1, /* SA, to be replaced with wifi mac */
0x40, 0x31, 0x3c, 0x05, 0xb1, 0x89, /* BSSID */
0xb0, 0xea, /* sequence num */
0xc8, 0x24, 0xe8, 0x6f, 0x5d, 0x01, 0x00, 0x00, 0x64, 0x00, 0x01, 0x00, /* Fixed parameters */
0x00, 0x0b, 0x74, 0x6d, 0x61, 0x6c, 0x6c, 0x5f, 0x67, 0x65, 0x6e, 0x69, 0x65,   /* ssid parameter set */
0x01, 0x04, 0x02, 0x04, 0x0b, 0x16, /* supported rate */
0xdd, 0x35, 0xd8, 0x96, 0xe0, 0xab, /* vendor specific start - 这一行除了第二个字节(表示长度), 其他几个字段要和本例子相同 */
0x01, 0x14, 0xb0, 0xc8, 0xfa, 0xc2, 0xb7,   /* vendor specific continue - 从这里开始内容是变化的, 不必和示例相同 */
0xbf, 0x04, 0xe2, 0x30, 0x10, 0x6a, 0x17, 0x9d, 0x54, 0x55, 0xc4, 0xec, 0x73, 0xb0, 0x4e, 
0x01, 0x08, 0x69, 0x70, 0x63, 0x5f, 0x64, 0x65, 0x6d, 0x6f, 0x0a, 0xb9, 0xd0, 0x31, 0xc6, 
0xd6, 0x19, 0x6c, 0x53, 0xb7, 0x7a, 0x40, 0x31,    /* vendor specific end */
0x3c, 0x05, 0xb1, 0x89 /* FCS */
```

可通过 WireShark 软件抓包检查设备收到的内容符合以下样本:
---
[下载正确接收的 Probe Response 帧样本](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/zero_config/tianmaojingling_response.pcapng)

筛选这种帧的过滤条件为

    (wlan.tag.vendor.oui.type == 171)

得到样本展示的截图如下

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/zero_config/rx_%E5%A4%A9%E7%8C%AB%E7%B2%BE%E7%81%B5.jpg" width="800" height="500" />

可通过测试工具程序自查
---
+ 实现 `HAL_Awss_Open_Monitor()`, 确保设备进入监听模式
+ 下载测试工具源码
    + [TestProbeRx.c](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/TestProbeRx.c)
    + [zconfig_ieee80211.c](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/zconfig_ieee80211.c)
    + [zconfig_ieee80211.h](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/zconfig_ieee80211.h)
    + [zconfig_protocol.h](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/zconfig_protocol.h)
    + [zconfig_utils.h](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/zconfig_utils.h)
+ 将上述文件和待测设备的其他代码一起编译
+ 在适当的地方调用 `verify_raw_frame()` 函数, 确保待配网设备发出了符合上面要求的零配请求帧
+ 在适当的地方调用 `verify_rx_mng_raw_frame()` 函数, 该函数调用 `HAL_Awss_Open_Monitor` 进入监听模式, 监听主配设备是否对零配请求回复
+ 看设备端日志, 如果出现了 `verify got zeroconfig reply` 这样的日志, 说明成功收到了主配设备回复的零配应答报文

