# <a name="目录">目录</a>
+ [移植概述](#移植概述)
    * [编译方式](#编译方式)
        - [对于Linux/Windows这样的高级系统而言, 可以使用make/cmake进行SDK的编译](#对于Linux/Windows这样的高级系统而言, 可以使用make/cmake进行SDK的编译)
        - [对于某些嵌入式系统而言, 无法使用make/cmake进行编译, 可以将SDK的源程序加入到芯片商提供的IDE中进行编译, 比如KEIL/IAR](#对于某些嵌入式系统而言, 无法使用make/cmake进行编译, 可以将SDK的源程序加入到芯片商提供的IDE中进行编译, 比如KEIL/IAR)
    * [SDK裁剪](#SDK裁剪)
    * [SDK的使用](#SDK的使用)

# <a name="移植概述">移植概述</a>

## <a name="编译方式">编译方式</a>
用户需要编译才能调用C语言Link Kit SDK提供的函数, 目前SDK的编译有下面的两种编译方式：

### <a name="对于Linux/Windows这样的高级系统而言, 可以使用make/cmake进行SDK的编译">对于Linux/Windows这样的高级系统而言, 可以使用make/cmake进行SDK的编译</a>

这种编译方式将会生成libiot_hal.a和libiot_sdk.a，用户在编译应用程序时需要链接这两个库文件。如果使用MQTT+TLS连接物联网平台，或者使能了设备本地控制或者WiFi配网，还需要链接libiot_tls.a.

目前设备端C-SDK的构建配置系统支持以下的编译方式:

---
+ 在`Linux`上以`GNU Make` + `各种工具链`编译, 产生`各种嵌入式目标架构`的SDK
    * 以GNU Make + gcc, 产生适用于 64位Linux 的SDK以及可执行例程
    * 以GNU Make + gcc, 产生适用于 32位Linux 的SDK以及可执行例程
    * 以GNU Make + i686-w64-mingw32-gcc, 产生适用于 Windows 平台的SDK以及可执行例程
    * 以GNU Make + arm-none-eabi-gcc, 产生适用于 MK3060/MK3080 嵌入式平台的SDK
    * 以GNU Make + xtensa-lx106-elf-gcc, 产生适用于 ESP8266 嵌入式平台的SDK
    * 以GNU Make + arm-linux-gnueabihf-gcc, 产生适用于 arm-linux 嵌入式平台的SDK

+ 在`Linux`上以`cmake` + `各种工具链`编译, 产生`各种目标架构`的SDK
    * 以cmake + gcc, 产生适用于 64位Linux 的SDK
    * 以cmake + gcc, 产生适用于 32位Linux 的SDK
    * 以cmake + arm-linux-gnueabihf-gcc, 产生适用于 arm-linux 嵌入式平台的SDK
    * 以cmake + i686-w64-mingw32-gcc, 产生适用于 Windows 平台的SDK

+ 在Windows上以VS Code 2015编译, 产生适用于 Windows 平台的SDK

### <a name="对于某些嵌入式系统而言, 无法使用make/cmake进行编译, 可以将SDK的源程序加入到芯片商提供的IDE中进行编译, 比如KEIL/IAR">对于某些嵌入式系统而言, 无法使用make/cmake进行编译, 可以将SDK的源程序加入到芯片商提供的IDE中进行编译, 比如KEIL/IAR</a>

## <a name="SDK裁剪">SDK裁剪</a>

+ Link Kit SDK对于支持使用make进行编译的系统, 提供了比较方便的裁剪工具, 用户可以通过make menuconfig的方式使能或者失效某些功能, 就可以只编译出只包含指定功能的SDK的二进制代码
+ 对于不支持make的系统, 用户可以在IDE中加入需要功能的源文件进行编译

## <a name="SDK的使用">SDK的使用</a>

用户的程序在调用SDK时，需要加入对下面两个头文件的包含：

<pre>
#include "iot_import.h"
#include "iot_export.h"
</pre>

