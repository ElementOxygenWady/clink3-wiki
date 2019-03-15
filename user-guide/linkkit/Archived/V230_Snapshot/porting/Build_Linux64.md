# <a name="目录">目录</a>
+ [目标系统为64位Linux](#目标系统为64位Linux)
    * [使用 `make`+`gcc` 编译SDK](#使用 `make`+`gcc` 编译SDK)
    * [使用 `cmake` 编译SDK](#使用 `cmake` 编译SDK)

# <a name="目标系统为64位Linux">目标系统为64位Linux</a>

## <a name="使用 `make`+`gcc` 编译SDK">使用 `make`+`gcc` 编译SDK</a>

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

编译
---

    make

获取二进制库
---

    cd output/release/lib
    ls

其中有三个主要产物, **它们都是64位架构的**:

| 产物文件名      | 说明
|-----------------|-------------------------------------------------------------------------
| `libiot_hal.a`  | HAL接口层的参考实现, 提供了 `HAL_XXX()` 接口
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口
| `libiot_tls.a`  | 裁剪过的 `mbedtls`, 提供了 `mbedtls_xxx()` 接口, 支撑 `libiot_hal.a`

获取可执行程序
---

    cd output/release/bin
    ls

其中有两个主要产物, **它们都是64位架构的**:

| 产物文件名              | 说明
|-------------------------|-------------------------------------------------------------
| `linkkit-example-solo`  | 高级版(旧版API)的例程, 可演示 `linkkit_xxx()` 接口的使用
| `mqtt-example`          | 基础版的例程, 可演示 `IOT_XXX()` 接口的使用

## <a name="使用 `cmake` 编译SDK">使用 `cmake` 编译SDK</a>

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
    mqtt-example  mqtt_example_multithread  mqtt_example_rrpc  ota-example-mqtt  uota_app-example

二进制库在 `lib/` 目录下:

    ls lib/

    libiot_hal.so  libiot_sdk.so  libiot_tls.so
