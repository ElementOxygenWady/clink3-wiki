# <a name="目录">目录</a>
+ [参考方案说明](#参考方案说明)
+ [参考交互步骤 ](#参考交互步骤 )
    * [参考代码](#参考代码)

# <a name="参考方案说明">参考方案说明</a>

利用有限的交互方式(输出:led颜色,led闪烁频率,语音播报等;输入:按钮等)让用户可以轻松地操作设备进入配网状态,并能清晰的感知配网过程状态变化,给用户提供友好的交互体验.

# <a name="参考交互步骤 ">参考交互步骤 </a>
+ 上电后判断是否存在AP信息(ssid,password);
+ 如果AP信息不存在,向外提示进入预配网状态(如修改指示灯频率或颜色,语音播报等),并调用awss_start()进入预配网状态(未使能);此时需要通过按钮触发awss_config_press()函数方能进入配网使能状态,也可以在调用awss_start()之前直接调用awss_config_press()跳过预配网状态(具体根据产品定义设计);
+ 如果AP信息存在,则提示wifi连接中状态(如修改指示灯频率或颜色,语音播报等);
+ 如果连接失败,则提示wifi连接失败状态(如修改指示灯频率或颜色,语音播报等);
+ 如果连接成功,则提示wifi连接成功状态(如修改指示灯频率或颜色,语音播报等),并开始linkkit 上云初始化动作;
+ 过程状态变化可以通过iotx_event_regist_cb()注册配网事件回调函数感知.

## <a name="参考代码">参考代码</a>

```
#if defined(WIFI_PROVISION_ENABLED)

    typedef enum{
        STATE_PRE_START,
        STATE_START,
        STATE_CONNECTING,
        STATE_CONNECT_AP_FAILED,
        STATE_CONNECT_AP_SUCCESS,
    }wifi_provision_state_e;

    int set_wifi_provision_state(wifi_provision_state_e state) {
        int ret = 0;
        switch(state) {
            case STATE_PRE_START:
            /*用户操作提示,如修改指示灯频率或颜色,语音播报等*/
            break;
            case STATE_START:
            /*用户操作提示,如修改指示灯频率或颜色,语音播报等*/
            break;
            case STATE_CONNECTING:
            /*用户操作提示,如修改指示灯频率或颜色,语音播报等*/
            break;
            case STATE_CONNECT_AP_FAILED:
            /*用户操作提示,如修改指示灯频率或颜色,语音播报等*/
            break;
            case STATE_CONNECT_AP_SUCCESS:
            /*用户操作提示,如修改指示灯频率或颜色,语音播报等*/
            break;     
            default:
            /*用户操作提示,如修改指示灯频率或颜色,语音播报等*/
            ret = -1;
            break;       
        }
        return ret;
    }

static void linkkit_event_monitor(int event)
{
    switch (event) {
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
            EXAMPLE_TRACE("IOTX_AWSS_START");
            set_wifi_provision_state(STATE_PRE_START);
            break;
        case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
            EXAMPLE_TRACE("IOTX_AWSS_ENABLE");
            set_wifi_provision_state(STATE_START);
            break;
        case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
            EXAMPLE_TRACE("IOTX_AWSS_LOCK_CHAN");
            // operate led to indicate user
            break;
        case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
            EXAMPLE_TRACE("IOTX_AWSS_PASSWD_ERR");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_SSID_PASSWD:
            EXAMPLE_TRACE("IOTX_AWSS_GOT_SSID_PASSWD");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
            // discover, router solution)
            EXAMPLE_TRACE("IOTX_AWSS_CONNECT_ADHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
            EXAMPLE_TRACE("IOTX_AWSS_CONNECT_ADHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
            EXAMPLE_TRACE("IOTX_AWSS_CONNECT_AHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
            EXAMPLE_TRACE("IOTX_AWSS_CONNECT_AHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
            // (AP and router solution)
            EXAMPLE_TRACE("IOTX_AWSS_SETUP_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
            EXAMPLE_TRACE("IOTX_AWSS_CONNECT_ROUTER");
            set_wifi_provision_state(STATE_CONNECTING);
            break;
        case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
            // router.
            EXAMPLE_TRACE("IOTX_AWSS_CONNECT_ROUTER_FAIL");
            set_wifi_provision_state(STATE_CONNECT_AP_FAILED);
            break;
        case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
            // ip address
            EXAMPLE_TRACE("IOTX_AWSS_GOT_IP");
            set_wifi_provision_state(STATE_CONNECT_AP_SUCCESS);
            
            // operate led to indicate user
            break;
        case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
            // sucess)
            EXAMPLE_TRACE("IOTX_AWSS_SUC_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
            // support bind between user and device
            EXAMPLE_TRACE("IOTX_AWSS_BIND_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
            // user needs to enable awss again to support get ssid & passwd of router
            EXAMPLE_TRACE("IOTX_AWSS_ENALBE_TIMEOUT");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD: // Device try to connect cloud
            EXAMPLE_TRACE("IOTX_CONN_CLOUD");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
            // net_sockets.h for error code
            EXAMPLE_TRACE("IOTX_CONN_CLOUD_FAIL");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            EXAMPLE_TRACE("IOTX_CONN_CLOUD_SUC");
            // operate led to indicate user
            break;
        case IOTX_RESET: // Linkkit reset success (just got reset response from
            // cloud without any other operation)
            EXAMPLE_TRACE("IOTX_RESET");
            // operate led to indicate user
            break;
        default:
            break;
    }
}

extern int awss_start();
int user_wifi_prepare()
{
    char ssid[HAL_MAX_SSID_LEN] = {0};
    char passwd[HAL_MAX_PASSWD_LEN] = {0};
    uint8_t bssid[ETH_ALEN] = {0};
    iotx_event_regist_cb(linkkit_event_monitor);
    do {
        HAL_Wifi_Get_Ap_Info(ssid, passwd,bssid);
        if(strlen(ssid) == 0) {
            awss_start(); 
        }
    } while(strlen(ssid) == 0);
    return 0;
}

int main(int argc, char **argv)
{
    ......
#if defined(WIFI_PROVISION_ENABLED)
    user_wifi_prepare();
#endif

    ......

    do {
        g_user_example_ctx.master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
        if (g_user_example_ctx.master_devid >= 0) {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Open failed! retry after %d ms\n", 2000);
        HAL_SleepMs(2000);
    } while (1);

    /* run_ubuntu_wifi_provision_example(); */

    do {
        res = IOT_Linkkit_Connect(g_user_example_ctx.master_devid);
        if (res >= 0) {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Connect failed! retry after %d ms\n", 5000);
        HAL_SleepMs(5000);
    } while (1);
    .......
}

#endif
```

