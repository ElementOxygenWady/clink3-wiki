# <a name="目录">目录</a>
+ [移植WiFi配网中的设备热点配网到ubuntu16.04](#移植WiFi配网中的设备热点配网到ubuntu16.04)
    * [抽取SDK中设备热点部分代码](#抽取SDK中设备热点部分代码)
    * [根据硬件平台对接HAL接口](#根据硬件平台对接HAL接口)
    * [开发和编译demo程序](#开发和编译demo程序)
    * [运行demo程序完成设备热点配网](#运行demo程序完成设备热点配网)

# <a name="移植WiFi配网中的设备热点配网到ubuntu16.04">移植WiFi配网中的设备热点配网到ubuntu16.04</a>

本文以 `SDK3.0.1` 和 `ubuntu16.04` 为例, 演示移植设备热点配网的全过程, 使用的网卡为Linksys思科wusb600n双频无线网卡

## <a name="抽取SDK中设备热点部分代码">抽取SDK中设备热点部分代码</a>

下载SDK
---
点击SDK3.0.1的长期维护分支[下载链接](https://code.aliyun.com/linkkit/c-sdk/repository/archive.zip?ref=v3.0.1), 获取代码保存到本地

配置SDK
---
本文编写时笔者是在Linux主机开发环境(`Ubuntu16.04-64位`)下工作, 因此进入SDK解压后的目录后, 用以下命令配置SDK

    make menuconfig

如动图所示, 笔者

+ 先使能配网功能(`FEATURE_WIFI_PROVISION_ENABLED`), 选中其中的设备热点配网模式(`FEATURE_AWSS_SUPPORT_DEV_AP`)
+ 然后进入日志系统子菜单(`Log Configurations`), 反选所有的日志禁用选项(`Mute LEVEL of ...`), 使能所有的 log 输出

<br>

<div align="center">
<img src="https://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/devap/devap_menuconfig.gif" width="750">
</div>

抽取所需代码
---
配置完成并保存后, 用以下命令运行代码抽取脚本, 将当前配置对应需要的代码抽取到 `output` 目录下

    ./extract.sh

在 `output` 目录下看到这些内容

    cd output

    ls
    eng  examples  Makefile

## <a name="根据硬件平台对接HAL接口">根据硬件平台对接HAL接口</a>

SDK对外界的依赖都以 `HAL_XXX` 形式的接口表达, 需要对接的已经都列在 `output/eng/wrapper.c` 中, 根据开发板情况对其填充为如下文件

+ [ieee80211_radiotap.h](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Vendors/Ubuntu/ieee80211_radiotap.h)
+ [wrapper.c](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Vendors/Ubuntu/wrapper.c)

直接用以上链接中的 `wrapper.c` 覆盖示意性的空函数列举文件 `output/eng/wrapper.c`

*实现 `HAL_Awss_Open_Monitor()` 等接口时, 使用了 `ieee80211_radiotap.h` 中定义的结构体, 因此要将 `ieee80211_radiotap.h` 也放置在 `output/eng` 目录下供 `wrapper.c` 引用*

## <a name="开发和编译demo程序">开发和编译demo程序</a>

修改例程加入设备热点配网功能
---
为演示设备热点配网功能, 直接使用现成的例子程序源文件 `output/examples/linkkit_example_solo.c` 作为主程序, 使用下面增加了设备热点配网调用的源文件把它替换掉

+ [linkkit_example_solo.c](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Vendors/Ubuntu/linkkit_example_solo.c)

编译demo程序
---
在 `output` 目录下, 借助已更新过工具链信息的makefile, 交叉编译可执行的demo程序

    cd output
    make clean
    make prog

得到的 `build/linkkit-example-solo` 则是已经嵌入了设备热点配网功能的demo程序, 可直接在`ubuntu16.04`上运行

## <a name="运行demo程序完成设备热点配网">运行demo程序完成设备热点配网</a>

直接运行`./build/linkkit-example-solo`, 如下:

    ===========================Network has been reset, start wifi-provision==============================
    ===========================Please press any key to continue==============================

此时输入任意字符回车, 开始进行设备热点配网

在手机上的云智能app开始配网
---

<div align="center">
<img src="https://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/devap/devap_app.gif" width="300">
</div>

配网中需要让app扫描的二维码可用[工具网页](https://cli.im), 输入以下字符串生成

    http://www.taobao.com?pk=a1X2bEnP82z

其中 `a1I98ylaERB` 是demo程序使用的设备产品标识, 是从[阿里云IoT控制台](https://iot.console.aliyun.com)申请得到的 `a1I98ylaERB`

设备开启的热点是以`adh_`为前缀, 后接`Product key`

观察成功解密SSID和密码
---
通过观察demo程序输出日志, 以下日志表明demo程序已成功从手机app处得到和解密当前无线路由器的SSID和密码

    ssid  : linkkit-test
    passwd: lk123456
