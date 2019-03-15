# <a name="目录">目录</a>
+ [基于Make的编译系统说明](#基于Make的编译系统说明)
    * [常用命令](#常用命令)
    * [输出说明](#输出说明)
        - [output/release/lib](#output/release/lib)
        - [output/release/include](#output/release/include)
        - [output/release/bin](#output/release/bin)
    * [配置系统组成部分](#配置系统组成部分)
        - [用户输入](#用户输入)
        - [构建单元](#构建单元)
        - [tools/board/config.xxx.yyy](#tools/board/config.xxx.yyy)
        - [调试方式](#调试方式)
        - [交叉编译相关](#交叉编译相关)
        - [目录文件相关](#目录文件相关)
    * [用 make.settings 文件裁剪 C-SDK 详解](#用 make.settings 文件裁剪 C-SDK 详解)
    * [典型产品的 make.settings 示例](#典型产品的 make.settings 示例)
        - [不具有网关功能的WiFi模组](#不具有网关功能的WiFi模组)
        - [具有网关功能的WiFi模组](#具有网关功能的WiFi模组)
        - [蜂窝网模组](#蜂窝网模组)
        - [基于Linux系统的网关](#基于Linux系统的网关)

# <a name="基于Make的编译系统说明">基于Make的编译系统说明</a>

在"快速体验"中使用到了make进行sdk的编译, 本章对make系统进行更详细的描述, 以及讲解如何对SDK进行裁剪

## <a name="常用命令">常用命令</a>

| 命令                | 解释
|---------------------|---------------------------------------------------------------------------------
| `make distclean`    | **清除一切构建过程产生的中间文件, 使当前目录仿佛和刚刚clone下来一样**
| `make`              | **使用默认的或已选中的平台配置文件平台配置文件开始编译**
| `make env`          | **显示当前编译配置, 非常有用, 比如可显示交叉编译链, 编译CFLAGS等**
| `make reconfig`     | **弹出多平台选择菜单, 用户可按数字键选择, 然后根据相应的硬件平台配置开始编译**
| `make config`       | **显示当前被选择的平台配置文件**
| `make menuconfig`   | **以图形化的方式编辑和生成功能配置文件make.settings**
| `make help`         | **打印帮助文本**

## <a name="输出说明">输出说明</a>
成功编译的话, 最终会打印类似如下的表格, 这是每个模块的ROM占用, 以及静态RAM占用的统计

    | RATE  | MODULE NAME                         | ROM       | RAM       | BSS        | DATA   |
    |-------|-------------------------------------|-----------|-----------|------------|--------|
    | 45.3% | src/dev_model                       | 28563     | 216       | 188        | 28     |
    | 28.1% | src/mqtt                            | 17737     | 28        | 28         | 0      |
    | 25.7% | src/infra                           | 16195     | 544       | 60         | 484    |
    | 1.65% | src/dev_sign                        | 1045      | 48        | 0          | 48     |
    |-------|-------------------------------------|-----------|-----------|------------|--------|
    |  100% | - IN TOTAL -                        | 63540     | 836       | 276        | 560    |

**用户需要关注的输出产物都在 `output/release` 目录下:**

### <a name="output/release/lib">output/release/lib</a>

| 产物文件名      | 说明
|-----------------|-------------------------------------------------------------------------
| `libiot_hal.a`  | HAL接口层的参考实现, 提供了 `HAL_XXX()` 接口
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口
| `libiot_tls.a`  | 裁剪过的 `mbedtls`, 提供了 `mbedtls_xxx()` 接口, 支撑 `libiot_hal.a`

### <a name="output/release/include">output/release/include</a>

| 产物文件名          | 说明
|---------------------|-----------------------------------------------------------------------------
| `mqtt_api.h`        | 当用户在配置环节选中"MQTT上云"时出现, 列出MQTT上云功能点提供的用户API
| `dev_model_api.h`   | 当用户在配置环节选中"物模型管理"时出现, 列出物模型管理功能点提供的用户API

*选中了什么功能, 该功能的API就会以 `<功能名字>_api.h` 的形式出现, 依次类推*

### <a name="output/release/bin">output/release/bin</a>

如果是在主机环境下不做交叉编译(Ubuntu/Windows), 是可以产生主机版本的demo程序, 可以直接运行的, 比如

| 产物文件名              | 说明
|-------------------------|-------------------------------------------------------------
| `linkkit-example-solo`  | 物模型管理功能的例程, 可演示 `linkkit_xxx()` 接口的使用
| `mqtt-example`          | MQTT上云功能的例程, 可演示 `IOT_XXX()` 接口的使用

## <a name="配置系统组成部分">配置系统组成部分</a>

### <a name="用户输入">用户输入</a>

设备端C-SDK的构建配置系统, 有以下输入文件可接受用户的配置, 您可以通过编辑它们, 将配置输入到构建系统中

+ **功能配置文件:** 即顶层目录的 `make.settings` 文本文件
+ **平台配置文件:** 即目录 `tools/board` 下的 `config.xxx.yyy` 系列文件, 也称config文件

---
构建系统最终是依据 `config.xxx.yyy` 文件进行编译, 然而由于功能配置/裁剪更为常用, 我们将它额外抽取到了 `make.settings` 中

> config.xxx.yyy 主要关注目标嵌入式硬件平台的工具链程序和编译/链接选项的指定, 用于跨平台移植
>
> config.xxx.yyy 此外也能以 CONFIG_ENV_CFLAGS += ... 的语法新增自定义 CFLAGS, 同理 CONFIG_ENV_LDFLAGS += ... 可以指定链接选项
>
> make.settings 则是在已被确定的目标硬件平台上, 专注于C-SDK的功能模块裁剪或者配置, 用于裁剪功能模块

### <a name="构建单元">构建单元</a>
从工程顶层目录以下, 每一个含有`iot.mk`的子目录, 都被构建系统认为是一个**构建单元**

*每一个构建单元, 若相对顶级makefile的路径是`bar`, `foo/bar1`, 则可以用`make bar`, `make foo/bar1`这样的命令单独编译*

### <a name="tools/board/config.xxx.yyy">tools/board/config.xxx.yyy</a>

文件名形式为`config.<VENDOR>.<MODEL>`的文本文件, 会被构建系统认为是硬件平台配置文件, 每个文件对应一个嵌入式软硬件平台

* 其中<VENDOR>部分, 一般是指明嵌入式平台的软件OS提供方, 如`mxchip`, `ubuntu`, `win7`等. 另外, 这也会导致构建系统到`$(IMPORT_DIR)/<VENDOR>`目录下寻找预编译库的二进制库文件和头文件
* 其中<MODEL>部分, 一般是标明嵌入式平台的具体硬件型号, 如`mtk7687`, `qcom4004`等, 不过也可以写上其它信息, 因为构建系统不会去理解它, 比如`mingw32`, `x86-64`等

### <a name="调试方式">调试方式</a>

+ 在`make ...`命令行中, 设置`TOP_Q`变量为空, 可打印工程顶层的执行逻辑, 例如硬件平台的选择, SDK主库的生成等

        make .... TOP_Q=

+ 在`make ...`命令行中, 设置`Q`变量为空, 可打印模块内部的构建过程, 例如目标文件的生成, 头文件搜寻路径的组成等

        make .... Q=

+ 可以用`make foo/bar`单独对`foo/bar`进行构建, 不过, 这可能需要先执行`make reconfig`
+ 可以进入`.O/foo/bar`路径, 看到完整的编译临时目录, 有makefile和全部源码, 所以在这里执行`make`, 效果和`make foo/bar`等同

### <a name="交叉编译相关">交叉编译相关</a>

以下是常在 `tools/board/config.xxx.yyy` 平台配置文件中使用的变量

| 变量                    | 说明
|-------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------
| `CONFIG_ENV_CFLAGS`     | 指定全局的`CFLAGS`编译选项, 传给`compiler`, 例如`CONFIG_ENV_CFLAGS += -DDEBUG`
| `CONFIG_ENV_LDFLAGS`    | 指定全局的`LDFLAGS`链接选项, 传给`linker`, 例如`CONFIG_ENV_LDFLAGS += -lcrypto`
| `CROSS_PREFIX`          | 指定交叉编译工具链共有的前缀, 例如`CROSS_PREFIX := arm-none-eabi-`, 会导致构建系统使用`arm-none-eabi-gcc`和`arm-none-eabi-ar`, 以及`arm-none-eabi-strip`等
| `OVERRIDE_CC`           | 当交叉工具链没有共有的前缀或者前缀不符合`prefix+gcc/ar/strip`类型时, 例如`armcc`, 可用`OVERRIDE_CC = armcc`单独指定C编译器
| `OVERRIDE_AR`           | 当交叉工具链没有共有的前缀或者前缀不符合`prefix+gcc/ar/strip`类型时, 例如`armar`, 可用`OVERRIDE_AR = armar`单独指定库压缩器
| `OVERRIDE_STRIP`        | 当交叉工具链没有共有的前缀或者前缀不符合`prefix+gcc/ar/strip`类型时, 例如`armcc`没有对应的strip程序, 可用`OVERRIDE_STRIP = true`单独指定strip程序不执行
| `CONFIG_LIB_EXPORT`     | 指定SDK产生的二进制库的形式, 例如``CONFIG_LIB_EXPORT := dynamic`可以指定产生linux上的`libiot_sdk.so`动态库文件, 默认为产生跨平台的`libiot_sdk.a`静态库

### <a name="目录文件相关">目录文件相关</a>

| 变量                | 说明
|---------------------|---------------------------------------------------------------------------------------------
| `CONFIG_mmm/nnn`    | 指定`mmm/nnn`目录是否需要编译的开关, 例如`CONFIG_mmm/nnn :=`的写法会导致该目录被跳过编译

## <a name="用 make.settings 文件裁剪 C-SDK 详解">用 make.settings 文件裁剪 C-SDK 详解</a>

> 对V3.0.1以上版本, 可以用 `make menuconfig` 命令或者 `config.bat` 脚本, 分别在Linux/Windows主机上图形化的配置SDK

![image](https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/windows_menuconfig.jpg)

在上面的界面中

+ 按下空格键可以选中或者失效某个功能, 使用小键盘的上下键来在不同功能之间选择
+ 如果想知道每个选项的具体含义, 先用方向键将高亮光条移到那个选项上, 再按键盘上的 **"h"按键**, 将出现帮助文本, 对选项进行详细说明。

注意: 不建议手动编辑 `make.settings` 文件改动配置, 一切配置都需通过上面的图形界面进行
---

#### <a name="FEATURE_MQTT_COMM_ENABLED">FEATURE_MQTT_COMM_ENABLED</a>
+ MQTT上云功能开关, 所谓MQTT上云是指搭载了C-SDK的嵌入式设备和阿里云服务器之间使用 `MQTT` 协议进行连接和交互

#### <a name="FEATURE_COAP_COMM_ENABLED">FEATURE_COAP_COMM_ENABLED</a>
+ CoAP上云功能开关, 所谓CoAP上云是指搭载了C-SDK的嵌入式设备和阿里云服务器之间使用 `CoAP` 协议进行连接和交互

#### <a name="FEATURE_HTTP_COMM_ENABLED">FEATURE_HTTP_COMM_ENABLED</a>
+ HTTP/S上云功能开关, 所谓HTTP/S上云是指搭载了C-SDK的嵌入式设备和阿里云服务器之间使用 `HTTP` 协议或 `HTTPS` 协议进行连接和交互

#### <a name="FEATURE_DYNAMIC_REGISTER">FEATURE_DYNAMIC_REGISTER</a>
+ 一型一密/动态注册功能开关, 所谓动态注册是指不需要为同个品类下的不同设备烧录不同的三元组, 只需烧录相同的`productSecret`, 每个设备在网络通信中动态注册自己

#### <a name="FEATURE_DEPRECATED_LINKKIT">FEATURE_DEPRECATED_LINKKIT</a>
+ 高级版接口风格的开关, 配置进行高级版物模型相关的编程时, C-SDK是提供 `linkkit_xxx_yyy()` 风格的旧版接口, 还是提供 `IOT_Linkkit_XXX()` 风格的新版接口

#### <a name="FEATURE_DEV_BIND_ENABLED">FEATURE_DEV_BIND_ENABLED</a>
> V2.3.0之后的版本才新增的这个开关

+ 绑定功能的开关, 只有用C-SDK连接飞燕平台的客户才需要关注并打开它, 用于使能设备被飞燕的公版app控制, 可绑定设备和用户账号

#### <a name="FEATURE_DEVICE_MODEL_GATEWAY">FEATURE_DEVICE_MODEL_GATEWAY</a>
> 在V2.3.0以前的版本中, 这个开关的曾用名是 `FEATURE_ENHANCED_GATEWAY`

+ 高级版网关能力的开关, 配置进行高级版物模型相关的编程时, C-SDK是提供 `linkkit_xxx_yyy()` 风格的单品接口, 还是提供 `linkkit_gateway_xxx_yyy()` 风格的网关接口

#### <a name="FEATURE_HTTP2_COMM_ENABLED">FEATURE_HTTP2_COMM_ENABLED</a>
+ HTTP2上云功能开关, 所谓HTTP2上云是指搭载了C-SDK的嵌入式设备和阿里云服务器之间使用 `HTTP2` 协议进行连接和交互

#### <a name="FEATURE_MQTT_DIRECT">FEATURE_MQTT_DIRECT</a>
+ MQTT直连功能开关, 所谓MQTT直连是指设备和阿里云服务器之间使用 `MQTT` 协议进行连接, 而不会前置基于 `HTTP` 协议认证的交互过程

#### <a name="FEATURE_OTA_ENABLED">FEATURE_OTA_ENABLED</a>
+ 固件升级功能开关, 所谓固件升级是指设备从阿里云服务器上下载用户在IoT控制台中上传的固件文件功能

#### <a name="FEATURE_DEVICE_MODEL_ENABLED">FEATURE_DEVICE_MODEL_ENABLED</a>
> 在V2.3.0以前的版本中, 这个开关的曾用名是 `FEATURE_SDK_ENHANCE`

+ 高级版物模型能力的功能开关, 所谓高级版物模型能力是指设备可使用基于服务/属性/事件三要素的Alink协议和服务端通信

#### <a name="FEATURE_SUPPORT_TLS">FEATURE_SUPPORT_TLS</a>
+ 在TLS层是否使用TLS的功能开关, 关闭则代表用不带TLS加密的TCP连接连云

#### <a name="FEATURE_WIFI_PROVISION_ENABLED">FEATURE_WIFI_PROVISION_ENABLED</a>
> 在V2.3.0以前的版本中, 这个开关的曾用名是 `FEATURE_WIFI_AWSS_ENABLED`

+ WiFi配网的功能开关, 所谓WiFi配网是阿里巴巴自研的一种从手机app发送WiFi网络的SSID和密码给设备端的通信协议

## <a name="典型产品的 make.settings 示例">典型产品的 make.settings 示例</a>
> 解压之后, 打开功能配置文件 `make.settings`, 根据需要编辑配置项, 使用不同的编译配置, 编译输出的SDK内容以及examples都有所不同
>
> 以下针对C-SDK的客户中, 较多出现的几种产品形态, 给出典型的配置文件, **并在注释中说明为什么这样配置**

注: 2.2.1版本中若要使用WiFi配网功能, 请将 `FEATURE_WIFI_AWSS_ENABLED = y` 一行放置为 make.setting 的第一个配置项, 否则会工作异常

### <a name="不具有网关功能的WiFi模组">不具有网关功能的WiFi模组</a>
这种场景下客户使用WiFi上行的MCU模组, 比如乐鑫ESP8266, 庆科MK3060等.

这种场景下, 设备和阿里云之间的连接只用于它自己和云端的通信, 不会用于代理给其它嵌入式设备做消息上报和指令下发或固件升级等.

    FEATURE_MQTT_COMM_ENABLED    = y          # 一般WiFi模组都有固定供电, 所以都采用MQTT的方式上云
    FEATURE_MQTT_DIRECT          = y          # MQTT直连效率更高, 该选项只在部分海外设备上才会关闭
    FEATURE_OTA_ENABLED          = y          # 一般WiFi模组的客户, 都会使用阿里提供的固件升级服务
    FEATURE_DEVICE_MODEL_ENABLED = y          # 一般WiFi模组片上资源充足, 可以容纳高级版, 所以打开
    FEATURE_DEVICE_MODEL_GATEWAY = n          # 如上述说明, 不具备高级版网关功能的场景, 当然关闭这个选项
    FEATURE_WIFI_PROVISION_ENABLED = y        # 一般WiFi模组的客户, 都会使用阿里的配网app或sdk, 告诉模组SSID和密码
    FEATURE_SUPPORT_TLS          = y          # 绝大多数的客户都是用标准的TLS协议连接公网

### <a name="具有网关功能的WiFi模组">具有网关功能的WiFi模组</a>
这种场景下客户使用WiFi上行的MCU模组, 比如庆科MK3080等.

这种场景下, 设备和阿里云直接的连接不仅用于它自己和云端的通信, 还会用于代理给其它嵌入式设备做消息上报和指令下发或固件升级等.

    FEATURE_MQTT_COMM_ENABLED    = y          # 一般WiFi模组都有固定供电, 所以都采用MQTT的方式上云
    FEATURE_MQTT_DIRECT          = y          # MQTT直连效率更高, 该选项只在部分海外设备上才会关闭
    FEATURE_OTA_ENABLED          = y          # 一般WiFi模组的客户, 都会使用阿里提供的固件升级服务
    FEATURE_DEVICE_MODEL_ENABLED = y          # 一般WiFi模组片上资源充足, 可以容纳高级版, 所以打开
    FEATURE_DEVICE_MODEL_GATEWAY = y          # 如上述说明, 要具备高级版网关功能的场景, 当然打开这个选项
    FEATURE_WIFI_PROVISION_ENABLED = y        # 一般WiFi模组的客户, 都会使用阿里的配网app或sdk, 告诉模组SSID和密码
    FEATURE_SUPPORT_TLS          = y          # 绝大多数的客户都是用标准的TLS协议连接公网

### <a name="蜂窝网模组">蜂窝网模组</a>
这种场景下设备直接连接GPRS/3G/4G网络

    FEATURE_MQTT_COMM_ENABLED    = y          # 虽然CoAP更省电, 但不能做云端消息的及时下推, 所以目前蜂窝网模组仍主要用MQTT的方式上云
    FEATURE_MQTT_DIRECT          = y          # MQTT直连效率更高, 对网络远慢于WiFi的蜂窝网模组而言, 直连开关必须打开
    FEATURE_OTA_ENABLED          = y          # 一般蜂窝网模组的客户, 也会使用阿里提供的固件升级服务
    FEATURE_DEVICE_MODEL_ENABLED = n          # 蜂窝网模组网速很慢, 资源较少, 所以这种模组的客户一般不会用高级版, 而只需要基础版的MQTT上云
    FEATURE_DEVICE_MODEL_GATEWAY = n          # 蜂窝网模组一般不集成高级版(物模型)功能, 并且它也不会下联其它嵌入式设备分享MQTT上云通道
    FEATURE_WIFI_PROVISION_ENABLED = n        # 蜂窝网模组不通过WiFi协议连接公网, 因此关闭WiFi配网的开关
    FEATURE_SUPPORT_TLS          = y          # 绝大多数的客户都是用标准的TLS协议连接公网, 对蜂窝网模组, 甚至可能连这个选项都关闭

### <a name="基于Linux系统的网关">基于Linux系统的网关</a>
比如家庭网关, 工业网关等, 一般是相对MCU模组来说, 性能强大的多, 资源丰富的多的设备

    FEATURE_MQTT_COMM_ENABLED    = y          # 一般Linux网关都有固定供电, 所以都采用MQTT的方式上云
    FEATURE_MQTT_DIRECT          = y          # MQTT直连效率更高, 该选项只在部分海外设备上才会关闭
    FEATURE_OTA_ENABLED          = y          # 一般Linux网关的客户, 都会使用阿里提供的固件升级服务
    FEATURE_DEVICE_MODEL_ENABLED = y          # 一般Linux网关片上资源充足, 可以容纳高级版, 所以打开
    FEATURE_DEVICE_MODEL_GATEWAY = y          # 如上述说明, 要具备高级版网关功能的场景, 当然打开这个选项
    FEATURE_WIFI_PROVISION_ENABLED = y        # 取决于Linux网关是否用WiFi做上行并使用阿里的配网app/sdk, 如果皆是, 则打开这个选项
    FEATURE_SUPPORT_TLS          = y          # 绝大多数的客户都是用标准的TLS协议连接公网

