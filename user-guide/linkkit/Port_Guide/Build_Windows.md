# <a name="目录">目录</a>
+ [目标平台为Windows](#目标平台为Windows)
    * [使用 `make`+`i686-w64-mingw32-gcc` 方式编译](#使用 `make`+`i686-w64-mingw32-gcc` 方式编译)
    * [使用 `cmake` 方式编译](#使用 `cmake` 方式编译)
    * [使用 `Visual Studio 2015` 的编译示例](#使用 `Visual Studio 2015` 的编译示例)

# <a name="目标平台为Windows">目标平台为Windows</a>

## <a name="使用 `make`+`i686-w64-mingw32-gcc` 方式编译">使用 `make`+`i686-w64-mingw32-gcc` 方式编译</a>

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

| 产物文件名                  | 说明
|-----------------------------|-----------------------------------------------------------------------------------------
| `mqtt-example.exe`          | 基础版的例程, 可演示 `IOT_XXX()` 接口的使用
| `linkkit_tsl_convert.exe`   | 高级版的物模型转换工具, 可以将设备的TSL文件转换为C语言的字符串，方便程序使用

目前对Windows平台仅提供基础版的例程, 可以把它拿到Windows主机上运行, 如下图则是在一台`Win10`主机上运行的效果

![image](https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/win_mqtt_example.png)

## <a name="使用 `cmake` 方式编译">使用 `cmake` 方式编译</a>

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

## <a name="使用 `Visual Studio 2015` 的编译示例">使用 `Visual Studio 2015` 的编译示例</a>

>本节的示例适用于开发者的开发环境是Windows7 SP1

*Windows7不能安装Visual Studio 2015, 请通过软件更新升级到Windows7 SP1*

**这些例子都是在64位主机上的执行情况, 推荐您和阿里开发者一样, 安装64位的操作系统**

#### <a name="环境配置">环境配置</a>
安装Visual Studio 2015
---

选自定义安装

![image](<https://linkkit-export.oss-cn-shanghai.aliyuncs.com/visual_studio_2015_install_setup_1.png>)

点击下一步

选C++组件
---
选C++有关的组件, 其他默认的组件建议不勾选

![image](<https://linkkit-export.oss-cn-shanghai.aliyuncs.com/visual_studio_c%2B%2B_install_setup_2.png>)

安装cmake, 并把cmake添加到系统环境的路径里面
---
从 [cmake官网](https://cmake.org/download/) 下载Windows的cmake的安装包

安装后将 `C:\Program Files\CMake\bin` 添加到

    控制面板-->系统和安全-->高级-->环境变量-->系统变量-->Path

确保cmd可以调用到cmake

安装gitbash
---
从 [gitbash官网](https://git-scm.com/download/win) 下载安装

#### <a name="配置Windows环境下的cmake文件">配置Windows环境下的cmake文件</a>

下载linkkit C SDK的压缩包, 解压

修改cmake文件
---
从链接中下载patch文件: [cmake patch](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/visual_studio_cmake.patch)

通过gitbash执行下列命令:

    cd linkkit_sdk-c
    patch -p1 < visual_studio_cmake.patch

visual_studio_cmake.patch的具体内容如下:

    diff --git a/sdk-c/CMakeLists.txt b/sdk-c/CMakeLists.txt
    index f676423..612b9e8 100644
    --- a/sdk-c/CMakeLists.txt
    +++ b/sdk-c/CMakeLists.txt
    @@ -34,31 +34,18 @@ MESSAGE ("---------------------------------------------------------------------"

    ADD_DEFINITIONS (-DDLL_IOT_EXPORTS)

    -ADD_DEFINITIONS (-DALCS_ENABLED)
    -ADD_DEFINITIONS (-DALCS_SERVER_ENABLED)
    -ADD_DEFINITIONS (-DAWSS_DISABLE_ENROLLEE)
    -ADD_DEFINITIONS (-DAWSS_DISABLE_REGISTRAR)
    -ADD_DEFINITIONS (-DAWSS_SUPPORT_ADHA)
    -ADD_DEFINITIONS (-DAWSS_SUPPORT_AHA)
    -ADD_DEFINITIONS (-DAWSS_SUPPORT_APLIST)
    -ADD_DEFINITIONS (-DAWSS_SUPPORT_PHONEASAP)
    -ADD_DEFINITIONS (-DAWSS_SUPPORT_ROUTER)
    -ADD_DEFINITIONS (-DAWSS_SUPPORT_SMARTCONFIG)
    -ADD_DEFINITIONS (-DAWSS_SUPPORT_SMARTCONFIG_WPS)
    -ADD_DEFINITIONS (-DAWSS_SUPPORT_ZEROCONFIG)
    ADD_DEFINITIONS (-DCOAP_SERV_MULTITHREAD)
    ADD_DEFINITIONS (-DCONFIG_GUIDER_AUTH_TIMEOUT=500)
    ADD_DEFINITIONS (-DCONFIG_HTTP_AUTH_TIMEOUT=500)
    ADD_DEFINITIONS (-DCONFIG_MID_HTTP_TIMEOUT=500)
    ADD_DEFINITIONS (-DDEVICE_MODEL_ENABLED)
    -ADD_DEFINITIONS (-DDEV_BIND_ENABLED)
    -ADD_DEFINITIONS (-DFORCE_SSL_VERIFY)
    +ADD_DEFINITIONS (-DFS_ENABLED)
    +ADD_DEFINITIONS (-DHTTP2_COMM_ENABLED)
    ADD_DEFINITIONS (-DMQTT_COMM_ENABLED)
    ADD_DEFINITIONS (-DMQTT_DIRECT)
    ADD_DEFINITIONS (-DOTA_ENABLED)
    ADD_DEFINITIONS (-DOTA_SIGNAL_CHANNEL=1)
    ADD_DEFINITIONS (-DSUPPORT_TLS)
    -ADD_DEFINITIONS (-DWIFI_PROVISION_ENABLED)
    ADD_DEFINITIONS (-D_PLATFORM_IS_HOST_)

    ADD_SUBDIRECTORY (src/infra/utils)
    @@ -71,10 +58,8 @@ ADD_SUBDIRECTORY (src/services/linkkit/ntp)
    ADD_SUBDIRECTORY (src/services/linkkit/cm)
    ADD_SUBDIRECTORY (src/services/linkkit/dm)
    ADD_SUBDIRECTORY (src/services/linkkit/dev_reset)
    -ADD_SUBDIRECTORY (src/protocol/coap/local)
    -ADD_SUBDIRECTORY (src/services/awss)
    -ADD_SUBDIRECTORY (src/services/dev_bind)
    -ADD_SUBDIRECTORY (src/protocol/alcs)
    +ADD_SUBDIRECTORY (src/services/http2_stream)
    +ADD_SUBDIRECTORY (src/protocol/http2)

    ADD_SUBDIRECTORY (src/ref-impl/hal)
    ADD_SUBDIRECTORY (examples)
    @@ -92,10 +77,8 @@ ADD_LIBRARY (iot_sdk SHARED
        $<TARGET_OBJECTS:iot_cm>
        $<TARGET_OBJECTS:iot_dm>
        $<TARGET_OBJECTS:iot_reset>
    -    $<TARGET_OBJECTS:iot_coap_local>
    -    $<TARGET_OBJECTS:iot_awss>
    -    $<TARGET_OBJECTS:iot_bind>
    -    $<TARGET_OBJECTS:iot_alcs>
    +    $<TARGET_OBJECTS:http2_stream>
    +    $<TARGET_OBJECTS:iot_http2>
    )
    if(WIN32)
        TARGET_LINK_LIBRARIES (iot_sdk ws2_32)

    diff --git a/sdk-c/examples/CMakeLists.txt b/sdk-c/examples/CMakeLists.txt
    index d61637d..5f5e0ad 100644
    --- a/sdk-c/examples/CMakeLists.txt
    +++ b/sdk-c/examples/CMakeLists.txt
    @@ -57,6 +57,14 @@ ADD_EXECUTABLE (mqtt-example-multithread
        app_entry.c
        mqtt/mqtt_example_multithread.c
    )
    +ADD_EXECUTABLE (http2-example
    +    http2/http2_example_stream.c
    +    app_entry.c
    +)
    +ADD_EXECUTABLE (http2-example-uploadfile
    +    http2/http2_example_uploadfile.c
    +    app_entry.c
    +)
    ADD_EXECUTABLE (ota-example-mqtt
        ota/ota-example-mqtt.c
    )
    @@ -106,6 +114,26 @@ IF (NOT MSVC)
    TARGET_LINK_LIBRARIES (mqtt-example-multithread rt)
    ENDIF (NOT MSVC)

    +TARGET_LINK_LIBRARIES (http2-example iot_sdk)
    +TARGET_LINK_LIBRARIES (http2-example iot_hal)
    +TARGET_LINK_LIBRARIES (http2-example iot_tls)
    +IF (NOT MSVC)
    +TARGET_LINK_LIBRARIES (http2-example pthread)
    +ENDIF (NOT MSVC)
    +IF (NOT MSVC)
    +TARGET_LINK_LIBRARIES (http2-example rt)
    +ENDIF (NOT MSVC)
    +
    +TARGET_LINK_LIBRARIES (http2-example-uploadfile iot_sdk)
    +TARGET_LINK_LIBRARIES (http2-example-uploadfile iot_hal)
    +TARGET_LINK_LIBRARIES (http2-example-uploadfile iot_tls)
    +IF (NOT MSVC)
    +TARGET_LINK_LIBRARIES (http2-example-uploadfile pthread)
    +ENDIF (NOT MSVC)
    +IF (NOT MSVC)
    +TARGET_LINK_LIBRARIES (http2-example-uploadfile rt)
    +ENDIF (NOT MSVC)
    +
    TARGET_LINK_LIBRARIES (ota-example-mqtt iot_sdk)
    TARGET_LINK_LIBRARIES (ota-example-mqtt iot_hal)
    TARGET_LINK_LIBRARIES (ota-example-mqtt iot_tls)

#### <a name="创建 Visual Studio 工程">创建 Visual Studio 工程</a>

    mkdir ooo
    cd ooo
    cmake ..

在执行cmake命令过程中可能报错, 请参考[cmake编译错误处理](#cmake编译错误处理)

#### <a name="使用 Visual Studio 工程">使用 Visual Studio 工程</a>
打开 Visual Studio 工程
---
鼠标右键打开红框标注的文件

![image](<https://linkkit-export.oss-cn-shanghai.aliyuncs.com/windows_cmake_ooo_folder.jpg>)

mqtt_example工程的使用
---
选中mqtt_example这个用例

![image](<https://linkkit-export.oss-cn-shanghai.aliyuncs.com/select_mqtt_class_view.jpg>)

右键, 选择"重新生成", 执行直至编译成功

右键, 选择"调试"-->"启动新实例", 可以看到对话框弹出

![image](<https://linkkit-export.oss-cn-shanghai.aliyuncs.com/mqtt-example-run.jpg>)

用例运行成功

![image](<https://linkkit-export.oss-cn-shanghai.aliyuncs.com/mqtt_example_sucess.jpg>)

http2_example工程的使用
---
选中http2_example, 其余操作跟mqtt_example类似

#### <a name="cmake编译错误处理">cmake编译错误处理</a>
如果碰到下列错误, 说明Visual Studio 2015 C++并没有安装完成, 建议在这个IDE中创建并且编译一个C++工程

按照提示安装所有缺失的工具, 直到编译和运行都正确为止

![image](<https://linkkit-export.oss-cn-shanghai.aliyuncs.com/compile_err.png>)
