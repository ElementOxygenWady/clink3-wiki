# <a name="目录">目录</a>
+ [知识背景](#知识背景)
    * [监听模式](#监听模式)
    * [底层原理](#底层原理)
+ [用手机app发送一键配网信息流](#用手机app发送一键配网信息流)
    * [在手机上安装配网app](#在手机上安装配网app)
    * [操作配网app发送报文](#操作配网app发送报文)
+ [用自测工具和设备接收信息流](#用自测工具和设备接收信息流)
    * [自测工具的获取](#自测工具的获取)
    * [自测工具使用示例](#自测工具使用示例)
    * [自测工具的能力概览](#自测工具的能力概览)

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

## <a name="自测工具的获取">自测工具的获取</a>

自测工具以源码形式提供, 需要用户下载它的C语言源文件后, 跟用户自己编写的 `HAL_XXX` 接口交叉编译到一起, 然后在待调试一键配网功能的开发板上运行使用

[下载地址](https://code.aliyun.com/edward.yangx/public-docs/raw/master/progs/smartconfig_hal_test.zip)

---
可以点击以上链接下载压缩包到本地之后, 用 `WinRAR`, `7zip` 等压缩工具将这个压缩包文件解压成一系列的C语言源文件

解压后的目录如下：

    ├── include
    │   ├── hal_awss.h                   //自测函数声明
    │   ├── hal_common.h                 //HAL接口声明
    │   ├── ieee80211.h                  
    │   ├── ieee80211_radiotap.h
    │   ├── smartconfig_ieee80211.h
    │   └── zconfig_protocol.h
    └── src
        ├── hal_awss.c                   //自测函数实现
        ├── haltest.c                    //自测函数调用示例
        ├── ieee80211.c
        ├── smartconfig_ieee80211.c
        ├── TestProbeRx.c
        └── wrapper.c                    //HAL接口实现

所有自测函数声明在`hal_awss.h`中，用户需要自行实现`wrapper.c`中的HAL接口，然后参考`haltest.c`的调用方式进行自测。

## <a name="自测工具使用示例">自测工具使用示例</a>

下面是`haltest.c`中的自测函数调用示例

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

## <a name="自测工具的能力概览">自测工具的能力概览</a>

自测工具提供以下函数用于自测，用户在适当的地方进行调用，工具会输出log用于确认当前HAL接口的正确性。以下自测函数包含的HAL为[WIFI配网概述](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/WiFi_Provision)中的[配网模块公共HAL](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/WiFi_Provision#%E9%85%8D%E7%BD%91%E6%A8%A1%E5%9D%97%E5%85%AC%E5%85%B1HAL)。一键配网仅使用这些公共HAL即可。

verify_awss_preprocess
---
对`HAL_Awss_Get_Timeout_Interval_Ms`、`HAL_Awss_Get_Channelscan_Interval_Ms`和`HAL_Wifi_Get_Mac`进行自测。这3个HAL返回的是一些配置参数，用户需要确认其正确性。输出日志例子如下：

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
对`HAL_Awss_Open_Monitor`进行自测，将打开监听模式，输出日志为当前收到的配网模块帧类型的表格，如果其中有`SmartConfig Start Frame`、`SmartConfig Group Frame`和`SmartConfig  Data Frame`字样，说明当前监听模式可以接收到一键配网所需的帧类型。

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
对`HAL_Awss_Close_Monitor`进行自测。用户调用该函数后，应该不再接收到任何配网帧类型。日志输出如下：

    /***********************************************/
    /*        Verify HAL_Awss_Close_Monitor        */
    /***********************************************/

verify_awss_switch_channel
---
对`HAL_Awss_Switch_Channel`进行自测。在监听模式打开的情况下，不断切换wifi信道进行侦听，可参考`haltest.c`进行自测。

verfiy_awss_connect_ap
---
对`HAL_Awss_Connect_Ap`进行自测。在调用该函数后，应当可以成功连上指定AP。日志输出如下：

    /***********************************************/
    /*          Verify HAL_Awss_Connect_Ap         */
    /*                  Result:  0                 */
    /***********************************************/

verify_awss_get_ap_info
---
对`HAL_Wifi_Get_Ap_Info`进行自测。在调用该函数后，会打印获取到的ssid、passwd和bssid。日志输出如下：

    /***********************************************/
    /*         Verify HAL_Wifi_Get_Ap_Info         */
    /*                  Result:  0                 */
    /*           SSID:       test_ssid             */
    /*           PASSWD:     test_passwd           */
    /***********************************************/

verify_awss_net_is_ready
---
对`HAL_Sys_Net_Is_Ready`进行自测。在滴啊用该函数后，会获取当前网络状态。日志输出如下：

    /***********************************************/
    /*         Verify HAL_Sys_Net_Is_Ready         */
    /*                  Result:  0                 */
    /***********************************************/