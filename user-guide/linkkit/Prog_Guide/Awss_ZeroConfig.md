# <a name="目录">目录</a>
+ [方案说明](#方案说明)
+ [如何配置使用这种配网模式](#如何配置使用这种配网模式)
+ [API使用范例](#API使用范例)
+ [API接口说明](#API接口说明)
    * [应用场景](#应用场景)
+ [需要对接的HAL接口](#需要对接的HAL接口)

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

| 序号    | 函数名                                                                                                                                              | 说明
|---------|-------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------
| 1       | [awss_start](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_start)                     | 开启配网服务(设备热点配网除外)
| 2       | [awss_config_press](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_config_press)       | 使能配网, 开始解包
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

| 序号    | 函数名                                                                                                                                                                          | 说明
|---------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------
| 1       | [HAL_Wifi_Enable_Mgmt_Frame_Filter](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Wifi_Enable_Mgmt_Frame_Filter)   | 在站点(Station)模式下使能或禁用对特定管理帧的过滤(只接受包含特定OUI的管理帧)
| 2       | [HAL_Wifi_Send_80211_Raw_Frame](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Wifi_Send_80211_Raw_Frame)   | 发送80211  raw数据