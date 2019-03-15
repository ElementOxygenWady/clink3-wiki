# <a name="目录">目录</a>
+ [庆科MK3080 + AliOS Things 移植示例](#庆科MK3080 + AliOS Things 移植示例)

# <a name="庆科MK3080 + AliOS Things 移植示例">庆科MK3080 + AliOS Things 移植示例</a>

选择平台配置
---
    make reconfig
    
    SELECT A CONFIGURATION:

    1) config.alios.esp8266
    2) config.alios.mk3080
    3) config.ubuntu.x86
    #? 2

    SELECTED CONFIGURATION:

    VENDOR :   alios
    MODEL  :   mk3080

编译
---
    make

如果您当前的开发主机上没有安装庆科 `MK3080` 的交叉工具链并导出到 `PATH` 中, C-SDK会自动下载它们

    make

    BUILDING WITH EXISTING CONFIGURATION:

    VENDOR :   alios
    MODEL  :   mk3080

    https://gitee.com/alios-things/gcc-arm-none-eabi-linux -> .O/compiler/gcc-arm-none-eabi-linux/main
    ---
    downloading toolchain for arm-none-eabi-gcc .................... [\]

下载完成后, 自动开始交叉编译SDK的源码

    https://gitee.com/alios-things/gcc-arm-none-eabi-linux -> .O/compiler/gcc-arm-none-eabi-linux/main
    ---
    downloading toolchain for arm-none-eabi-gcc .................... done

    [CC] infra_mem_stats.o                  <=  ...
    [CC] infra_log.o                        <=  ...
    [CC] infra_timer.o                      <=  ...    ...
    ...
    [AR] libiot_sdk.a                       <=  ...

获取二进制库
---

    cd output/release/lib
    ls

其中有一个主要产物, **它是 MK3080 架构的**:

| 产物文件名      | 说明
|-----------------|-------------------------------------------------------------
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口