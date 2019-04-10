# <a name="目录">目录</a>
+ [方案说明](#方案说明)
+ [如何配置使用这种配网模式](#如何配置使用这种配网模式)
+ [API使用范例](#API使用范例)
+ [API接口说明](#API接口说明)
    * [应用场景](#应用场景)
+ [需要对接的HAL接口](#需要对接的HAL接口)
+ [一键配网模式的对接自查](#一键配网模式的对接自查)
    * [知识背景](#知识背景)
        - [监听模式](#监听模式)
        - [底层原理](#底层原理)
    * [用手机app发送一键配网信息流](#用手机app发送一键配网信息流)
        - [在手机上安装配网app](#在手机上安装配网app)
        - [操作配网app发送报文](#操作配网app发送报文)
    * [用自测工具和设备接收信息流](#用自测工具和设备接收信息流)
        - [自测工具的获取](#自测工具的获取)
        - [自测工具的使用](#自测工具的使用)
        - [自测函数及测试方法详解](#自测函数及测试方法详解)

# <a name="方案说明">方案说明</a>

一键配网方案中, 手机将WiFi热点的SSID/密码通过特殊编码方法在WiFi的数据或管理帧上进行广播发送

设备可监听WiFi数据帧或管理帧来将SSID/密码进行解码, 然后用获取到的SSID/密码去连接WiFi热点

工作原理如下所示
---

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/brief_awss_smartconfig.png" width="800" height="240" />

说明
---
+ 手机连接到WiFi热点上, 然后在固定的信道上广播发送SSID/密码的编码字节
+ 设备轮询所有的WiFi信道去检测是否有配网通告, 如果检测到则从配网通告中解码出WiFi热点的SSID/密码

*注: IoT设备大多数只支持2.4GHz频段, 所以目前的一键配网方案设备端实现只是在2.4GHz的1~13信道上进行配网帧检测*

# <a name="如何配置使用这种配网模式">如何配置使用这种配网模式</a>

通过menuconfig选择配网模块并选定一键配网:
```
make distclean

make menuconfig
```

使能配网:

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/6.png" width="600"  />

选择一键配网:

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/8-1.png"  width="800" />


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

| 序号    | 函数名                                                                                                                                              | 说明
|---------|-------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------
| 1       | [awss_start](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_start)                     | 开启配网服务(设备热点配网除外)
| 2       | [awss_config_press](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_config_press)       | 使能配网, 开始解包
| 3       | [awss_stop](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_stop)                   | 关闭配网服务(设备热点配网除外)

## <a name="应用场景">应用场景</a>

awss_start
---
调用该API用于启动配网服务. 启动配网服务的场景包括:

设备首次上电, 设备上不存在热点SSID/密码.
用户希望设备重新进行配网
调用该配网服务后, 设备进入配网发现状态, 用于让网络中的其它设备发现自己的存在. 但是在这个状态中, 设备并不会接受来自手机或者其它设备推送的SSID/密码

设备上电时若已有SSID/密码, 直接调用HAL_Awss_Connect_Ap()去连接WiFi热点

awss_config_press
---

为了安全考虑, 设备上通常具备一个配网按键, 当该按键被按下时设备才会接受来自手机或者其它设备推送的SSID/密码, 设备商需要实现代码来感知配网按键被按下, 并调用本函数让设备接收手机或者其它设备推送的SSID/密码信息

该函数被调用之后, 只有在HAL_Awss_Get_Timeout_Interval_Ms()指定的时间间隔内会接收手机或者设备推送的SSID/密码

说明:
---

+ 若设备无法设计一个专门的配网按键, 可以通过长按(比如3秒)设备上的某个按键来表示允许接收WiFi热点信息.
+ 若设备希望设备上电就可以接受手机或者其它设备推送的WiFi热点信息, 那么可以调用awss_start()之后直接调用awss_config_press()

awss_stop
---

调用该API用于停止当前配网. 停止服务的场景包括:
+ 当前不需要配网, 希望停止配网服务
+ 希望通过其他途径配网, 切换配网(如设备热点配网, 第三方配网)

# <a name="需要对接的HAL接口">需要对接的HAL接口</a>
除wifi配网概述里面列出的通用HAL API需要对接外, 还需要对接以下HAL:

+ 无

# <a name="一键配网模式的对接自查">一键配网模式的对接自查</a>

## <a name="知识背景">知识背景</a>

一键配网的原理十分简单, 由于WiFi报文传播的物理介质是无处不在的空气, 所以

+ 手机app向空中加密发送热点的SSID和密码
+ 设备在监听模式即可接收和上报给SDK
+ SDK解密到SSID和密码后, 作为入参调用 `HAL_Awss_Connect_Ap()` 接口, 设备就能连接上热点

### <a name="监听模式">监听模式</a>

所谓监听模式, 就是SDK希望通过 `HAL_Awss_Open_Monitor(cb)` 调用, 使设备WiFi芯片进入如下的工作模式

+ 对于组播(Multicast)的WiFi报文, 不论其是否属于当前设备加入的组播组, 都会接收并通过回调函数 `cb` 传递给SDK
+ 对于广播(Broadcast)的WiFi报文, 不论来源如何, 都会接收并通过回调函数 `cb` 传递给SDK
+ 对于单播(Unicast)的WiFi报文, 不论其目的 mac 地址是否为自己的 mac 地址, 都会接收并通过回调函数 `cb` 传递给SDK

### <a name="底层原理">底层原理</a>

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

## <a name="用手机app发送一键配网信息流">用手机app发送一键配网信息流</a>
为确认一键配网的相关HAL接口实现正确, 需用手机app向空中发送配网信息流, 通过设备是否可从空中正常接收到相应信息来判断

### <a name="在手机上安装配网app">在手机上安装配网app</a>

+ 公版app的下载说明: https://living.aliyun.com/doc#muti-app.html
+ 也可以通过扫描下面的二维码下载安装
+ 如果是已发布的产品, 且使用的是设备厂商开发的APP, 那么请向设备厂商索取APP

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/download_ilop_app.png" width="700" height="400" />

### <a name="操作配网app发送报文">操作配网app发送报文</a>

+ 手机连接上期望设备将要连接到的热点, 确保热点和设备距离足够近并无阻挡
+ 用二维码生成工具, 将以下文本转换成二维码

        http://www.taobao.com?pk=a1X2bEnP82z&dn=test_01

+ 打开配网app, 扫描该二维码后, 输入热点连接密码, 点击**"下一步"**按钮和**"我确认在闪烁"**按钮, 发起配网

<br>
<div align=center>
<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/app_awss_steps.gif" width="280" height="500" align="center" />
</div>

## <a name="用自测工具和设备接收信息流">用自测工具和设备接收信息流</a>

### <a name="自测工具的获取">自测工具的获取</a>

自测工具以源码形式提供, 需要用户下载它的C语言源文件后, 跟用户自己编写的 `HAL_XXX` 接口交叉编译到一起, 然后在待调试一键配网功能的开发板上运行使用

[下载地址](https://code.aliyun.com/edward.yangx/public-docs/raw/master/progs/smartconfig_hal_test.zip)

---
可以点击以上链接下载压缩包到本地之后, 用 `WinRAR`, `7zip` 等压缩工具将这个压缩包文件解压成一系列的C语言源文件

解压后的内容及说明如下:

    +-- include
    |   +-- hal_awss.h
    |   +-- hal_common.h
    |   +-- ieee80211.h
    |   +-- ieee80211_radiotap.h
    |   +-- smartconfig_ieee80211.h
    |   +-- zconfig_protocol.h
    +-- src
        +-- hal_awss.c
        +-- haltest.c
        +-- ieee80211.c
        +-- smartconfig_ieee80211.c
        +-- TestProbeRx.c
        +-- wrapper.c

| 文件名                  | 说明
|-------------------------|-----------------------------
| `include/hal_awss.h`    | 自测函数声明
| `include/hal_common.h`  | HAL接口声明
| `src/hal_awss.c`        | 自测函数实现
| `src/haltest.c`         | 自测函数调用示例
| **`src/wrapper.c`**     | **被测试的HAL接口实现**


### <a name="自测工具的使用">自测工具的使用</a>

自测工具的使用方式设计为
---
+ 用户将 `src/wrapper.c` 中所有 `HAL_XXX()` 的函数内容实现替换为适合自己系统的代码
    + 注意由 `#if 0 ... #endif` 包围的部分是我们提供的参考实现
    + 这些参考实现由于是在特定的嵌入式Linux开发板上调通, **不能直接在其他的Linux开发板甚至非Linux开发板上直接使用, 列出仅为示意**
+ 用户将替换后的 `src/wrapper.c` 和其它的源文件编译到自己的固件中
+ 用户可以浏览 `include/hal_awss.h` 文件, 其中列举的函数 `verfiy_xxx()` 称为 **"自测函数"**, 每个自测函数可以验证一到多个 `HAL_XXX()` 实现
+ 用户可以选择合适的程序入口, 调用自测函数 `verify_xxx()`, 如果不知道如何调用, 可以参考 `src/haltest.c`, 这个文件是我们在嵌入式Linux开发板上验证过的
+ 用户自行编译 **嵌入了自己HAL实现的自测工具全部源码**, 通过观察 `verify_xxx()` 这类函数的运行输出, 来检验自己的 `HAL_XXX()` 实现是否基本正确

> 下面是 `src/haltest.c` 中对自测函数进行调用的示意代码

    #include <stdio.h>
    #include <unistd.h>
    #include "hal_common.h"

    #define TEST_SWITCH_CHANNEL_INTERNVAL_MS (250)

    ...
    ...

    int main(int argc, char *argv[])
    {
        int time_passed = 0;
        uint8_t bssid[ETH_ALEN] = {0x11,0x22,0x33,0x44,0x55,0x66};

        verify_awss_preprocess();
        verify_awss_close_monitor();
        verify_awss_open_monitor();

        while(1) {
            if (time_passed > 10 *1000) {
                break;
            }
            verify_awss_switch_channel(switch_channel());
            usleep(TEST_SWITCH_CHANNEL_INTERNVAL_MS*1000);
            time_passed += TEST_SWITCH_CHANNEL_INTERNVAL_MS;
        }

        verfiy_awss_connect_ap(5000, "ssid", "passwd", AWSS_AUTH_TYPE_WPAPSKWPA2PSK, AWSS_ENC_TYPE_AES, bssid, 6);
        verify_awss_get_ap_info();
        verify_awss_net_is_ready();

        return 0;
    }

### <a name="自测函数及测试方法详解">自测函数及测试方法详解</a>

> 根据上一节的说明, 自测工具通过提供 **"自测函数"**, 触发自测动作, 并输出log帮助用户确认某些HAL接口实现的正确性

以下自测函数包含的HAL为 [WIFI配网概述](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/WiFi_Provision) 中的 [配网模块公共HAL](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/WiFi_Provision#%E9%85%8D%E7%BD%91%E6%A8%A1%E5%9D%97%E5%85%AC%E5%85%B1HAL)

---
*若使用一键配网模式, 仅需要对接这些公共HAL接口即可, 没有更多的HAL接口需要对接*

verify_awss_preprocess
---
+ 对 `HAL_Awss_Get_Timeout_Interval_Ms`, `HAL_Awss_Get_Channelscan_Interval_Ms` 和 `HAL_Wifi_Get_Mac` 进行自测
+ 这3个HAL返回的是一些配置参数, 用户需要确认这些参数的值符合自己填入的预期

> 输出日志例子如下

    /***********************************************/
    /*   Verify HAL_Awss_Get_Timeout_Interval_Ms   */
    /*   Awss Timeout Interval Ms:     600000 ms   */
    /***********************************************/

    /***********************************************/
    /* Verify HAL_Awss_Get_Channelscan_Interval_Ms */
    /* Awss Channel Scan Interval Ms:       250 ms */
    /***********************************************/

    /***********************************************/
    /*           Verify HAL_Wifi_Get_Mac           */
    /*         Wifi Mac: 11:22:33:44:55:66         */
    /***********************************************/

    /***********************************************/
    /*   Verify HAL_Awss_Switch_Channel In Loop    */
    /***********************************************/

verify_awss_open_monitor
---
+ 对 `HAL_Awss_Open_Monitor` 进行自测, 根据定义这将打开设备的监听模式, 开始抓取空中的WiFi报文, **所以运行的时候务必按照上文确保设备附近有运行配网app发送配网报文**
+ 输出日志为当前收到的配网模块帧类型的表格
+ 如果其中有 `SmartConfig Start Frame`, `SmartConfig Group Frame` 和 `SmartConfig  Data Frame` 字样, 说明当前监听模式可以接收到一键配网所需的帧类型

> 输出日志例子如下

    /***********************************************/
    /*        Verify HAL_Awss_Open_Monitor         */
    /***********************************************/

    |---------------------------------------------------------------------|
    |          Frame Type         | Direction |  Packet Length  | Channel |
    |-----------------------------|-----------|-----------------|---------|
    |   SmartConfig  Data Frame   |   FromDS  |  1256 (0x04E8)  |     8   |
    |   SmartConfig Start Frame   |   FromDS  |  1248 (0x04E0)  |     8   |
    |   SmartConfig  Data Frame   |   FromDS  |  1256 (0x04E8)  |     8   |
    |   SmartConfig  Data Frame   |   FromDS  |  0532 (0x0214)  |     9   |
    |   SmartConfig  Data Frame   |   FromDS  |  0288 (0x0120)  |     1   |
    |   SmartConfig  Data Frame   |   FromDS  |  0457 (0x01C9)  |     3   |
    |   SmartConfig Group Frame   |   FromDS  |  1002 (0x03EA)  |     4   |
    |   SmartConfig Group Frame   |   FromDS  |  0994 (0x03E2)  |     4   |
    |   SmartConfig  Data Frame   |   FromDS  |  0284 (0x011C)  |     4   |
    |   SmartConfig  Data Frame   |   FromDS  |  0421 (0x01A5)  |     4   |
    |   SmartConfig  Data Frame   |   FromDS  |  0413 (0x019D)  |     4   |

verify_awss_close_monitor
---
+ 对 `HAL_Awss_Close_Monitor` 进行自测, 观察是否可以按预期停止监听模式
+ 用户调用该函数后, 应该不再接收到任何配网帧类型, 上面的打印停止

> 输出日志例子如下

    /***********************************************/
    /*        Verify HAL_Awss_Close_Monitor        */
    /***********************************************/

verify_awss_switch_channel
---
+ 对 `HAL_Awss_Switch_Channel` 进行自测, 观察是否可以按预期切换到指定的WiFi信道(1-13)
+ 在监听模式打开的情况下, 不断切换wifi信道进行侦听, 可参考 `haltest.c`

verfiy_awss_connect_ap
---
+ 对 `HAL_Awss_Connect_Ap` 进行自测, 观察是否可以根据SSID和密码, 按预期连接到指定的AP热点上
+ 在调用该函数后, 应当可以成功连上指定AP

> 输出日志例子如下, `0`表示成功, `-1`表示失败

    /***********************************************/
    /*          Verify HAL_Awss_Connect_Ap         */
    /*                  Result:  0                 */
    /***********************************************/

verify_awss_get_ap_info
---
+ 对 `HAL_Wifi_Get_Ap_Info` 进行自测, 观察是否可以按预期收集AP热点的连接信息
+ 在调用该函数后, 会打印获取到的ssid, 密码和bssid

> 输出日志例子如下

    /***********************************************/
    /*         Verify HAL_Wifi_Get_Ap_Info         */
    /*                  Result:  0                 */
    /*           SSID:       test_ssid             */
    /*           PASSWD:     test_passwd           */
    /***********************************************/

verify_awss_net_is_ready
---
+ 对 `HAL_Sys_Net_Is_Ready` 进行自测, 观察是否可以按
+ 在调用该函数后, 会获取当前网络状态

> 输出日志例子如下

    /***********************************************/
    /*         Verify HAL_Sys_Net_Is_Ready         */
    /*                  Result:  0                 */
    /***********************************************/

