# <a name="目录">目录</a>
+ [目标系统为32位Linux](#目标系统为32位Linux)
    * [使用 `make`+`gcc` 编译SDK](#使用 `make`+`gcc` 编译SDK)

# <a name="目标系统为32位Linux">目标系统为32位Linux</a>

## <a name="使用 `make`+`gcc` 编译SDK">使用 `make`+`gcc` 编译SDK</a>

> 如果您编译SDK是在一台安装了32位 Linux 的机器上, 那么直接重复上面 [用 make 为64位Linux编译](#用 make 为64位Linux编译) 的步骤, 即可得到32位的库和例程
>
> 如果您是在安装了64位 `Ubuntu16.04` 的机器上, 需要编译出32位的库, 请按照下文操作

安装32位工具链
---

    sudo apt-get install -y libc6:i386 libstdC++6:i386 gcc:i386

修改平台配置文件
---

    vim tools/board/config.ubuntu.x86

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

    1) config.alios.esp8266
    2) config.alios.mk3080
    3) config.ubuntu.x86
    #? 3

    SELECTED CONFIGURATION:

    VENDOR :   ubuntu
    MODEL  :   x86


编译
---

    make

获取二进制库
---

    cd output/release/lib
    ls

其中有三个主要产物, **它们都是32位架构的**:

| 产物文件名      | 说明
|-----------------|-------------------------------------------------------------------------
| `libiot_hal.a`  | HAL接口层的参考实现, 提供了 `HAL_XXX()` 接口
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口
| `libiot_tls.a`  | 裁剪过的 `mbedtls`, 提供了 `mbedtls_xxx()` 接口, 支撑 `libiot_hal.a`

获取可执行程序
---

    cd output/release/bin
    ls

其中有两个主要产物, **它们都是32位架构的**:

| 产物文件名              | 说明
|-------------------------|-------------------------------------------------------------
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
    output/release/bin/ota-example-mqtt:          ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/sdk-testsuites:            ELF 32-bit LSB executable, Intel 80386, ... stripped
    output/release/bin/uota_app-example:          ELF 32-bit LSB executable, Intel 80386, ... stripped
