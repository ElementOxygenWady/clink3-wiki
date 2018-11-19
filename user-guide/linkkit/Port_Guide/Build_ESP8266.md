# <a name="目录">目录</a>
+ [乐鑫ESP8266](#乐鑫ESP8266)

# <a name="乐鑫ESP8266">乐鑫ESP8266</a>

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

| 产物文件名      | 说明
|-----------------|-------------------------------------------------------------
| `libiot_sdk.a`  | SDK的主库, 提供了 `IOT_XXX` 接口和 `linkkit_xxx()` 接口
