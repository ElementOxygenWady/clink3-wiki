# <a name="目录">目录</a>
+ [设备影子API](#设备影子API)

# <a name="设备影子API">设备影子API</a>

| 函数名                                                              | 说明
|---------------------------------------------------------------------|-------------------------------------------------
| [IOT_Shadow_Construct](#IOT_Shadow_Construct)                       | 建立一个设备影子的MQTT连接, 并返回被创建的会话句柄
| [IOT_Shadow_Destroy](#IOT_Shadow_Destroy)                           | 摧毁一个设备影子的MQTT连接, 销毁所有相关的数据结构, 释放内存, 断开连接
| [IOT_Shadow_Pull](#IOT_Shadow_Pull)                                 | 把服务器端被缓存的JSON数据下拉到本地, 更新本地的数据属性
| [IOT_Shadow_Push](#IOT_Shadow_Push)                                 | 把本地的数据属性上推到服务器缓存的JSON数据, 更新服务端的数据属性
| [IOT_Shadow_Push_Async](#IOT_Shadow_Push_Async)                     | 和 [IOT_Shadow_Push](#IOT_Shadow_Push) 接口类似, 但是异步的, 上推后便返回, 不等待服务端回应
| [IOT_Shadow_PushFormat_Add](#IOT_Shadow_PushFormat_Add)             | 向已创建的数据类型格式中增添成员属性
| [IOT_Shadow_PushFormat_Finalize](#IOT_Shadow_PushFormat_Finalize)   | 完成一个数据类型格式的构造过程
| [IOT_Shadow_PushFormat_Init](#IOT_Shadow_PushFormat_Init)           | 开始一个数据类型格式的构造过程
| [IOT_Shadow_RegisterAttribute](#IOT_Shadow_RegisterAttribute)       | 创建一个数据类型注册到服务端, 注册时需要**`PushFormat()`**接口创建的数据类型格式
| [IOT_Shadow_DeleteAttribute](#IOT_Shadow_DeleteAttribute)           | 删除一个已被成功注册的数据属性
| [IOT_Shadow_Yield](#IOT_Shadow_Yield)                               | MQTT的主循环函数, 调用后接受服务端的下推消息, 更新本地的数据属性


