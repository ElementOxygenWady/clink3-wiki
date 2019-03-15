# <a name="目录">目录</a>
+ [目标系统为arm-linux](#目标系统为arm-linux)
    * [使用 `make` 方式编译](#使用 `make` 方式编译)

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
    vim tools/board/config.arm-linux.demo

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

或者写成

    CONFIG_ENV_CFLAGS = \
        -D_PLATFORM_IS_LINUX_ \
        -Wall \
        -DNO_EXECUTABLES \

    CONFIG_ENV_LDFLAGS = \
        -lpthread -lrt \

    CROSS_PREFIX := arm-linux-gnueabihf-

    CONFIG_wrappers :=

选择平台配置
---
    make reconfig

    SELECT A CONFIGURATION:

    1) config.alios.mk3080
    2) config.arm-linux.demo
    3) config.ubuntu.x86
    #? 2

    SELECTED CONFIGURATION:

    VENDOR :   arm-linux
    MODEL  :   demo

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
