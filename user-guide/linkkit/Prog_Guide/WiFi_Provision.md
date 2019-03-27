# <a name="目录">目录</a>
+ [功能概述](#功能概述)
+ [现有方案](#现有方案)
+ [使用步骤和常见问题](#使用步骤和常见问题)
+ [配网通用HAL移植说明](#配网通用HAL移植说明)
    * [配网依赖的Linkkit SDK公共HAL](#配网依赖的Linkkit SDK公共HAL)
    * [配网模块公共HAL](#配网模块公共HAL)
+ [扩展 ](#扩展 )

# <a name="功能概述">功能概述</a>

WiFi设备需要连接到WiFi热点(WiFi AP)之后才能与其它设备进行基于IP的通信, 我们将WiFi设备获取到WiFi热点的SSID/密码的步骤称为WiFi配网

对于手机/电脑/平板而言, 用户可以通过键盘或者触摸屏输入WiFi热点的SSID/密码

但是对于没有键盘, 没有触摸屏的IoT设备而言, 如何获取WiFi热点的SSID/密码是实现设备网络管理的第一个关键步骤

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/1.png" width="700" height="150" />

为了节约WiFi设备厂商开发配网方案的开发成本, 阿里为WiFi类型的IoT设备提供了几种配网方案, 设备厂商可以根据自己的需要相应集成

注:
---
+ 目前阿里提供的配网方案仅针对WiFi家庭网络(即设备使用SSID/密码连接一个WiFi热点), 暂不支持对WiFi企业网络场景进行配网(使用SSID/用户名/密码方式连接WiFi热点)
+ 如果设备可以通过触摸屏/键盘, 或其它方式获取到AP热点的SSID/密码, 可以不用移植下面的配网方案, 因为下面的配网方案的移植工作量和难度都并不小

# <a name="现有方案">现有方案</a>

现有的C-SDK中已支持如下的几种WiFi配网模式
---

+ 一键配网(`smartconfig`): 手机app直接给IoT设备配网, 设备需能够工作在嗅探(sniffer)状态
+ 手机热点配网(`phone-as-ap`): 手机app充当临时热点, 直接给IoT设备配网
+ 路由器热点配网(`router-ap`): 输出到路由器厂商/通信运营商
+ 零配(`zeroconfig`): 用已配网的IoT设备给其它的IoT设备配网
+ 设备热点配网(`dev-ap`): IoT设备充当临时热点, 手机app连接设备热点为其配网, 设备需能够工作在热点(ap)状态

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/1-2.png" width="900" height="600" />

# <a name="使用步骤和常见问题">使用步骤和常见问题</a>

[IoT配网操作手册](https://code.aliyun.com/edward.yangx/public-docs/raw/21851e07768667f9d3baee9cffd07aa06551da9b/docs/iLOP_Awss_Steps.pdf)

# <a name="配网通用HAL移植说明">配网通用HAL移植说明</a>

设备端针对不同的配网方式需要实现的HAL函数有一定区别. 其中部分HAL是SDK公共的,部分HAL是配网模块独有但是所有配网方式公共的, 本文档列举所有公共部分HAL, 即配网能工作必须要实现的HAL;针对每种不同的配网模式分特有的HAL将在每种配网模式中进行单独列举

## <a name="配网依赖的Linkkit SDK公共HAL">配网依赖的Linkkit SDK公共HAL</a>

| 序号    | 函数名                                                                                                                                                      | 说明
|---------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------
| 1       | [HAL_MutexCreate](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_MutexCreate)            | 创建一个互斥量对象, 返回指向所创建互斥量的指针, 用于同步访问, 对于仅支持单线程应用, 可实现为空函数
| 2       | [HAL_MutexDestroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_MutexDestroy)          | 销毁一个互斥量对象, 释放资源
| 3       | [HAL_MutexLock](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_MutexLock)                | 锁住一个互斥量
| 4       | [HAL_MutexUnlock](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_MutexUnlock)            | 解锁一个互斥量
| 5       | [HAL_UptimeMs](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_UptimeMs)                        | 获取设备从上电到当前时刻所经过的毫秒数
| 6       | [HAL_Malloc](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Malloc)                            | 申请一块堆内存
| 7       | [HAL_Free](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Free)                                | 释放参数ptr指向的一块堆内存, 当传入的参数为NULL时不执行任何操作
| 8       | [HAL_SleepMs](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_SleepMs)                          | 睡眠函数, 使当前执行线程睡眠指定的毫秒数
| 9       | [HAL_GetProductKey](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_GetProductKey)              | 获取设备的ProductKey, 用于标识设备的品类, 四元组之一
| 10      | [HAL_GetProductSecret](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_GetProductSecret)        | 获取设备的ProductSecret, 用于标识设备的品类, 四元组之一
| 11      | [HAL_GetDeviceName](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_GetDeviceName)              | 获取设备的DeviceName, 用于标识设备单品的名字, 四元组之一
| 12      | [HAL_GetDeviceSecret](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_GetDeviceSecret)          | 获取设备的DeviceSecret, 用于标识设备单品的密钥, 四元组之一
| 13      | [HAL_Kv_Set](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Kv_Set)                            | Flash中写入键值对(Key-Value)
| 14      | [HAL_Kv_Get](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Kv_Get)                            | Flash中读取键值对的Value
| 15      | [HAL_Aes128_Init](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Aes128_Init)                   | 初始化AES加密的结构体
| 16      | [HAL_Aes128_Destroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Aes128_Destroy)             | 销毁AES加密的结构体
| 17      | [HAL_Aes128_Cbc_Decrypt](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Aes128_Cbc_Decrypt)     | 以AES-CBC-128方式, 根据HAL_Aes128_Init()时传入的密钥解密指定的密文
| 18      | [HAL_Aes128_Cfb_Decrypt](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Aes128_Cfb_Decrypt)     | 以AES-CFB-128方式, 根据HAL_Aes128_Init()时传入的密钥解密指定的密文
| 19      | [HAL_Timer_Create](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Timer_Create)                | 根据Name/TimerFunc和用户上下文创建Timer
| 20      | [HAL_Timer_Start](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Timer_Start)                  | Timer开始计时, Timer超时时调用回调函数TimerFunc
| 21      | [HAL_Timer_Stop](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Timer_Stop)                    | 停止Timer计时
| 22      | [HAL_Timer_Delete](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Timer_Delete)                | 删除Timer, 释放资源

注:
---
+ 包括配网模块在内的SDK公用HAL

## <a name="配网模块公共HAL">配网模块公共HAL</a>

| 序号    | 函数名                                                                                                                                                                                  | 说明
|---------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------
| 1       | [HAL_Awss_Get_Timeout_Interval_Ms](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Get_Timeout_Interval_Ms)             | 获取配网服务(AWSS)的超时时间长度, 单位是毫秒
| 2       | [HAL_Sys_Net_Is_Ready](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Sys_Net_Is_Ready)                                     | 检查Wi-Fi网卡/芯片或模组当前的IP地址是否有效
| 3       | [HAL_Wifi_Get_Ap_Info](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Wifi_Get_Ap_Info)                                     | 获取设备所连接的热点(Access Point)的信息
| 4       | [HAL_Awss_Close_Monitor](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Close_Monitor)                                 | 关闭监听(Monitor)模式
| 5       | [HAL_Awss_Open_Monitor](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Open_Monitor)                                   | 设备工作在监听(Monitor)模式, 并在收到802.11帧的时候调用被传入的回调函数(包括管理帧和数据帧)
| 6       | [HAL_Awss_Switch_Channel](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Switch_Channel)                               | 设置Wi-Fi设备切换到指定的信道(channel)上
| 7       | [HAL_Awss_Get_Channelscan_Interval_Ms](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Get_Channelscan_Interval_Ms)     | 获取在每个信道(channel)上扫描的时间长度, 单位是毫秒
| 8       | [HAL_Wifi_Get_Mac](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Wifi_Get_Mac)                                             | 获取设备的MAC地址, 格式应当是"XX:XX:XX:XX:XX:XX"
| 9      | [HAL_Awss_Connect_Ap](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Awss_Requires#HAL_Awss_Connect_Ap)                                       | 要求Wi-Fi网卡连接指定热点(Access Point)的函数, bssid指定特定AP, 另外bssid也可能为空或无效值(全0或全0xff)

注:
---
+ 配网通用HAL, 仅配网模块调用

关于WiFi配网后绑定环节的说明
---
+ 设备端连接到云端之后会立即发送一个 token 到云端, 然后记录 token 的时间
    - 设备端不会周期的发送 token 到云端, 即使 token 已超时
    - 设备端收到来自手机的 token 查询时, 如果发现 token 已超时, 将会立即发送 token 到云端, 然后再响应手机端的查询
    - 设备端收到来自手机的 token 查询/并且 token 未超时, 将立即应答未超时的 token . 然后再生成新的 token 上报云端, 更新 token 超时时间
    - 云端收到设备端的 token 时会一直保存 token , 直到绑定成功或者 token 超时才会删除, 因此如果设备端在短时间内上报多个 token , 云端都会进行记录
    - 配网成功时设备端会主动通告 token , 然后手机端可以收到 token 之后到云端去进行设备绑定, 然后云端将会删除该 token 的记录
        + 但是设备端并不知道上报云端的 token 已经被删除了
        + 所以如果手机立即再去设备端查询 token, 设备端还是会返回一个在云端已经无效的 token, 此时手机端去绑定设备就会收到无效 token 的错误


# <a name="扩展 ">扩展 </a>

- [一键配网](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Awss_Smartconfig)
- [设备热点配网](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Awss_DevAP)
- [手机热点配网](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Awss_PhoneAP)
- [零配](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/Awss_ZeroConfig)