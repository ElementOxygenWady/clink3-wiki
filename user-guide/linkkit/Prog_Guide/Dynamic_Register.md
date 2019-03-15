# <a name="目录">目录</a>
+ [功能说明](#功能说明)
+ [例子程序讲解](#例子程序讲解)
+ [功能API接口](#功能API接口)
+ [需要对接的HAL接口](#需要对接的HAL接口)

# <a name="功能说明">功能说明</a>

一型一密功能也称为直连设备动态注册功能, 用户只需在同一类产品固件中写入相同的 `ProductKey` 和 `ProductSecret`, 即可通过动态注册的方式在首次建连时从云端服务器获取设备的 `DeviceSecret`, 而不必为不同设备烧录不同的三元组

使用的流程示意
---

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/dynreg_console_flow.png" width="1200" height="200" />

它工作时, IoT设备端和外界发生网络通信的交互过程是
---
> 简写说明
>
+ PK: ProductKey, 设备品类标识字符串
+ PS: ProductSecret, 设备品类密钥
+ DN: DeviceName, 某台设备的标识字符串
+ DS: DeviceSecret, 某台设备的设备密钥


<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/dynreg_network_flow.png" width="800" height="600" />

*注1: 用户必须将获取的DeviceSecret持久化到设备, 以备后续使用. 若获取的DeviceSecret丢失可能导致设备无法上线等严重后果, 云端服务器不会接受已激活设备重复的动态注册请求*

*注2: 使用一型一密功能, 用户必须对每个设备进行预注册, 即在控制台上以设备实际DeviceName创建好设备. 并且在控制台上打开对应产品的动态注册功能*

# <a name="例子程序讲解">例子程序讲解</a>

一型一密功能的例子程序在 `src/dynamic_register/examples/dynreg_example.c`, 以下对其逐段讲解

要使用一型一密功能, 要包含它的头文件 `dynreg_api.h`
---

    #include <stdio.h>
    #include <string.h>
    #include "infra_types.h"
    #include "infra_defs.h"
    #include "dynreg_api.h"

准备输入参数 `region` 和出入参结构体 `meta`
---

    iotx_http_region_types_t region = IOTX_HTTP_REGION_SHANGHAI;
    HAL_Printf("dynreg example\n");

    memset(&meta,0,sizeof(iotx_dev_meta_info_t));
    HAL_GetProductKey(meta.product_key);
    HAL_GetProductSecret(meta.product_secret);
    HAL_GetDeviceName(meta.device_name);

以上例子程序用 `IOTX_CLOUD_REGION_SHANGHAI` 代表的华东二站点作为例子, 演示连接上海服务器时候的情况, 另一个入参 `meta` 其实就是填入设备的 `PK/PS/DN`

调用一型一密的API获取 `DeviceSecret`
---

    res = IOT_Dynamic_Register(region, &meta);
    if (res < 0) {
        HAL_Printf("IOT_Dynamic_Register failed\n");
        return -1;
    }

    HAL_Printf("\nDevice Secret: %s\n\n", meta.device_secret);

这个 `IOT_Dynamic_Register()` 接口就是一型一密功能点唯一提供的用户接口, 若执行成功, 在参数 `meta` 中将填上从服务器成功获取到的 `DeviceSecret`

其它
---
参考上面的图示

+ 第1步和第2步对应用户接口: `IOT_Dynamic_Register()`
+ 第3步对应用户接口: `IOT_Sign_MQTT()`
+ 第4步对应用户接口: `IOT_MQTT_Construct()`

# <a name="功能API接口">功能API接口</a>

原型
---

    int32_t IOT_Dynamic_Register(iotx_http_region_types_t region, iotx_dev_meta_info_t *meta);

接口说明
---

根据输入参数中指定的站点区域, 以及`productKey`和`productSecret`, 去云端为`deviceName`指定的设备去云端申请`deviceSecret`

参数说明

| 参数    | 数据类型                    | 方向        | 说明
|---------|-----------------------------|-------------|---------------------------------------------------------------------
| region  | iotx_http_region_types_t    | 输入        | 表示设备将要工作的区域, 例如美西/新加坡/日本/华东2站点等
| meta    | iotx_dev_meta_info_t *      | 输入输出    | 输入的时候带入设备的 `PK/PS/DN`, 输出的时候返回从服务器取到的 `DS`

# <a name="需要对接的HAL接口">需要对接的HAL接口</a>

| 函数名                                                                                                                                  | 说明
|-----------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------
| [HAL_Malloc](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Malloc)        | 申请一片内存并返回其起始地址
| [HAL_Free](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Free)            | 释放入参中指定的内存区域
| [HAL_Printf](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Printf)        | 格式化的打印字符串显示到输出终端上
| [HAL_Snprintf](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Snprintf)    | 格式化的打印字符串到目标缓冲区内存中

