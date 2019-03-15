# <a name="目录">目录</a>
+ [目标系统为64位Linux](#目标系统为64位Linux)
    * [使用 `make`+`gcc` 编译SDK](#使用 `make`+`gcc` 编译SDK)

# <a name="目标系统为64位Linux">目标系统为64位Linux</a>

## <a name="使用 `make`+`gcc` 编译SDK">使用 `make`+`gcc` 编译SDK</a>

> C-SDK对其HAL和TLS都已有官方提供的参考实现, 因此可以完整编译出所有的库和例子程序

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

