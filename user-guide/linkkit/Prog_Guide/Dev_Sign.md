# <a name="目录">目录</a>
+ [功能说明](#功能说明)
+ [例子程序讲解](#例子程序讲解)
+ [功能API接口](#功能API接口)
+ [需要对接的HAL接口](#需要对接的HAL接口)

# <a name="功能说明">功能说明</a>

> 阿里巴巴IoT物联网平台使用通用的MQTT协议作为物联网设备和云平台之间的通信协议

云平台在处理MQTT连接请求时, 根据网络报文中的签名信息判定当前请求连接的设备是否合法

**功能点 `dev_sign` 提供的就是该签名字段的计算封装, 这个功能称为"设备签名"**

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/brief_dev_sign.png" width="800" hegiht="600" align=center />

如上图所示

+ 左边的是该模块的输入参数, 三元组和region分别表达设备身份和想要连接的站点所在区域(华东2/日本/美西/新加坡)
+ 右边的是该模块的返回参数, hostname/port是连接服务器的域名和端口
+ 另外3个则是IoT设备连接云平台时证明自己合法的身份认证信息, 统称为"设备签名信息"

图示正中间的是该模块提供的唯一一个用户接口API
---

# <a name="例子程序讲解">例子程序讲解</a>

设备签名的例子程序在 `src/dev_sign/examples/dev_sign_example.c`, 以下对其进行逐段讲解

要使用 `dev_sign` 功能, 需包含它的API头文件 `dev_sign_api.h`
---

    #include "dev_sign_api.h"

从IoT控制台申请到一台新的IoT设备, 记录其三元组
---

    #define EXAMPLE_PRODUCT_KEY     "a1X2bEnP82z"
    #define EXAMPLE_PRODUCT_SECRET  "7jluWm1zql7bt8qK"
    #define EXAMPLE_DEVICE_NAME     "example1"
    #define EXAMPLE_DEVICE_SECRET   "ga7XA6KdlEeiPXQPpRbAjOZXwG8ydgSe"

声明需要用户提供的底层接口
---
**功能点 `dev_sign` 是零依赖的, 不需要用户做任何开发动作就可以使用. 这里声明了 `HAL_Printf` 仅是因为例程可能需要输出字符**

    /* Implenment this HAL or using "printf" of your own system if you want to print something in example*/
    void HAL_Printf(const char *fmt, ...);

准备出入参结构体
---

    int main(int argc, char *argv[])
    {
        iotx_mqtt_region_types_t region = IOTX_CLOUD_REGION_SHANGHAI;
        iotx_dev_meta_info_t meta;
        iotx_sign_mqtt_t sign_mqtt;

        memset(&meta,0,sizeof(iotx_dev_meta_info_t));
        memcpy(meta.product_key,EXAMPLE_PRODUCT_KEY,strlen(EXAMPLE_PRODUCT_KEY));
        memcpy(meta.product_secret,EXAMPLE_PRODUCT_SECRET,strlen(EXAMPLE_PRODUCT_SECRET));
        memcpy(meta.device_name,EXAMPLE_DEVICE_NAME,strlen(EXAMPLE_DEVICE_NAME));
        memcpy(meta.device_secret,EXAMPLE_DEVICE_SECRET,strlen(EXAMPLE_DEVICE_SECRET));

准备一个类型为 `iotx_dev_meta_info_t` 的结构体变量, 把需要计算签名的设备标识信息填入其中即可

然后调用 `dev_sign` 功能点唯一的一个用户接口 `IOT_Sign_MQTT()`, 计算签名
---

        if (IOT_Sign_MQTT(region,&meta,&sign_mqtt) < 0) {
            return -1; 
        }

结果就在输出参数 `sign_mqtt` 中, 它是一个 `iotx_sign_mqtt_t` 类型的结构体变量

打印签名结果或者去连MQTT服务器
---

    #if 0   /* Uncomment this if you want to show more information */
        HAL_Printf("sign_mqtt.hostname: %s\n",sign_mqtt.hostname);
        HAL_Printf("sign_mqtt.port    : %d\n",sign_mqtt.port);
        HAL_Printf("sign_mqtt.username: %s\n",sign_mqtt.username);
        HAL_Printf("sign_mqtt.password: %s\n",sign_mqtt.password);
        HAL_Printf("sign_mqtt.clientid: %s\n",sign_mqtt.clientid);
    #endif

为了减轻用户对接实现 `HAL_Printf` 的负担, `IOT_Sign_MQTT()` 在执行的过程中不会做任何打印动作, 如果想观察结果可打开上述注释

---
输出参数中的各个成员意义如下

| **成员名**              | **含义**
|-------------------------|-------------------------------------------------------------------------------------------------
| `sign_mqtt.hostname`    | 可以连接的MQTT服务器域名地址, 根据入参 `region` 自动计算
| `sign_mqtt.port`        | 可以连接的MQTT服务器端口号, 根据标准MQTT协议一般是 `1883` 或者 `443`
| `sign_mqtt.username`    | 连接MQTT服务器时将要使用的用户名
| `sign_mqtt.password`    | 连接MQTT服务器时将要使用的密码, 和用户名一一对应
| `sign_mqtt.clientid`    | 连接MQTT服务器时标识设备的自定义ID, 服务器对此不做校验, 鉴权通过 `username` 和 `password` 进行

# <a name="功能API接口">功能API接口</a>

原型
---

    int32_t IOT_Sign_MQTT(iotx_mqtt_region_types_t region, iotx_dev_meta_info_t *meta, iotx_sign_mqtt_t *signout);

接口说明
---

用于计算给定的IoT设备身份认证信息, 这些信息统称为它的"签名", 将在输出参数 `signout` 中返回

参数说明

| 参数        | 数据类型                    | 方向    | 说明
|-------------|-----------------------------|---------|-------------------------------------------------------------------------
| region      | iotx_mqtt_region_types_t    | 输入    | 表示设备将要工作的区域, 例如美西/新加坡/日本/华东2站点等
| meta        | iotx_dev_meta_info_t *      | 输入    | 存放设备的标识字符串, 包括 `productKey`, `deviceName` 等
| signout     | iotx_sign_mqtt_t *          | 输出    | 存放计算好的签名信息, 包括MQTT服务器地址/端口, 连接时的用户名/密码等

# <a name="需要对接的HAL接口">需要对接的HAL接口</a>

**功能点 `dev_sign` 是零依赖的, 不需要用户做任何开发动作就可以使用, 所以不需要对接HAL接口**
