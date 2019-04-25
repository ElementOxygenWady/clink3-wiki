# <a name="目录">目录</a>
+ [MQTT客户端直连](#MQTT客户端直连)
    * [下载直连C语言源码](#下载直连C语言源码)

# <a name="MQTT客户端直连">MQTT客户端直连</a>

## <a name="下载直连C语言源码">下载直连C语言源码</a>

+ [sign_mqtt.c](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/sign_mqtt.c)
+ [sign_api.h](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/sign_api.h)
+ [sign_sha256.c](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/sign_sha256.c)

---
若您希望测试以上文件提供的功能, 可以参考

+ [sign_test.c](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/sign_test.c)

---
+ 推荐使用TLS加密。如果使用TLS加密，需要下载[根证书](http://aliyun-iot.oss-cn-hangzhou.aliyuncs.com/cert_pub/root.crt?spm=a2c4g.11186623.2.10.15f978dc6P4hrM&file=root.crt)，并在sign_api.h中加上`#define SUPPORT_TLS`。

+ 使用位于`sign_api.h`中的`IOT_Sign_MQTT`计算MQTT签名，该API参数如下：

|参数|方向|说明|
|---|---|---|
|region|输入|选择需要连接的域名，域名定义在`sign_api.h`的`iotx_mqtt_region_types_t`中|
|meta|输入|设备三元组，此处填写设备的`Product Key`、`Device Name`和`Device Secret`
|signout|输出|建立MQTT连接所需的信息|

+ C语言源码中提供了一份hmacsha256的参考实现，若您需要使用自己的sha256实现，替换`sign_mqtt.c`中的`utils_hmac_sha256`函数即可。

+ 在使用上述API获取MQTT连接参数后，使用MQTT客户端连接服务器。连接方法，请参见[开源MQTT客户端参考](https://github.com/mqtt/mqtt.github.io/wiki/libraries?spm=a2c4g.11186623.2.11.15f978dc0KmtDz)。如果需了解MQTT协议，请参考 http://mqtt.org 相关文档。