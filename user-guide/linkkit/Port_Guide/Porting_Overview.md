# <a name="目录">目录</a>
+ [移植概述](#移植概述)
    * [SDK结构说明](#SDK结构说明)
    * [SDK编译方式](#SDK编译方式)
        - [使用SDK自带的编译系统](#使用SDK自带的编译系统)
        - [抽取代码加入用户的开发环境](#抽取代码加入用户的开发环境)

# <a name="移植概述">移植概述</a>

## <a name="SDK结构说明">SDK结构说明</a>

C语言Link Kit SDK的结构如下图所示：

![image.png | left | 666x277](https://cdn.nlark.com/lark/0/2018/png/56016/1539694654162-e50f9a51-6e74-4248-9df4-ed898cd02bcb.png "")

SDK被设计为可以在各种OS/硬件平台上运行，因此凡是与OS、目标硬件相关的操作都被抽象为HAL函数，设备商在开发产品时除了需要调用SDK的API实现产品业务逻辑外，还需要对SDK依赖的HAL函数进行实现。

设备商下载SDK之后，进行的操作如下：<br>
1. 进行HAL函数的实现<br>
2. 调用SDK提供的API进行产品功能实现<br>
3. 对SDK进行编译<br>

下面主要讲解如何对SDK进行编译

## <a name="SDK编译方式">SDK编译方式</a>

将SDK移植到目标嵌入式平台有两种方式：
* 使用SDK自带的编译系统，当开发环境为Linux时使用
* 抽取SDK的代码文件加入到用户的开发环境编译

---

### <a name="使用SDK自带的编译系统">使用SDK自带的编译系统</a>

如果使用Linux作为开发环境，可以直接使用SDK自带的编译系统对SDK进行编译，

使用SDK自带的编译系统时, 您需要
* 准备一台 `Linux` 的开发主机, 安装 `Ubuntu16.04 64bits`或以上版本<br>
* 运行 `make menuconfig` 命令配置功能点<br>
* 在 `tools/board` 目录下新增自己的硬件平台描述文件，进行交叉编译配置<br>
* 运行 `make reconfig` 选择自己的平台<br>
* 运行 `make`, <strong>产物是已经交叉编译好的 </strong><strong><code>libiot_sdk.a</code></strong>, 在 `output/release/lib` 目录下

用户可以通过文档:[基于Make的编译系统说明](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Make_Usage) 了解编译系统如何使用

可参考文档[基于Make的交叉编译示例](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Build_Example)了解交叉编译的过程

### <a name="抽取代码加入用户的开发环境">抽取代码加入用户的开发环境</a>

KEIL、IAR这样的开发工具无法使用make，需要将SDK中的功能代码抽取出来加入工程进行编译。

代码抽取以及编译过程如下：
* 准备一台 `Linux` 的开发主机, 安装 `Ubuntu16.04 64bits`；或者准备一台 `Windows` 的开发主机<br>
* 运行 `make menuconfig` 命令(Ubuntu) 或者 `config.bat` 脚本(Windows)配置需要使用到的SDK功能点<br>
* 运行 `extract.sh` 脚本(Ubuntu) 或者 `extract.bat` 脚本(Windows)根据被选择的功能点抽取代码<br>
* 用户选中的SDK功能相关的代码将被放置在 `output/eng` 目录下<br>
* 用户将`output/eng`下的代码添加到开发工具的项目中进行编译<br>

用户可参考文档[基于代码抽取时的移植示例](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Extract_Example)了解相关过程。

