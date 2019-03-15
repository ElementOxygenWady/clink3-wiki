# <a name="目录">目录</a>
+ [设备影子API](#设备影子API)
+ [Shadow用户API详解](#Shadow用户API详解)
    * [IOT_Shadow_Construct](#IOT_Shadow_Construct)
    * [IOT_Shadow_Destroy](#IOT_Shadow_Destroy)
    * [IOT_Shadow_Yield](#IOT_Shadow_Yield)
    * [IOT_Shadow_RegisterAttribute](#IOT_Shadow_RegisterAttribute)
    * [IOT_Shadow_DeleteAttribute](#IOT_Shadow_DeleteAttribute)
    * [IOT_Shadow_Push](#IOT_Shadow_Push)
    * [IOT_Shadow_Push_Async](#IOT_Shadow_Push_Async)
    * [IOT_Shadow_Pull](#IOT_Shadow_Pull)
    * [IOT_Shadow_PushFormat_Init](#IOT_Shadow_PushFormat_Init)
    * [IOT_Shadow_PushFormat_Add](#IOT_Shadow_PushFormat_Add)
    * [IOT_Shadow_PushFormat_Finalize](#IOT_Shadow_PushFormat_Finalize)

# <a name="设备影子API">设备影子API</a>

| 函数名                                                              | 说明
|---------------------------------------------------------------------|-------------------------------------------------
| [IOT_Shadow_Construct](#IOT_Shadow_Construct)                       | 建立一个设备影子的MQTT连接, 并返回被创建的会话句柄
| [IOT_Shadow_Destroy](#IOT_Shadow_Destroy)                           | 销毁一个设备影子的MQTT连接, 销毁所有相关的数据结构, 释放内存, 断开连接
| [IOT_Shadow_Pull](#IOT_Shadow_Pull)                                 | 把服务器端被缓存的JSON数据下拉到本地, 更新本地的数据属性
| [IOT_Shadow_Push](#IOT_Shadow_Push)                                 | 把本地的数据属性上推到服务器缓存的JSON数据, 更新服务端的数据属性
| [IOT_Shadow_Push_Async](#IOT_Shadow_Push_Async)                     | 和 [IOT_Shadow_Push](#IOT_Shadow_Push) 接口类似, 但是异步的, 上推后便返回, 不等待服务端回应
| [IOT_Shadow_PushFormat_Add](#IOT_Shadow_PushFormat_Add)             | 向已创建的数据类型格式中增添成员属性
| [IOT_Shadow_PushFormat_Finalize](#IOT_Shadow_PushFormat_Finalize)   | 完成一个数据类型格式的构造过程
| [IOT_Shadow_PushFormat_Init](#IOT_Shadow_PushFormat_Init)           | 开始一个数据类型格式的构造过程
| [IOT_Shadow_RegisterAttribute](#IOT_Shadow_RegisterAttribute)       | 创建一个数据类型注册到服务端, 注册时需要**`PushFormat()`**接口创建的数据类型格式
| [IOT_Shadow_DeleteAttribute](#IOT_Shadow_DeleteAttribute)           | 删除一个已被成功注册的数据属性
| [IOT_Shadow_Yield](#IOT_Shadow_Yield)                               | MQTT的主循环函数, 调用后接受服务端的下推消息, 更新本地的数据属性
------
# <a name="Shadow用户API详解">Shadow用户API详解</a>

## <a name="IOT_Shadow_Construct">IOT_Shadow_Construct</a>

原型
---
```
void *IOT_Shadow_Construct(iotx_shadow_para_pt pparams)
```

接口说明
---
建立一个设备影子的MQTT连接, 并返回被创建的会话句柄, 并Subscribe设备影子的Topic（`/shadow/get/${product_key}/${device_name}`）

参数说明
---

| 参数            | 数据类型                | 方向    | 说明
|-----------------|-------------------------|---------|-----------------
| pparams     | iotx_shadow_para_pt     | 输入     | Shadow初始化参数

返回值说明
---
| 值      | 说明
|---------|-------------
| NULL    | 失败
| 非NULL  | MQTT句柄

参数附加说明
---
```
typedef struct {
    iotx_mqtt_param_t mqtt;
} iotx_shadow_para_t, *iotx_shadow_para_pt;

typedef struct {
    uint16_t                   port;
    const char                 *host;
    const char                 *client_id;
    const char                 *username;
    const char                 *password;
    const char                 *pub_key;
    uint8_t                    clean_session;
    uint32_t                   request_timeout_ms;
    uint32_t                   keepalive_interval_ms;
    uint32_t                   write_buf_size;
    uint32_t                   read_buf_size;
    iotx_mqtt_event_handle_t    handle_event;
} iotx_mqtt_param_t, *iotx_mqtt_param_pt;
```

+ `port`: 云端服务器端口, 可通 [IOT_SetupConnInfo](#IOT_SetupConnInfo) 输出参数得到
+ `host`: 云端服务器地址, 可通 [IOT_SetupConnInfo](#IOT_SetupConnInfo) 输出参数得到
+ `client_id`: MQTT客户端ID, 可通 [IOT_SetupConnInfo](#IOT_SetupConnInfo) 输出参数得到
+ `username`: 登录MQTT服务器用户名, 可通 [IOT_SetupConnInfo](#IOT_SetupConnInfo) 输出参数得到
+ `password`: 登录MQTT服务器密码, 可通 [IOT_SetupConnInfo](#IOT_SetupConnInfo) 输出参数得到
+ `pub_key`: MQTT连接加密方式及密钥, 可通 [IOT_SetupConnInfo](#IOT_SetupConnInfo) 输出参数得到
+ `clean_session`: 选择是否使用MQTT协议的clean session特性
+ `request_timeout_ms`: MQTT消息发送的超时时间
+ `keepalive_interval_ms`: MQTT心跳超时时间
+ `write_buf_size`: MQTT消息发送buffer最大长度
+ `read_buf_size`: MQTT消息接收buffer最大长度
+ `handle_event`: 用户回调函数, 用与接收MQTT模块的事件信息

-----

## <a name="IOT_Shadow_Destroy">IOT_Shadow_Destroy</a>

原型
---
```
iotx_err_t IOT_Shadow_Destroy(void *phandle);
```

接口说明
---
销毁一个设备影子的MQTT连接, 销毁所有相关的数据结构, 释放内存, 断开连接

参数说明
---

| 参数        | 数据类型    | 方向    | 说明
|-------------|-------------|---------|-------------
| phandle     | void *     | 输入    | Shadow句柄,可为NULL

返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败

-----

## <a name="IOT_Shadow_Yield">IOT_Shadow_Yield</a>

原型
---
```
void IOT_Shadow_Yield(void *handle, int timeout_ms);
```

接口说明
---
MQTT的主循环函数, 调用后接受服务端的下推消息, 更新本地的数据属性

参数说明
---

| 参数        | 数据类型| 方向    | 说明
|-------------|---------|---------|-------------------------
| handle      | void *  | 输入    | MQTT句柄,可为NULL
| timeout_ms  | int     | 输入    | 尝试接收报文的超时时间

-----

## <a name="IOT_Shadow_RegisterAttribute">IOT_Shadow_RegisterAttribute</a>

原型
---
```
iotx_err_t IOT_Shadow_RegisterAttribute(void *handle, iotx_shadow_attr_pt pattr);
```

接口说明
---
创建一个数据类型注册到服务端, 注册时需要**`PushFormat()`**接口创建的数据类型格式

参数说明
---

| 参数    | 数据类型| 方向    | 说明
|---------|---------|---------|-------------
| handle  | void *  | 输入    | MQTT句柄,可为NULL
| pattr   | iotx_shadow_attr_pt| 输入    | 属性参数

返回值说明
---
| 值  | 说明
|-----|---------
| 0   | 注册成功
| 其他   | 注册失败

参数附加说明
---
```
typedef struct iotx_shadow_attr_st {
    iotx_shadow_datamode_t mode;        ///< data mode
    const char *pattr_name;             ///< attribute name
    void *pattr_data;                   ///< pointer to the attribute data
    iotx_shadow_attr_datatype_t attr_type;    ///< data type
    uint32_t timestamp;                 ///timestamp in Epoch(Unix) format
    iotx_shadow_attr_cb_t callback;     ///< callback when related control message come.
} iotx_shadow_attr_t, *iotx_shadow_attr_pt;

typedef enum {
    IOTX_SHADOW_READONLY,
    IOTX_SHADOW_WRITEONLY,
    IOTX_SHADOW_RW
} iotx_shadow_datamode_t;

typedef enum {
    IOTX_SHADOW_NULL,
    IOTX_SHADOW_INT32,
    IOTX_SHADOW_STRING,
} iotx_shadow_attr_datatype_t;

typedef void (*iotx_shadow_attr_cb_t)(struct iotx_shadow_attr_st *pattr);

```

+ `mode`: 数据访问属性（`只读，只写，读写`）
+ `pattr_name`: 属性名
+ `pattr_data`: 属性值
+ `attr_type`: 属性数据类型（`NULL， STRING，INT32`）
+ `timestamp`: 时间戳（`Epoch(Unix) format`）
+ `callback`: 与属性对应的回调函数

-----
## <a name="IOT_Shadow_DeleteAttribute">IOT_Shadow_DeleteAttribute</a>

原型
---
```
iotx_err_t IOT_Shadow_DeleteAttribute(void *handle, iotx_shadow_attr_pt pattr);
```

接口说明
---
删除一个已被成功注册的数据属性

参数说明
---

| 参数    | 数据类型| 方向    | 说明
|---------|---------|---------|-------------
| handle  | void *  | 输入    | MQTT句柄,可为NULL
| pattr   | iotx_shadow_attr_pt| 输入    | 属性参数

返回值说明
---
| 值  | 说明
|-----|---------
| 0   | 注销成功
| 其他   | 注销失败
-----

## <a name="IOT_Shadow_Push">IOT_Shadow_Push</a>

原型
---
```
iotx_err_t IOT_Shadow_Push(void *handle, char *data, uint32_t data_len, uint16_t timeout_s);

```

接口说明
---
把本地的数据属性上推到服务器缓存的JSON数据, 更新服务端的数据属性

参数说明
---

| 参数                | 数据类型                            | 方向    | 说明
|---------------------|-------------------------------------|---------|-------------------------------------
| handle              | void *                              | 输入    | Shadow句柄
| data        | char *                        | 输入    | Shadow属性数据
| data_len                 | uint32_t                     | 输入    | Shadow属性数据长度
| timeout_s   | uint16_t     | 输入    | 超时时间（同步操作）

返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败

------
## <a name="IOT_Shadow_Push_Async">IOT_Shadow_Push_Async</a>

原型
---
```
iotx_err_t IOT_Shadow_Push_Async(
            void *handle,
            char *data,
            size_t data_len,
            uint16_t timeout_s,
            iotx_push_cb_fpt cb_fpt,
            void *pcontext);
```

接口说明
---
和 [IOT_Shadow_Push](#IOT_Shadow_Push) 接口类似, 但是异步的, 上推后便返回, 不等待服务端回应

参数说明
---

| 参数                | 数据类型                            | 方向    | 说明
|---------------------|-------------------------------------|---------|-------------------------------------
| handle              | void *                              | 输入    | Shadow句柄
| data        | char *                        | 输入    | Shadow属性数据
| data_len                 | uint32_t                     | 输入    | Shadow属性数据长度
| timeout_s   | uint16_t     | 输入    | 超时时间
| cb_fpt | iotx_push_cb_fpt | 输入    | 
| pcontext | void * | 输入    | 

返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败

------
## <a name="IOT_Shadow_Pull">IOT_Shadow_Pull</a>

原型
---
```
iotx_err_t IOT_Shadow_Pull(void *handle);
```
接口说明
---
把服务器端被缓存的JSON数据下拉到本地, 更新本地的数据属性

参数说明
---

| 参数                | 数据类型                            | 方向    | 说明
|---------------------|-------------------------------------|---------|-------------------------------------
| handle              | void *                              | 输入    | Shadow句柄


返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败

------
## <a name="IOT_Shadow_PushFormat_Init">IOT_Shadow_PushFormat_Init</a>

原型
---
```
iotx_err_t IOT_Shadow_PushFormat_Init(
            void *handle,
            format_data_pt pformat,
            char *buf,
            uint16_t size);
```

接口说明
---
开始一个数据类型格式的构造过程

参数说明
---

| 参数                | 数据类型                            | 方向    | 说明
|---------------------|-------------------------------------|---------|-------------------------------------
| handle              | void *                              | 输入    | Shadow句柄
| pformat        | format_data_pt                        | 输入    | Shadow属性数据格式
| buf                 | char *                     | 输入    | Shadow属性
| size   | uint16_t     | 输入    | 长度

返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败
---
参数附加说明
---
```
typedef struct {
    bool flag_new;
    uint32_t buf_size;
    uint32_t offset;
    char *buf;
} format_data_t, *format_data_pt;

```
------
## <a name="IOT_Shadow_PushFormat_Add">IOT_Shadow_PushFormat_Add</a>

原型
---
```
iotx_err_t IOT_Shadow_PushFormat_Add(
            void *handle,
            format_data_pt pformat,
            iotx_shadow_attr_pt pattr);

```

接口说明
---
向已创建的数据类型格式中增添成员属性

参数说明
---

| 参数                | 数据类型                            | 方向    | 说明
|---------------------|-------------------------------------|---------|-------------------------------------
| handle              | void *                              | 输入    | Shadow句柄
| pformat        | format_data_pt                        | 输入    | Shadow属性数据格式
| pattr                 | iotx_shadow_attr_pt                     | 输入    | Shadow属性


返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败
---
参数附加说明
---
```
typedef struct {
    bool flag_new;
    uint32_t buf_size;
    uint32_t offset;
    char *buf;
} format_data_t, *format_data_pt;

typedef struct iotx_shadow_attr_st {
    iotx_shadow_datamode_t mode;       ///< data mode
    const char *pattr_name;             ///< attribute name
    void *pattr_data;                   ///< pointer to the attribute data
    iotx_shadow_attr_datatype_t attr_type;    ///< data type
    uint32_t timestamp;                 ///timestamp in Epoch(Unix) format
    iotx_shadow_attr_cb_t callback;    ///< callback when related control message come.
} iotx_shadow_attr_t, *iotx_shadow_attr_pt;
```
-----
## <a name="IOT_Shadow_PushFormat_Finalize">IOT_Shadow_PushFormat_Finalize</a>

原型
---
```
iotx_err_t IOT_Shadow_PushFormat_Finalize(void *handle, format_data_pt pformat);

```

接口说明
---
完成一个数据类型格式的构造过程

参数说明
---

| 参数                | 数据类型                            | 方向    | 说明
|---------------------|-------------------------------------|---------|-------------------------------------
| handle              | void *                              | 输入    | Shadow句柄
| pformat        | format_data_pt                        | 输入    | Shadow属性数据格式


返回值说明
---
| 值      | 说明
|---------|---------
| 0       | 成功
| < 0     | 失败
-----

