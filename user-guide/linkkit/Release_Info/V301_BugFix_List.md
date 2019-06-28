# <a name="目录">目录</a>

V3.0.1版本的重要补丁
---

| **修复日期**    | **补丁地址**                                                                                                | **症状描述**                                                                                                | **修复备注**
|-----------------|-------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------|-----------------------------------------
| 2019/06/28      | [`3afe23ad`](https://code.aliyun.com/linkkit/c-sdk/commit/3afe23adcd746a9968f419a498a7ad05efa4659d.diff)    | 在极少数的情况下, 连云通道中收到格式不合法的非MQTT类型报文, 会出现SDK内存访问错误, 导致程序崩溃             | 检测到报文非法时, 报错中止处理
| 2019/06/27      | [`0cdadfe4`](https://code.aliyun.com/linkkit/c-sdk/commit/0cdadfe4142dbde584a0ba80fab9f61866a78f51.diff)    | 在MQTT连云时, 当前用户缺少定制化在建连时上报信息的手段, 如OS的类型, 厂商信息等                              | 新增`IOT_Ioctl()`选项定制连接参数
| 2019/06/26      | [`61407d85`](https://code.aliyun.com/linkkit/c-sdk/commit/61407d854609c3b250221f13d2552872ccc00ba7.diff)    | 在一键配网功能中, 若锁定信道后配网失败不能顺利重新开始, 会出现配网失败的现象                                | 清理状态机重新配网
| 2019/06/26      | [`f88cad83`](https://code.aliyun.com/linkkit/c-sdk/commit/f88cad83889779ae6260aa5daca1e55a9b574eff.diff)    | 在广域网连接或弱网连接情况下, 设备以不超过3分钟的心跳间隔建连, 有时会出现因为心跳超时而被云端主动断开连接   | 允许最长以20分钟作为心跳间隔
| 2019/06/26      | [`580c3f5c`](https://code.aliyun.com/linkkit/c-sdk/commit/580c3f5c77a4c0221d7572abb6b5cf64e56ce93c.diff)    | 在设备绑定功能中, 若局域网路由器或设备自身平台不允许, 会出现Token无法广播而导致的概率性绑定失败             | 在全网广播之外新增直接广播
| 2019/06/25      | [`1a7807aa`](https://code.aliyun.com/linkkit/c-sdk/commit/1a7807aa2249aa48ab185847d4faeccd1fa9fc86.diff)    | 在设备绑定过程中, 若本地控制功能(ALCS)被同时启用, 低概率会出现CoAP服务重复初始化导致的绑定失败              | 增加锁保护确保服务启动互斥
| 2019/06/25      | [`f94d723d`](https://code.aliyun.com/linkkit/c-sdk/commit/f94d723d09131cc0a797a5d6d8c55f85dce91646.diff)    | 若设备连接的局域网路由器工作不稳定, 可能因为阿里云服务器域名解析为IP地址失败, 出现连云失败                  | 若域名解析失败重试8次
| 2019/06/22      | [`9b5e1ffd`](https://code.aliyun.com/linkkit/c-sdk/commit/9b5e1ffdc79e2ab07e970a148aa58d0cf3ecc58b.diff)    | 若云端下推的MQTT报文长度超过`CONFIG_MQTT_MESSAGE_MAXLEN`(默认是1024字节), 低概率出现连接断开, 程序崩溃      | 设置报文类型为 `MQTT_CPT_RESERVED`
| 2019/06/04      | [`c8d32cc1`](https://code.aliyun.com/linkkit/c-sdk/commit/c8d32cc1c2ed7ed7e78d9f4c9ac2b5394e27d9c4.diff)    | 对于在Windows主机上开发的用户, 如果不使用微软的Visual Studio而是比如Eclipse开发环境, 可能会出现编译失败     | 将为了VS产生dll库定义的标记全部删除
| 2019/06/04      | [`f19b7548`](https://code.aliyun.com/linkkit/c-sdk/commit/f19b7548f85b3260046c0583b3b3fcd6c309e5b4.diff)    | 在设备绑定功能中, 概率性出现手机App上绑定失败, 调试时可发现设备上报给云的Token与手机获取到的不匹配          | 将Token的生成提前到向外传输它之前
| 2019/05/30      | [`4131ed65`](https://code.aliyun.com/linkkit/c-sdk/commit/4131ed6563995048cfcdd5b5f0794fac1a4bd8df.diff)    | 如果用户直接复制SDK的HAL参考实现在Linux设备上使用, 在TLS发送的时候碰到网络异常, 有可能发生较长时间的卡顿    | 传输前设置socket的发送超时时间
| 2019/05/23      | [`b54c4d6d`](https://code.aliyun.com/linkkit/c-sdk/commit/b54c4d6d2b3fd55c7619974c99317e9b5488779b.diff)    | 在OTA功能中, 概率性出现固件长度收取不准确, 下载完成后仍在继续等待数据, 且新OTA指令可能无法成功下推          | 变量初始化, 确保准确收取长度
| 2019/05/17      | [`8a375ad4`](https://code.aliyun.com/linkkit/c-sdk/commit/8a375ad4f64eef2b973c960678ebb46c3c615134.diff)    | 在OTA功能中, 若固件写硬件异常而升级失败, 可能后续的OTA指令无法从云端下推                                    | 增加异常处理, 上报失败
| 2019/05/16      | [`0f3ad98f`](https://code.aliyun.com/linkkit/c-sdk/commit/0f3ad98fddeba3ad89dc85f77c186a00a561f35d.diff)    | Linux系统集成SDK后, 有较低概率发生和天猫精灵绑定失败, 同时也包含有不必要的HAL函数                           | 调整`HAL_Timer_Start()`参考实现
| 2019/05/14      | [`e79e73f9`](https://code.aliyun.com/linkkit/c-sdk/commit/e79e73f9baf8dc081b0bd893251083dcb057da97.diff)    | 网络不佳时极低概率发生, 网络断开导致MQTT自动重连后, 不到2个心跳周期很快发生心跳丢失的超时而再次重连         | 重连成功时清除心跳丢失累积计数
| 2019/05/11      | [`b9306331`](https://code.aliyun.com/linkkit/c-sdk/commit/b930633173a94515d2bd6f4701e7c3690e4c7908.diff)    | 在MQTT连云时, 若选择`FEATURE_MQTT_DIRECT`关闭, 在海外连接新加坡站点预认证, 可能因为网络延时大而建连失败     | 将例子中对超时设置的示范删除
| 2019/05/09      | [`0cc7c093`](https://code.aliyun.com/linkkit/c-sdk/commit/0cc7c093762d2b247d2f74b6b52182665e4b0158.diff)    | 在子设备管理中, 用户知道子设备的三元组想要查询到该设备的`devid`的时候, 没有API可用只能自己管理对应关系      | `IOT_Ioctl(...QUERY_DEVID)`可查
| 2019/05/06      | [`663bea7d`](https://code.aliyun.com/linkkit/c-sdk/commit/663bea7d91b05e4910bd8ccd085a0a6fea00d762.diff)    | 在设备绑定功能中, 若有离线reset尚未上报, 概率性出现绑定成功之后又无故解绑, 手机不能控制设备的现象           | 保证reset请求在bind请求之前发送

