# <a name="目录">目录</a>
+ [目标系统为arm-linux](#目标系统为arm-linux)
    * [使用 `make` 方式编译](#使用 `make` 方式编译)
    * [使用 `cmake` 方式编译](#使用 `cmake` 方式编译)

# <a name="目标系统为arm-linux">目标系统为arm-linux</a>

## <a name="使用 `make` 方式编译">使用 `make` 方式编译</a>

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

| 产物文件名      | 说明
|-----------------|-------------------------------------------------------------------------
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口
| `libiot_tls.a`  | 裁剪过的 `mbedtls`, 提供了 `mbedtls_xxx()` 接口, 支撑 `libiot_hal.a`

## <a name="使用 `cmake` 方式编译">使用 `cmake` 方式编译</a>

安装交叉编译工具链
---

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
    mqtt-example  mqtt_example_multithread  mqtt_example_rrpc  ota-example-mqtt  uota_app-example

可以用如下方式验证, 注意 `file` 命令的输出中, 已经显示程序都是32位ARM架构的了(`ELF 32-bit LSB executable, ARM, EABI5 version 1`)

    file ooo/bin/*

    ooo/bin/linkkit-example-countdown: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/linkkit-example-sched:     ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/linkkit-example-solo:      ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/linkkit_tsl_convert:       ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/mqtt-example:              ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/mqtt_example_multithread:  ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/mqtt_example_rrpc:         ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/ota-example-mqtt:          ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped
    ooo/bin/uota_app-example:          ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV) ... not stripped

二进制库在 `lib/` 目录下, 它们同样是是32位ARM架构的

    ls lib/

    libiot_hal.so  libiot_sdk.so  libiot_tls.so
