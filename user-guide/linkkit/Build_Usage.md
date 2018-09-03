# <a name="目录">目录</a>
+ [第四章 构建配置系统](#第四章 构建配置系统)
    * [4.1 基于 make 的编译系统详解](#4.1 基于 make 的编译系统详解)
        - [常用命令](#常用命令)
        - [输出说明](#输出说明)
        - [组成部分](#组成部分)
            + [用户输入](#用户输入)
            + [构建单元](#构建单元)
            + [makefile](#makefile)
            + [build-rules](#build-rules)
            + [project.mk](#project.mk)
            + [src/board/config.xxx.yyy](#src/board/config.xxx.yyy)
            + [src/xxx/yyy/iot.mk](#src/xxx/yyy/iot.mk)
        - [调试方式](#调试方式)
    * [4.2 config.xxx.yyy 文件详解](#4.2 config.xxx.yyy 文件详解)
        - [交叉编译相关](#交叉编译相关)
        - [目录文件相关](#目录文件相关)
        - [资源耗费相关](#资源耗费相关)
            + [CONFIG_MBEDTLS_DEBUG_LEVEL](#CONFIG_MBEDTLS_DEBUG_LEVEL)
            + [CONFIG_MQTT_TX_MAXLEN](#CONFIG_MQTT_TX_MAXLEN)
            + [CONFIG_MQTT_RX_MAXLEN](#CONFIG_MQTT_RX_MAXLEN)
    * [4.3 基于 make 编译到主机例程](#4.3 基于 make 编译到主机例程)
        - [用 make 为64位Linux/OSX编译](#用 make 为64位Linux/OSX编译)
        - [用 make 为32位Linux/OSX编译](#用 make 为32位Linux/OSX编译)
        - [用 make 为Windows编译](#用 make 为Windows编译)
    * [4.4 基于 make 交叉编译到嵌入式平台](#4.4 基于 make 交叉编译到嵌入式平台)
        - [用 make 为 arm-linux 编译](#用 make 为 arm-linux 编译)
        - [用 make 为庆科模组 MK3060/MK3080 编译](#用 make 为庆科模组 MK3060/MK3080 编译)
        - [用 make 为乐鑫模组 ESP8266 编译](#用 make 为乐鑫模组 ESP8266 编译)
    * [4.5 典型产品的 make.settings 示例](#4.5 典型产品的 make.settings 示例)
        - [不具有网关功能的WiFi模组](#不具有网关功能的WiFi模组)
        - [具有网关功能的WiFi模组](#具有网关功能的WiFi模组)
        - [GSM模组](#GSM模组)
        - [基于Linux系统的网关](#基于Linux系统的网关)
    * [4.6 用 make.settings 文件裁剪 C-SDK 详解](#4.6 用 make.settings 文件裁剪 C-SDK 详解)
        - [FEATURE_ALCS_ENABLED](#FEATURE_ALCS_ENABLED)
        - [FEATURE_COAP_COMM_ENABLED](#FEATURE_COAP_COMM_ENABLED)
        - [FEATURE_COAP_DTLS_SUPPORT](#FEATURE_COAP_DTLS_SUPPORT)
        - [FEATURE_DEPRECATED_LINKKIT](#FEATURE_DEPRECATED_LINKKIT)
        - [FEATURE_ENHANCED_GATEWAY](#FEATURE_ENHANCED_GATEWAY)
        - [FEATURE_HTTP2_COMM_ENABLED](#FEATURE_HTTP2_COMM_ENABLED)
        - [FEATURE_HTTP_COMM_ENABLED](#FEATURE_HTTP_COMM_ENABLED)
        - [FEATURE_MQTT_COMM_ENABLED](#FEATURE_MQTT_COMM_ENABLED)
        - [FEATURE_MQTT_DIRECT](#FEATURE_MQTT_DIRECT)
        - [FEATURE_MQTT_SHADOW](#FEATURE_MQTT_SHADOW)
        - [FEATURE_OTA_ENABLED](#FEATURE_OTA_ENABLED)
        - [FEATURE_OTA_FETCH_CHANNEL](#FEATURE_OTA_FETCH_CHANNEL)
        - [FEATURE_OTA_SIGNAL_CHANNEL](#FEATURE_OTA_SIGNAL_CHANNEL)
        - [FEATURE_SDK_ENHANCE](#FEATURE_SDK_ENHANCE)
        - [FEATURE_SUBDEVICE_ENABLED](#FEATURE_SUBDEVICE_ENABLED)
        - [FEATURE_SUPPORT_ITLS](#FEATURE_SUPPORT_ITLS)
        - [FEATURE_SUPPORT_TLS](#FEATURE_SUPPORT_TLS)
        - [FEATURE_WIFI_AWSS_ENABLED](#FEATURE_WIFI_AWSS_ENABLED)
    * [4.7 在 Ubuntu/OSX 上使用 cmake 的编译示例](#4.7 在 Ubuntu/OSX 上使用 cmake 的编译示例)
        - [用 cmake 为64位Linux/OSX编译](#用 cmake 为64位Linux/OSX编译)
        - [用 cmake 为32位Linux/OSX编译](#用 cmake 为32位Linux/OSX编译)
        - [用 cmake 为arm-linux编译](#用 cmake 为arm-linux编译)
        - [用 cmake 为Windows编译](#用 cmake 为Windows编译)

# <a name="第四章 构建配置系统">第四章 构建配置系统</a>

目前设备端C-SDK的构建配置系统支持以下的编译方式
---
+ 在`Linux`上或者`MacOS`上以`GNU Make` + `各种工具链`编译, 产生`各种嵌入式目标架构`的SDK, 本章将演示
    * 以`GNU Make` + `gcc`, 产生适用于 `64位Linux/OSX` 的SDK以及可执行例程
    * 以`GNU Make` + `gcc`, 产生适用于 `32位Linux/OSX` 的SDK以及可执行例程
    * 以`GNU Make` + `i686-w64-mingw32-gcc`, 产生适用于 `Windows` 平台的SDK以及可执行例程
    * 以`GNU Make` + `arm-none-eabi-gcc`, 产生适用于 `MK3060/MK3080` 嵌入式平台的SDK
    * 以`GNU Make` + `xtensa-lx106-elf-gcc`, 产生适用于 `ESP8266` 嵌入式平台的SDK
    * 以`GNU Make` + `arm-linux-gnueabihf-gcc`, 产生适用于 `arm-linux` 嵌入式平台的SDK

+ 在`Linux`上或者`MacOS`上以`cmake` + `各种工具链`编译, 产生`各种目标架构`的SDK, 本章将演示
    * 以`cmake` + `gcc`, 产生适用于 `64位Linux/OSX` 的SDK
    * 以`cmake` + `gcc`, 产生适用于 `32位Linux/OSX` 的SDK
    * 以`cmake` + `arm-linux-gnueabihf-gcc`, 产生适用于 `arm-linux` 嵌入式平台的SDK
    * 以`cmake` + `i686-w64-mingw32-gcc`, 产生适用于 `Windows` 平台的SDK

未来可能支持以下的编译方式
---
+ 在`Windows`上以`Keil IDE`编译, 产生适用于 `ARM` 嵌入式平台的SDK
+ 在`Windows`上以`IAR IDE`编译, 产生适用于 `ARM` 嵌入式平台的SDK
+ 在`Windows`上以`cmake` + `mingw32`编译, 产生适用于 `Windows` 平台的SDK
+ 在`Windows`上以`VS Code 2017`编译, 产生适用于 `Windows` 平台的SDK
+ 在`Windows`上以`QT Creator`编译, 产生适用于 `Windows` 平台的SDK
+ 在`Windows`上以`Visual Studio 2017`编译, 产生适用于 `Windows` 平台的SDK

目前设备端C-SDK的构建配置系统的配置/裁剪接口是由以下三者组合提供
---
+ **硬件平台维度:** `src/board/config.xxx.yyy`, 这些文件在下文中, **也称 `config` 文件**
+ **功能模块维度:** `make.settings`
+ **资源伸缩维度:** `include/imports/iot_import_config.h`

> 本章先说明编译系统及其 config 文件的语法, 接着演示如何交叉编译到嵌入式目标平台和不交叉的编译 Linux/OSX/Windows 主机 demo 版本
>
> 再说明配置相关的文件 make.settings 的用法, 介绍在平台选定之后, 如何裁剪和配置功能
>
> 最后介绍了跨平台的 cmake 编译系统, 它接受配置的地方仍是 make.settings 文件, 但可适用于更多的开发环境

## <a name="4.1 基于 make 的编译系统详解">4.1 基于 make 的编译系统详解</a>

### <a name="常用命令">常用命令</a>

| 命令                  | 解释
|-----------------------|-----------------------------------------------------------------------------------
| `make distclean`    | **清除一切构建过程产生的中间文件, 使当前目录仿佛和刚刚clone下来一样**
| `make [all]`        | **使用默认的平台配置文件开始编译**
| `make env`          | **显示当前编译配置, 非常有用, 比如可显示交叉编译链, 编译CFLAGS等**
| `make reconfig`     | **弹出多平台选择菜单, 用户可按数字键选择, 然后根据相应的硬件平台配置开始编译**
| `make config`       | **显示当前被选择的平台配置文件**
| `make help`         | **打印帮助文本**
| `make <directory>`  | **单独编译被<directory>指定的目录, 或者叫构建单元**
| `make test`         | **运行指定的测试集程序, 统计显示测试例的通过率和源代码的覆盖率**

### <a name="输出说明">输出说明</a>
成功编译的话, 最终会打印如下表格, 这是每个模块的ROM占用, 以及静态RAM占用的统计

    | MODULE NAME                                 | ROM      | RAM      | BSS      | DATA |
    |---------------------------------------------|----------|----------|----------|------|
    | src/services/linkkit/dm                     | 174964   | 256      | 244      | 12   |
    | src/ref-impl/tls                            | 156594   | 8992     | 8920     | 72   |
    | src/protocol/alcs                           | 73517    | 281      | 241      | 40   |
    | src/services/awss                           | 64675    | 1639     | 1584     | 55   |
    | src/infra/utils                             | 48015    | 368      | 296      | 72   |
    | src/protocol/mqtt                           | 44310    | 64       | 48       | 16   |
    | src/services/uOTA                           | 43527    | 916      | 556      | 360  |
    | src/ref-impl/hal                            | 31626    | 84       | 60       | 24   |
    | src/services/linkkit/cm                     | 30943    | 99       | 99       | 0    |
    | src/sdk-impl                                | 14601    | 40       | 40       | 0    |
    | src/infra/system                            | 6707     | 1504     | 1432     | 72   |
    | src/infra/log                               | 2112     | 528      | 528      | 0    |
    |---------------------------------------------|----------|----------|----------|------|
    | - IN TOTAL -                                | 691591   | 14771    | 14048    | 723  |

**用户需要关注的输出产品都在 `output/release` 目录下:**

output/release/lib
---
| 产物文件名 | 说明
|------------|------
| `libiot_hal.a`  | HAL接口层的参考实现, 提供了 `HAL_XXX()` 接口
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口
| `libiot_tls.a`  | 裁剪过的 `mbedtls`, 提供了 `mbedtls_xxx()` 接口, 支撑 `libiot_hal.a`

output/release/include
---
| 产物文件名 | 说明
|------------|------
| `iot_import.h`  | 列出所有需要C-SDK的用户提供给SDK的底层支撑接口, 详见 [第五章 HAL说明](#第五章 HAL说明) 部分
| `iot_export.h`  | 列出所有C-SDK向用户提供的底层API编程接口, 详见 [第六章 API说明](#第六章 API说明) 部分

output/release/bin
---
如果是在主机环境下不做交叉编译(Ubuntu/OSX/Windows), 是可以产生主机版本的demo程序, 可以直接运行的, 比如

| 产物文件名 | 说明
|------------|------
| `linkkit-example-solo`  | 高级版的例程, 可演示 `linkkit_xxx()` 接口的使用
| `mqtt-example`          | 基础版的例程, 可演示 `IOT_XXX()` 接口的使用

### <a name="组成部分">组成部分</a>

#### <a name="用户输入">用户输入</a>
设备端C-SDK的构建配置系统, 有以下三个输入文件可接受用户的配置, 您可以通过编辑它们, 将配置输入到构建系统中
+ **功能配置文件:** 即顶层目录的 `make.settings` 文本文件
+ **平台配置文件:** 即目录 `src/board` 下的 `config.xxx.yyy` 系列文件, 也称config文件
+ **伸缩配置文件:** 即 `include/imports/iot_import_config.h` 文件

---
构建系统最终是依据 `config.xxx.yyy` 文件进行编译, 然而由于功能配置/裁剪更为常用, 我们将它额外抽取到了 `make.settings` 中

> config.xxx.yyy 主要关注目标嵌入式硬件平台的工具链程序和编译/链接选项的指定, 用于跨平台移植
>
> config.xxx.yyy 此外也能以 CONFIG_ENV_CFLAGS += ... 的语法新增自定义 CFLAGS, 覆盖 iot_import_config.h 中列出的可配置选项
>
> make.settings 则是在已被确定的目标硬件平台上, 专注于C-SDK的功能模块裁剪或者配置, 用于裁剪功能模块
>
> iot_import_config.h 是接着对已被确定的功能, 列出"资源耗费"方面, 伸缩性质的可配置项, 如时间长度/内存大小/重试次数/线程多少/日志简繁等

#### <a name="构建单元">构建单元</a>
+ 片段文件: 定义为用来指导某个具体的文件夹下源码应如何获取和编译以及链接的makefile文本片段, 基本只包含变量的赋值
+ 默认的片段文件是名为`iot.mk`的文本文件, 可以在工程顶层目录`project.mk`里, 用`MAKE_SEGMENT := <new>`更改片段文件名
+ 从工程顶层目录以下, 每一个含有`iot.mk`的子目录, 都被构建系统认为是一个**构建单元**

> 举例来说:

> 假设在/path/to/project/project.mk文件中有
>
>     MAKE_SEGEMENT := build.mk

> 那么, 如下的目录排布
>
>     /path/to/project/bar/build.mk
>     /path/to/project/foo/sub1/build.mk
>     /path/to/project/foo/sub2/build.mk
>     /path/to/project/mmm/nnn/ppp/qqq/build.mk

> 会导致
>
>     bar
>     foo/sub1
>     foo/sub2
>     mmm/nnn/ppp/qqq

> 都被认为是构建单元, 可以用类似`make bar`, `make foo/bar1`这样的命令单独编译, 也可以调试完成后通过修改顶层`makefile`集成到工程中

#### <a name="makefile">makefile</a>
构建系统是基于`GNU Make`的, 所以它工作过程的起点是顶层目录的`makefile`文件

#### <a name="build-rules">build-rules</a>
构建系统核心, 指定编译的规则, 用户不需要关注, 也不应去修改

#### <a name="project.mk">project.mk</a>
指定目录排布, 工程名称, 版本信息等

| 变量名            | 说明
|-------------------|-------------------------------------------
| `CONFIG_DIR`    | *硬件平台配置文件*的存放目录, 其中的文件需要以`config.XXX.YYY`形式命名, 其中`XXX`一般表示操作系统, `YYY`一般表示具体的硬件型号
| `DIST_DIR`      | 最终的编译产物, 例如可执行程序, SDK的总库等, 存放的顶层目录
| `FINAL_DIR`     | 最终的编译产物摆放时, 会放在`$(FINAL_DIR)`下, 这个变量可以指定最终摆放位置在顶层目录下的子目录路径
| `IMPORT_DIR`    | 输入目录, 用于摆放预编译的二进制库, 以及使用这些预编译库对应的头文件
| `LCOV_DIR`      | 覆盖率文件产生目录, 用于存放`make test`命令产生的中间文件和最终的`HTML`格式图形化文件
| `MAKE_SEGMENT`  | 构建单元中的片段文件, 开发者唯一需要编辑的文件, 指导构建系统如何编译该文件所在的构建单元
| `OUTPUT_DIR`    | 编译树中间目录
| `PACKAGE_DIR`   | 打包目录, 其中可以存放其它组件, 也可以存放压缩包形式的第三方软件, 例如`openssl-0.9.8.tar.gz`等
| `RULE_DIR`      | 构建系统核心目录, 如果不喜欢`build-rules`存放在自己的工程里, 完全可以通过这个变量把它移出去
| `TOP_DIR`       | 工程的顶级目录, 默认就是顶层`makefile`所在的路径, 极少改动

可以完全不对它们做任何改动, 依照上文的含义和默认值来排布自己工程

#### <a name="src/board/config.xxx.yyy">src/board/config.xxx.yyy</a>

在默认的`configs`目录, 或者在指定的`$(CONFIG_DIR)`目录下, 文件名形式为`config.<VENDOR>.<MODEL>`的文本文件, 会被构建系统认为是硬件平台配置文件, 每个文件对应一个嵌入式软硬件平台

* 其中<VENDOR>部分, 一般是指明嵌入式平台的软件OS提供方, 如`mxchip`, `ubuntu`, `win7`等. 另外, 这也会导致构建系统到`$(IMPORT_DIR)/<VENDOR>`目录下寻找预编译库的二进制库文件和头文件
* 其中<MODEL>部分, 一般是标明嵌入式平台的具体硬件型号, 如`mtk7687`, `qcom4004`等, 不过也可以写上其它信息, 因为构建系统不会去理解它, 比如`mingw32`, `x86-64`等

> 例如`config.mxchip.3080c`文件, 如果在`make reconfig`的时候被选择, 则会导致:

* 构建系统在`import/mxchip/`目录下寻找预编译库的二进制库文件和头文件
* 构建系统使用该文件内的变量指导编译行为, 具体来说, 可以根据说明使用如下变量

#### <a name="src/xxx/yyy/iot.mk">src/xxx/yyy/iot.mk</a>
以下是在片段文件中可以使用的变量及其含义

输出相关
---
| 变量名            | 用处
|-------------------|-----------------------------------------------------------------------------------------------
| ORIGIN          | 如果你的模块希望按自己独特的方式编译, 比如`./configure ...; make; ...`, 需要设置此变量为1
| LIBSO_TARGET    | 设置本单元被编译出的`Linux`动态库文件名, 对它赋值导致系统帮助你产生`*.so`共享库文件
| LIBA_TARGET     | 设置本单元被编译出的静态库文件名, 对它赋值导致系统帮助你产生`*.a`静态库文件
| TARGET          | 设置本单元被编译出的可执行程序文件名, 对它赋值导致系统帮助你产生`Linux`下的可执行程序
| KMOD_TARGET     | 设置本单元被编译出的内核模块文件名, 对它赋值导致系统帮助你产生`Linux`下的`*.ko`内核模块

输入相关
---
| 变量名            | 用处
|-------------------|-----------------------------------------------------------------------------------------------
| LIB_SRCS    | 可选, 列出哪些`*.c`文件被编译成`*.a`或者`*.so`
| SRCS        | 可选, 列出哪些`*.c`文件被编译成可执行程序
| EXTRA_SRCS  | 可选, 列出哪些源文件需要在开始编译之前被复制到临时的编译目录
| CFLAGS      | 可选, 列出需要特别应用在本模块上的编译选项
| LDFLAGS     | 可选, 列出需要特别应用在本模块上的链接选项

模块组合相关
---
| 变量名            | 用处
|-------------------|-----------------------------------------------------------------------------------------------
| LIB_HEADERS     | 可选, 如果你的模块被编译完成提供一些头文件供其它模块使用, 可以在这个变量列出, 编译后会安装到系统目录
| LIB_HDRS_DIR    | 可选, 如果你的模块希望把被安装的头文件(`LIB_HEADERS`指定)安装到系统目录下的某个子目录, 可以用这个变量指定子目录的名字
| HDR_REFS        | 可选, 如果你的模块引用了其它模块的头文件, 那么需要设置这个变量, 列出被引用模块在工程里的相对路径
| DEPENDS         | 可选, 如果你的模块需要在其它模块构建完成后才能编译, 那么需要设置这个变量, 列出被依赖模块在工程里的相对路径

高级用法
---
| 变量名            | 用处
|-------------------|-----------------------------------------------------------------------------------------------
| PKG_SOURCE      | 可选, 表示源码来自`*.c`之外的形式, 例如压缩包文件, 或者独立的git仓库等
| PKG_BRANCH      | 可选, 仅在`PKG_SOURCE`是git仓库的时候才有意义, 指定使用该仓库的哪个分支作为源码输入
| PKG_REVISION    | 可选, 仅在`PKG_SOURCE`是git仓库的时候才有意义, 指定使用该仓库的哪个分支上的哪个`commit`或者`tag`
| PKG_UPSTREAM    | 可选, 仅在`PKG_SOURCE`是git仓库的时候才有意义, 指定使用哪个云端的git仓库地址来更新本地git仓库

多个可执行程序
---
* 如果你的模块希望产生单个的可执行程序, 那么对变量`TARGET`和`SRCS`赋值就可以了, 但如果希望产生多个可执行程序, 比如

        TARGET = prog1 prog2 prog3

* 那么需要用`SRCS_<name>`变量对这多个可执行程序分别设定他们的源文件, 比如

        SRCS_prog1 = prog1.c
        SRCS_prog2 = prog2.c library_A.c
        SRCS_prog3 = prog3.c library_A.c library_B.c

* 目前除了`SRCS`变量, 没有其它变量支持这种带后缀的语法, 对多个可执行程序的`DEPENDS`, `LDFLAGS`等, 仍是整体指定

### <a name="调试方式">调试方式</a>

+ 在`make ...`命令行中, 设置`TOP_Q`变量为空, 可打印工程顶层的执行逻辑, 例如硬件平台的选择, SDK主库的生成等

        make .... TOP_Q=

+ 在`make ...`命令行中, 设置`Q`变量为空, 可打印模块内部的构建过程, 例如目标文件的生成, 头文件搜寻路径的组成等

        make .... Q=

+ 可以用`make foo/bar`单独对`foo/bar`进行构建, 不过, 这可能需要先执行`make reconfig`
+ 可以进入`.O/foo/bar`路径, 看到完整的编译临时目录, 有makefile和全部源码, 所以在这里执行`make`, 效果和`make foo/bar`等同

## <a name="4.2 config.xxx.yyy 文件详解">4.2 config.xxx.yyy 文件详解</a>

### <a name="交叉编译相关">交叉编译相关</a>

| 变量                  | 说明
|-----------------------|-------------------------------------------
| `CONFIG_ENV_CFLAGS`     | 指定全局的`CFLAGS`编译选项, 传给`compiler`, 例如`CONFIG_ENV_CFLAGS += -DDEBUG`
| `CONFIG_ENV_LDFLAGS`    | 指定全局的`LDFLAGS`链接选项, 传给`linker`, 例如`CONFIG_ENV_LDFLAGS += -lcrypto`
| `CROSS_PREFIX`          | 指定交叉编译工具链共有的前缀, 例如`CROSS_PREFIX := arm-none-eabi-`, 会导致构建系统使用`arm-none-eabi-gcc`和`arm-none-eabi-ar`, 以及`arm-none-eabi-strip`等
| `OVERRIDE_CC`           | 当交叉工具链没有共有的前缀或者前缀不符合`prefix+gcc/ar/strip`类型时, 例如`armcc`, 可用`OVERRIDE_CC = armcc`单独指定C编译器
| `OVERRIDE_AR`           | 当交叉工具链没有共有的前缀或者前缀不符合`prefix+gcc/ar/strip`类型时, 例如`armar`, 可用`OVERRIDE_AR = armar`单独指定库压缩器
| `OVERRIDE_STRIP`        | 当交叉工具链没有共有的前缀或者前缀不符合`prefix+gcc/ar/strip`类型时, 例如`armcc`没有对应的strip程序, 可用`OVERRIDE_STRIP = true`单独指定strip程序不执行
| `CONFIG_LIB_EXPORT`     | 指定SDK产生的二进制库的形式, 例如``CONFIG_LIB_EXPORT := dynamic`可以指定产生linux上的`libiot_sdk.so`动态库文件, 默认为产生跨平台的`libiot_sdk.a`静态库

### <a name="目录文件相关">目录文件相关</a>

| 变量                  | 说明
|-----------------------|-------------------------------------------
| `CONFIG_mmm/nnn`    | 指定`mmm/nnn`目录是否需要编译的开关, 例如`CONFIG_mmm/nnn :=`的写法会导致该目录被跳过编译

### <a name="资源耗费相关">资源耗费相关</a>

文件 `include/imports/iot_import_config.h` 是 SDK 的内部文件, **用户不应直接修改这个文件**

它的使命是在目标硬件平台确定, 以及C-SDK内容功能确定之后, 进行"资源耗费"方面的伸缩性质的配置, 如时间长度/内存大小/重试次数/线程多少/日志简繁等

---
之所以会对该文件进行说明, 是因为可用的配置项都在这个文件中列出, 对这些配置项的值, 正确的调整方式是在 `config` 文件中, 以 `CONFIG_ENV_CFLAGS` 操作, 例如

`src/board/config.ubuntu.x86` 文件中的如下段落:

    CONFIG_ENV_CFLAGS   += \
        -DCONFIG_HTTP_AUTH_TIMEOUT=500 \
        -DCONFIG_MID_HTTP_TIMEOUT=500 \
        -DCONFIG_GUIDER_AUTH_TIMEOUT=500 \
        -DCONFIG_MQTT_RX_MAXLEN=5000 \
        -DCONFIG_MQTT_SUBTOPIC_MAXNUM=65535 \
        -DCONFIG_MBEDTLS_DEBUG_LEVEL=0 \

> 以下说明当前比较常用的配置项

#### <a name="CONFIG_MBEDTLS_DEBUG_LEVEL">CONFIG_MBEDTLS_DEBUG_LEVEL</a>
+ 调整在TLS连接过程中, 调试日志打印的详细程度
+ 可配置的值从 `0` 到 `10`, 数字越大, 打印越详细, 数字为 `0` 表示不打印调试信息
+ 起作用的源码目录在 `src/ref-impl/tls`

#### <a name="CONFIG_MQTT_TX_MAXLEN">CONFIG_MQTT_TX_MAXLEN</a>
+ 调整在高级版中, 通信模块为MQTT上行报文所开辟的常驻内存缓冲区长度
+ 可配置的值为整数, 建议`512`到`2048`, 过小会导致上行报文不够空间组装, 过大会导致设备上RAM资源消耗增大
+ 起作用的源码目录在 `src/services/linkkit`

#### <a name="CONFIG_MQTT_RX_MAXLEN">CONFIG_MQTT_RX_MAXLEN</a>
+ 调整在高级版中, 通信模块为MQTT上行报文所开辟的常驻内存缓冲区长度
+ 可配置的值为整数, 建议`512`到`2048`, 过小会导致上行报文不够空间组装, 过大会导致设备上RAM资源消耗增大
+ 起作用的源码目录在 `src/services/linkkit`

## <a name="4.3 基于 make 编译到主机例程">4.3 基于 make 编译到主机例程</a>
> 本节的示例适用于开发者的开发环境是 Ubuntu16.04 的Linux主机, 或者是Apple公司的 OSX 的MacOS的情况

**这些例子都是在64位主机上的执行情况, 推荐您和阿里开发者一样, 安装64位的操作系统**

### <a name="用 make 为64位Linux/OSX编译">用 make 为64位Linux/OSX编译</a>
> 希望编译产物适用于64位的Ubuntu或OSX的目标平台时
>
> C-SDK对其HAL和TLS都已有官方提供的参考实现, 因此可以完整编译出所有的库和例子程序

选择平台配置
---

    make reconfig
    SELECT A CONFIGURATION:

    1) config.macos.make    3) config.ubuntu.x86
    2) config.rhino.make    4) config.win7.mingw32
    #? 3

    SELECTED CONFIGURATION:

    VENDOR :   ubuntu
    MODEL  :   x86
    ...
    ...

*如果是为64位的OSX目标平台编译, 比如您希望例程直接在64位系统的苹果电脑上执行, 这一步需选择 `config.macos.make`*

编译
---

    make

获取二进制库
---

    cd output/release/lib
    ls

其中有三个主要产物, **它们都是64位架构的**:

| 产物文件名 | 说明
|------------|------
| `libiot_hal.a`  | HAL接口层的参考实现, 提供了 `HAL_XXX()` 接口
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口
| `libiot_tls.a`  | 裁剪过的 `mbedtls`, 提供了 `mbedtls_xxx()` 接口, 支撑 `libiot_hal.a`

获取可执行程序
---

    cd output/release/bin
    ls

其中有两个主要产物, **它们都是64位架构的**:

| 产物文件名 | 说明
|------------|------
| `linkkit-example-solo`  | 高级版(旧版API)的例程, 可演示 `linkkit_xxx()` 接口的使用
| `mqtt-example`          | 基础版的例程, 可演示 `IOT_XXX()` 接口的使用

### <a name="用 make 为32位Linux/OSX编译">用 make 为32位Linux/OSX编译</a>
> 如果您编译SDK是在一台安装了32位 Linux/OSX 的机器上, 那么直接重复上面 [用 make 为64位Linux/OSX编译](#用 make 为64位Linux/OSX编译) 的步骤, 即可得到32位的库和例程
>
> 如果您是在安装了64位 `Ubuntu16.04` 的机器上, 需要编译出32位的库, 请按照下文操作

安装32位工具链
---

    sudo apt-get install -y libc6:i386 libstdc++6:i386 gcc:i386

修改平台配置文件
---

    vim src/board/config.ubuntu.x86

增加如下一行

    CONFIG_ENV_CFLAGS   += -m32

比如:

    cat src/board/config.ubuntu.x86

    CONFIG_ENV_CFLAGS   += \
        -Os -Wall \
        -g3 --coverage \
        -D_PLATFORM_IS_LINUX_ \
        -D__UBUNTU_SDK_DEMO__ \

    ...
    ...
    CONFIG_ENV_LDFLAGS  += -lpthread -lrt

    CONFIG_ENV_LDFLAGS  += -m32
    OVERRIDE_STRIP      := strip

选择平台配置
---

    make reconfig
    SELECT A CONFIGURATION:

    1) config.macos.make    3) config.ubuntu.x86
    2) config.rhino.make    4) config.win7.mingw32
    #? 3

*如果是为32位的OSX目标平台编译, 比如您希望例程直接在32位系统的苹果电脑上执行, 这一步需修改和选择 `config.macos.make`*

编译
---

    make

获取二进制库
---

    cd output/release/lib
    ls

其中有三个主要产物, **它们都是32位架构的**:

| 产物文件名 | 说明
|------------|------
| `libiot_hal.a`  | HAL接口层的参考实现, 提供了 `HAL_XXX()` 接口
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口
| `libiot_tls.a`  | 裁剪过的 `mbedtls`, 提供了 `mbedtls_xxx()` 接口, 支撑 `libiot_hal.a`

获取可执行程序
---

    cd output/release/bin
    ls

其中有两个主要产物, **它们都是32位架构的**:

| 产物文件名 | 说明
|------------|------
| `linkkit-example-solo`  | 高级版(旧版API)的例程, 可演示 `linkkit_xxx()` 接口的使用
| `mqtt-example`          | 基础版的例程, 可演示 `IOT_XXX()` 接口的使用

可以用如下方式验证, 注意 `file` 命令的输出中, 已经显示程序都是32位的了(`ELF 32-bit LSB executable`)

    file output/release/bin/*

    output/release/bin/linkkit-example-countdown: ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/linkkit-example-sched:     ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/linkkit-example-solo:      ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/linkkit_tsl_convert:       ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/mqtt-example:              ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/mqtt-example-multithread:  ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/mqtt-example-rrpc:         ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/ota_mqtt-example:          ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/sdk-testsuites:            ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/uota_app-example:          ELF 32-bit LSB executable, Intel 80386, ... stripped

### <a name="用 make 为Windows编译">用 make 为Windows编译</a>

安装 mingw-w64-i686 工具链
---
在 `Ubuntu16.04` 上, 运行如下命令安装交叉编译工具链

    sudo apt-get install -y gcc-mingw-w64-i686

以如下命令和输出确认交叉编译工具链已安装好

    i686-w64-mingw32-gcc --version

    i686-w64-mingw32-gcc (GCC) 5.3.1 20160211
    Copyright (C) 2015 Free Software Foundation, Inc.

选择平台配置
---
    make reconfig
    SELECT A CONFIGURATION:

    1) config.esp8266.aos   4) config.mk3080.aos    7) config.win7.mingw32
    2) config.macos.make    5) config.rhino.make
    3) config.mk3060.aos    6) config.ubuntu.x86
    #? 7

编译
---
    make -j32 all

**需要注意, 对Windows平台, 使用 `make` 命令编译只能得到二进制库, 使用 `make all` 命令编译能同时得到库和可执行程序**

获取和运行可执行例程
---
    cd output/release/bin
    ls

其中有两个主要产物, **它们都是可直接在Windows上运行的**:

| 产物文件名 | 说明
|------------|------
| `mqtt-example.exe`          | 基础版的例程, 可演示 `IOT_XXX()` 接口的使用
| `linkkit_tsl_convert.exe`   | 高级版的物模型转换工具, 何时需要使用它详见 [第二章 快速开始](#第二章 快速开始) 章节

目前对Windows平台仅提供基础版的例程, 可以把它拿到Windows主机上运行, 如下图则是在一台`Win10`主机上运行的效果

![image](https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/win_mqtt_example.png)

## <a name="4.4 基于 make 交叉编译到嵌入式平台">4.4 基于 make 交叉编译到嵌入式平台</a>
> 本节的示例适用于开发者的开发环境是 Ubuntu16.04 的Linux主机, 或者是Apple公司的 OSX 的MacOS的情况

**这些例子都是在64位主机上的执行情况, 推荐您和阿里开发者一样, 安装64位的操作系统**

### <a name="用 make 为 arm-linux 编译">用 make 为 arm-linux 编译</a>
*本节的例子, 在 [第三章 移植指南](#第三章 移植指南) 中也有描述, 是作为一个演示跨平台移植的典型, 会写的更为详细*

安装交叉编译工具链
---
    sudo apt-get install -y gcc-arm-linux-gnueabihf

以如下命令和输出确认交叉编译工具链已安装好

    arm-linux-gnueabihf-gcc --version

    arm-linux-gnueabihf-gcc (Ubuntu/Linaro 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609
    Copyright (C) 2015 Free Software Foundation, Inc.

创建平台配置文件
---
    vim src/board/config.arm-linux.demo

    CONFIG_ENV_CFLAGS = \
        -D_PLATFORM_IS_LINUX_ \
        -Wall

    CONFIG_src/ref-impl/hal         :=
    CONFIG_examples                 :=
    CONFIG_tests                    :=
    CONFIG_src/tools/linkkit_tsl_convert :=

    OVERRIDE_CC = arm-linux-gnueabihf-gcc
    OVERRIDE_AR = arm-linux-gnueabihf-ar
    OVERRIDE_LD = arm-linux-gnueabihf-ld

或者写成

    CONFIG_ENV_CFLAGS = \
        -D_PLATFORM_IS_LINUX_ \
        -Wall

    CONFIG_src/ref-impl/hal         :=
    CONFIG_examples                 :=
    CONFIG_tests                    :=
    CONFIG_src/tools/linkkit_tsl_convert :=

    CROSS_PREFIX := arm-linux-gnueabihf-

选择平台配置
---
    make reconfig
    SELECT A CONFIGURATION:

    1) config.arm-linux.demo  4) config.mk3060.aos      7) config.ubuntu.x86
    2) config.esp8266.aos     5) config.mk3080.aos      8) config.win7.mingw32
    3) config.macos.make      6) config.rhino.make
    #? 1

编译
---
    make

获取二进制库
---

    cd output/release/lib
    ls

其中有两个主要产物, **它们都是arm-linux架构的**:

| 产物文件名 | 说明
|------------|------
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口
| `libiot_tls.a`  | 裁剪过的 `mbedtls`, 提供了 `mbedtls_xxx()` 接口, 支撑 `libiot_hal.a`

### <a name="用 make 为庆科模组 MK3060/MK3080 编译">用 make 为庆科模组 MK3060/MK3080 编译</a>

选择平台配置
---
    make reconfig
    SELECT A CONFIGURATION:

    1) config.esp8266.aos   4) config.mk3080.aos    7) config.win7.mingw32
    2) config.macos.make    5) config.rhino.make
    3) config.mk3060.aos    6) config.ubuntu.x86
    #? 3

*如果是为庆科MK3080编译, 则选择4*

编译
---
    make

如果您当前的开发主机上没有安装庆科 `MK3060/MK3080` 的交叉工具链并导出到 `PATH` 中, C-SDK会自动下载它们

    make

    BUILDING WITH EXISTING CONFIGURATION:

    VENDOR :   mk3060
    MODEL  :   aos

    https://gitee.com/alios-things/gcc-arm-none-eabi-linux -> .O/compiler/gcc-arm-none-eabi-linux/main
    ---
    downloading toolchain for arm-none-eabi-gcc .................... [\]

下载完成后, 自动开始交叉编译SDK的源码

    https://gitee.com/alios-things/gcc-arm-none-eabi-linux -> .O/compiler/gcc-arm-none-eabi-linux/main
    ---
    downloading toolchain for arm-none-eabi-gcc .................... done

    [CC] utils_epoch_time.o                 <=  ...
    [CC] json_parser.o                      <=  ...
    ...
    ...
    [AR] libiot_sdk.a                       <=  ...

获取二进制库
---

    cd output/release/lib
    ls

其中有一个主要产物, **它是 MK3060/MK3080 架构的**:

| 产物文件名 | 说明
|------------|------
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口

### <a name="用 make 为乐鑫模组 ESP8266 编译">用 make 为乐鑫模组 ESP8266 编译</a>

选择平台配置
---
    make reconfig
    SELECT A CONFIGURATION:

    1) config.esp8266.aos   4) config.mk3080.aos    7) config.win7.mingw32
    2) config.macos.make    5) config.rhino.make
    3) config.mk3060.aos    6) config.ubuntu.x86
    #? 1

编译
---
    make

如果您当前的开发主机上没有安装乐鑫 `ESP8266` 的交叉工具链并导出到 `PATH` 中, C-SDK会自动下载它们

    make

    BUILDING WITH EXISTING CONFIGURATION:

    VENDOR :   esp8266
    MODEL  :   aos

    https://gitee.com/alios-things/gcc-xtensa-lx106-linux -> .O/compiler/gcc-xtensa-lx106-linux/main
    ---
    downloading toolchain for xtensa-lx106-elf-gcc .................... [/]

下载完成后, 自动开始交叉编译SDK的源码

    https://gitee.com/alios-things/gcc-arm-none-eabi-linux -> .O/compiler/gcc-arm-none-eabi-linux/main
    ---
    downloading toolchain for xtensa-lx106-elf-gcc .................... done

    [CC] utils_epoch_time.o                 <=  ...
    [CC] json_parser.o                      <=  ...
    ...
    ...
    [AR] libiot_sdk.a                       <=  ...


获取二进制库
---

    cd output/release/lib
    ls

其中有一个主要产物, **它是 ESP8266 架构的**:

| 产物文件名 | 说明
|------------|------
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口

## <a name="4.5 典型产品的 make.settings 示例">4.5 典型产品的 make.settings 示例</a>
> 解压之后, 打开功能配置文件 `make.settings`, 根据需要编辑配置项, 使用不同的编译配置, 编译输出的SDK内容以及examples都有所不同
>
> 以下针对C-SDK的客户中, 较多出现的几种产品形态, 给出典型的配置文件, **并在注释中说明为什么这样配置**

### <a name="不具有网关功能的WiFi模组">不具有网关功能的WiFi模组</a>
这种场景下客户使用WiFi上行的MCU模组, 比如乐鑫ESP8266, 庆科MK3060等.

这种场景下, 设备和阿里云直接的连接只用于它自己和云端的通信, 不会用于代理给其它嵌入式设备做消息上报和指令下发或固件升级等.

    FEATURE_MQTT_COMM_ENABLED    = y          # 一般WiFi模组都有固定供电, 所以都采用MQTT的方式上云
    FEATURE_MQTT_DIRECT          = y          # MQTT直连效率更高, 该选项只在部分海外设备上才会关闭
    FEATURE_OTA_ENABLED          = y          # 一般WiFi模组的客户, 都会使用阿里提供的固件升级服务
    FEATURE_SDK_ENHANCE          = y          # 一般WiFi模组片上资源充足, 可以容纳高级版, 所以打开
    FEATURE_ENHANCED_GATEWAY     = n          # 如上述说明, 不具备高级版网关功能的场景, 当然关闭这个选项
    FEATURE_WIFI_AWSS_ENABLED    = y          # 一般WiFi模组的客户, 都会使用阿里的配网app或sdk, 告诉模组SSID和密码
    FEATURE_SUPPORT_TLS          = y          # 绝大多数的客户都是用标准的TLS协议连接公网
    FEATURE_SUPPORT_ITLS         = n          # 阿里私有的iTLS标准和TLS是互斥的, 上面把TLS打开了, 这里必须关闭

### <a name="具有网关功能的WiFi模组">具有网关功能的WiFi模组</a>
这种场景下客户使用WiFi上行的MCU模组, 比如庆科MK3080等.

这种场景下, 设备和阿里云直接的连接不仅用于它自己和云端的通信, 还会用于代理给其它嵌入式设备做消息上报和指令下发或固件升级等.

    FEATURE_MQTT_COMM_ENABLED    = y          # 一般WiFi模组都有固定供电, 所以都采用MQTT的方式上云
    FEATURE_MQTT_DIRECT          = y          # MQTT直连效率更高, 该选项只在部分海外设备上才会关闭
    FEATURE_OTA_ENABLED          = y          # 一般WiFi模组的客户, 都会使用阿里提供的固件升级服务
    FEATURE_SDK_ENHANCE          = y          # 一般WiFi模组片上资源充足, 可以容纳高级版, 所以打开
    FEATURE_ENHANCED_GATEWAY     = y          # 如上述说明, 要具备高级版网关功能的场景, 当然打开这个选项
    FEATURE_WIFI_AWSS_ENABLED    = y          # 一般WiFi模组的客户, 都会使用阿里的配网app或sdk, 告诉模组SSID和密码
    FEATURE_SUPPORT_TLS          = y          # 绝大多数的客户都是用标准的TLS协议连接公网
    FEATURE_SUPPORT_ITLS         = n          # 阿里私有的iTLS标准和TLS是互斥的, 上面把TLS打开了, 这里必须关闭

### <a name="GSM模组">GSM模组</a>
这种场景下设备直接连接2G网络

    FEATURE_MQTT_COMM_ENABLED    = y          # 虽然CoAP更省电, 但不能做云端消息的及时下推, 所以目前GSM模组仍主要用MQTT的方式上云
    FEATURE_MQTT_DIRECT          = y          # MQTT直连效率更高, 对网络远慢于WiFi的GSM模组而言, 直连开关必须打开
    FEATURE_OTA_ENABLED          = y          # 一般GSM模组的客户, 也会使用阿里提供的固件升级服务
    FEATURE_SDK_ENHANCE          = n          # GSM模组网速很慢, 资源较少, 所以这种模组的客户一般不会用高级版, 而只需要基础版的MQTT上云 
    FEATURE_ENHANCED_GATEWAY     = n          # GSM模组一般不集成高级版(物模型)功能, 并且它也不会下联其它嵌入式设备分享MQTT上云通道
    FEATURE_WIFI_AWSS_ENABLED    = n          # GSM模组不通过WiFi协议连接公网, 因此关闭WiFi配网的开关
    FEATURE_SUPPORT_TLS          = y          # 绝大多数的客户都是用标准的TLS协议连接公网, 对GSM模组, 甚至可能连这个选项都关闭
    FEATURE_SUPPORT_ITLS         = n          # GSM模组有不少是做不加密通信(FEATURE_SUPPORT_TLS = n), 有加密也是走TLS, 所以ITLS都是关闭的

### <a name="基于Linux系统的网关">基于Linux系统的网关</a>
比如家庭网关, 工业网关等, 一般是相对MCU模组来说, 性能强大的多, 资源丰富的多的设备

    FEATURE_MQTT_COMM_ENABLED    = y          # 一般Linux网关都有固定供电, 所以都采用MQTT的方式上云
    FEATURE_MQTT_DIRECT          = y          # MQTT直连效率更高, 该选项只在部分海外设备上才会关闭
    FEATURE_OTA_ENABLED          = y          # 一般Linux网关的客户, 都会使用阿里提供的固件升级服务
    FEATURE_SDK_ENHANCE          = y          # 一般Linux网关片上资源充足, 可以容纳高级版, 所以打开
    FEATURE_ENHANCED_GATEWAY     = y          # 如上述说明, 要具备高级版网关功能的场景, 当然打开这个选项
    FEATURE_WIFI_AWSS_ENABLED    = y          # 取决于Linux网关是否用WiFi做上行并使用阿里的配网app/sdk, 如果皆是, 则打开这个选项
    FEATURE_SUPPORT_TLS          = y          # 绝大多数的客户都是用标准的TLS协议连接公网
    FEATURE_SUPPORT_ITLS         = n          # 阿里私有的iTLS标准和TLS是互斥的, 上面把TLS打开了, 这里必须关闭

## <a name="4.6 用 make.settings 文件裁剪 C-SDK 详解">4.6 用 make.settings 文件裁剪 C-SDK 详解</a>
> 解压之后, 打开功能配置文件 `make.settings`, 根据需要编辑配置项, 使用不同的编译配置, 编译输出的SDK内容以及examples都有所不同

默认的配置状态为
---
    FEATURE_MQTT_COMM_ENABLED    = y          # 是否打开MQTT通道的总开关
    FEATURE_MQTT_DIRECT          = y          # 是否打开MQTT直连的分开关
    FEATURE_OTA_ENABLED          = y          # 是否打开固件升级功能的分开关
    FEATURE_SDK_ENHANCE          = y          # 是否打开高级版功能的总开关
    FEATURE_ENHANCED_GATEWAY     = n          # 是否产生高级版网关SDK的分开关
    FEATURE_WIFI_AWSS_ENABLED    = y          # 是否打开WiFi配网功能的开关
    FEATURE_SUPPORT_TLS          = y          # 选择TLS安全连接的开关, 此开关与iTLS开关互斥
    FEATURE_SUPPORT_ITLS         = n          # 选择iTLS安全连接的开关, 此开关与TLS开关互斥, 使能ID2时需打开此开关

除此以外, 即使并未出现在默认 `make.settings` 文件中的选项, 只要在以下的列表中, 也仍然可以被添加到 `make.settings` 文件

### <a name="FEATURE_ALCS_ENABLED">FEATURE_ALCS_ENABLED</a>
+ 本地通信功能开关, 所谓本地通信是指搭载了C-SDK的嵌入式设备和手机app之间的局域网直接通信, 而不经过因特网和阿里云服务器中转
+ 可取的值是 `y` 或者 `n`
+ 它不依赖其它的 `FEATURE_XXX`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/protocol/alcs`

### <a name="FEATURE_COAP_COMM_ENABLED">FEATURE_COAP_COMM_ENABLED</a>
+ CoAP上云功能开关, 所谓CoAP上云是指搭载了C-SDK的嵌入式设备和阿里云服务器之间使用 `CoAP` 协议进行连接和交互
+ 可取的值是 `y` 或者 `n`
+ 它不依赖其它的 `FEATURE_XXX`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/protocol/coap`

### <a name="FEATURE_COAP_DTLS_SUPPORT">FEATURE_COAP_DTLS_SUPPORT</a>
+ CoAP上云DTLS开关, 配置设备和阿里云服务器之间使用 `CoAP` 协议进行连接和交互时, 是否要以 `DTLS` 方式进行身份认证和报文加解密
+ 可取的值是 `y` 或者 `n`
+ 它依赖于 `FEATURE_COAP_COMM_ENABLED` 的值是 `y`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/protocol/coap`

### <a name="FEATURE_DEPRECATED_LINKKIT">FEATURE_DEPRECATED_LINKKIT</a>
+ 高级版接口风格的开关, 配置进行高级版物模型相关的编程时, C-SDK是提供 `linkkit_xxx_yyy()` 风格的旧版接口, 还是提供 `IOT_Linkkit_XXX()` 风格的新版接口
+ 可取的值是 `y` 或者 `n`
+ 它依赖于 `FEATURE_SDK_ENHANCE` 的值是 `y`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/sdk-impl`

### <a name="FEATURE_ENHANCED_GATEWAY">FEATURE_ENHANCED_GATEWAY</a>
+ 高级版网关能力的开关, 配置进行高级版物模型相关的编程时, C-SDK是提供 `linkkit_xxx_yyy()` 风格的单品接口, 还是提供 `linkkit_gateway_xxx_yyy()` 风格的网关接口
+ 可取的值是 `y` 或者 `n`
+ 它依赖于 `FEATURE_SDK_ENHANCE` 的值是 `y`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/services/linkkit/dm`

### <a name="FEATURE_HTTP2_COMM_ENABLED">FEATURE_HTTP2_COMM_ENABLED</a>
+ HTTP2上云功能开关, 所谓HTTP2上云是指搭载了C-SDK的嵌入式设备和阿里云服务器之间使用 `HTTP2` 协议进行连接和交互
+ 可取的值是 `y` 或者 `n`
+ 它不依赖其它的 `FEATURE_XXX`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/protocol/http2`

### <a name="FEATURE_HTTP_COMM_ENABLED">FEATURE_HTTP_COMM_ENABLED</a>
+ HTTP/S上云功能开关, 所谓HTTP/S上云是指搭载了C-SDK的嵌入式设备和阿里云服务器之间使用 `HTTP` 协议或 `HTTPS` 协议进行连接和交互
+ 可取的值是 `y` 或者 `n`
+ 它不依赖其它的 `FEATURE_XXX`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/protocol/http`

### <a name="FEATURE_MQTT_COMM_ENABLED">FEATURE_MQTT_COMM_ENABLED</a>
+ MQTT上云功能开关, 所谓MQTT上云是指搭载了C-SDK的嵌入式设备和阿里云服务器之间使用 `MQTT` 协议进行连接和交互
+ 可取的值是 `y` 或者 `n`
+ 它不依赖其它的 `FEATURE_XXX`
+ 它被以下 `FEATURE_XXX` 所依赖, 是它们必需的前提
    * FEATURE_MQTT_DIRECT
    * FEATURE_MQTT_SHADOW
    * FEATURE_SUBDEVICE_ENABLED
    * FEATURE_WIFI_AWSS_ENABLED
    * FEATURE_SDK_ENHANCE
+ 对应的源码目录是 `src/protocol/mqtt`

### <a name="FEATURE_MQTT_DIRECT">FEATURE_MQTT_DIRECT</a>
+ MQTT直连功能开关, 所谓MQTT直连是指设备和阿里云服务器之间使用 `MQTT` 协议进行连接, 而不会前置基于 `HTTP` 协议认证的交互过程
+ 可取的值是 `y` 或者 `n`
+ 它依赖于 `FEATURE_MQTT_COMM_ENABLED` 的值是 `y`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/infra/system`

### <a name="FEATURE_MQTT_SHADOW">FEATURE_MQTT_SHADOW</a>
+ MQTT设备影子开关, 所谓MQTT设备影子是指设备在阿里云服务器之间上以JSON文档保留一份设备数据的镜像
+ 可取的值是 `y` 或者 `n`
+ 它依赖于 `FEATURE_MQTT_COMM_ENABLED` 的值是 `y`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/services/shadow`

### <a name="FEATURE_OTA_ENABLED">FEATURE_OTA_ENABLED</a>
+ 固件升级功能开关, 所谓固件升级是指设备从阿里云服务器上下载用户在IoT控制台中上传的固件文件功能
+ 可取的值是 `y` 或者 `n`
+ 它依赖于 `FEATURE_MQTT_COMM_ENABLED` 或者 `FEATURE_COAP_COMM_ENABLED` 的值是 `y`
+ 它被以下 `FEATURE_XXX` 的依赖
    * FEATURE_OTA_FETCH_CHANNEL
    * FEATURE_OTA_SIGNAL_CHANNEL
+ 对应的源码目录是 `src/services/uOTA`

### <a name="FEATURE_OTA_FETCH_CHANNEL">FEATURE_OTA_FETCH_CHANNEL</a>
+ 固件升级中文件下载的通道配置, 所谓文件下载通道是指设备从阿里云服务器下载固件文件的数据通道
+ 可取的值是 `HTTP` 或者 `COAP`
+ 它在被配置为 `COAP` 的时候, 依赖于 `FEATURE_COAP_COMM_ENABLED` 的值是 `y`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/services/uOTA`

### <a name="FEATURE_OTA_SIGNAL_CHANNEL">FEATURE_OTA_SIGNAL_CHANNEL</a>
+ 固件升级中推送消息的通道配置, 所谓推送消息通道是指设备从阿里云服务器接收到有新固件需要下载的控制通道
+ 可取的值是 `MQTT` 或者 `COAP`
+ 它依赖于 `FEATURE_MQTT_COMM_ENABLED` 或者 `FEATURE_COAP_COMM_ENABLED` 的值是 `y`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/services/uOTA`

### <a name="FEATURE_SDK_ENHANCE">FEATURE_SDK_ENHANCE</a>
+ 高级版物模型能力的功能开关, 所谓高级版物模型能力是指设备可使用基于服务/属性/事件三要素的Alink协议和服务端通信
+ 可取的值是 `y` 或者 `n`
+ 它依赖于 `FEATURE_MQTT_COMM_ENABLED` 的值是 `y`
+ 它被 `FEATURE_ENHANCED_GATEWAY` 所依赖
+ 对应的源码目录是 `src/services/linkkit`

### <a name="FEATURE_SUBDEVICE_ENABLED">FEATURE_SUBDEVICE_ENABLED</a>
+ 旧版子设备管理能力的功能开关, 所谓子设备管理能力是指设备可代理其它不具有直接连云通道的设备, 上报和接收MQTT消息
+ 可取的值是 `y` 或者 `n`
+ 它依赖于 `FEATURE_MQTT_COMM_ENABLED` 的值是 `y`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/services/subdev`

### <a name="FEATURE_SUPPORT_ITLS">FEATURE_SUPPORT_ITLS</a>
### <a name="FEATURE_SUPPORT_TLS">FEATURE_SUPPORT_TLS</a>
+ 在TLS层是否使用iTLS的功能开关, 所谓iTLS是阿里巴巴基于ID2商业产品提供的特有闭源安全连接协议
+ 可取的值是 `y` 或者 `n`, `FEATURE_SUPPORT_ITLS` 和 `FEATURE_SUPPORT_TLS` 的取值**必须不同**
+ 它不依赖其它的 `FEATURE_XXX`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/ref-impl/hal`

### <a name="FEATURE_WIFI_AWSS_ENABLED">FEATURE_WIFI_AWSS_ENABLED</a>
+ WiFi配网的功能开关, 所谓WiFi配网是阿里巴巴自研的一种从手机app发送WiFi网络的SSID和密码给设备端的通信协议
+ 可取的值是 `y` 或者 `n`
+ 它依赖于 `FEATURE_ALCS_ENABLED` 的值是 `y`
+ 它不是其它 `FEATURE_XXX` 的依赖之一
+ 对应的源码目录是 `src/services/awss`

## <a name="4.7 在 Ubuntu/OSX 上使用 cmake 的编译示例">4.7 在 Ubuntu/OSX 上使用 cmake 的编译示例</a>
> 本节的示例适用于开发者的开发环境是 Ubuntu16.04 的Linux主机, 或者是Apple公司的 OSX 的MacOS的情况

**这些例子都是在64位主机上的执行情况, 推荐您和阿里开发者一样, 安装64位的操作系统**

### <a name="用 cmake 为64位Linux/OSX编译">用 cmake 为64位Linux/OSX编译</a>

从 CMakeLists.txt 构建makefile
---
    ~/srcs/iotx-sdk-c$ mkdir oooo
    ~/srcs/iotx-sdk-c$ cd oooo

    ~/srcs/iotx-sdk-c/oooo$ cmake ..
    -- The C compiler identification is GNU 5.4.0
    -- Check for working C compiler: /usr/bin/cc
    -- Check for working C compiler: /usr/bin/cc -- works
    -- Detecting C compiler ABI info
    -- Detecting C compiler ABI info - done
    -- Detecting C compile features
    -- Detecting C compile features - done
    ---------------------------------------------------------------------
    Project Name              : iotkit-embedded-V2.2.1
    Source Dir                : /disk2/yusheng.yx/srcs/iotx-sdk-c
    Binary Dir                : /disk2/yusheng.yx/srcs/iotx-sdk-c/oooo
    System Processor          : x86_64
    System Platform           : Linux-4.4.0-87-generic
    C Compiler                : /usr/bin/cc
    Executable Dir            : /disk2/yusheng.yx/srcs/iotx-sdk-c/oooo/bin
    Library Dir               : /disk2/yusheng.yx/srcs/iotx-sdk-c/oooo/lib
    SDK Version               : V2.2.1

    Building on LINUX ...
    ---------------------------------------------------------------------
    -- Configuring done
    -- Generating done
    -- Build files have been written to: /disk2/yusheng.yx/srcs/iotx-sdk-c/oooo

编译
---
    make -j32

产物
---
    ~/srcs/iotx-sdk-c/oooo$ ls
    bin  CMakeCache.txt  CMakeFiles  cmake_install.cmake  examples  lib  Makefile  src  tests

可执行程序在 `bin/` 目录下, 这些都是64位的可执行程序:

    ls bin/

    linkkit-example-countdown  linkkit-example-sched  linkkit-example-solo  linkkit_tsl_convert
    mqtt-example  mqtt_example_multithread  mqtt_example_rrpc  ota_mqtt-example  uota_app-example

二进制库在 `lib/` 目录下:

    ls lib/

    libiot_hal.a  libiot_sdk.a  libiot_tls.a

### <a name="用 cmake 为32位Linux/OSX编译">用 cmake 为32位Linux/OSX编译</a>

修改 CMakeLists.txt 文件
---
在默认的文件中修改CFLAGS, 加入`-m32`

    SET (CMAKE_C_FLAGS " -Iexamples -Os -Wall")

改成

    SET (CMAKE_C_FLAGS " -Iexamples -Os -Wall -m32")

从 CMakeLists.txt 构建makefile
---
    mkdir ooo
    cd ooo
    cmake ..

编译
---
    make -j32

产物
---
    ~/srcs/iotx-sdk-c/ooo$ ls
    bin  CMakeCache.txt  CMakeFiles  cmake_install.cmake  examples  lib  Makefile  src  tests

可执行程序在 `bin/` 目录下:

    ls bin/

    linkkit-example-countdown  linkkit-example-sched  linkkit-example-solo  linkkit_tsl_convert
    mqtt-example  mqtt_example_multithread  mqtt_example_rrpc  ota_mqtt-example  uota_app-example

可以用如下方式验证, 注意 `file` 命令的输出中, 已经显示程序都是32位的了(`ELF 32-bit LSB executable`)

    file ooo/bin/*

    ooo/bin/linkkit-example-countdown: ELF 32-bit LSB executable, Intel 80386, ... stripped
    ooo/bin/linkkit-example-sched:     ELF 32-bit LSB executable, Intel 80386, ... stripped
    ooo/bin/linkkit-example-solo:      ELF 32-bit LSB executable, Intel 80386, ... stripped
    ooo/bin/linkkit_tsl_convert:       ELF 32-bit LSB executable, Intel 80386, ... stripped
    ooo/bin/mqtt-example:              ELF 32-bit LSB executable, Intel 80386, ... stripped
    ooo/bin/mqtt-example-multithread:  ELF 32-bit LSB executable, Intel 80386, ... stripped
    ooo/bin/mqtt-example-rrpc:         ELF 32-bit LSB executable, Intel 80386, ... stripped
    ooo/bin/ota_mqtt-example:          ELF 32-bit LSB executable, Intel 80386, ... stripped
    ooo/bin/sdk-testsuites:            ELF 32-bit LSB executable, Intel 80386, ... stripped
    ooo/bin/uota_app-example:          ELF 32-bit LSB executable, Intel 80386, ... stripped

二进制库在 `lib/` 目录下:

    ls lib/

    libiot_hal.a  libiot_sdk.a  libiot_tls.a

### <a name="用 cmake 为arm-linux编译">用 cmake 为arm-linux编译</a>
    sudo apt-get install -y gcc-arm-linux-gnueabihf

以如下命令和输出确认交叉编译工具链已安装好

    arm-linux-gnueabihf-gcc --version

    arm-linux-gnueabihf-gcc (Ubuntu/Linaro 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609
    Copyright (C) 2015 Free Software Foundation, Inc.

修改 CMakeLists.txt 文件
---
在默认的 `CMakeList.txt` 文件中加入以下一行

    SET (CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)

比如

      5 CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
      6 PROJECT (iotkit-embedded-V2.2.1 C)
      7
      8 SET (CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
      9 SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
     10 SET (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
     11 SET (CMAKE_C_FLAGS " -Iexamples -Os -Wall")

从 CMakeLists.txt 构建makefile
---
    mkdir ooo
    cd ooo
    cmake ..

可以注意到这一环节中已经按照指定的编译器生成

    -- The C compiler identification is GNU 5.4.0
    -- Check for working C compiler: /usr/bin/cc
    -- Check for working C compiler: /usr/bin/cc -- works
    -- Detecting C compiler ABI info
    -- Detecting C compiler ABI info - done
    -- Detecting C compile features
    -- Detecting C compile features - done
    ---------------------------------------------------------------------
    Project Name              : iotkit-embedded-V2.2.1
    Source Dir                : /disk2/yusheng.yx/srcs/iotx-sdk-c
    Binary Dir                : /disk2/yusheng.yx/srcs/iotx-sdk-c/ooo
    System Processor          : x86_64
    System Platform           : Linux-4.4.0-87-generic
    C Compiler                : arm-linux-gnueabihf-gcc
    Executable Dir            : /disk2/yusheng.yx/srcs/iotx-sdk-c/ooo/bin
    Library Dir               : /disk2/yusheng.yx/srcs/iotx-sdk-c/ooo/lib
    SDK Version               : V2.2.1

    Building on LINUX ...
    ---------------------------------------------------------------------
    -- Configuring done
    -- Generating done
    -- Build files have been written to: /disk2/yusheng.yx/srcs/iotx-sdk-c/ooo

编译
---
    make -j32

产物
---
    ~/srcs/iotx-sdk-c/ooo$ ls
    bin  CMakeCache.txt  CMakeFiles  cmake_install.cmake  examples  lib  Makefile  src  tests

可执行程序在 `bin/` 目录下:

    ls bin/

    linkkit-example-countdown  linkkit-example-sched  linkkit-example-solo  linkkit_tsl_convert
    mqtt-example  mqtt_example_multithread  mqtt_example_rrpc  ota_mqtt-example  uota_app-example

可以用如下方式验证, 注意 `file` 命令的输出中, 已经显示程序都是32位ARM架构的了(`ELF 32-bit LSB executable, ARM, EABI5 version 1`)

    file ooo/bin/*

    ooo/bin/linkkit-example-countdown: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/linkkit-example-sched:     ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/linkkit-example-solo:      ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/linkkit_tsl_convert:       ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/mqtt-example:              ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/mqtt_example_multithread:  ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/mqtt_example_rrpc:         ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/ota_mqtt-example:          ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/uota_app-example:          ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped

二进制库在 `lib/` 目录下, 它们同样是是32位ARM架构的

    ls lib/

    libiot_hal.a  libiot_sdk.a  libiot_tls.a

### <a name="用 cmake 为Windows编译">用 cmake 为Windows编译</a>
安装 mingw-w64-i686 工具链
---
在 `Ubuntu16.04` 上, 运行如下命令安装交叉编译工具链

    sudo apt-get install -y gcc-mingw-w64-i686

以如下命令和输出确认交叉编译工具链已安装好

    i686-w64-mingw32-gcc --version

    i686-w64-mingw32-gcc (GCC) 5.3.1 20160211
    Copyright (C) 2015 Free Software Foundation, Inc.

修改 CMakeLists.txt 文件
---
在默认的 `CMakeList.txt` 文件中加入以下一行

    SET (CMAKE_C_COMPILER i686-w64-mingw32-gcc)

比如

      5 CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
      6 PROJECT (iotkit-embedded-V2.2.1 C)
      7
      8 SET (CMAKE_C_COMPILER i686-w64-mingw32-gcc)
      9 SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
     10 SET (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
     11 SET (CMAKE_C_FLAGS " -Iexamples -Os -Wall")

从 CMakeLists.txt 构建makefile
---
    mkdir ooo
    cd ooo
    cmake ..

可以注意到这一环节中已经按照指定的编译器生成

    cmake ..
    -- The C compiler identification is GNU 5.4.0
    -- Check for working C compiler: /usr/bin/cc
    -- Check for working C compiler: /usr/bin/cc -- works
    -- Detecting C compiler ABI info
    -- Detecting C compiler ABI info - done
    -- Detecting C compile features
    -- Detecting C compile features - done
    ---------------------------------------------------------------------
    Project Name              : iotkit-embedded-V2.2.1
    Source Dir                : /disk2/yusheng.yx/srcs/iotx-sdk-c
    Binary Dir                : /disk2/yusheng.yx/srcs/iotx-sdk-c/ooo
    System Processor          : x86_64
    System Platform           : Linux-4.4.0-87-generic
    C Compiler                : i686-w64-mingw32-gcc
    Executable Dir            : /disk2/yusheng.yx/srcs/iotx-sdk-c/ooo/bin
    Library Dir               : /disk2/yusheng.yx/srcs/iotx-sdk-c/ooo/lib
    SDK Version               : V2.2.1

    Building on LINUX ...
    ---------------------------------------------------------------------
    -- Configuring done
    -- Generating done
    -- Build files have been written to: /disk2/yusheng.yx/srcs/iotx-sdk-c/ooo

编译
---
    make -j32
