# <a name="目录">目录</a>
+ [向云端发送更新设备标签的消息](#向云端发送更新设备标签的消息)
+ [向云端发送删除设备标签的消息](#向云端发送删除设备标签的消息)
+ [API功能详解](#API功能详解)

> 设备标签是用于展示如设备信息, 如厂商/设备型号的静态扩展信息, 以键值对的形式存储在云端

# <a name="向云端发送更新设备标签的消息">向云端发送更新设备标签的消息</a>
---
上报设备标签信息更新的消息时, 标签信息放在成对的一组或多组`attrKey`和`attrValue`中. `attrKey`的定义支持`大小写字母及数字`, 但不能以`数字`开头, 长度为`2~32`个字节, payload为JSON数组类型, 可包含多对`{"attrKey":"","attrValue":""}`, 示例如下:
```
/* 设备标签有两组, 第一组attrKey为abc, attrValue为Hello,World. 第二组attrKey为def, attrValue为Hello,Aliyun */
char *payload = "[{\"attrKey\":\"abc\",\"attrValue\":\"Hello,World\"},{\"attrKey\":\"def\",\"attrValue\":\"Hello,Aliyun\"}]";

/* 设备标签payload准备好以后, 就可以使用如下接口进行上报了 */
IOT_Linkkit_Report(devid, ITM_MSG_DEVICEINFO_UPDATE, (unsigned char *)payload, strlen(payload));

```

# <a name="向云端发送删除设备标签的消息">向云端发送删除设备标签的消息</a>
---
上报设备标签信息删除的消息时, 将标签信息的attrKey放在一个或多个`attrKey`中
```
/* 设备标签attrKey有两个, 第一个attrKey为abc. 第二个attrKey为def */
char *payload = "[{\"attrKey\":\"abc\"},{\"attrKey\":\"def\"}]";

/* 设备标签payload准备好以后, 就可以使用如下接口进行上报了 */
IOT_Linkkit_Report(devid, ITM_MSG_DEVICEINFO_DELETE, (unsigned char *)payload, strlen(payload));
```

# <a name="API功能详解">API功能详解</a>

`IOT_Linkkit_Report`的详细说明请见`物模型编程`
