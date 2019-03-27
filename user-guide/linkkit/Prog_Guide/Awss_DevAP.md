# <a name="目录">目录</a>
+ [方案说明](#方案说明)
+ [如何配置使用这种配网模式](#如何配置使用这种配网模式)
+ [API使用范例](#API使用范例)
+ [API接口说明](#API接口说明)
    * [应用场景](#应用场景)
+ [需要对接的HAL接口](#需要对接的HAL接口)

# <a name="方案说明">方案说明</a>

方案简述:  设备开启默认热点, 手机搜索发现到热点后连接到设备热点, 手机将连接公网的路由器热点的SSID/密码(AES加密)发送给WiFi设备, 然后WiFi设备就使用手机发送过来的SSID/密码(AES解密)连接路由器

工作原理如下所示
---

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/13.png"  />


# <a name="如何配置使用这种配网模式">如何配置使用这种配网模式</a>
通过menuconfig选择配网模块并选定手机热点配网:
```
make distclean

make menuconfig
```

使能配网:

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/6.png" width="600"  />

选择手机热点配网:

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/12.png"  width="800" />


# <a name="API使用范例">API使用范例</a>
下面是设备上电后的配网代码示例:

```
uint8_t bssid[ETH_ALEN] = {0};
char ssid[HAL_MAX_SSID_LEN] = {0};
char passwd[HAL_MAX_PASSWORD_LEN] = {0};
// Users need to get the stord ssid/passwd, to decide whether to start wifi provisioning service
if (INVALID_SSID(ssid) || INVALID_BSSID(bssid) ) {
    awss_dev_ap_start();
} else {
    HAL_Awss_Connect_Ap(TIMEOUT_MS, ssid, passwd, 0, 0, bssid, 0);
}
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
|---------|-------------------------------------------------------------------------------------------------------------------------------------------------|---------------------
| 1       | [awss_dev_ap_start](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_dev_ap_start)       | 开启设备热点配网
| 2       | [awss_dev_ap_stop](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Awss_Provides#awss_dev_ap_stop)     | 关闭设备热点配网


## <a name="应用场景">应用场景</a>
int awss_dev_ap_start
---

调用该API用于启动设备热点配网服务. 启动配网服务的场景包括:

设备首次上电, 设备上不存在热点SSID/密码.
用户希望设备使用设备热点方式重新进行配网
调用该配网服务后, 设备进入配网发现状态, 用于让网络中的其它设备发现自己的存在
设备上电时若已有SSID/密码, 直接调用HAL_Awss_Connect_Ap()去连接WiFi热点

int awss_dev_ap_stop
---

调用该API用于停止当前配网. 停止服务的场景包括:
+ 当前不需要配网
+ 希望通过其他途径配网

# <a name="需要对接的HAL接口">需要对接的HAL接口</a>

| 序号    | 函数名                                                                                                                                                              | 说明
|---------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------
| 1       | [HAL_Awss_Close_Ap](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Close_Ap)                   | 关闭设备热点
| 2       | [HAL_Awss_Open_Ap](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Open_Ap)                     | 打开设备热点配网

