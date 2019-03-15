# <a name="目录">目录</a>
+ [使用SDK自带编译系统时的移植示例](#使用SDK自带编译系统时的移植示例)
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
    * [交叉编译样例程序](#交叉编译样例程序)
        - [重新载入配置文件, 交叉编译可执行程序](#重新载入配置文件, 交叉编译可执行程序)
        - [尝试运行样例程序](#尝试运行样例程序)


# <a name="使用SDK自带编译系统时的移植示例">使用SDK自带编译系统时的移植示例</a>

> 本文以将SDK移植到 `arm-linux` 平台为例, 演示一个完整的交叉编译移植过程

提示: 如果您移植SDK的时候选择的是"不使用SDK自带编译系统", 则可以跳过本章
---

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

## <a name="交叉编译样例程序">交叉编译样例程序</a>

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
