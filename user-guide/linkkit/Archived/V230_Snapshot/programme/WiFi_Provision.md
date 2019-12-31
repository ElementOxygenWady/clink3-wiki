# <a name="目录">目录</a>
+ [配网的定义](#配网的定义)
+ [AWSS用户API](#AWSS用户API)
+ [AWSS用户API详解](#AWSS用户API详解)
    * [awss_start](#awss_start)
    * [awss_config_press](#awss_config_press)
    * [awss_stop](#awss_stop)
    * [awss_report_set](#awss_report_set)
+ [AWSS示例](#AWSS示例)
+ [需要实现的HAL接口](#需要实现的HAL接口)
+ [绑定与解绑通知](#绑定与解绑通知)

# <a name="配网的定义">配网的定义</a>
![image](http://git.cn-hangzhou.oss-cdn.aliyun-inc.com/uploads/Apsaras64/pub/4b7616e37b9b2eecbd2d3664190d018e/image.png)

用户拿到一个新设备时, 需要添加设备来建立用户和设备的绑定关系, 而添加设备的整个流程分成三个步骤, 如上图所示:

> - 用户添加设备前置步骤
> - 设备配网(**AWSS: Alibaba Wireless Setup Service**)
> - 用户与设备绑定

大家有可能误以为用户添加设备的整个流程都是配网. 其实, 配网仅仅是把路由器**AP**的**SSID**和**PASSWD**告诉设备, 然后设备连接上路由器, 不包括用户添加设备的前置操作和后续的绑定操作.

# <a name="AWSS用户API">AWSS用户API</a>

| 函数名                                      | 说明
|---------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
| [awss_start](#awss_start)                   | 启动**AWSS**服务(未使能), **AWSS**采用**AES128**保证数据安全, 加解密依赖于设备四元组, 启动**AWSS**服务之前先确认设备四元组(`DeviceName + DeviceSecret + ProductKey + ProductSecret`)已经成功烧录
| [awss_config_press](#awss_config_press)     | 使能**AWSS**服务,启动**AWSS**服务用户设备发现和扫描周围的**AP**列表, 只有使能后, **AWSS**才会真正解析**AWSS**包, 并且使能**AWSS**只作用一段时间(具体时间由用户对接的**HAL_Awss_Get_Timeout_Interval_Ms**决定), 超时之后用户需要再次使能**AWSS**.
| [awss_stop](#awss_stop)                     | 停止**AWSS**服务
| [awss_report_reset](#awss_report_reset)     | **AWSS**上报设备恢复出厂设置到云端, 云端解除设备与用户的绑定关系

# <a name="AWSS用户API详解">AWSS用户API详解</a>
## <a name="awss_start">awss_start</a>

原型
---
```c
int awss_start();
```

接口说明
---
启动**AWSS**服务, 用于设备发现和扫描周围的**AP**列表, 仅仅启动**AWSS**服务, **AWSS**不会解析配网包, 只有使能**AWSS**后, **AWSS**才会真正配网包. 另外, 由于**AWSS**采用**AES128**保证数据的安全性, 而**AES128**的密钥依赖于设备四元组信息(`DeviceName + DeviceSecret + ProductKey + ProductSecret`), 在启动**AWSS**之前一定要确保设备的四元组信息已经烧录成功, 否则AWSS无法正确解析出**SSID**和**PASSWD**

返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败

---
## <a name="awss_config_press">awss_config_press</a>

原型
---
```c
int awss_config_press();
```

接口说明
---
使能**AWSS**服务, 只有启动并使能**AWSS**后, **AWSS**才会真正配网包

返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败

---

## <a name="awss_stop">awss_stop</a>

原型
---
```c
int awss_stop();
```

接口说明
---
停止**AWSS**服务, **awss_start**和**awss_stop**都是**Block**操作, 请使用时不要再一个线程中使用, 否则会造成死锁

返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败

---
## <a name="awss_report_set">awss_report_set</a>

原型
---
```c
int awss_report_set();
```

接口说明
---
**AWSS**向服务器上报恢复出厂操作, 云端收到此消息后解除设备与用户之间的绑定关系
厂家自己需要清除的信息, 如**SSID**和**PASSWD**需要厂商自己清除.

返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败

---

# <a name="AWSS示例">AWSS示例</a>

```c
uint8_t bssid[ETH_ALEN] = {0};
char ssid[HAL_MAX_SSID_LEN] = {0};
char passwd[HAL_MAX_PASSWORD_LEN] = {0};

// die or chipset uses itself API to get ssid/passwd/bssid

if (INVALID_SSID(ssid) || INVALID_BSSID(bssid) || AP_NOT_EXIST(ssid, bssid) {
    awss_start();
} else {
    HAL_Awss_Connect_Ap(TIMEOUT_MS, ssid, passwd, 0, 0, bssid, 0);
}
```

# <a name="需要实现的HAL接口">需要实现的HAL接口</a>
以下函数在**AliOS Things**已经实现, 如果希望单独使用**SDK**, 则需要用户对接**

| 函数名                                                                          | 说明
|---------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------
| [HAL_Awss_Open_Monitor](#HAL_Awss_Open_Monitor)                                 | 设备工作在监听(**Monitor**)模式, 并在收到**802.11**帧的时候调用被传入的回调函数(包括管理帧和数据帧)
| [HAL_Awss_Close_Monitor](#HAL_Awss_Close_Monitor)                               | 关闭监听(**Monitor**)模式, 并开始以站点(**Station**)模式工作
| [HAL_Awss_Connect_Ap](#HAL_Awss_Connect_Ap)                                     | 要求设备连接指定热点(**Access Point**)的函数
| [HAL_Awss_Get_Channelscan_Interval_Ms](#HAL_Awss_Get_Channelscan_Interval_Ms)   | 获取在每个信道(**Channel**)上扫描的时间长度, 单位是毫秒
| [HAL_Awss_Get_Timeout_Interval_Ms](#HAL_Awss_Get_Timeout_Interval_Ms)           | 获取配网服务(**AWSS**)的超时时间长度, 单位是毫秒
| [HAL_Awss_Switch_Channel](#HAL_Awss_Switch_Channel)                             | 设置**Wi-Fi**设备切换到指定的信道(**Channel**)上
| [HAL_Wifi_Scan](#HAL_Wifi_Scan)                                                 | 启动一次**Wi-Fi**的空中扫描(**Scan**)
| [HAL_Wifi_Send_80211_Raw_Frame](#HAL_Wifi_Send_80211_Raw_Frame)                 | 在当前信道(**Channel**)上以基本数据速率(**1Mbps**)发送裸的**802.11**帧(**RAW 802.11 Frame**)
| [HAL_Wifi_Enable_Mgmt_Frame_Filter](#HAL_Wifi_Enable_Mgmt_Frame_Filter)         | 在站点(**Station**)模式下使能或禁用对管理帧的过滤
| [HAL_Wifi_Get_Ap_Info](#HAL_Wifi_Get_Ap_Info)                                   | 获取设备所连接的热点(**Access Point**)的信息
| [HAL_Sys_Net_Is_Ready](#HAL_Sys_Net_Is_Ready)                                   | 检查**Wi-Fi**网卡/芯片或模组当前的**IP**地址是否有效

---
除了这些专门的**HAL**之外, **AWS**S还使用了系统的**HAL**:

| 函数名                                              | 说明
|-----------------------------------------------------|-------------------------------------------------------------------------------------------------------------
| [HAL_GetDeviceName](#HAL_GetDeviceName)             | 获取设备的**DeviceName**, 用于标识设备单品的名字, 四元组之一
| [HAL_GetDeviceSecret](#HAL_GetDeviceSecret)         | 获取设备的**DeviceSecret**, 用于标识设备单品的名字, 四元组之一
| [HAL_GetProductKey](#HAL_GetProductKey)             | 获取设备的**ProductKey**, 用于标识设备单品的名字, 四元组之一
| [HAL_GetProductSecret](#HAL_GetProductSecret)       | 获取设备的**ProductSecret**, 用于标识设备单品的名字, 四元组之一
| [HAL_Aes128_Init](#HAL_Aes128_Init)                 | 初始化**AES**加密的结构体
| [HAL_Aes128_Destroy](#HAL_Aes128_Destroy)           | 销毁**AES**加密的结构体
| [HAL_Aes128_Cfb_Encrypt](#HAL_Aes128_Cfb_Encrypt)   | 以**AES-CFB-128**方式, 根据**HAL_Aes128_Init()**时传入的密钥加密指定的明文
| [HAL_Aes128_Cfb_Decrypt](#HAL_Aes128_Cfb_Decrypt)   | 以**AES-CFB-128**方式, 根据**HAL_Aes128_Init()**时传入的密钥解密指定的密文
| [HAL_Aes128_Cbc_Encrypt](#HAL_Aes128_Cbc_Encrypt)   | 以**AES-CBC-128**方式, 根据**HAL_Aes128_Init()**时传入的密钥加密指定的明文
| [HAL_Aes128_Cbc_Decrypt](#HAL_Aes128_Cbc_Decrypt)   | 以**AES-CBC-128**方式, 根据**HAL_Aes128_Init()**时传入的密钥解密指定的密文
| [HAL_Timer_Create](#HAL_Timer_Create)               | 根据**Name**/**TimerFunc**和用户上下文创建**Timer**
| [HAL_Timer_Start](#HAL_Timer_Start)                 | **Timer**开始计时, **Time**r超时时调用回调函数**TimerFunc**
| [HAL_Timer_Stop](#HAL_Timer_Stop)                   | 停止**Timer计**时
| [HAL_Timer_Delete](#HAL_Timer_Delete)               | 删除**Timer**, 释放资源
| [HAL_Kv_Set](#HAL_Kv_Set)                           | **Flash**中写入键值对(**Key-Value**)
| [HAL_Kv_Get](#HAL_Kv_Get)                           | **Flash**中读取键值对的**Value**
| [HAL_Kv_Del](#HAL_Kv_Del)                           | 删除**Flash**中的键值对
| [HAL_Kv_Erase_All](#HAL_Kv_Erase_All)               | 擦除存储键值对的整个区域
| [HAL_Wifi_Get_IP](#HAL_Wifi_Get_IP)                 | 获取设备的**IP**地址, 点分十进制格式保存在字符串数组出参, 二进制格式则作为返回值, 并以网络字节序(大端)表达
| [HAL_Wifi_Get_Mac](#HAL_Wifi_Get_Mac)               | 获取设备的**MAC**地址, 格式应当是**"XX:XX:XX:XX:XX:XX"**


# <a name="绑定与解绑通知">绑定与解绑通知</a>
在用户程序中, 可以通过注册`ITE_EVENT_NOTIFY`来获取app或者云端对设备的控制消息, 比如绑定与解绑的通知

参考代码
---
```c
static int user_event_notifier_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Event Notify Received, Devid: %d, Request: %s", devid, request);

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}
IOT_RegisterCallback(ITE_EVENT_NOTIFY, user_event_notifier_event_handler);
```

设备被绑定的时候, 设备收到的通知报文例子
---
```json
{
	"method": "_thing.event.notify",
	"id": "1225872209",
	"params": {
		"identifier": "awss.BindNotify",
		"value": {
			"Operation": "Bind"
		}
	},
	"version": "1.0.0"
}

```

绑定关系被解除的时候, 设备收到的通知报文例子
---
```json
	{
		"method": "_thing.event.notify",
		"id": "1225874889",
		"params": {
			"identifier": "awss.BindNotify",
			"value": {
				"Operation": "Unbind"
			}
		},
		"version": "1.0.0"
	}
```

在`IOT_RegisterCallback()`注册的回调函数中, 对以上的JSON报文做解析, 就能知道设备是当前被解绑了或是被绑定了

