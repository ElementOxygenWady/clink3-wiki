# <a name="目录">目录</a>
+ [例程讲解](#例程讲解)
    * [编译配置和相关文件](#编译配置和相关文件)
    * [建连与身份认证](#建连与身份认证)
    * [发送消息](#发送消息)
    * [接收消息](#接收消息)
    * [Yield与Deinit](#Yield与Deinit)
+ [使用CoAP对接物联网平台](#使用CoAP对接物联网平台)
    * [连接基础版设备](#连接基础版设备)
    * [连接高级版设备](#连接高级版设备)
+ [CoAP提供的API](#CoAP提供的API)
+ [需要的HAL接口](#需要的HAL接口)

# <a name="例程讲解">例程讲解</a>

## <a name="编译配置和相关文件">编译配置和相关文件</a>

在linux环境中，运行`make menuconfig`，选中`FEATURE_COAP_COMM_ENABLED`后保存退出；运行`make`即可编译出CoAP例程。

SDK的CoAP功能源码存放在`src/coap`中；
SDK提供CoAP用户层接口包含在`src/coap/coap_api.h`中；
CoAP需要用到用户实现的HAL接口包含在`src/coap/coap_wrapper.h`中；
CoAP例程路径为`src/coap/examples/coap_example.c`。

## <a name="建连与身份认证">建连与身份认证</a>

调用`IOT_CoAP_Init`完成初始化并与云端建立连接, 调用`IOT_CoAP_DeviceNameAuth`进行身份认证并获取token

> 目前CoAP连接协议支持2种安全模式, `IOT_CoAP_Init`会解析填入的URL字符串以决定使用哪种安全模式
> * DTLS: 使用的URL字符串为`coaps://%s.coap.cn-shanghai.link.aliyuncs.com:5684`
> * 对称秘钥: 使用的URL字符串为`coap-psk://%s.coap.cn-shanghai.link.aliyuncs.com:5682`

```
int iotx_get_devinfo(iotx_deviceinfo_t *p_devinfo)
{
    if (NULL == p_devinfo) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(p_devinfo, 0x00, sizeof(iotx_deviceinfo_t));

    /* get device info */
    HAL_GetProductKey(p_devinfo->product_key);
    HAL_GetDeviceName(p_devinfo->device_name);
    HAL_GetDeviceSecret(p_devinfo->device_secret);
    HAL_GetDeviceID(p_devinfo->device_id);

    return IOTX_SUCCESS;
}
......
iotx_coap_config_t config;
char url[256] = {0};
iotx_deviceinfo_t deviceinfo;

snprintf(url, sizeof(url), IOTX_ONLINE_PSK_SERVER_URL, IOTX_PRODUCT_KEY);
iotx_get_devinfo(&deviceinfo);
config.p_devinfo = (iotx_device_info_t *)&deviceinfo;
config.wait_time_ms = 3000;
config.p_url = url;

p_ctx = IOT_CoAP_Init(&config);
if (NULL != p_ctx) {
    IOT_CoAP_DeviceNameAuth(p_ctx);
    do {
        if (count == 11 || 0 == count) {
            iotx_post_data_to_server((void *)p_ctx);
            count = 1;
        }
        count ++;
        IOT_CoAP_Yield(p_ctx);
    } while (m_coap_client_running);

    IOT_CoAP_Deinit(&p_ctx);
} else {
    HAL_Printf("IoTx CoAP init failed\r\n");
}
......
```

## <a name="发送消息">发送消息</a>

CoAP是一种请求/响应模型协议，SDK使用[IOT_CoAP_SendMessage](#IOT_CoAP_SendMessage)发送CoAP请求报文，同时注册响应处理回调函数用于处理CoAP响应。
> 如果函数调用返回错误码`IOTX_ERR_NOT_AUTHED`，则说明认证未成功或者token已过期，应该重新调用[IOT_CoAP_DeviceNameAuth](#IOT_CoAP_DeviceNameAuth)进行身份认证。

> `path`参数是格式为`/topic/${topic}`的字符串, `${topic}`可以在物联网平台的控制台**产品管理 > 消息通信**页面查看到, 如例程中默认的`path`为`/topic/${ProductKey}/${DeviceName}/update/`

> SDK目前仅支持向有publish操作权限的topic发送消息

> SDK限制：目前对CoAP请求的响应超时没有对应的回调函数通知用户。

```
static void iotx_post_data_to_server(void *param)
{
    char               path[IOTX_URI_MAX_LEN + 1] = {0};
    iotx_message_t     message;
    iotx_deviceinfo_t  devinfo;

    memset(&message, 0, sizeof(iotx_message_t));
    memset(&devinfo, 0, sizeof(iotx_deviceinfo_t));

    message.p_payload = (unsigned char *)"{\"name\":\"hello world\"}";
    message.payload_len = strlen("{\"name\":\"hello world\"}");
    message.resp_callback = iotx_response_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;
    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)param;

    iotx_set_devinfo(&devinfo);
    snprintf(path, IOTX_URI_MAX_LEN, "/topic/%s/%s/update/", (char *)devinfo.product_key,
             (char *)devinfo.device_name);

    IOT_CoAP_SendMessage(p_ctx, path, &message);
}
```
*注: 如果产品使用高级版定义, 使用物模型中的函数来收发数据*

## <a name="接收消息">接收消息</a>

在响应处理回调中使用[IOT_CoAP_GetMessageCode](#IOT_CoAP_GetMessageCode)获取响应状态码, 使用[IOT_CoAP_GetMessagePayload](#IOT_CoAP_GetMessagePayload)获取响应数据

```
static void iotx_response_handler(void *arg, void *p_response)
{
    int len = 0;
    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    HAL_Printf("[APPL]: Message response code: 0x%x\r\n", resp_code);
    HAL_Printf("[APPL]: Len: %d, Payload: %s\r\n", len, p_payload);
}
```

*注: 如果产品使用高级版定义, 使用物模型中的函数来收发数据*

## <a name="Yield与Deinit">Yield与Deinit</a>

[IOT_CoAP_Yield](#IOT_CoAP_Yield)接口用于接收网络报文, 解析出CoAP报文并做相关处理, 同时还会做请求超时重传. 若设备不支持多线程, 则必须轮询此接口. 若设备支持多线程, 可在单独线程中调用此接口

最终用户可以使用[IOT_CoAP_Deinit](#IOT_CoAP_Deinit)断开云端连接，释放系统资源。
```
    ......
    iotx_coap_context_t *p_ctx = IOT_CoAP_Init(&config);
    while (1) {
        /* 用户应用代码 */

        IOT_CoAP_Yield(p_ctx);
    }

    IOT_CoAP_Deinit(&p_ctx);
    ......
```

# <a name="使用CoAP对接物联网平台">使用CoAP对接物联网平台</a>

本节将演示如何CoAP连接物联网平台

## <a name="连接基础版设备">连接基础版设备</a>

1. 在`物联网控制台`新建一个基础版产品, 在**产品详情 > 消息通信**页面可查看到默认的topic定义
![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/Linkkit_CoAP/coap_cloud_basic_01.png)

2. 新建设备`example1`, 用新设备三元组信息替换`examples/coap/coap_example.c`中的三元组

```
    #define IOTX_PRODUCT_KEY         "a1RP1qZfrEi"
    #define IOTX_DEVICE_NAME         "example1"
    #define IOTX_DEVICE_SECRET       "fPloaszb6saUFJte4EhyuBHQpR9SuKHb"
    #define IOTX_DEVICE_ID           "example1.1"   /* 可忽略不修改 */
```

3. 目前SDK支持向操作权限为**发布**的topic发送消息, 所以我们使用默认的topic: `/topic/%s/%s/update/`进行演示. 用户可自行添加自定义的topic进行调试

4. 在`make.setting`文件中添加`FEATURE_COAP_COMM_ENABLED=y`后保存退出

5. 运行`make`编译成功后, 执行`./output/release/bin/coap-example -e online -s dtls -l`以DTLS模式建立连接, 可查到以下log:

```
[COAP-Client]: Enter Coap Client
*****The Product Key  : a1RP1qZfrEi *****
*****The Device Name  : example1 *****
*****The Device Secret: fPloaszb6saUFJte4EhyuBHQpR9SuKHb *****
*****The Device ID    : a1RP1qZfrEi.example1 *****
[dbg] Cloud_CoAPUri_parse(31): The uri is coaps://a1RP1qZfrEi.coap.cn-shanghai.link.aliyuncs.com:5684
[dbg] Cloud_CoAPUri_parse(69): The endpoint type is: 1
[dbg] Cloud_CoAPUri_parse(97): The host name is: a1RP1qZfrEi.coap.cn-shanghai.link.aliyuncs.com
[dbg] Cloud_CoAPUri_parse(120): The port is: 5684

......

[dbg] iotx_calc_sign(70): The device name sign: f5204b935c67866fee8b8791cab9d6b0
[dbg] IOT_CoAP_DeviceNameAuth(572): The payload is: {"productKey":"a1RP1qZfrEi","deviceName":"example1","clientId":"a1RP1qZfrEi.example1","sign":"f5204b935c67866fee8b8791cab9d6b0"}
[dbg] IOT_CoAP_DeviceNameAuth(573): Send authentication message to server
[dbg] Cloud_CoAPMessage_send(287): ----The message length 146-----
[dbg] Cloud_CoAPMessage_send(294): Add message id 1 len 146 to the list
[dbg] Cloud_CoAPNetworkDTLS_read(30): << secure_datagram_read, read buffer len 1280, timeout 3000
[trc] mbedtls_ssl_read len 53 bytes

[dbg] Cloud_CoAPNetwork_read(135): << CoAP recv 53 bytes data
[dbg] Cloud_CoAPMessage_handle(382): Version     : 1
[dbg] Cloud_CoAPMessage_handle(383): Code        : 2.05(0x45)
[dbg] Cloud_CoAPMessage_handle(384): Type        : 0x2
[dbg] Cloud_CoAPMessage_handle(385): Msgid       : 1
[dbg] Cloud_CoAPMessage_handle(386): Option      : 0
[dbg] Cloud_CoAPMessage_handle(387): Payload Len : 44
[dbg] Cloud_CoAPMessage_handle(401): Receive CoAP Response Message,ID 1
[dbg] Cloud_CoAPRespMessage_handle(342): Find the node by token
[dbg] iotx_device_name_auth_callback(195): Receive response message:
[dbg] iotx_device_name_auth_callback(196): * Response Code : 0x45
[dbg] iotx_device_name_auth_callback(197): * Payload: {"token":"6586059d320940bb833f81c6768086d9"}
[inf] iotx_device_name_auth_callback(209): CoAP authenticate success!!!
[dbg] Cloud_CoAPRespMessage_handle(354): Remove the message id 1 from list
[dbg] Cloud_CoAPNetworkDTLS_read(30): << secure_datagram_read, read buffer len 1280, timeout 3000
[trc] DTLS recv timeout

*****The Product Key  : a1RP1qZfrEi *****
*****The Device Name  : example1 *****
*****The Device Secret: fPloaszb6saUFJte4EhyuBHQpR9SuKHb *****
*****The Device ID    : a1RP1qZfrEi.example1 *****
[dbg] iotx_split_path_2_option(614): The uri is /topic/a1RP1qZfrEi/example1/update/
[dbg] iotx_split_path_2_option(621): path: topic,len=5
[dbg] iotx_split_path_2_option(621): path: a1RP1qZfrEi,len=11
[dbg] iotx_split_path_2_option(621): path: example1,len=8
[dbg] iotx_split_path_2_option(621): path: update,len=6
[dbg] Cloud_CoAPMessage_send(287): ----The message length 105-----
[dbg] Cloud_CoAPMessage_send(294): Add message id 2 len 105 to the list
[dbg] Cloud_CoAPNetworkDTLS_read(30): << secure_datagram_read, read buffer len 1280, timeout 3000
[trc] mbedtls_ssl_read len 19 bytes

[dbg] Cloud_CoAPNetwork_read(135): << CoAP recv 19 bytes data
[dbg] Cloud_CoAPMessage_handle(382): Version     : 1
[dbg] Cloud_CoAPMessage_handle(383): Code        : 2.05(0x45)
[dbg] Cloud_CoAPMessage_handle(384): Type        : 0x2
[dbg] Cloud_CoAPMessage_handle(385): Msgid       : 2
[dbg] Cloud_CoAPMessage_handle(386): Option      : 1
[dbg] Cloud_CoAPMessage_handle(387): Payload Len : 0
[dbg] Cloud_CoAPMessage_handle(401): Receive CoAP Response Message,ID 2
[dbg] Cloud_CoAPRespMessage_handle(342): Find the node by token
[APPL]: Message response code: 0x45
[APPL]: Len: 0, Payload: (null)
......
```

同时在控制台可以查看到日志信息:
![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/Linkkit_CoAP/coap_cloud_basic_02.png)

**注意:目前物联网控制台不支持显示CoAP的在线状态**

## <a name="连接高级版设备">连接高级版设备</a>

本章节主要介绍使用CoAP协议接口连接

1. 在`物联网控制台`新建一个高级版产品, 使用ALink JSON数据格式. 在**产品详情 > 消息通信**页面可查看到默认的topic定义. 在**产品详情 > 功能定义**页面添加一个标识符为`Data`的字符串格式属性, 如下图所示
![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/Linkkit_CoAP/coap_cloud_adv_01.png)
![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/Linkkit_CoAP/coap_cloud_adv_02.png)
2. 同样新建设备`example1`, 用新设备三元组信息替换`examples/coap/coap_example.c`中的三元组
3. 我们使用高级版的默认topic: `/topic/sys/${productKey}/${deviceName}/thing/event/property/post`来演示属性的上报功能. 修改`examples/coap/coap_example.c`中的上报`path`为`/topic/sys/%s/%s/thing/event/property/post`
4. 使用标准的Alink JSON格式上报属性, 属性参数填写为`Post by CoAP`, 即上报Payload为`"{\"id\": \"1\",\"version\": \"1.0\",\"params\": {\"Data\":\"Post by CoAP\"},\"method\": \"thing.event.property.post\"}"`. 可参考下面代码进行修改
```
static void iotx_post_data_to_server(void *param)
{
    char               path[IOTX_URI_MAX_LEN + 1] = {0};
    iotx_message_t     message;
    iotx_deviceinfo_t  devinfo;
    const char        *payload = "{\"id\": \"1\",\"version\": \"1.0\",\"params\": {\"Data\":\"Post by CoAP\"},\"method\": \"thing.event.property.post\"}";

    memset(&message, 0, sizeof(iotx_message_t));
    memset(&devinfo, 0, sizeof(iotx_deviceinfo_t));

    message.p_payload = (unsigned char *)payload;
    message.payload_len = strlen(payload);
    message.resp_callback = iotx_response_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;
    iotx_coap_context_t *p_ctx = (iotx_coap_context_t *)param;

    iotx_set_devinfo(&devinfo);
    snprintf(path, IOTX_URI_MAX_LEN, "/topic/sys/%s/%s/thing/event/property/post", (char *)devinfo.product_key,
             (char *)devinfo.device_name);

    IOT_CoAP_SendMessage(p_ctx, path, &message);
}
```
5. 运行`make menuconfig`进行配置, 然后运行`make`编译, 执行`./output/release/bin/coap-example -e online -s psk -l`以对称秘钥建立连接, 可查到以下log:
```
......
[COAP-Client]: Enter Coap Client
*****The Product Key  : a1oD2MD64xV *****
*****The Device Name  : example1 *****
*****The Device Secret: i6Ad04DyBcF9GD9mQ5lpKh2oubErhjAV *****
*****The Device ID    : a1oD2MD64xV.example1 *****
[dbg] Cloud_CoAPUri_parse(31): The uri is coap-psk://a1oD2MD64xV.coap.cn-shanghai.link.aliyuncs.com:5682
[dbg] Cloud_CoAPUri_parse(69): The endpoint type is: 2
[dbg] Cloud_CoAPUri_parse(97): The host name is: a1oD2MD64xV.coap.cn-shanghai.link.aliyuncs.com
[dbg] Cloud_CoAPUri_parse(120): The port is: 5682
The host IP 47.101.22.127, port is 5682
[dbg] iotx_calc_sign_with_seq(91): The source string: clientIda1oD2MD64xV.example1deviceNameexample1productKeya1oD2MD64xVseq3444
[dbg] iotx_calc_sign_with_seq(95): The device name sign with seq: 1630981f4520039f6948dd8d2ff44068
[dbg] IOT_CoAP_DeviceNameAuth(572): The payload is: {"productKey":"a1oD2MD64xV","deviceName":"example1","clientId":"a1oD2MD64xV.example1","sign":"1630981f4520039f6948dd8d2ff44068","seq":"3444"}
[dbg] IOT_CoAP_DeviceNameAuth(573): Send authentication message to server
[dbg] Cloud_CoAPMessage_send(287): ----The message length 159-----
[dbg] Cloud_CoAPNetwork_write(103): [CoAP-NWK]: Network write return 159
[dbg] Cloud_CoAPMessage_send(294): Add message id 1 len 159 to the list
[dbg] Cloud_CoAPNetwork_read(135): << CoAP recv 98 bytes data
[dbg] Cloud_CoAPMessage_handle(382): Version     : 1
[dbg] Cloud_CoAPMessage_handle(383): Code        : 2.05(0x45)
[dbg] Cloud_CoAPMessage_handle(384): Type        : 0x2
[dbg] Cloud_CoAPMessage_handle(385): Msgid       : 1
[dbg] Cloud_CoAPMessage_handle(386): Option      : 0
[dbg] Cloud_CoAPMessage_handle(387): Payload Len : 89
[dbg] Cloud_CoAPMessage_handle(401): Receive CoAP Response Message,ID 1
[dbg] Cloud_CoAPRespMessage_handle(342): Find the node by token
[dbg] iotx_device_name_auth_callback(195): Receive response message:
[dbg] iotx_device_name_auth_callback(196): * Response Code : 0x45
[dbg] iotx_device_name_auth_callback(197): * Payload: {"random":"915ede6cbc8a059f","seqOffset":1,"token":"voPM9GYIXwvrSNzvHZlz00106e5300.1a12"}
[dbg] iotx_parse_auth_from_json(168): The src:i6Ad04DyBcF9GD9mQ5lpKh2oubErhjAV,915ede6cbc8a059f
[dbg] iotx_parse_auth_from_json(171): The key is:
[dbg] iotx_parse_auth_from_json(172): HEXDUMP key @ 0x7ffffb720b90[32]
+---------------+
| key: (len=32) |
+---------------------------------------------------------------------
| FB720B90: 6CD3 2B87 E65F 2881 29D6 5B27 6B8C 15A0  l.+.._(.).['k...
| FB720BA0: A02F 6D3A A4F2 6C70 1C56 E285 36F8 AC4B  ./m:..lp.V..6..K
+---------------------------------------------------------------------
[dbg] iotx_parse_auth_from_json(173): The short key:
[dbg] iotx_parse_auth_from_json(174): HEXDUMP p_iotx_coap->key @ 0x2261038[16]
+----------------------------+
| p_iotx_coap->key: (len=16) |
+---------------------------------------------------------------------
| 02261038: 29D6 5B27 6B8C 15A0 A02F 6D3A A4F2 6C70  ).['k..../m:..lp
+---------------------------------------------------------------------
[inf] iotx_device_name_auth_callback(209): CoAP authenticate success!!!
[dbg] Cloud_CoAPRespMessage_handle(354): Remove the message id 1 from list
*****The Product Key  : a1oD2MD64xV *****
*****The Device Name  : example1 *****
*****The Device Secret: i6Ad04DyBcF9GD9mQ5lpKh2oubErhjAV *****
*****The Device ID    : a1oD2MD64xV.example1 *****
[dbg] iotx_split_path_2_option(614): The uri is /topic/sys/a1oD2MD64xV/example1/thing/event/property/post
[dbg] iotx_split_path_2_option(621): path: topic,len=5
[dbg] iotx_split_path_2_option(621): path: sys,len=3
[dbg] iotx_split_path_2_option(621): path: a1oD2MD64xV,len=11
[dbg] iotx_split_path_2_option(621): path: example1,len=8
[dbg] iotx_split_path_2_option(621): path: thing,len=5
[dbg] iotx_split_path_2_option(621): path: event,len=5
[dbg] iotx_split_path_2_option(621): path: property,len=8
[dbg] iotx_split_path_2_option(631): path: post,len=4
[dbg] iotx_aes_cbc_encrypt(437): to encrypt src: 1, len: 16
[dbg] IOT_CoAP_SendMessage(706): HEXDUMP seq @ 0x7ffffb720f87[16]
+---------------+
| seq: (len=16) |
+---------------------------------------------------------------------
| FB720F87: 1EA3 756C 2CFF 521D 2353 0A18 DD9C B840  ..ul,.R.#S.....@
+---------------------------------------------------------------------
[dbg] iotx_aes_cbc_encrypt(437): to encrypt src: {"id": "1","version": "1.0","params": {"Data":"Post by CoAP"},"method": "thing.event.property.post"}, len: 112
[dbg] IOT_CoAP_SendMessage(720): HEXDUMP payload @ 0x2263a30[112]
+--------------------+
| payload: (len=112) |
+---------------------------------------------------------------------
| 02263A30: 2C80 4DBB 46D6 F555 DCD9 7A04 FFA7 702C  ,.M.F..U..z...p,
| 02263A40: 009E 96A9 388F A156 1632 FB63 E753 7CE9  ....8..V.2.c.S|.
| 02263A50: 7A59 CB1A 7379 376E 3951 7866 B774 5DBF  zY..sy7n9Qxf.t].
| 02263A60: 5980 FAFC 10C3 69E7 CAEF 0112 5714 CEA6  Y.....i.....W...
| 02263A70: 0CAF 4581 71A9 0EBC 5296 F07D F6A6 4E25  ..E.q...R..}..N%
| 02263A80: 5424 123E 1B3F E842 D45D 73F8 43A7 E150  T$.>.?.B.]s.C..P
| 02263A90: FAF3 5207 C20E 7D80 E407 2CF5 9EBC 0184  ..R...}...,.....
+---------------------------------------------------------------------
[dbg] Cloud_CoAPMessage_send(287): ----The message length 239-----
[dbg] Cloud_CoAPNetwork_write(103): [CoAP-NWK]: Network write return 239
[dbg] Cloud_CoAPMessage_send(294): Add message id 2 len 239 to the list
[dbg] Cloud_CoAPNetwork_read(135): << CoAP recv 19 bytes data
[dbg] Cloud_CoAPMessage_handle(382): Version     : 1
[dbg] Cloud_CoAPMessage_handle(383): Code        : 2.05(0x45)
[dbg] Cloud_CoAPMessage_handle(384): Type        : 0x2
[dbg] Cloud_CoAPMessage_handle(385): Msgid       : 2
[dbg] Cloud_CoAPMessage_handle(386): Option      : 1
[dbg] Cloud_CoAPMessage_handle(387): Payload Len : 0
[dbg] Cloud_CoAPMessage_handle(401): Receive CoAP Response Message,ID 2
[dbg] Cloud_CoAPRespMessage_handle(342): Find the node by token
[APPL]: Message response code: 0x45
[APPL]: Len: 0, Payload: (null)
[dbg] Cloud_CoAPRespMessage_handle(354): Remove the message id 2 from list
......
```
6. 这时, 在物联网控制台对应产品下的**设备管理 > 设备详情 > 运行状态**中可查看到设备上报的属性
![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/Linkkit_CoAP/coap_cloud_adv_03.png)
7. 同理, 用户可自行对接高级版/数据格式为透传的产品

# <a name="CoAP提供的API">CoAP提供的API</a>

| 函数名                                                      | 说明
|-------------------------------------------------------------|---------------------------------------------------------------------------------
| [IOT_CoAP_Init](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides#iot_coap_init)                             | CoAP实例的构造函数, 入参为`iotx_coap_config_t`结构体, 返回创建的CoAP会话句柄
| [IOT_CoAP_Deinit](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides#iot_coap_deinit)                         | CoAP实例的销毁函数, 入参为 [IOT_CoAP_Init](#IOT_CoAP_Init) 所创建的句柄
| [IOT_CoAP_DeviceNameAuth](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides#iot_coap_devicenameauth)         | 基于控制台申请的`DeviceName`, `DeviceSecret`, `ProductKey`做设备认证
| [IOT_CoAP_GetMessageCode](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides#iot_coap_getmessagecode)         | CoAP会话阶段, 从服务器的`CoAP Response`报文中获取`Respond Code`
| [IOT_CoAP_GetMessagePayload](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides#iot_coap_getmessagepayload)   | CoAP会话阶段, 从服务器的`CoAP Response`报文中获取报文负载
| [IOT_CoAP_SendMessage](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides#iot_coap_sendmessage)               | CoAP会话阶段, 连接已成功建立后调用, 组织一个完整的CoAP报文向服务器发送
| [IOT_CoAP_Yield](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides#iot_coap_yield)                           | CoAP会话阶段, 连接已成功建立后调用, 检查和收取服务器对`CoAP Request`的回复报文

# <a name="需要的HAL接口">需要的HAL接口</a>

| 函数名                                              | 说明
|-----------------------------------------------------|-----------------------------------------------------------------
| [HAL_DTLSSession_create](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires#hal_dtlssession_create)   | 初始化DTLS资源并建立一个DTLS会话, 用于CoAP功能
| [HAL_DTLSSession_free](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires#hal_dtlssession_free)       | 销毁一个DTLS会话并释放DTLS资源, 用于CoAP功能
| [HAL_DTLSSession_read](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires#hal_dtlssession_read)       | 从DTLS连接中读数据, 用于CoAP功能
| [HAL_DTLSSession_write](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires#hal_dtlssession_write)     | 向DTLS连接中写数据, 用于CoAP功能
| [HAL_Aes128_Cbc_Decrypt](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#hal_aes128_cbc_decrypt)   | AES128解密, CBC模式, 用于CoAP报文加解密
| [HAL_Aes128_Cbc_Encrypt](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#hal_aes128_cbc_encrypt)   | AES128加密, CBC模式, 用于CoAP报文加解密
| [HAL_Aes128_Cfb_Decrypt](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#hal_aes128_cfb_decrypt)   | AES128解密, CFB模式, 用于CoAP报文加解密
| [HAL_Aes128_Cfb_Encrypt](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#hal_aes128_cfb_encrypt)   | AES128加密, CFB模式, 用于CoAP报文加解密
| [HAL_Aes128_Destroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#hal_aes128_destroy)           | AES128反初始化
| [HAL_Aes128_Init](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#hal_aes128_init)                 | AES128初始化
| [HAL_UDP_close_without_connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires#hal_udp_close_without_connect)                     | 关闭一个UDP socket
| [HAL_UDP_create](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires#hal_udp_create)                   | 创建一个UDP socket
| [HAL_UDP_read](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires#hal_udp_read)                       | 阻塞的从一个UDP socket中读取数据包, 并返回读到的字节数
| [HAL_UDP_readTimeout](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires#hal_udp_readtimeout)         | 在指定时间内, 从一个UDP socket中读取数据包, 返回读到的字节数
| [HAL_UDP_write](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/CoAP_Requires#hal_udp_write)                     | 阻塞的向一个UDP socket中发送数据包, 并返回发送的字节数
