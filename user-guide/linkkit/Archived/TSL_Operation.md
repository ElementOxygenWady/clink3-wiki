# <a name="目录">目录</a>
+ [TSL静态集成和动态拉取](#TSL静态集成和动态拉取)
    * [TSL静态集成](#TSL静态集成)
    * [TSL动态拉取](#TSL动态拉取)
        - [高级版单品使用TSL动态拉取](#高级版单品使用TSL动态拉取)
        - [高级版网关使用TSL动态拉取](#高级版网关使用TSL动态拉取)

# <a name="TSL静态集成和动态拉取">TSL静态集成和动态拉取</a>

物模型指将物理空间中的实体数字化, 并在云端构建该实体的数据模型. 在物联网平台中, 定义物模型即定义产品功能. 完成功能定义后, 系统将自动生成该产品的物模型

物模型描述产品是什么, 能做什么, 可以对外提供哪些服务

物模型以 JSON 格式表述, 称之为 TSL(即 Thing Specification Language). 在产品的功能定义页面, 单击查看物模型即可查看 JSON 格式的 TSL:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/LP-%E4%BA%A7%E5%93%81%E7%AE%A1%E7%90%86-%E4%BA%A7%E5%93%81%E8%AF%A6%E6%83%85-%E7%89%A9%E6%A8%A1%E5%9E%8B.png)

此时有两种方式在物联网套件中使用TSL:

- 静态集成: 指将TSL直接写到代码中, 这时需要上图中选择导出物模型, 将设备的TSL(JSON格式)导出到文件, 并手动集成到代码中
- 动态拉取: 指在物联网套件运行时去云端拉取TSL

## <a name="TSL静态集成">TSL静态集成</a>
如上所述, 将设备的TSL(JSON格式)导出到文件后(默认文件名为model.json), 由于C语言需要将字符串中的双引号`"`进行转义, 所以需要将导出的TSL文件进行转义

在网上有很多这样的转义工具, 物联网套件中也自带了一个可以转义JSON的小工具

当SDK编译完成后, 使用 `iotx-sdk-c/output/release/bin` 目录下的 `linkkit_tsl_convert` 可以完成该转义任务. 使用方法如下:

     $./linkkit_tsl_convert -i model.json

上述命令执行完成后, 会在调用命令的当前目录生成一个 `conv.txt` 文件, 里面就是转义后的JSON字符串. 目前仅高级版单品支持静态集成TSL, 集成方法如下:

高级版单品使用TSL静态集成
---
默认情况下, 物联网套件编译单品版本的例子程序, 源代码位于 `iotx-sdk-c/examples/linkkit` 目录下的 `linkkit_example_solo.c`

同目录下的 `example_tsl_solo.data` 为存放静态集成TSL的文件

将刚才 `conv.txt` 中的内容复制, 替换 `example_tsl_solo.data` 文件中的 `TSL_STRING` 变量, 重新编译即可

不要忘了将 `linkkit_example_solo.c` 中的三元组替换成该TSL对应产品下设备的三元组

## <a name="TSL动态拉取">TSL动态拉取</a>
如果使用动态拉取TSL, 就不用去替换代码中的静态TSL

需要注意的是, 使用动态拉取TSL时, 在TSL占用空间较大(TSL定义的服务/属性/事件越多, TSL越大)的情况下, 有可能需要更改物联网套件默认的MQTT接收buffer长度

可以按如下两种方法进行配置更改:

- 修改平台配置文件, 在当前使用平台的config.xxx.xxx文件(位于iotx-sdk-c/src/board目录下)中, 修改 `CONFIG_ENV_CFLAGS` 中的 `CONFIG_MQTT_RX_MAXLEN` 编译选项

> 以ubuntu为例, 如下图所示:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/iotkit-MQTT%E9%BB%98%E8%AE%A4%E6%8E%A5%E6%94%B6%E9%95%BF%E5%BA%A6%E5%9C%A8Makefile%E4%B8%AD%E7%9A%84%E9%85%8D%E7%BD%AE.png)

- 在代码中修改默认值, 在 `iotx-sdk-c/include/imports/iot_import_config.h` 文件中修改 `CONFIG_MQTT_RX_MAXLEN` 即可, 如下图所示:

![image](https://linkkit-export.oss-cn-shanghai.aliyuncs.com/iotkit-MQTT%E9%BB%98%E8%AE%A4%E6%8E%A5%E6%94%B6%E9%95%BF%E5%BA%A6%E5%9C%A8iot_import_config.h%E4%B8%AD%E7%9A%84%E9%85%8D%E7%BD%AE.png)

*注意: 在Makefile中设置的 `CONFIG_MQTT_RX_MAXLEN` 值会覆盖掉 `iot_import_config.h` 中设置的值*

### <a name="高级版单品使用TSL动态拉取">高级版单品使用TSL动态拉取</a>
默认情况下, 在高级版单品的example(源代码位于 `iotx-sdk-c/examples/linkkit` 目录下的 `deprecated/solo.c` )中, 使用的是TSL静态集成

若要使用TSL动态拉取, 只需要将 `linkkit_start` 的第二个入参 `get_tsl_from_cloud` 设为`1`即可

### <a name="高级版网关使用TSL动态拉取">高级版网关使用TSL动态拉取</a>
默认情况下, 在高级版网关的example(源代码位于 `iotx-sdk-c/examples/linkkit` 目录下的 `deprecated/gateway.c` )中, 使用的总是TSL动态拉取
