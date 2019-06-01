# <a name="目录">目录</a>
+ [移植WiFi配网中的一键配网到嵌入式Linux](#移植WiFi配网中的一键配网到嵌入式Linux)
    * [抽取SDK中一键配网部分代码](#抽取SDK中一键配网部分代码)
    * [根据硬件平台对接HAL接口](#根据硬件平台对接HAL接口)
    * [开发和编译demo程序](#开发和编译demo程序)
    * [运行demo程序完成一键配网](#运行demo程序完成一键配网)
    * [Wireshark 抓包方法](#Wireshark 抓包方法)

# <a name="移植WiFi配网中的一键配网到嵌入式Linux">移植WiFi配网中的一键配网到嵌入式Linux</a>

本文以 `SDK3.0.1` 和阿里巴巴深度合作伙伴[捷高智能](http://www.cnjabsco.com)提供的嵌入式Linux视频开发板为素材, 演示移植一键配网的全部过程

## <a name="抽取SDK中一键配网部分代码">抽取SDK中一键配网部分代码</a>

下载SDK
---
点击SDK3.0.1的长期维护分支[下载链接](https://code.aliyun.com/linkkit/c-sdk/repository/archive.zip?ref=v3.0.1), 获取代码保存到本地

配置SDK
---
本文编写时笔者是在Linux主机开发环境(`Ubuntu16.04-64位`)下工作, 因此进入SDK解压后的目录后, 用以下命令配置SDK

    make menuconfig

如动图所示, 笔者

+ 先使能配网功能(`FEATURE_WIFI_PROVISION_ENABLED`), 选中其中的一键配网模式(`FEATURE_AWSS_SUPPORT_SMARTCONFIG`)
+ 然后进入日志系统子菜单(`Log Configurations`), 反选所有的日志禁用选项(`Mute LEVEL of ...`), 使能所有的 log 输出

<br>

<div align="center">
<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/1556076897987.gif" width="750">
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

+ [ieee80211_radiotap.h](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Vendors/Jabsco/ieee80211_radiotap.h)
+ [wrapper.c](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Vendors/Jabsco/wrapper.c)

直接用以上链接中的 `wrapper.c` 覆盖示意性的空函数列举文件 `output/eng/wrapper.c`

*实现 `HAL_Awss_Open_Monitor()` 等接口时, 使用了 `ieee80211_radiotap.h` 中定义的结构体, 因此要将 `ieee80211_radiotap.h` 也放置在 `output/eng` 目录下供 `wrapper.c` 引用*

---
其中, 必须要实现的HAL的说明如下:

+ `HAL_Awss_Open_Monitor`

        这个HAL实现的思路是用 iwconfig wlan0 mode monitor 将WiFi进入到Monitor模式.
        创建RAW_SOCKET(TCP/UDP类型的套接字只能够访问传输层以及传输层以上的数据, 因为当IP层把数据传递给传输层时, 下层的数据包头已经被丢掉了, 而RAW_SOCKET却可以访问传输层以下的数据)

        通过 setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr)) 将它跟网卡绑定. 创建一个收包线程, 不断调用 recvfrom() 去收前述RAW_SOCKET中收包
+ `HAL_Awss_Switch_Channel`

        调用 iwconfig wlan0 channel 去实现信道的切换

+ `HAL_Wifi_Get_IP`

        获取本机的IP地址, 已有参考实现

+ `HAL_Wifi_Get_Ap_Info`

        读取设备所连接的AP的ssid/passwd/bssid

+ `HAL_Awss_Connect_Ap`

        根据ssid/passwd, 连接AP

其他HAL, 比如 `HAL_MutexCreate` 跟 `HAL_Wifi_Get_Mac`, 是linkkit跑起来所需要的基础的hal, 不是配网所必需的

HAL的自验证, 请见[一键配网自验证](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Awss_Smartconfig)

## <a name="开发和编译demo程序">开发和编译demo程序</a>

修改`output/Makefile`
---
笔者在编写本文时, 直接使用示例makefile编译SDK和demo程序, 为进行交叉编译, makefile中的工具链信息需做如下更新

    CC := arm-buildroot-linux-uclibcgnueabihf-gcc
    AR := arm-buildroot-linux-uclibcgnueabihf-ar

在此之前, 交叉编译工具链 `arm-buildroot-linux-uclibcgnueabihf` 已被安装到开发主机上, 并被以下命令加入了搜索路径

    export PATH=${PATH}:/home/bin/arm-buildroot-linux-uclibcgnueabihf

修改例程加入一键配网功能
---
为演示一键配网功能, 直接使用现成的例子程序源文件 `output/examples/linkkit_example_solo.c` 作为主程序, 修改如下

    int main(int argc, char **argv)
    {
        int res = 0;
        int cnt = 0;
        iotx_linkkit_dev_meta_info_t master_meta_info;
        int domain_type = 0, dynamic_register = 0, post_reply_need = 0;
    #ifdef ATM_ENABLED
        if (IOT_ATM_Init() < 0) {
            EXAMPLE_TRACE("IOT ATM init failed!\n");
            return -1;
        }
    #endif

        awss_config_press();    /* 打开配网开关 */
        awss_start();           /* 调用配网入口 */

        EXAMPLE_TRACE("after awss_start!\n");
        memset(&g_user_example_ctx, 0, sizeof(user_example_ctx_t));

也就是说, 通过加入 `awss_config_press()` 和 `awss_start()` 的API调用, 启用SDK中的一键配网服务, 其它一切不变

编译demo程序
---
在 `output` 目录下, 借助已更新过工具链信息的makefile, 交叉编译可执行的demo程序

    cd output
    make clean
    make prog

得到的 `build/linkkit-example-solo` 则是已经嵌入了一键配网功能的demo程序, 并已交叉编译成可在开发板上运行的格式

## <a name="运行demo程序完成一键配网">运行demo程序完成一键配网</a>

将demo程序传送到开发板
---
可用TFTP协议将demo程序传送到开发板, 为此, 首先要在开发主机上搭建一个tftp-hpa服务器, 可参考[ubuntu下tftp服务器搭建](https://www.jianshu.com/p/d90696a069df)

然后以开发板为TFTP Client, 从开发主机上获取demo程序, 由于捷高开发板上有一张以太网卡和一张WiFi网卡, 可通过以太网卡登录到开发板上, 运行以下命令

> 假设tftp服务器(Ubuntu开发主机)的ip地址为`192.168.1.104`

    cd /tmp
    tftp -g -r linkkit-example-solo -l linkkit-example-solo 192.168.1.104

启动demo程序
---
通过以太网卡登录到开发板上, 运行以下命令

    cd /tmp
    chmod 777 linkkit-example-solo

    ./linkkit-example-solo > /tmp/logs & # 运行demo程序并将输出的日志保存到 /tmp/logs 文件
    ifconfig eth0 down && sleep 100 && ifconfig eth0 up # 断开以太网卡后休眠100秒, 确保使用WiFi网卡通信, 体现一键配网功能

*注: 需确保设备开机后没有其他后台程序在调用SDK的API运行, 如果有的话需先停止这些程序*

在手机上的云智能app开始配网
---
<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/app_awss_steps.gif" width="300">

配网中需要让app扫描的二维码可用[工具网页](https://cli.im), 输入以下字符串生成

    http://www.taobao.com?pk=a1X2bEnP82z

其中 `a1X2bEnP82z` 是demo程序使用的设备产品标识, 是从[阿里云IoT控制台](https://iot.console.aliyun.com)申请得到的 `productKey`

观察成功解密SSID和密码
---
通过观察 `/tmp/logs` 文件, 以下日志表明demo程序已成功从手机app处得到和解密当前无线路由器的SSID和密码

    [dbg] zconfig_got_ssid_passwd_callback(103): ssid:aos-linux-test1, bssid:28f366b2f160, 2

观察成功连接到无线路由器
---
以下日志表明demo程序已使用SSID和密码, 成功连接到无线路由器, 具备了IP通信的条件

    [dbg] __awss_start(62): awss connect ssid:aos-linux-test1 success

配网成功时, app的截图为

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/done_awss.jpg" width="300">

之后可点击 **"开始使用"** 按钮, 进入设备绑定环节, 绑定成功时, app的截图为

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/done_binding.jpg" width="300">

捷高摄像头没有配网有关的指示灯, 因此没法从设备上观察出配网过程设备的状态变化, 只能通过app观察进度来判断

另外, 由于上述例子是以 `./linkkit-example-solo > /tmp/logs &` 的方式运行的, 在运行结束后可以分析这个 `logs` 文件查看设备在连接过程中出现的问题

---
一键配网过程中, 可以通过wireshark抓包, 具体抓包的过程见

## <a name="Wireshark 抓包方法">Wireshark 抓包方法</a>

> **注: Windows系统会过滤一些 WiFi 报文不传递给自己的应用程序, 导致 WireShark 软件抓不到空中包, 所以一般以 mac 系统抓包**

可以使用 Wireshark 软件对空气中的WiFi帧进行抓取来帮助自查, 以验证待配网设备是否发出正确的探测请求帧, 以及主配设备是否进行过回复, 步骤如下

在mac电脑安装 Wireshark 软件
---

点击齿轮状的 Capture Options (图中的红框)进入设置, 参考下图

<br>
<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/zero_config/wireshark_setting_1.jpg" width="800" height="400" />

配置 Wireshark 软件为抓包模式
---

反向选择其他所有Interface, 仅仅保留wifi interface, 并且勾选其中的 Promiscuous 和 Monitor 两种抓包模式, 具体见下图

<br>
<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/80211_wireshark_config.png" width="800"/>

一键配网过程的抓包样本
---
参考
<br>
<img src="https://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/sample_broadcast.jpg" width="800"/>
