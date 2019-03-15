# <a name="目录">目录</a>
+ [移植指南](#移植指南)
    * [在Ubuntu上编译主机版本](#在Ubuntu上编译主机版本)
        - [正常的编译过程演示](#正常的编译过程演示)
        - [得到的编译产物说明](#得到的编译产物说明)
    * [交叉编译到嵌入式硬件平台](#交叉编译到嵌入式硬件平台)
        - [安装交叉编译工具链](#安装交叉编译工具链)
        - [添加配置文件](#添加配置文件)
        - [编辑配置文件](#编辑配置文件)
        - [选择配置文件](#选择配置文件)
        - [交叉编译产生库文件`libiot_sdk.a`](#交叉编译产生库文件`libiot_sdk.a`)
        - [获取交叉编译的产物, 包括静态库和头文件](#获取交叉编译的产物, 包括静态库和头文件)
    * [开发未适配平台的HAL层](#开发未适配平台的HAL层)
        - [复制一份HAL层实现代码](#复制一份HAL层实现代码)
        - [打开之前被关闭的编译开关](#打开之前被关闭的编译开关)
        - [尝试交叉编译被复制的HAL层代码](#尝试交叉编译被复制的HAL层代码)
        - [交叉编译样例程序](#交叉编译样例程序)
        - [重新载入配置文件, 交叉编译可执行程序](#重新载入配置文件, 交叉编译可执行程序)
        - [尝试运行样例程序](#尝试运行样例程序)

# <a name="移植指南">移植指南</a>

> 物联网平台C-SDK以全开源的纯C语言编写而成, 旨在提供与目标平台硬件CPU体系架构无关, 与目标平台嵌入式OS操作系统无关的跨平台SDK

+ 自SDK3.0.1版本之后, 将C-SDK移植到目标嵌入式平台有两种方式
    + 使用SDK自带的编译系统
    + 不使用SDK自带的编译系统, 直接抽取源文件集成

+ 使用SDK自带的编译系统时, 您只要
    + 准备一台 `Linux` 的开发主机, 安装 `Ubuntu16.04 64bits`
    + 运行 `make menuconfig` 命令配置功能点
    + 在 `tools/board` 目录下新增自己的硬件平台描述文件
    + 运行 `make reconfig` 选择自己的平台
    + 运行 `make`, 产物是已经交叉编译好的 `libiot_sdk.a`, 在 `output/release/lib` 目录下

+ 不使用SDK自带的编译系统时, 需要将SDK中需要的C文件和H文件抽取出来, 自行集成到您的产品中
    + 准备一台 `Linux` 的开发主机, 安装 `Ubuntu16.04 64bits`, 或者一台 `Windows` 的开发主机
    + 运行 `config.bat` 脚本配置功能点
    + 运行 `extract.bat` 脚本根据被选择的功能点抽取代码
    + 产物是尚未交叉编译的源文件, 在 `output/eng` 目录下
    + 由于用户得到的只是源码, 需按照自己喜欢的方式自行交叉编译到设备的固件中


本章假设用户使用SDK自带的编译系统, 如果不符合您的使用情况, 那么可以略过
---

我们建议移植时遵循如下的流程, 将C-SDK源码适配移植到您需要接入到阿里云物联网平台的嵌入式硬件上

+ **在Ubuntu上编译主机版本:** 这一步并不进行交叉编译, 而是希望您可以体验主机(X86)版本的SDK及其例程, 熟悉SDK的编译过程和产物
+ **交叉编译到嵌入式硬件平台:** 接着可以安装目标平台的编译工具链, 按本文说明, 交叉编译嵌入式体系架构的二进制库 `libiot_sdk.a`
+ **了解C-SDK的构建系统使用:** 在交叉编译环节, 您将接触到C-SDK的构建配置系统
+ **开发未适配平台的HAL层:** 要使用以上步骤中产生的 `libiot_sdk.a` 所提供的API, 您还需要为C-SDK提供第五章中列出的HAL接口实现

---
本文为了快速走通以上流程, 简写了第3步和第4步, 以移植到 `arm-linux` 平台为例, 直接演示了一个完整的移植过程

## <a name="在Ubuntu上编译主机版本">在Ubuntu上编译主机版本</a>

具体步骤是:

    $ make distclean
    $ make

即可得到`libiot_sdk.a`

### <a name="正常的编译过程演示">正常的编译过程演示</a>

    $ make distclean
    $ make

    SELECTED CONFIGURATION:

    VENDOR :   ubuntu
    MODEL  :   x86


    CONFIGURE .............................. [certs]
    CONFIGURE .............................. [external_libs/mbedtls]
    CONFIGURE .............................. [external_libs/nghttp2]

### <a name="得到的编译产物说明">得到的编译产物说明</a>

SDK编译的产物在编译成功之后都存放在 `output` 目录下:

    $ tree -d output/

    output/
    +-- eng
    |   +-- dev_model
    |   +-- dev_sign
    |   +-- infra
    |   +-- mqtt
    |   +-- wrappers
    |       +-- ubuntu
    +-- examples
    +-- release
        +-- bin
        +-- include
        |   +-- infra
        +-- lib

说明:

| 产物                                                | 说明
|-----------------------------------------------------|-----------------------------------------------------------------
| output/release/bin/*                                | 例子程序, 在Ubuntu上运行, 并对照阅读 `output/examples/` 目录下的源代码, 以体验SDK的功能
| output/release/include/mqtt_api.h                   | 选中了MQTT上云功能后出现, 列举了MQTT上云功能提供的用户API, 其它的 `xxx_api.h` 与此类似, 陈列 `xxx` 功能的用户API
| output/release/include/infra/*.h                    | 支撑SDK, 用户可以不必关心这个目录下的内容
| output/lib/libiot_sdk.a                             | SDK主库, 集中提供了所有用户接口的实现, 它的上层是用户业务逻辑, 下层是`libiot_hal.a`
| output/lib/libiot_hal.a                             | HAL主库, 集中提供了所有`HAL_XXX_YYY()`接口的实现, 它的上层是`libiot_sdk.a`, 下层是`libiot_tls.a`
| output/lib/libiot_tls.a                             | TLS主库, 集中提供了所有`mbedtls_xxx_yyy()`接口的实现, 它的上层是`libiot_hal.a`
| output/lib/*.a                                      | 其它分库, 它们是从SDK源码目录的`prebuilt/`目录移动过来的, 主要提供一些闭源发布的功能, 比如`ID2`等

## <a name="交叉编译到嵌入式硬件平台">交叉编译到嵌入式硬件平台</a>

对于嵌入式硬件平台的情况, 对编译出目标平台的`libiot_sdk.a`, 需要经历如下几个步骤:

- 在`tools/board/`目录下添加一个对应的配置文件, 文件名规范为`config.XXX.YYY`, 其中`XXX`部分就对应后面`wrappers/os/XXX`目录的HAL层代码
- 在配置文件中, 至少要指定:
    * 交叉编译器 `OVERRIDE_CC` 的路径
    * 交叉链接器 `OVERRIDE_LD` 的路径
    * 静态库压缩器 `OVERRIDE_AR` 的路径
    * 编译选项 `CONFIG_ENV_CFLAGS`, 用于C文件的编译
    * 链接选项 `CONFIG_ENV_LDFLAGS`, 用于可执行程序的链接

- 尝试编译SDK, 对可能出现的跨平台问题进行修正, 直到成功产生目标格式的`libiot_sdk.a`
- 最后, 您需要以任何您喜欢的编译方式, 产生目标架构的`libiot_hal.a`
- 若目标平台尚未被适配, 则`libiot_hal.a`对应的源代码在C-SDK中并未包含, 需要您自行实现`HAL_*()`接口

---
下面以某款目前未官方适配的 `arm-linux` 目标平台为例, 演示如何编译出该平台上可用的`libiot_sdk.a`

### <a name="安装交叉编译工具链">安装交叉编译工具链</a>

> 仍以Ubuntu16.04开发环境为例

    $ sudo apt-get install -y gcc-arm-linux-gnueabihf
    $ arm-linux-gnueabihf-gcc --version

    arm-linux-gnueabihf-gcc (Ubuntu/Linaro 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609
    Copyright (C) 2015 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

### <a name="添加配置文件">添加配置文件</a>

    $ touch tools/board/config.arm-linux.demo
    $ ls tools/board/
    config.alios.mk3080  config.arm-linux.demo  config.ubuntu.x86

### <a name="编辑配置文件">编辑配置文件</a>

在这一步, 需要设置编译选项和工具链, 以及跳过编译的目录

    $ vim tools/board/config.arm-linux.demo

    CONFIG_ENV_CFLAGS = \
        -D_PLATFORM_IS_LINUX_ \
        -Wall \
        -DNO_EXECUTABLES \

    CONFIG_ENV_LDFLAGS = \
        -lpthread -lrt \

    OVERRIDE_CC = arm-linux-gnueabihf-gcc
    OVERRIDE_AR = arm-linux-gnueabihf-ar
    OVERRIDE_LD = arm-linux-gnueabihf-ld

    CONFIG_wrappers :=

**注意, 上面的最后1行表示跳过对 `wrappers` 目录的编译, 以及 `-DNO_EXECUTABLES` 表示不要产生可执行程序**

**在编译未被适配平台的库时在最初是必要的, 这样可以避免产生过多的错误**

### <a name="选择配置文件">选择配置文件</a>

    $ make reconfig

    SELECT A CONFIGURATION:

    1) config.alios.mk3080
    2) config.arm-linux.demo
    3) config.ubuntu.x86
    #? 2

    SELECTED CONFIGURATION:

    VENDOR :   arm-linux
    MODEL  :   demo

### <a name="交叉编译产生库文件`libiot_sdk.a`">交叉编译产生库文件`libiot_sdk.a`</a>

> 注: 本步骤不编译HAL, 只是为了验证配置文件中的交叉编译参数是否正确, 如果出现错误请对配置文件再次进行修改, 直到编译成功

    $ make
    BUILDING WITH EXISTING CONFIGURATION:

    VENDOR :   arm-linux
    MODEL  :   demo

    [CC] infra_timer.o                      <=  ...
    [CC] infra_json_parser.o                <=  ...
    [CC] infra_preauth.o                    <=  ...

### <a name="获取交叉编译的产物, 包括静态库和头文件">获取交叉编译的产物, 包括静态库和头文件</a>

    $ ls -1 output/release/lib/
    libiot_sdk.a
    libiot_tls.a

这里, `libiot_sdk.a`文件就是编译好的物联网套件SDK, 已经是`ELF 32-bit LSB relocatable, ARM, EABI5 version 1 (SYSV)`格式, 也就是`arm-linux`格式的交叉编译格式了

另外, `libiot_tls.a`是一个裁剪过的加解密库, 您可以选择使用它, 也可以选择使用平台自带的加解密库, 以减小最终固件的尺寸

    $ ls -1 output/release/include/

    dev_model_api.h
    dev_sign_api.h
    infra
    mqtt_api.h

这里, `dev_sign_api.h`就是使用SDK中"设备签名"功能需要包含的头文件, 类似`mqtt_api.h`是使用SDK中"MQTT上云"功能需要的, `infra`下的头文件也请加入编译搜索路径

## <a name="开发未适配平台的HAL层">开发未适配平台的HAL层</a>

对于实现平台抽象层接口 `HAL_XXX_YYY()` 的库 `libiot_hal.a`, 不限制其编译和产生的方式

但是如果你愿意的话, 当然仍然可以借助物联网套件设备端C-SDK的编译系统来开发和产生它

---
仍然以上一节中, 某款目前未适配的`arm-linux`目标平台为例, 假设这款平台和`Ubuntu`差别很小, 完全可以用`Ubuntu`上开发测试的HAL层代码作为开发的基础, 则可以这样做:

### <a name="复制一份HAL层实现代码">复制一份HAL层实现代码</a>

> 注: 在 `wrappers/os` 下需要创建一个与 `tools/board/confg.XXX.YYY` 中的 `XXX` 一样的目录用于存放HAL实现

    $ cd wrappers/os/
    $ ls
    freertos  nos  nucleus  ubuntu

    wrappers/os$ cp -rf ubuntu arm-linux
    wrappers/os$ rm -f arm-linux/HAL_UART_linux.c

    wrappers/os$ ls
    arm-linux freertos  nos  nucleus  ubuntu

    wrappers/os$ tree -A arm-linux/

    arm-linux/
    +-- HAL_AWSS_linux.c
    +-- HAL_Crypt_Linux.c
    +-- HAL_FS_Linux.c
    +-- HAL_KV_linux.c
    +-- HAL_OS_linux.c
    +-- HAL_TCP_linux.c
    +-- HAL_UDP_linux.c

### <a name="打开之前被关闭的编译开关">打开之前被关闭的编译开关</a>

    $ vim tools/board/config.arm-linux.demo

    CONFIG_ENV_CFLAGS = \
        -D_PLATFORM_IS_LINUX_ \
        -Wall

    CONFIG_ENV_LDFLAGS = \
        -lpthread -lrt \

    OVERRIDE_CC = arm-linux-gnueabihf-gcc
    OVERRIDE_AR = arm-linux-gnueabihf-ar
    OVERRIDE_LD = arm-linux-gnueabihf-ld

    # CONFIG_wrappers :=

可以看到在`CONFIG_wrappers :=`这一行前添加了一个`#`符号, 代表这一行被注释掉了, `wrappers`将会进入编译过程

### <a name="尝试交叉编译被复制的HAL层代码">尝试交叉编译被复制的HAL层代码</a>

    $ make reconfig

    SELECT A CONFIGURATION:

    1) config.alios.mk3080
    2) config.arm-linux.demo
    3) config.ubuntu.x86
    #? 2

    SELECTED CONFIGURATION:

    VENDOR :   arm-linux
    MODEL  :   demo

    ...

    $ make

可以看到我们进展的十分顺利, 被复制的代码 `wrappers/os/arm-linux/*.c` 直接编译成功了, 产生了 `arm-linux` 格式的 `libiot_hal.a`

### <a name="交叉编译样例程序">交叉编译样例程序</a>

这样有了`libiot_hal.a`, `libiot_tls.a`, 以及`libiot_sdk.a`, 已经可以尝试交叉编译样例的可执行程序, 并在目标嵌入式硬件开发板上运行一下试试了

方法是去掉 `config.arm-linux.demo` 里面的 `-DNO_EXECUTABLES`开关, 使得`*/examples/`目录下的样例源码被编译出来

    $ vi tools/board/config.arm-linux.demo

    CONFIG_ENV_CFLAGS = \
        -D_PLATFORM_IS_LINUX_ \
        -Wall

    CONFIG_ENV_LDFLAGS = \
        -lpthread -lrt

    OVERRIDE_CC = arm-linux-gnueabihf-gcc
    OVERRIDE_AR = arm-linux-gnueabihf-ar
    OVERRIDE_LD = arm-linux-gnueabihf-ld

    # CONFIG_wrappers :=

可以看到在 `-DNO_EXECUTABLES` 开关从 `CONFIG_ENV_CFLAGS` 中去掉了, 例子可执行程序进入了编译范围

### <a name="重新载入配置文件, 交叉编译可执行程序">重新载入配置文件, 交叉编译可执行程序</a>

    $ make reconfig
    $ make

如果有如下的编译输出, 则代表 `mqtt-example` 等一系列样例程序已经被成功的编译出来, 它们存放在 `output/release/bin` 目录下

    [LD] dev-sign-example                   <=  ...
    [LD] mqtt-example                       <=  ...
    [LD] linkkit-example-solo               <=  ...

    $ cd output/release/bin/
    $ ls
    dev-sign-example  linkkit-example-solo  mqtt-example

    $ file *

    dev-sign-example:     ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked ...
    linkkit-example-solo: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked ...
    mqtt-example:         ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked ...

可以用`file`命令验证, 这些可执行程序确实是交叉编译到 `arm-linux` 架构上的

### <a name="尝试运行样例程序">尝试运行样例程序</a>

接下来, 您就可以把样例程序例如`mqtt-example`, 用`SCP`, `TFTP`或者其它方式, 拷贝下载到您的目标开发板上运行调试了

- 如果一切顺利, 样例程序和同样例程在 `Ubuntu` 上运行效果相同, 则证明 `wrappers/os/arm-linux` 部分的HAL层代码工作正常
- 如果样例程序运行起来, 和同样例程在 `Ubuntu` 上运行效果不同, 则需要再重点修改调试HAL实现
- 也就是指 `wrappers/os/arm-linux` 目录的HAL层代码, 因为这些代码是我们从 `Ubuntu` 主机部分复制的, 完全可能并不适合 `arm-linux`

如此反复直到确保 `libiot_hal.a` 的开发没问题为止

