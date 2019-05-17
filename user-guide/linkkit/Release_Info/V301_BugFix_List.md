# <a name="目录">目录</a>

V3.0.1版本的重要补丁
---

| **修复日期**    | **补丁地址**                                                                                                | **症状描述**                                                                                                | **修复备注**
|-----------------|-------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------|-------------------------------------
| 2019/05/17      | [`bd1336d3`](https://code.aliyun.com/linkkit/c-sdk/commit/bd1336d3d1a59ddcd7dfb017b82b96b8e9f044b5.diff)    | 在OTA功能中, 若固件写硬件异常而升级失败, 可能后续的OTA指令无法从云端下推                                    | 增加异常处理, 上报失败
| 2019/05/16      | [`0f3ad98f`](https://code.aliyun.com/linkkit/c-sdk/commit/0f3ad98fddeba3ad89dc85f77c186a00a561f35d.diff)    | Linux系统集成SDK后, 有较低概率发生和天猫精灵绑定失败, 同时也包含有不必要的HAL函数                           | 调整`HAL_Timer_Start()`参考实现
| 2019/05/14      | [`e79e73f9`](https://code.aliyun.com/linkkit/c-sdk/commit/e79e73f9baf8dc081b0bd893251083dcb057da97.diff)    | 网络不佳时极低概率发生, 网络断开导致MQTT自动重连后, 不到2个心跳周期很快发生心跳丢失的超时而再次重连         | 重连成功时清除心跳丢失累积计数
| 2019/05/11      | [`b9306331`](https://code.aliyun.com/linkkit/c-sdk/commit/b930633173a94515d2bd6f4701e7c3690e4c7908.diff)    | 在MQTT连云时, 若选择`FEATURE_MQTT_DIRECT`关闭, 在海外连接新加坡站点预认证, 可能因为网络延时大而建连失败     | 将例子中对超时设置的示范删除
| 2019/05/09      | [`0cc7c093`](https://code.aliyun.com/linkkit/c-sdk/commit/0cc7c093762d2b247d2f74b6b52182665e4b0158.diff)    | 在子设备管理中, 用户知道子设备的三元组想要查询到该设备的`devid`的时候, 没有API可用只能自己管理对应关系      | `IOT_Ioctl(...QUERY_DEVID)`可查
| 2019/05/06      | [`663bea7d`](https://code.aliyun.com/linkkit/c-sdk/commit/663bea7d91b05e4910bd8ccd085a0a6fea00d762.diff)    | 在设备绑定功能中, 若有离线reset尚未上报, 概率性出现绑定成功之后又无故解绑, 手机不能控制设备的现象           | 保证reset请求在bind请求之前发送

