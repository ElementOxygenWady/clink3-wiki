# <a name="目录">目录</a>
+ [概述](#概述)
    * [自定义MQTT连接域名和端口号](#自定义MQTT连接域名和端口号)
        - [适用场景](#适用场景)
        - [编译期配置方式](#编译期配置方式)
        - [运行期调用方式](#运行期调用方式)
        - [示例运行日志](#示例运行日志)
    * [基于MQTT的动态注册/一型一密](#基于MQTT的动态注册/一型一密)
        - [适用场景](#适用场景)
        - [编译期配置方式](#编译期配置方式)
        - [运行期调用方式](#运行期调用方式)
        - [示例使用代码](#示例使用代码)
        - [示例运行日志](#示例运行日志)
    * [基于X.509证书的TLS连接](#基于X.509证书的TLS连接)
        - [适用场景](#适用场景)
        - [编译期配置方式](#编译期配置方式)
        - [运行期调用方式](#运行期调用方式)
        - [示例使用代码](#示例使用代码)
        - [示例运行日志](#示例运行日志)
    * [基于PSK方式的TLS连接](#基于PSK方式的TLS连接)
        - [适用场景](#适用场景)
        - [编译期配置方式](#编译期配置方式)
        - [运行期调用方式](#运行期调用方式)
        - [示例运行日志](#示例运行日志)

# <a name="概述">概述</a>

IoT设备与阿里云平台之间的连接, 在默认的形态之外, 还可以有诸多配置和变种, 以满足不同应用场景的需要

---
以下列出的连接参数配置, 相互没有排斥关系, 可以自由组合使用

## <a name="自定义MQTT连接域名和端口号">自定义MQTT连接域名和端口号</a>

### <a name="适用场景">适用场景</a>
1. 连接阿里云IOT其他行业线平台（如飞凤平台等）
2. 连接第三方MQTT服务器
3. 使用itls/id2建连
4. 内部日常/预发调试

### <a name="编译期配置方式">编译期配置方式</a>
+ MQTT直连方式，即直接使用MQTT连接指定域名：

  make menuconfig->打开 MQTT Configurations 中的 MQTT_DIRECT 开关（默认打开）

+ https预认证方式，即先通过https获取MQTT url再连接MQTT：

  make menuconfig->关闭 MQTT Configurations 中的 MQTT_DIRECT 开关

### <a name="运行期调用方式">运行期调用方式</a>

#### <a name="MQTT直连">MQTT直连</a>

仅使用MQTT建立连接，无需通过https动态获取MQTT url及port。

##### <a name="1.连接飞凤平台">1.连接飞凤平台</a>

  + MQTT基础版（直接使用MQTT API编程）:

```
iotx_mqtt_param_t       mqtt_params;

memset(&mqtt_params, 0x0, sizeof(mqtt_params));
mqtt_params.host = "mqtt.as.feifengiot.com";
mqtt_params.port = 1883;
IOT_MQTT_Construct(&mqtt_params);
```
  + 高级版（使用物模型）：

```
uint16_t custom_port = 1883;
IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void*)"mqtt.as.feifengiot.com");
IOT_Ioctl(IOTX_IOCTL_SET_MQTT_PORT, (void *)&custom_port);
```

##### <a name="2.连接第三方服务器">2.连接第三方服务器</a>

+ MQTT基础版（直接使用MQTT API编程）:

```
iotx_mqtt_param_t       mqtt_params;

memset(&mqtt_params, 0x0, sizeof(mqtt_params));
mqtt_params.host = "xxxxxx";
mqtt_params.port = xxxx;
mqtt_params.client_id = "xxxxx";
mqtt_params.username = "xxxxx";
mqtt_params.password = "xxxxx";

IOT_MQTT_Construct(&mqtt_params);

```
+ 高级版（使用物模型）：

不支持（物模型与阿里云IOT平台绑定）；

##### <a name="3.连接itls服务器">3.连接itls服务器</a>

+ MQTT基础版（直接使用MQTT API编程）:

```
iotx_mqtt_param_t       mqtt_params;
char id2_url[128] = {0};
char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0}; 

memset(&mqtt_params, 0x0, sizeof(mqtt_params)); 
IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, product_key);
snprintf(id2_url,127,"%s.itls.cn-shanghai.aliyuncs.com", product_key);
mqtt_params.host = id2_url;
mqtt_params.port = 1883;
mqtt_params.customize_info = "authtype=id2";
pclient = IOT_MQTT_Construct(&mqtt_params);

```

+ 高级版（使用物模型）：

```
char id2_url[128] = {0};
char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0}; 

IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, product_key);
snprintf(id2_url,127,"%s.itls.cn-shanghai.aliyuncs.com", product_key);
uint16_t custom_port = 1883;
IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void*)id2_url);
IOT_Ioctl(IOTX_IOCTL_SET_MQTT_PORT, (void *)&custom_port);
IOT_Ioctl(IOTX_IOCTL_SET_CUSTOMIZE_INFO, (void *)"authtype=id2");

```

##### <a name="4.内部日常/预发调试,先向云端同学沟通日常/预发的ip地址及端口号，然后按下文配置">4.内部日常/预发调试,先向云端同学沟通日常/预发的ip地址及端口号，然后按下文配置</a>

+ 基础版（直接使用MQTT API编程）：

```
iotx_mqtt_param_t       mqtt_params;
memset(&mqtt_params, 0x0, sizeof(mqtt_params));
mqtt_params.host = "xxx.xxx.xxx.xxx";
mqtt_params.port = xxxx;
IOT_MQTT_Construct(&mqtt_params);
```

+ 高级版（使用物模型）：

```
uint16_t custom_port = xxxx;
IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void*)"xxx.xxx.xxx.xxx");
IOT_Ioctl(IOTX_IOCTL_SET_MQTT_PORT, (void *)&custom_port);

```

#### <a name="https预认证和动态注册">https预认证和动态注册</a>
预认证方式仅支持连接阿里云IOT平台，常用于海外设备，先通过https建连动态获取mqtt连接所需的url;
且高级版跟基础版设置方式相同：
```
IOT_Ioctl(IOTX_IOCTL_SET_HTTP_DOMAIN, (void*)"xxx.xxx.xxx.xxx");
```

### <a name="示例运行日志">示例运行日志</a>

#### <a name="直连模式">直连模式</a>
通过-0x0327 事件可以得到当前mqtt的url,port，对比与自己设置的是否一致，一致则说明设置成功。
 
```
received state: -0x0327(a1RIsMLz2BJ.iot-as-mqtt.cn-shanghai.aliyuncs.com)
received state: -0x0327(443)
received state: -0x0327(_v=sdk-c-3.0.1,securemode=2,signmethod=hmacsha256,lan=C,_ss=1,g)
received state: -0x0327(example1&a1RIsMLz2BJ)
received state: -0x0327(962B1892E19A9BB0F740F6F5AC0769674D52F4AE30D138572C00CA2E56D8EFB)
Loading the CA root certificate ...
 ok (0 skipped)
start prepare client cert .
start mbedtls_pk_parse_key[(null)]
Connecting to /a1RIsMLz2BJ.iot-as-mqtt.cn-shanghai.aliyuncs.com/443...
setsockopt SO_SNDTIMEO timeout: 10s
connecting IP_ADDRESS: 139.196.135.135
 ok
  . Setting up the SSL/TLS structure...
 ok
Performing the SSL/TLS handshake...
 ok
  . Verifying peer X.509 certificate..
certificate verification result: 0x00
received state: -0x0000(mqtt connected in 686 ms)
```
#### <a name="预认证模式：">预认证模式：</a>

收到0x0606事件，说明当前使能了https预认证，附加的url及port是否为自己设置的值。

```
received state: -0x0606(http://iot-auth.cn-shanghai.aliyuncs.com/auth/devicename)
received state: -0x0606(443)
received state: -0x0606(10000)
Loading the CA root certificate ...
 ok (0 skipped)
start prepare client cert .
start mbedtls_pk_parse_key[(null)]
Connecting to /iot-auth.cn-shanghai.aliyuncs.com/443...
setsockopt SO_SNDTIMEO timeout: 10s
connecting IP_ADDRESS: 139.196.135.162
 ok
  . Setting up the SSL/TLS structure...
 ok
Performing the SSL/TLS handshake...
 ok
  . Verifying peer X.509 certificate..
certificate verification result: 0x00
received state: -0x0606(preauth done in 255 ms - ret: 216)
need release client crt&key
ssl_disconnect

< {
<     "code": 200,
<     "data": {
<         "iotId": "pKHxzGq1N25r44L7oK570010743500",
<         "iotToken": "a14236bde4434fa78b4bdb6abdda2cce",
<         "resources": {
<             "mqtt": {
<                 "host": "public.iot-as-mqtt.cn-shanghai.aliyuncs.com",
<                 "port": 1883
<             }
<         }
<     },
<     "message": "success"
< }

received state: -0x0607(code: 200)
received state: -0x0607(host: public.iot-as-mqtt.cn-shanghai.aliyuncs.com)
received state: -0x0607(port: 1883)
received state: -0x0327(public.iot-as-mqtt.cn-shanghai.aliyuncs.com)
received state: -0x0327(1883)
received state: -0x0327(_v=sdk-c-3.0.1,securemode=-1,signmethod=hmacsha256,lan=C,_ss=1,)
received state: -0x0327(pKHxzGq1N25r44L7oK570010743500)
received state: -0x0327(a14236bde4434fa78b4bdb6abdda2cce)
Loading the CA root certificate ...
 ok (0 skipped)
start prepare client cert .
start mbedtls_pk_parse_key[(null)]
Connecting to /public.iot-as-mqtt.cn-shanghai.aliyuncs.com/1883...
setsockopt SO_SNDTIMEO timeout: 10s
connecting IP_ADDRESS: 139.196.135.135
 ok
  . Setting up the SSL/TLS structure...
 ok
Performing the SSL/TLS handshake...
 ok
  . Verifying peer X.509 certificate..
certificate verification result: 0x00
received state: -0x0000(mqtt connected in 681 ms)
```

## <a name="基于MQTT的动态注册/一型一密">基于MQTT的动态注册/一型一密</a>

### <a name="适用场景">适用场景</a>
一型一密功能也称为直连设备动态注册功能, 用户只需在同一类产品固件中写入相同的 ProductKey 和 ProductSecret, 即可通过动态注册的方式在首次建连时从云端服务器获取设备的 DeviceSecret, 而不必为不同设备烧录不同的三元组

### <a name="编译期配置方式">编译期配置方式</a>
编译的时候，通过下图方式选中
<img src="https://linkkit-export.oss-cn-shanghai.aliyuncs.com/kuanju/%E5%8A%A8%E6%80%81%E6%B3%A8%E5%86%8C.png" width="800" height="600" />

### <a name="运行期调用方式">运行期调用方式</a>
通过下述调用获取到device_secrete, 存储于meta数据结构中
```
res = IOT_Dynamic_Register(region, &meta);
```

### <a name="示例使用代码">示例使用代码</a>
一型一密功能的例子程序在 `src/dynamic_register/examples/dynreg_example.c`, 以下对其逐段讲解

#### <a name="1.要使用一型一密功能, 要包含它的头文件 `dynreg_api.h`">1.要使用一型一密功能, 要包含它的头文件 `dynreg_api.h`</a>

    #include <stdio.h>
    #include <string.h>
    #include "infra_types.h"
    #include "infra_defs.h"
    #include "dynreg_api.h"
#### <a name="2.准备输入参数 `region` 和出入参结构体 `meta`">2.准备输入参数 `region` 和出入参结构体 `meta`</a>

    iotx_http_region_types_t region = IOTX_HTTP_REGION_SHANGHAI;
    HAL_Printf("dynreg example\n");

    memset(&meta,0,sizeof(iotx_dev_meta_info_t));
    HAL_GetProductKey(meta.product_key);
    HAL_GetProductSecret(meta.product_secret);
    HAL_GetDeviceName(meta.device_name);

以上例子程序用 `IOTX_CLOUD_REGION_SHANGHAI` 代表的华东二站点作为例子, 演示连接上海服务器时候的情况, 另一个入参 `meta` 其实就是填入设备的 `PK/PS/DN`

#### <a name="3.调用一型一密的API获取 `DeviceSecret`">3.调用一型一密的API获取 `DeviceSecret`</a>

    res = IOT_Dynamic_Register(region, &meta);
    if (res < 0) {
        HAL_Printf("IOT_Dynamic_Register failed\n");
        return -1;
    }

    HAL_Printf("\nDevice Secret: %s\n\n", meta.device_secret);

这个 `IOT_Dynamic_Register()` 接口就是一型一密功能点唯一提供的用户接口, 若执行成功, 在参数 `meta` 中将填上从服务器成功获取到的 `DeviceSecret`

### <a name="示例运行日志">示例运行日志</a>
```
[07:01:50 PM] Connecting to /iot-auth.cn-shanghai.aliyuncs.com/443...
[07:01:50 PM] setsockopt SO_SNDTIMEO timeout: 10s
[07:01:50 PM] connecting IP_ADDRESS: 106.15.100.152
[07:01:50 PM]  ok
[07:01:50 PM]   . Setting up the SSL/TLS structure...
[07:01:50 PM]  ok
[07:01:50 PM] Performing the SSL/TLS handshake...
[07:01:50 PM]  ok
[07:01:50 PM]   . Verifying peer X.509 certificate..
[07:01:50 PM] certificate verification result: 0x00
[07:01:50 PM] [dbg] httpclient_connect(427): rc = client->net.connect() = 0, success @ [1/3] iteration
[07:01:50 PM] [dbg] _http_send_header(171): REQUEST (Length: 211 Bytes)
[07:01:50 PM] > POST /auth/register/device HTTP/1.1
[07:01:50 PM] > Host: iot-auth.cn-shanghai.aliyuncs.com
[07:01:50 PM] > Accept: text/xml,text/javascript,text/html,application/json
[07:01:50 PM] > Content-Type: application/x-www-form-urlencoded
[07:01:50 PM] > Content-Length: 162
[07:01:50 PM] > 
[07:01:50 PM] [dbg] _http_send_userdata(191): client_data->post_buf: productKey=a1J3wtoxPRb&deviceName=mytestauto003&random=8Ygb7ULYh53B6OA&sign=87740D2D82A6E1EBBBFF840B49C361982B0FC02F39EA48D896D123C7DAB95DE7&signMethod=hmacsha256, ret is 162
[07:01:50 PM] [inf] _http_recv(214): ret of _http_recv is 32
[07:01:50 PM] [dbg] httpclient_recv_response(488): RESPONSE (Length: 32 Bytes)
[07:01:50 PM] < HTTP/1.1 200 OK
[07:01:50 PM] < Server: Tengine
[07:01:50 PM] [dbg] _http_parse_response_header(374): Reading headers: HTTP/1.1 200 OK
[07:01:50 PM] [inf] _http_recv(214): ret of _http_recv is 32
[07:01:50 PM] [inf] _http_recv(214): ret of _http_recv is 32
[07:01:50 PM] [inf] _http_recv(214): ret of _http_recv is 32
[07:01:50 PM] [inf] _http_recv(214): ret of _http_recv is 32
[07:01:50 PM] [inf] _http_recv(214): ret of _http_recv is 129
[07:01:50 PM] [dbg] _http_get_response_body(326): Total- remaind Payload: 129 Bytes; currently Read: 129 Bytes
[07:01:50 PM] need release client crt&key
[07:01:50 PM] ssl_disconnect
[07:01:50 PM] [inf] httpclient_close(503): client disconnected
[07:01:50 PM] [inf] _fetch_dynreg_http_resp(231): Http Response Payload: {"code":200,"data":{"deviceName":"mytestauto003","deviceSecret":"6nJGXBFtJKkDRd4D24fn8SsgNZJJIRCy","productKey":"a1J3wtoxPRb"},"message":"success"}
[07:01:50 PM] [inf] _fetch_dynreg_http_resp(234): Dynamic Register Code: 200
[07:01:50 PM] [inf] _fetch_dynreg_http_resp(245): Dynamic Register Device Secret: 6nJGXBFtJKkDRd4D24fn8SsgNZJJIRCy
[07:01:50 PM] [dbg] _http_dynamic_register(305): Downstream Payload:
[07:01:50 PM] 
[07:01:50 PM] < {
[07:01:50 PM] <     "code": 200,
[07:01:50 PM] <     "data": {
[07:01:50 PM] <         "deviceName": "mytestauto003",
[07:01:50 PM] <         "deviceSecret": "6nJGXBFtJKkDRd4D24fn8SsgNZJJIRCy",
[07:01:50 PM] <         "productKey": "a1J3wtoxPRb"
[07:01:50 PM] <     },
[07:01:50 PM] <     "message": "success"
[07:01:50 PM] < }
[07:01:50 PM] 
```

## <a name="基于X.509证书的TLS连接">基于X.509证书的TLS连接</a>

### <a name="适用场景">适用场景</a>
X.509是一种使用广泛的数字证书的标准，是互联网行业的实现身份认证、数字签名的通用选择。AWS、Azure都为IoT设备提供了基于X.509证书的认证方式。部分低功耗的设备、富资源的IOT设备，可以通过X.509证书连云。

### <a name="编译期配置方式">编译期配置方式</a>
Linkkit SDK默认的配置方式

### <a name="运行期调用方式">运行期调用方式</a>
1. 创建设备所属产品的时候，选择x509认证方式


2. 要将pk/dn/ds设置为空


3. 要将目标域名地址设置为x509.itls.cn-shanghai.aliyuncs.com


4. 在HAL_TLS_mbedtls.c中输入产品的秘钥和证书


### <a name="示例使用代码">示例使用代码</a>
x509使用方法, 以mqtt_example.c为例:

- 在mqtt_example.c中, 将g_product_key, g_device_name, g_device_secret置为空字符串, 即:

```
char g_product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "";
char g_device_name[IOTX_DEVICE_NAME_LEN + 1]       = "";
char g_device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "";
```

- 在mqtt_example.c中, 注册ITE_IDENTITY_RESPONSE事件, 在mqtt建联成功后, 该事件中可收到云端下发的product key和device name:

```
int identity_response_handle(const char *payload)
{
    EXAMPLE_TRACE("identify: %s", payload);

    return 0;
}

IOT_RegisterCallback(ITE_IDENTITY_RESPONSE, identity_response_handle);
```

- 在HAL_TLS_mbedtls.c中, 将g_cli_crt替换为云端颁发的设备x509证书, 将g_cli_key替换为云端颁发的设备私钥

- 使用x509证书时, 域名需要自行设置, 如下:

```
...
memset(&mqtt_params, 0x0, sizeof(mqtt_params));
mqtt_params.host = "x509.itls.cn-shanghai.aliyuncs.com";
mqtt_params.port = 1883;
...
```

设备的证书可私钥可在LP控制台设备详情页获取, 注意创建产品时选择x509认证方式

```
const char *g_cli_crt = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDhzCCAm+gAwIBAgIHNHa/qHDMmTANBgkqhkiG9w0BAQsFADBTMSgwJgYDVQQD\r\n"
    "DB9BbGliYWJhIENsb3VkIElvVCBPcGVyYXRpb24gQ0ExMRowGAYDVQQKDBFBbGli\r\n"
    "YWJhIENsb3VkIElvVDELMAkGA1UEBhMCQ04wIBcNMTkxMDE3MDExODE3WhgPMjEx\r\n"
    "OTEwMTcwMTE4MTdaMFExJjAkBgNVBAMMHUFsaWJhYmEgQ2xvdWQgSW9UIENlcnRp\r\n"
    "ZmljYXRlMRowGAYDVQQKDBFBbGliYWJhIENsb3VkIElvVDELMAkGA1UEBhMCQ04w\r\n"
    "ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCsNkQ57z1lpe/dSQqHmrfm\r\n"
    "2Hml0YPhac8yXdx1brUqzWg+robUsK8bxyr1yqlTL/aNNT6Kjeq8m8pZv/fEy42o\r\n"
    "WmQogrJ8nBYCdXGOYmfsdDrcvInkde8jiRLRduW+/aSeGqkWiKoInqvvSFRelGNv\r\n"
    "N9wZ7uRcL16jJGQFpvlowps/+Lkg6vNiJ0mjKLQRy3bjrwEazLBijfI5oyDQAcXj\r\n"
    "J4GEdA1vw/ZxQJkrI2ZXp2bfc3q1QciWb8mQDqNIG9DldfRddS2Vj4iDI3TNoFFf\r\n"
    "PF7eoSbycjaVfXnJY+cflapnwm/1LoCUNEi7ut25PzqP5JJbhV1XYg4A/OED1NRT\r\n"
    "AgMBAAGjYDBeMB8GA1UdIwQYMBaAFIo3m6hwzdX5SMiXfiGfWW9JjiQRMB0GA1Ud\r\n"
    "DgQWBBRtrOWad4BnTKm9l5n2scfjaUEoTjAOBgNVHQ8BAf8EBAMCA/gwDAYDVR0T\r\n"
    "AQH/BAIwADANBgkqhkiG9w0BAQsFAAOCAQEAiug/d0unpbHHtsg6RKrmIK/VLvAd\r\n"
    "F8Mjkohv8XBxu+8xqIb5JXsUEtdHWDHBXubmQvvh4OIVxByx967fUFT8u/mZFRwl\r\n"
    "banwK9wQgua3Y5M84k3jnWo3PfKERCOZ0DqY4ARjv8gZnZ1UEzDeslbft9SXwK5F\r\n"
    "1OtDmwAez+Kr+8Zw/r2bKu70eq5A+KAIF9KJEnLnxNrtAU7cw7BjfNGCYa3DswlC\r\n"
    "5cEdQe3llFl8RZJtznWdttQt5yJwweMEz9D4ac+aS8Kjiav51MC/DyBc7u4IeDYe\r\n"
    "1R1ufWK+qRKZKanTqWA3PS0pF54jmSwiRzQanmCST7nyKCIqoZXZJg476g==\r\n"
    "-----END CERTIFICATE-----\r\n"
};

const char *g_cli_key = \
{
    \
    "-----BEGIN RSA PRIVATE KEY-----\r\n"
    "MIIEpAIBAAKCAQEArDZEOe89ZaXv3UkKh5q35th5pdGD4WnPMl3cdW61Ks1oPq6G\r\n"
    "1LCvG8cq9cqpUy/2jTU+io3qvJvKWb/3xMuNqFpkKIKyfJwWAnVxjmJn7HQ63LyJ\r\n"
    "5HXvI4kS0Xblvv2knhqpFoiqCJ6r70hUXpRjbzfcGe7kXC9eoyRkBab5aMKbP/i5\r\n"
    "IOrzYidJoyi0Ect2468BGsywYo3yOaMg0AHF4yeBhHQNb8P2cUCZKyNmV6dm33N6\r\n"
    "tUHIlm/JkA6jSBvQ5XX0XXUtlY+IgyN0zaBRXzxe3qEm8nI2lX15yWPnH5WqZ8Jv\r\n"
    "9S6AlDRIu7rduT86j+SSW4VdV2IOAPzhA9TUUwIDAQABAoIBAEs5LRYtZaFIGX2M\r\n"
    "kTfooloIxyJiYXwOLv5uN4VoRWSb8uQmcijnwKWemypgSxRWSBkoX9g/44BdgMcx\r\n"
    "bnYdABTA3y0+4d5Cg+6xBFhQrdqAbtHK3WTZCwt3raunhvWlb8nMFX4NVlCLlsjf\r\n"
    "zFK/+Ks1OERvsHqACpOyqqo5ZIqYdiJ/onjW2FmR6v6midB2qKch9pyCTZGoxhJb\r\n"
    "8KgVYUzUoTxLgwwLzOdaZSCieHLJA67GtUDVyLPN0eD8apNlhdmCXCVUMgixNGuE\r\n"
    "Mz71GHnPi+uLLUUige7m1Qk9qTN/deilRgJ1nOp4AWu7sHbkilVXY7O47C6vFLWv\r\n"
    "NuGqplECgYEA7xkOfdS1TWbV/wL0QWCQWSagx+EjnpuxxIikyopFB8EOb3GlAN81\r\n"
    "DQzhmBWbrR8mqfGdjkqGy/XZyQjI7Apyyv1HJlwiPqsihULMs0vHZGxK7aQ7C06/\r\n"
    "yOgsiS3REF4paTUKqilCB+fpCdT8w1r+sT8APAb5XDP+iWSAVeO7LgsCgYEAuGLH\r\n"
    "Ye84tH3puN172b80OGqJX9ytDFcML08N0QcBKwjMx9dw6oh3cBNG4bjcjWZPzMSt\r\n"
    "owUkXuImAfhLmFGp+kyWqs9Soom1m5dTt6/Twwbii9u/243OtpmuKH1Jpg194H3n\r\n"
    "2ebl4uI1qMQpGXpHNukyKhKCVFNtEvrLwIOJh9kCgYEA1Ri167wqooOm8pOFcsdH\r\n"
    "n81nuv2Tq4cEVy+Z/9JgV9rerMDS2QYW2DzEy7FmgFOlOA3Z6LnPjfTeV+h2ermN\r\n"
    "OhGm2i9/be9E4xR4MMy3N2+1EDwxS5CyKbOUDedBz568tcHKcYI4EafF8Tdc2Bg/\r\n"
    "q2WFJfrpQB4vG/Dw2JwRdaUCgYEAtiV2e0ubg8ni6il54KgeySCNZrKUDF2tfsqk\r\n"
    "hnpZ8A44Za5xMG7xNjkEy5UX42NPr6QXN7SrskPLciS7YNRDiyy/pis6obBSRErg\r\n"
    "e9Po6H9Tqv/Bbv2OWBpGzl6+M5ieYfde44ZSUKhQg9CMHUwLL7Z8RM2WDAGB/b7d\r\n"
    "KueGJDkCgYBJzldXgBqOVDgoWLpcuMSZD8bvHHRsY0RSj3Otx4utKOw4ILEXAIyb\r\n"
    "d6BpbrZAinsNWOFXfpIeOobgs+VGOYClni4ULmW4XzRtb1n+9AWP33sMjr4xt8Qs\r\n"
    "fMLnYxXHzu/+uCVEpR3XLL0+bwMDdSqYRL9tc9EkQbvLzvZALl4flA==\r\n"
    "-----END RSA PRIVATE KEY-----\r\n"
};
```

### <a name="示例运行日志">示例运行日志</a>
```
xicai.cxc@r10c05067:~/srcs/core_sdk$ ./output/release/bin/mqtt-example
main|166 :: mqtt example
[wrn] IOT_MQTT_Construct(289): Using default client_id: .|timestamp=2524608000000,_v=sdk-c-3.0.1,securemode=2,signmethod=hmacsha256,lan=C,_ss=1,gw=0,ext=0|
[wrn] IOT_MQTT_Construct(296): Using default username: &
[wrn] IOT_MQTT_Construct(304): Using default password: ******
[wrn] IOT_MQTT_Construct(317): Using default request_timeout_ms: 5000, configured value(0) out of [1000, 10000]
[wrn] IOT_MQTT_Construct(332): Using default keepalive_interval_ms: 60000, configured value(0) out of [30000, 1200000]
[wrn] IOT_MQTT_Construct(338): Using default read_buf_size: 3072
[wrn] IOT_MQTT_Construct(344): Using default write_buf_size: 3072
everything_state_handle|128 :: recv -0x0327(x509.itls.cn-shanghai.aliyuncs.com)
everything_state_handle|128 :: recv -0x0327(1883)
everything_state_handle|128 :: recv -0x0327(_v=sdk-c-3.0.1,securemode=2,signmethod=hmacsha256,lan=C,_ss=1,g)
everything_state_handle|128 :: recv -0x0327(&)
everything_state_handle|128 :: recv -0x0327(B36DABAE2EBB7B8FA6FFD92B217E13D97C1AB196441561B0F71B3C3BF1DACA0)
[dbg] _mqtt_nwk_connect(2708): calling TCP or TLS connect HAL for [1/3] iteration
Loading the CA root certificate ...
 ok (0 skipped)
start prepare client cert .
start mbedtls_pk_parse_key[(null)]
Connecting to /x509.itls.cn-shanghai.aliyuncs.com/1883...
setsockopt SO_SNDTIMEO timeout: 10s
connecting IP_ADDRESS: 106.15.100.171
 ok
  . Setting up the SSL/TLS structure...
 ok
Performing the SSL/TLS handshake...
 ok
  . Verifying peer X.509 certificate..
certificate verification result: 0x00
[dbg] _mqtt_nwk_connect(2726): rc = pClient->ipstack.connect() = 0, success @ [1/3] iteration
[inf] _mqtt_connect(722): connect params: MQTTVersion=4, clientID=.|timestamp=2524608000000,_v=sdk-c-3.0.1,securemode=2,signmethod=hmacsha256,lan=C,_ss=1,gw=0,ext=0|, keepAliveInterval=120, username=&
[inf] _mqtt_connect(768): mqtt connect success!
[dbg] iotx_mc_cycle(1565): PUBLISH
```


## <a name="基于PSK方式的TLS连接">基于PSK方式的TLS连接</a>

### <a name="适用场景">适用场景</a>
PSK即pre-shared keys缩写，是一种轻量级的TLS认证方式。主要适用于以下场景：
+ 网络带宽小，对建连时间及稳定性有要求（无需交换大的证书，建连流量小）；
+ 设备端资源拮据（psk方式可以节省证书所占内存开销）；


### <a name="编译期配置方式">编译期配置方式</a>
打开文件 wrappers/tls/HAL_TLS_mbedtls.c
将 TLS_AUTH_MODE 定义为 TLS_AUTH_MODE_PSK
```
#define TLS_AUTH_MODE_CA        0
#define TLS_AUTH_MODE_PSK       1

#ifndef TLS_AUTH_MODE
#define TLS_AUTH_MODE           TLS_AUTH_MODE_PSK
#endif
```

### <a name="运行期调用方式">运行期调用方式</a>

目前psk方式仅支持{pk}.itls.cn-shanghai.aliyuncs.com及{pk}.iot-as-mqtt.cn-shanghai.aliyuncs.com这两个域名。
故需先设置对应的域名：
+ 基础版：

```
iotx_mqtt_param_t       mqtt_params;
char psk_url[128] = {0};
char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0}; 

memset(&mqtt_params, 0x0, sizeof(mqtt_params)); 
IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, product_key);
snprintf(psk_url,127,"%s.itls.cn-shanghai.aliyuncs.com", product_key);
mqtt_params.host = psk_url;
mqtt_params.port = 1883;
pclient = IOT_MQTT_Construct(&mqtt_params);

```

+ 高级版

```
char psk_url[128] = {0};
char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0}; 
uint16_t custom_port = 1883;
IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, product_key);
snprintf(psk_url,127, "%s.itls.cn-shanghai.aliyuncs.com", product_key);
IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void*)psk_url);
IOT_Ioctl(IOTX_IOCTL_SET_MQTT_PORT, (void *)&custom_port);
```

### <a name="示例运行日志">示例运行日志</a>

```
[prt] log level set as: [ 5 ]
[wrn] IOT_MQTT_Construct(267): Using default hostname: 'a1RIsMLz2BJ.itls.cn-shanghai.aliyuncs.com'
[wrn] IOT_MQTT_Construct(281): Using default client_id: a1RIsMLz2BJ.example1|timestamp=2524608000000,_v=sdk-c-3.0.1,securemode=2,signmethod=hmacsha256,lan=C,_ss=1,gw=0,ext=0|
[wrn] IOT_MQTT_Construct(288): Using default username: example1&a1RIsMLz2BJ
[wrn] IOT_MQTT_Construct(296): Using default password: ******
received state: -0x0327(a1RIsMLz2BJ.itls.cn-shanghai.aliyuncs.com)
received state: -0x0327(1883)
received state: -0x0327(_v=sdk-c-3.0.1,securemode=2,signmethod=hmacsha256,lan=C,_ss=1,g)
received state: -0x0327(example1&a1RIsMLz2BJ)
received state: -0x0327(962B1892E19A9BB0F740F6F5AC0769674D52F4AE30D138572C00CA2E56D8EFB)
[dbg] _mqtt_nwk_connect(2708): calling TCP or TLS connect HAL for [1/3] iteration
Connecting to /a1RIsMLz2BJ.itls.cn-shanghai.aliyuncs.com/1883...
setsockopt SO_SNDTIMEO timeout: 10s
connecting IP_ADDRESS: 139.196.135.153
 ok
  . Setting up the SSL/TLS structure...
 ok
mbedtls psk config finished
Performing the SSL/TLS handshake...
 ok
  . Verifying peer X.509 certificate..
certificate verification result: 0x00
[dbg] _mqtt_nwk_connect(2726): rc = pClient->ipstack.connect() = 0, success @ [1/3] iteration
[inf] _mqtt_connect(722): connect params: MQTTVersion=4, clientID=a1RIsMLz2BJ.example1|timestamp=2524608000000,_v=sdk-c-3.0.1,securemode=2,signmethod=hmacsha256,lan=C,_ss=1,gw=0,ext=0|, keepAliveInterval=60, username=example1&a1RIsMLz2BJ
[inf] _mqtt_connect(768): mqtt connect success!


```
