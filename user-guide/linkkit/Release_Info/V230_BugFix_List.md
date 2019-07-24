# <a name="目录">目录</a>

V2.3.0版本的重要补丁
---

| **修复日期**    | **补丁地址**                                                                                                | **症状描述**                                                                                                    | **修复备注**
|-----------------|-------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------|-------------------------------------
| 2019/07/24      | [`eddb99cf`](https://code.aliyun.com/linkkit/c-sdk/commit/eddb99cfc97615fe60b57f37b328b65e1aa7db64.diff)    | 在WiFi配网功能中, 若连云不稳定, 设备概率性会刚完成绑定后就从手机app的设备发现列表中消失                         | 用hash重新实现`HAL_KV_XXX()`
| 2019/07/24      | [`dea6b6f2`](https://code.aliyun.com/linkkit/c-sdk/commit/dea6b6f21c096b6dfc074eab4a7b8d6f33aaae92.diff)    | 如果用户直接复制SDK的HAL参考实现在Linux设备上使用, 将设备断网静置数个小时, 概率性发生内存越界写而程序崩溃       | DNS解析失败时回收socket资源
| 2019/07/05      | [`464f9382`](https://code.aliyun.com/linkkit/c-sdk/commit/464f938265456ff67700477dbfb1989fb10f976a.diff)    | 在WiFi配网功能中, 用户调用接口`awss_stop()`不能停止先前调用`awss_start()`的线程, 导致无法按预期退出配网过程     | 增加SDK内部状态同步
| 2019/07/04      | [`9527b2b1`](https://code.aliyun.com/linkkit/c-sdk/commit/9527b2b1ddf23012614eddef09e039f1522bbbc8.diff)    | 在家庭路由器不稳定或其DNS设置对阿里云域名不友好时, 会出现路由器下的IoT设备解析域名失败, 连云不成功              | Linux设备可由阿里云官服解析域名
| 2019/06/28      | [`f3fbad54`](https://code.aliyun.com/linkkit/c-sdk/commit/f3fbad54d76eec7a19f9424fbb3ab802ea3d2ac4.diff)    | 在极少数的情况下, 连云通道中收到格式不合法的非MQTT类型报文, 会出现SDK内存访问错误, 导致程序崩溃                 | 检测到报文非法时, 报错中止处理
| 2019/06/27      | [`4f6f40a1`](https://code.aliyun.com/linkkit/c-sdk/commit/4f6f40a1a6fec56526770e9e786d146e474f5064.diff)    | 在某些嵌入式Linux硬件上, DNS解析文件不会自动刷新, 使DNS解析失败后重试也无法成功, 连云不会成功                   | 用`res_init()`强制刷新DNS缓存
| 2019/06/26      | [`17559141`](https://code.aliyun.com/linkkit/c-sdk/commit/1755914116403a80190194124c09dd1aa92ed201.diff)    | 在一键配网功能中, 若锁定信道后配网失败不能顺利重新开始, 会出现配网失败的现象                                    | 清理状态机重新配网
| 2019/06/26      | [`af7a1916`](https://code.aliyun.com/linkkit/c-sdk/commit/af7a1916b582397040a9c5312faa93ab11b5553e.diff)    | 在设备绑定功能中, 若局域网路由器或设备自身平台不允许, 会出现Token无法广播而导致的概率性绑定失败                 | 在全网广播之外新增直接广播
| 2019/06/25      | [`a3f9fe16`](https://code.aliyun.com/linkkit/c-sdk/commit/a3f9fe1690f1ed5f732c02571ee5ebc638efe3e1.diff)    | 在广域网连接或弱网连接情况下, 设备以不超过3分钟的心跳间隔建连, 有时会出现因为心跳超时而被云端主动断开连接       | 允许最长以20分钟作为心跳间隔
| 2019/06/25      | [`3d501ad9`](https://code.aliyun.com/linkkit/c-sdk/commit/3d501ad9b6f94b392bdbdaad7f78f74528c4bd17.diff)    | 在设备绑定功能中, 若本地控制功能(ALCS)被同时启用, 低概率会出现CoAP服务重复初始化导致的绑定失败                  | 增加锁保护确保服务启动互斥
| 2019/06/25      | [`349e0252`](https://code.aliyun.com/linkkit/c-sdk/commit/349e0252cf464dbf9d33233c12553a963e601d3a.diff)    | 若设备连接的局域网路由器工作不稳定, 可能因为阿里云服务器域名解析为IP地址失败, 出现连云失败                      | 若域名解析失败重试8次
| 2019/06/22      | [`88263ebf`](https://code.aliyun.com/linkkit/c-sdk/commit/88263ebf18ef90e7a6db8bd0b005c90a0fef67af.diff)    | 若云端下推的MQTT报文长度超过`CONFIG_MQTT_MESSAGE_MAXLEN`(默认是1024字节), 低概率出现连接断开, 程序崩溃          | 设置报文类型为 `MQTT_CPT_RESERVED`
| 2019/05/30      | [`8e2de3a3`](https://code.aliyun.com/linkkit/c-sdk/commit/8e2de3a3cb07a26177c74e40b2c4e3d2c7084523.diff)    | 如果用户直接复制SDK的HAL参考实现在Linux设备上使用, 在TLS发送的时候碰到网络异常, 有可能发生较长时间的卡顿        | 传输前设置socket的发送超时时间
| 2019/05/23      | [`f1c6f9f0`](https://code.aliyun.com/linkkit/c-sdk/commit/f1c6f9f00ba68dce67215d92bdfc042eed843279.diff)    | 在OTA功能中, 概率性出现固件长度收取不准确, 下载完成后仍在继续等待数据, 且新OTA指令可能无法成功下推              | 变量初始化, 确保准确收取长度
| 2019/05/17      | [`bd1336d3`](https://code.aliyun.com/linkkit/c-sdk/commit/bd1336d3d1a59ddcd7dfb017b82b96b8e9f044b5.diff)    | 在OTA功能中, 若固件写硬件异常而升级失败, 可能后续的OTA指令无法从云端下推                                        | 增加异常处理, 上报失败
| 2019/05/16      | [`f5a46c2c`](https://code.aliyun.com/linkkit/c-sdk/commit/f5a46c2c8ad89de37066ef742b06f59678b937b9.diff)    | Linux系统集成SDK后, 有较低概率发生和天猫精灵绑定失败, 同时也包含有不必要的HAL函数                               | 调整`HAL_Timer_Start()`参考实现
| 2019/05/14      | [`c90754bf`](https://code.aliyun.com/linkkit/c-sdk/commit/c90754bf42b44089a49ee8b6830f21e0669e2d14.diff)    | 网络不佳时极低概率发生, 网络断开导致MQTT自动重连后, 不到2个心跳周期很快发生心跳丢失的超时而再次重连             | 重连成功时清除心跳丢失累积计数
| 2019/05/11      | [`14ff7c94`](https://code.aliyun.com/linkkit/c-sdk/commit/14ff7c94f9a1635b22c683a5e6d701490c8347c5.diff)    | 在MQTT连云时, 若选择`FEATURE_MQTT_DIRECT`关闭, 在海外连接新加坡站点预认证, 可能因为网络延时大而建连失败         | 将例子中对超时设置的示范删除
| 2019/05/09      | [`ec7b82d1`](https://code.aliyun.com/linkkit/c-sdk/commit/ec7b82d162c4e448f279272f7102dc26e556a60e.diff)    | 在子设备管理中, 用户知道子设备的三元组想要查询到该设备的`devid`的时候, 没有API可用只能自己管理对应关系          | `IOT_Ioctl(...QUERY_DEVID)`可查
| 2019/05/07      | [`ddd7586e`](https://code.aliyun.com/linkkit/c-sdk/commit/ddd7586e9507277ad7b2b25b7112e45ae9152faa.diff)    | 在OTA功能中, 升级1个合法固件但不升级, 再升级第2个合法固件, 下载后SDK将误认为校验和错误而失败                    | 新固件下载时初始化校验和上下文
| 2019/05/06      | [`46fc6c79`](https://code.aliyun.com/linkkit/c-sdk/commit/46fc6c79bc901a76105596844b46a4b51ac8a5ec.diff)    | 在设备绑定功能中, 若有离线reset尚未上报, 概率性出现绑定成功之后又无故解绑, 手机不能控制设备的现象               | 保证reset请求在bind请求之前发送
| 2019/05/03      | [`223f3c53`](https://code.aliyun.com/linkkit/c-sdk/commit/223f3c533a6aa7a3a517cf676abb533a1733cb64.diff)    | 在WiFi配网功能中, 若将设备和手机置于信号不佳的弱网环境下, iOS手机用云智能app配网概率性失败                      |
| 2019/05/03      | [`ef6e5b43`](https://code.aliyun.com/linkkit/c-sdk/commit/ef6e5b43c02b6aeb0d1592b2edd2a90e58613c89.diff)    | 在WiFi配网功能中, 若将设备置于桥接模式的路由器级联环境中, 一键配网概率性失败                                    |
| 2019/04/30      | [`faf728d9`](https://code.aliyun.com/linkkit/c-sdk/commit/faf728d9833cad61e7bf061f485266a69951d1c2.diff)    | 在WiFi配网功能中, 调用`awss_stop()`接口概率性失效, 表现为获取到SSID和密码后SDK仍在扫描信道, 且不退出监听模式    | 调整控制变量解决
| 2019/04/30      | [`f6e10946`](https://code.aliyun.com/linkkit/c-sdk/commit/f6e109462f2a9a137dff112a26b61963ec1d741b.diff)    | 在TLS加密上云通信时用`1883`端口, 有时会因为防火墙软件设置而无法建连成功                                         | 使用更通用的`443`端口建立连云通道
| 2019/04/30      | [`b5877227`](https://code.aliyun.com/linkkit/c-sdk/commit/b5877227e87355f3a17335b579eb3a3bd1e9c5a1.diff)    | 网关子设备的上线信息缺失, 阻碍问题定位和控制台显示                                                              | 按新上线的云端协议补齐丢失的信息
| 2019/03/11      | [`3968ea37`](https://code.aliyun.com/linkkit/c-sdk/commit/3968ea37e5d9cade5dc04661398da19d68fd94eb.diff)    | 在设备绑定功能中, 低概率出现手机App或天猫精灵从设备查询到的Token与云侧不一致, 导致绑定失败                      | 调整Token更新与应答查询的顺序

