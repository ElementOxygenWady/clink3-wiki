# <a name="目录">目录</a>
+ [概述](#概述)
    * [共享密钥](#共享密钥)
        - [编译期配置方式](#编译期配置方式)
        - [运行期调用方式](#运行期调用方式)
        - [示例运行日志](#示例运行日志)
    * [一型一密](#一型一密)
        - [编译期配置方式](#编译期配置方式)
        - [运行期调用方式](#运行期调用方式)
        - [示例代码段](#示例代码段)
        - [示例运行日志](#示例运行日志)
    * [ID2](#ID2)
        - [编译期配置方式](#编译期配置方式)
        - [运行期调用方式](#运行期调用方式)
    * [X.509证书](#X.509证书)
        - [编译期配置方式](#编译期配置方式)
        - [运行期调用方式](#运行期调用方式)
        - [示例代码段](#示例代码段)
        - [示例运行日志](#示例运行日志)

# <a name="概述">概述</a>

为确保信息安全, 阿里云IoT平台需藉由不同的手段, 确认当前连接过来的IoT设备拥有合法的身份

---
以下几种认证方式都是IoT平台认证设备的方式, 它们是互斥的, 取其中任何一种认证成功即可使用阿里云IoT平台提供的云端服务

## <a name="共享密钥">共享密钥</a>

### <a name="编译期配置方式">编译期配置方式</a>

1. 运行make menuconfig，使用默认的编译配置选项即可。
2. 修改HAL层配置选项，在`wrappers/tls/HAL_TLS_mbedtls.c`中，将`#define TLS_AUTH_MODE TLS_AUTH_MODE_CA`修改为`#define TLS_AUTH_MODE TLS_AUTH_MODE_PSK`。
3. 修改mbedtls库的PSK最大长度默认值，将`ssl.h`中的`MBEDTLS_PSK_MAX_LEN`define为64。

### <a name="运行期调用方式">运行期调用方式</a>

以`src/mqtt/examples/mqtt_example.c`为例，修改服务器域名和端口，PSK模式目前指定连接域名为`{$productKey}.itls.cn-shanghai.aliyuncs.com`，端口号为1883:

```
    mqtt_params.handle_event.h_fp = example_event_handle;
    mqtt_params.host = "a1MZxOdcBnO.itls.cn-shanghai.aliyuncs.com";
    mqtt_params.port = 1883;
    pclient = IOT_MQTT_Construct(&mqtt_params);
```

### <a name="示例运行日志">示例运行日志</a>

编译后运行`./output/release/bin/mqtt-example`，日志如下：
```
main|158 :: mqtt example
everything_state_handle|120 :: recv -0x0327(a1MZxOdcBnO.itls.cn-shanghai.aliyuncs.com)
everything_state_handle|120 :: recv -0x0327(1883)
everything_state_handle|120 :: recv -0x0327(_v=sdk-c-3.0.1,securemode=2,signmethod=hmacsha256,lan=C,_ss=1,g)
everything_state_handle|120 :: recv -0x0327(example1&a1MZxOdcBnO)
everything_state_handle|120 :: recv -0x0327(E157285389659C45DDB66E19B5C2385377E3CE627C712129DEB32263D3ACB22)
Connecting to /a1MZxOdcBnO.itls.cn-shanghai.aliyuncs.com/1883...
setsockopt SO_SNDTIMEO timeout: 10s
connecting IP_ADDRESS: 106.15.100.129
 ok
  . Setting up the SSL/TLS structure...
 ok
mbedtls psk config finished
Performing the SSL/TLS handshake...
 ok
  . Verifying peer X.509 certificate..
certificate verification result: 0x00
everything_state_handle|120 :: recv -0x0000(mqtt connected in 675 ms)
```

## <a name="一型一密">一型一密</a>

### <a name="编译期配置方式">编译期配置方式</a>

运行make menuconfig，在图形化界面中选中`FEATURE_DYNAMIC_REGISTER`

![image](http://git.cn-hangzhou.oss-cdn.aliyun-inc.com/uploads/Apsaras64/pub/ceba7a51bb099b5b5462adcab9877f10/image.png)

### <a name="运行期调用方式">运行期调用方式</a>

一型一密功能的例子程序在 `src/dynamic_register/examples/dynreg_example.c`，运行前填入自己设备的product key，product secret和device name，device secret留空即可

```
char g_product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "xxx";
char g_product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "xxx";
char g_device_name[IOTX_DEVICE_NAME_LEN + 1]       = "xxx";
char g_device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "";
```

### <a name="示例代码段">示例代码段</a>

如例子程序`src/dynamic_register/examples/dynreg_example.c`所示：

```
int main(int argc, char *argv[])
{
    int32_t res = 0;
    iotx_dev_meta_info_t meta;
    iotx_http_region_types_t region = IOTX_HTTP_REGION_SHANGHAI;
    
    HAL_Printf("dynreg example\n");

    memset(&meta, 0, sizeof(iotx_dev_meta_info_t));
    memcpy(meta.product_key, g_product_key, strlen(g_product_key));
    memcpy(meta.product_secret, g_product_secret, strlen(g_product_secret));
    memcpy(meta.device_name, g_device_name, strlen(g_device_name));

    res = IOT_Dynamic_Register(region, &meta);
    if (res < 0) {
        HAL_Printf("IOT_Dynamic_Register failed\n");
        return -1;
    }

    HAL_Printf("\nDevice Secret: %s\n\n", meta.device_secret);

    return 0;
}
```

### <a name="示例运行日志">示例运行日志</a>

编译完成后，运行`output/release/bin/dynreg-example`，正常运行日志如下

```
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
need release client crt&key
ssl_disconnect

< {
<     "code": 200,
<     "data": {
<         "deviceName": "example1",
<         "deviceSecret": "xvrQk38ZCJ6bdClB3E0XpY4JdPzvoq3g",
<         "productKey": "a1ZETBPbycq"
<     },
<     "message": "success"
< }


Device Secret: xvrQk38ZCJ6bdClB3E0XpY4JdPzvoq3g
```

需要注意的是，在使用一型一密拿到设备的device secret，并使用该设备上线后，就不能再次获取device secret，否则会报如下错误

```
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
need release client crt&key
ssl_disconnect

< {
<     "code": 6289,
<     "message": "device is already active"
< }

IOT_Dynamic_Register failed
```

## <a name="ID2">ID2</a>

### <a name="编译期配置方式">编译期配置方式</a>

1. 运行make menuconfig，使用默认的编译配置选项即可。
2. 用户需使用ID2官方提供的安全库，并适配SDK的TLS HAL层接口，即`HAL_SSL_Establish`,`HAL_SSL_Read`,`HAL_SSL_Write`,`HAL_SSL_Destroy`这4个接口。

### <a name="运行期调用方式">运行期调用方式</a>

需在用户层API配置ITLS服务的域名和端口号。
+ 如果使用Linkkit API，需在建连前做以下配置:
    ```
        IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)"${pk}.itls.cn-shanghai.aliyuncs.com");
        IOT_Ioctl(IOTX_IOCTL_SET_CUSTOMIZE_INFO, (void *)"authtype=id2");
    ```
+ 如果使用MQTT API，需在建连前做以下配置:
    ```
        mqtt_params.host = "${pk}.itls.cn-shanghai.aliyuncs.com");
        mqtt_params.customize_info = "authtype=id2"; /* 增加此项配置 */
        pclient = IOT_MQTT_Construct(&mqtt_params);
    ```

## <a name="X.509证书">X.509证书</a>

### <a name="编译期配置方式">编译期配置方式</a>

目前TLS默认使能，menuconfig编译选项无需配置

### <a name="运行期调用方式">运行期调用方式</a>

- 从阿里云物联网平台的设备详情中获得设备端的X.509证书和私钥，然后修改`wrappers/tls/HAL_TLS_mbedtls.c`，将`HAL_TLS_mbedtls.c`中的`g_cli_crt`和`g_cli_key`分别替换为从物联网平台获取的设备证书和私钥

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

- 修改`src/mqtt/examples/mqtt_example.c`，将product key、device name和device secret置为空字符串，并手动设置域名为`x509.itls.cn-shanghai.aliyuncs.com`，端口号为`1883`

### <a name="示例代码段">示例代码段</a>

在`src/mqtt/examples/mqtt_example.c`中，做如下修改

```
char g_product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "";
char g_product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "";
char g_device_name[IOTX_DEVICE_NAME_LEN + 1]       = "";
char g_device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "";

...
...

int identity_response_handle(const char *payload)
{
    EXAMPLE_TRACE("identify: %s", payload);

    return 0;
}

...
...

int main(int argc, char *argv[])
{
    ...
    ...

    /* Initialize MQTT parameter */
    /*
     * Note:
     *
     * If you did NOT set value for members of mqtt_params, SDK will use their default values
     * If you wish to customize some parameter, just un-comment value assigning expressions below
     *
     **/
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.host = "x509.itls.cn-shanghai.aliyuncs.com";
    mqtt_params.port = 1883;

    ...
    ...

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, g_product_key);
    IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_NAME, g_device_name);

    ...
    ...
```

### <a name="示例运行日志">示例运行日志</a>

编译成功后，运行`output/release/bin/mqtt-example`，正常运行日志如下：

```
main|158 :: mqtt example
everything_state_handle|120 :: recv -0x0327(x509.itls.cn-shanghai.aliyuncs.com)
everything_state_handle|120 :: recv -0x0327(1883)
everything_state_handle|120 :: recv -0x0327(_v=sdk-c-3.0.1,securemode=2,signmethod=hmacsha256,lan=C,_ss=1,g)
everything_state_handle|120 :: recv -0x0327(&)
everything_state_handle|120 :: recv -0x0327(B36DABAE2EBB7B8FA6FFD92B217E13D97C1AB196441561B0F71B3C3BF1DACA0)
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

< {
<     "productKey": "a1W96l6R2FI",
<     "deviceName": "example1"
< }

identity_response_handle|126 :: identify: {"productKey":"a1W96l6R2FI","deviceName":"example1"}
example_event_handle|093 :: msg->event_type : 12
everything_state_handle|120 :: recv -0x0000(mqtt connected in 797 ms)

> {
>     "id": "0",
>     "params": {
>         "version": "app-1.0.0-20180101.1000"
>     }
> }

everything_state_handle|120 :: recv -0x0329(pub - '/ota/device/inform/a1W96l6R2FI/example1': 1)
everything_state_handle|120 :: recv -0x0328(sub - '/a1W96l6R2FI/example1/user/get')

> {
>     "message": "hello!"
> }

everything_state_handle|120 :: recv -0x0329(pub - '/a1W96l6R2FI/example1/user/get': 0)
example_event_handle|093 :: msg->event_type : 9
example_event_handle|093 :: msg->event_type : 3
```