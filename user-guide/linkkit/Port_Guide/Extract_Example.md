# <a name="目录">目录</a>
+ [不使用SDK自带编译系统时的移植示例](#不使用SDK自带编译系统时的移植示例)
    * [SDK功能配置](#SDK功能配置)
    * [SDK代码抽取](#SDK代码抽取)
    * [获取被抽取的功能点代码](#获取被抽取的功能点代码)
    * [获取被抽取的例子程序](#获取被抽取的例子程序)
    * [其它](#其它)
    * [对接HAL接口或者wrapper接口](#对接HAL接口或者wrapper接口)
    * [结束](#结束)

# <a name="不使用SDK自带编译系统时的移植示例">不使用SDK自带编译系统时的移植示例</a>

> 提示: 如果您移植SDK的时候选择的是"使用SDK自带编译系统", 则可以跳过本章

在SDK的V3.0.1版本以上, 新增了直接基于源代码文件移植对接的能力, 差别在于
---

+ 过去用户要裁剪出SDK中的某部分能力取用, 不得不基于其自带的编译系统, 交叉编译出适合自己目标平台和功能集合的二进制库
    - 然后再将这个库文件(`libiot_sdk.a`)跟自己的软件系统整合, 也就是 **基于库文件对接**
    - 所以如果用户不是很熟悉编译系统或者这个库文件不能很顺利的编译出来, 就会卡住无法继续移植

+ 现在用户若要选用SDK中的某部分能力, 除了使用以前的方法之外, 还可以选择 **基于C文件对接**
    - 适应用户不论是用安装了 `Ubuntu16.04 64位` 的主机还是用安装了 `Windows XP`, `Windows7`, `Windows10` 的主机进行开发
    - 用户都可以通过 `make menuconfig` 命令(Ubuntu)或者通过点击 `config.bat` 脚本(Windows)来选择功能点
    - 选好功能点之后, 都可以通过 `extract.sh` 命令(Ubuntu)或者是通过点击 `extract.bat` 脚本(Windows) 来抽取需要的源文件
    - 有了这些源文件, 不论用户在什么主机上用什么方式开发, 都可以方便的将这些文件加入自己的工程中, 仍用自己熟悉的源码编译方式, 就可以完成移植了
    - 在这个过程中, **所有需要用户自行实现的函数接口也都已被自动整理到一个单独的文件中, 位于 `output/eng/wrappers/wrapper.c`, 逐个填写即可**

本文将以获取 `MQTT上云` 和 `OTA固件升级` 能力为例, 详细演示一个 Windows 开发主机上的完整抽取过程
---

设备端的整体开发过程如下所示

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/extract_workflow.png" width="800" hegiht="600" />

或者更加具体的, 以图中红色文字表示用户执行的动作, 用绿色文字表示用户得到的产物, 则这个过程可用下图表示为

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/extract_flow_chart.png" width="1000" hegiht="800" align=center />


图中左上角是移植过程的开始, 到右下角是移植过程的结束, 以获得目标设备架构的二进制库 `libiot_sdk.a` 和 `libiot_hal.a` 为标志

---
以下是详细步骤讲解

## <a name="SDK功能配置">SDK功能配置</a>

> SDK中有各种功能模块, 用户需要决定需要使用哪些功能, 在本例中假设用户需要"MQTT上云"和"OTA固件下载"的功能

*SDK提供了配置工具用于配置需要使能哪些功能, 每个功能的配置选项名称类似 `FEATURE_MQTT_XXX`*

---
在SDK的根目录双击 `config.bat` 脚本运行, 弹出如下的功能选择界面(相当于Ubuntu16.04 64位主机上的 `make menuconfig` 命令)

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/windows_menuconfig.jpg" width="1000" hegiht="800" align=center />

+ 按下空格键可以选中或者失效某个功能, 使用小键盘的上下键来在不同功能之间切换
+ 如果想知道每个选项的具体含义, 先用方向键将高亮光条移到那个选项上, 再按键盘上的"h"按键, 将出现帮助文本, 对选项进行详细说明

比如:

+ 如果编译环境有自带标准头文件<stdint.h>, 请使能选项 `PLATFORM_HAS_STDINT`
+ 如果目标系统上运行有嵌入式操作系统, 请使能选项 `PLATFORM_HAS_OS`
+ 如果目标系统上支持 `malloc` 和 `free` 这样的动态内存管理能力, 请使能选项 `PLATFORM_HAS_DYNMEM`

---
在本例中, 我们需要"MQTT上云"和"OTA固件下载"的功能, 所以需要选中

+ `FEATURE_MQTT_COMM_ENABLED`: 对应"MQTT上云"功能
+ `FEATURE_OTA_ENABLED`: 对应"OTA固件下载"功能

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/example_mqtt_ota.png" width="800" hegiht="600" align=center />

然后将光标移动到窗口底部的 **`<Exit>`** 上, 在随后弹出的保存对话框中选 **`<Yes>`** 存盘退出配置界面

## <a name="SDK代码抽取">SDK代码抽取</a>

经过上面的步骤, SDK根目录下的 `make.settings` 功能配置文件的内容就会发生变化, 对应到用户所选择的功能

    FEATURE_MQTT_COMM_ENABLED=y
    FEATURE_MQTT_DEFAULT_IMPL=y
    # FEATURE_MQTT_PRE_AUTH is not set
    FEATURE_MQTT_DIRECT=y
    ...
    FEATURE_OTA_ENABLED=y

可以注意到其中对应所选功能点的开关 `FEATURE_MQTT_COMM_ENABLED` 和 `FEATURE_OTA_ENABLED` 都已被设置为 `y`, 表示打开了

---
接下来运行代码抽取工具 `extract.bat`, 在SDK根目录下, 双击它开始运行

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/example_output_makefile.png" width="800" hegiht="600" align=center />

## <a name="获取被抽取的功能点代码">获取被抽取的功能点代码</a>

如同 `config.bat` 脚本的输出是 `make.settings` 文件, `extract.bat` 脚本的输入是 `make.settings`, 输出是 `output` 文件夹

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/example_got_output.png" width="600" hegiht="400" align=center />


> 点击进去, 观察它的内容

`output`目录结构如下
---

    output/
    +-- eng
    |   +-- certs
    |   +-- dev_model
    |   +-- dev_sign
    |   +-- infra
    |   +-- mqtt
    |   +-- ota
    |   +-- wrappers
    |       +-- external_libs
    +-- examples

其中 `output/eng` 目录就对应的是SDK及其支撑接口的实现, 然后 `output/example` 就对应使用SDK中API接口的例子程序

| **文件或目录**          | **说明**
|-------------------------|-------------------------------------------------------------------------------------------------
| `output/eng`            | 对应SDK和支撑SDK的HAL接口
| `output/eng/infra`      | 不对应到某个具体的功能点, 表示"基础", 但是内容的多少会随着被选择的功能点多少而变化
| `output/eng/certs`      | 不对应到某个具体的功能点, 存放着验证阿里云IoT服务端的根证书, 使能TLS传输时出现
| `output/eng/xxx`        | 对应当前被选中的 `xxx` 功能的实现源码, **其中 `output/eng/xxx/xxx_api.h` 列出了这个功能的API**
| `output/example/xxx`    | 对应当前被选中的 `xxx` 功能的例子程序, 演示了如何用这个功能的API编写业务逻辑

在本例中, 情形为

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/example_output_eng.png" width="600" hegiht="400" align=center />

## <a name="获取被抽取的例子程序">获取被抽取的例子程序</a>

对应SDK功能点的代码在 `output/eng` 下, 这些功能点的例子程序都在 `output/examples` 文件夹

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/example_output_demo.png" width="600" hegiht="400" align=center />

## <a name="其它">其它</a>

SDK虽然不假设用户惯用什么方式编译源码, 但也不假设用户一定不会使用 `GNU Make`, 所以仍然会提供一个示例的makefile, 表达如何编译SDK

> 这个makefile中, 几个二进制文件的含义以及产生的方式是这样, 其中前面两者都是在 `output` 目录下运行 `make` 命令即可编译产生的

| **二进制文件**  | **说明**                                        | **产生方式**
|-----------------|-------------------------------------------------|---------------------------------------------------------------------------------------------
| `libiot_sdk.a`  | SDK主库文件, 提供所有形如 `IOT_XXX` 的用户API   | 从 `output/eng` 下除了 `output/wrapper/*` 的目录产生
| `libiot_hal.a`  | 支撑SDK的HAL库, 提供形如 `HAL_XXX` 的底层接口   | 从 `output/eng/wrappers` 下源文件产生, 其中 `output/eng/wrappers/wrapper.c` 需要用户填写(3.0.1+sp1及之后版本更名wrappers/wrappers.c)
| `xxx-example`   | 功能点的例子程序                                | 从 `output/examples/xxx` 下源文件产生, **需要通过 `output` 目录下运行 `make prog` 命令编译**

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/example_output_makefile.png" width="600" hegiht="400" align=center />

> 这个makefile非常简单, 有效内容不超出50行, 它想表达的就是 libiot_sdk.a + libiot_hal.a + example 例程是应该怎样编译出来的

*注: 如果用户不排斥使用 `GNU Make`, 也可以使用这个 makefile, 修改其中的 `CC` 等变量传入交叉编译工具链来实现交叉编译目标板的库文件*
---

## <a name="对接HAL接口或者wrapper接口">对接HAL接口或者wrapper接口</a>

最后, SDK本身的运行对外界所需要的依赖, 现在都以 `HAL_XXX` 接口或者 `wrapper_xxx` 接口表达, **只有被选中功能所必要的HAL接口会被抽取**

---
被抽取的HAL接口是需要用户根据SDK定义的语义自行实现的(可以参考SDK源码目录的 wrappers 下对应文件)

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/example_hal_func.png" width="600" hegiht="400" align=center />

对每个 `HAL_XXX` 接口都有详细的注释和文档, 用户所**需要做的只是填充完 `output/eng/wrappers/wrapper.c` 中所有的空函数, 所需的源码就全部就绪了**

*注: 若移植到Linux平台时, 直接复制使用 `wrappers/os/ubuntu` 下的参考实现中 `HAL_Timer_XXX()` 的部分, 则编译的时候需要加上 `-lrt` 的链接选项*
---

## <a name="结束">结束</a>

到此为止整个SDK的抽取都已经讲解完毕, 您可以将 `output/eng` 下的所有目录加入自己的工程中编译和集成, 并参考 `output/examples` 下的例程开始调用SDK提供的API了
