# <a name="目录">目录</a>
+ [零配模式WiFi配网的对接自查](#零配模式WiFi配网的对接自查)
    * [知识背景](#知识背景)
        - [WiFi帧通用格式](#WiFi帧通用格式)
        - [WiFi管理帧格式](#WiFi管理帧格式)
    * [Wireshark 抓包方法](#Wireshark 抓包方法)
    * [零配请求帧发送自查](#零配请求帧发送自查)
    * [零配应答帧接收自查](#零配应答帧接收自查)

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
*零配中的待配网设备只需实现**1种管理帧(Probe Request)**的发送和接收的能力, 即可对接完成*

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
| `1000`          | `00`        | Beacon                  | 信标
| `1010`          | `00`        | Disassociation          | 断开关联

> 所以对接零配只需要用到表格中的 `Probe Request`帧即可, 即探测请求帧, 既用来探测WiFi连接信息, 也用来获取主配设备传递过来的WiFi连接信息

### <a name="WiFi管理帧格式">WiFi管理帧格式</a>

零配用到的都是WiFi管理帧, 所有的WiFi管理帧都符合如下的帧格式

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/80211_mgmt_frame.png" width="800" height="240" />

探测请求帧格式
---
探测请求帧即 `Probe Request` 帧, 在零配中

+ 它由待配网设备广播发送, 已入网的主配设备接收时, **待配网设备用这种方式向周围的设备广播表达自己需要被配网**
+ 它有已入网的主配设备单播发送, 待配网设备接收时, **已入网设备用这种方式向待配网设备单播表达当前SSID和密码这样的连接信息**

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
配网的信息将在 `Vendor Specific` 这个字段里面传播, **注意: 该字段包含设备名称等信息, 因此不能硬编码实现**

## <a name="Wireshark 抓包方法">Wireshark 抓包方法</a>

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
0xdd, 0x45, 0xd8, 0x96, 0xe0, 0xaa, /* vendor specific start - 到这里为止报文内容是固定的, 需和示例完全相同 */
0x01, 0x0a, 0x74, 0x74, 0x5f, 0x74, 0x65, /* vendor specific continue - 从这里开始内容是变化的, 不必和示例相同 */
0x73, 0x74, 0x5f, 0x30, 0x31, 0x00, 0x0b, 0x61, 0x31, 0x58, 0x32, 0x62, 0x45,
0x6e, 0x50, 0x38, 0x32, 0x7a, 0x10, 0xf4, 0xe5, 0x2c, 0x43, 0x31, 0x62, 0xfc,
0x21, 0x74, 0x2f, 0x32, 0x04, 0xb0, 0xb3, 0xf5, 0xda, 0x04, 0x00, 0x14, 0x9f,
0xc5, 0x25, 0x7f, 0xf6, 0xec, 0xe0, 0xa3, 0xfe, 0xbc, 0x0d, 0x3e, 0x26, 0x9e,
0x61, 0x42, 0xb2, 0x05, 0xc5, 0x34, /* vendor specific end */
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

> `HAL_Awss_Open_Monitor()` 的参数是一个函数指针, 该指针指向的函数应当在 WiFi 网卡嗅探到 WiFi 帧时被调用到, SDK在其中解析应答类型的 `Probe Request` 报文得到SSID和密码

---
待配网设备收到的探测应答帧的格式与探测请求帧的帧格式相同, 因为它们同属 `Probe Request` 的管理帧, 但 `Vendor Specific` 字段内容有区别

+ 在这个字段的第6个字节, 零配应答帧的数据为 `0xab`,  而零配请求帧中为 `0xaa`
+ 探测应答帧的 `Vendor Specific` 字段包含了 SSID 和密码

可检查回调函数被调用时的二进制报文打印符合如下内容:
---
```
0x40, 0x00, /* management type, in frame control */
0x00, 0x00, /* duration */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* DA */
0xb0, 0xf8, 0x93, 0x10, 0x58, 0x35, /* SA, to be replaced with wifi mac */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* BSSID */
0xc0, 0x79, /* seq */
0x00, 0x00, /* hidden ssid */
0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x8c, 0x92, 0x98, 0xa4, /* supported rates */
0x32, 0x04, 0xb0, 0x48, 0x60, 0x6c, /* extended supported rates */
0xdd, 0x37, 0xd8, 0x96, 0xe0, 0xab, /* vendor specific start - 到这里为止报文内容是固定的, 需和示例完全相同 */
0x01, 0x14, 0x1a, 0x48, 0xf6, 0x66, /* vendor specific continue - 从这里开始内容是变化的, 不必和示例相同 */
0x5a, 0x1a, 0x6d, 0xf1, 0x58, 0xc5, 0xb3, 0xef, 0x7e, 0xf0, 0xef, 0xeb,
0x39, 0x78, 0x28, 0xee, 0x01, 0x0c, 0x6c, 0x69, 0x6e, 0x6b, 0x6b, 0x69,
0x74, 0x2d, 0x74, 0x65, 0x73, 0x74, 0x08, 0xc8, 0xfb, 0xc4, 0xe9, 0x47,
0x3e, 0x81, 0x1e, 0xcc, 0x08, 0xfb, 0x7c, 0xcb, 0x10, /* vendor specific end */
0x3f, 0x84, 0x10, 0x9e /* FCS */
```

可通过 WireShark 软件抓包检查设备收到的内容符合以下样本:
---
[下载正确接收的 Probe Request 帧样本](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/zero_config/mk3060_tt_test_01_send_probe_request.pcapng)

筛选这种帧的过滤条件为

    (wlan.da == ff:ff:ff:ff:ff:ff) && (wlan.tag.vendor.oui.type == 171)

得到样本展示的截图如下

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/zero_config/demo_rx_updated.jpg" width="1000" height="500" />

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
+ 在适当的地方调用 `verify_raw_frame()` 函数, 确保待配网设备发出了Probe Request.
+ 在适当的地方调用 `verify_rx_mng_raw_frame()` 函数, 该函数调用 `HAL_Awss_Open_Monitor` 进入监听模式, 监听主配设备是否对零配请求进行回复
+ 看设备端日志, 如果出现了 `verify got zeroconfig reply` 这样的日志, 说明成功收到了主配设备回复的零配应答报文
