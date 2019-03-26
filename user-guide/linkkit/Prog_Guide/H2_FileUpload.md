# <a name="目录">目录</a>
+ [功能说明](#功能说明)
+ [编译配置](#编译配置)
+ [例子程序讲解](#例子程序讲解)
    * [1. 与云端建立连接](#1. 与云端建立连接)
    * [2. 文件上传](#2. 文件上传)
    * [3. 断开连接](#3. 断开连接)
+ [功能API接口](#功能API接口)
    * [HTTP2建立连接](#HTTP2建立连接)
    * [文件上传请求](#文件上传请求)
    * [HTTP2断开连接](#HTTP2断开连接)
+ [需要对接的HAL接口](#需要对接的HAL接口)

# <a name="功能说明">功能说明</a>

SDK的文件上传功能使用HTTP2流式传输协议, 将文件上传至阿里云物联网平台服务器.

+ 支持多种上传模式, 如以创建文件的方式上传, 或以覆盖文件的方式上传
+ 支持指定上传长度, 并在下次上传时续传, 用户可在上传时根据网络带宽配置上传分配大小(`part_len`), 以提高带宽利用效率

# <a name="编译配置">编译配置</a>

1. 运行`make menuconfig`, 打开`FEATURE_HTTP2_COMM_ENABLED`选项使能HTTP2功能, 保存后退出
2. 运行`make`即可编译出包含HTTP2文件上传的固件

# <a name="例子程序讲解">例子程序讲解</a>

本节以 `src/http2/http2_example_uploadfile.c` 为例讲解如何使用文件上传功能

## <a name="1. 与云端建立连接">1. 与云端建立连接</a>

调用`IOT_HTTP2_UploadFile_Connect`建立HTTP2连接, 用户指定设备的三元组信息和服务器地址/端口号

        http2_upload_conn_info_t conn_info;
        void *handle;

        memset(&conn_info, 0, sizeof(http2_upload_conn_info_t));
        conn_info.product_key = HTTP2_PRODUCT_KEY;
        conn_info.device_name = HTTP2_DEVICE_NAME;
        conn_info.device_secret = HTTP2_DEVICE_SECRET;
        conn_info.url = HTTP2_ONLINE_SERVER_URL;
        conn_info.port = HTTP2_ONLINE_SERVER_PORT;

        handle = IOT_HTTP2_UploadFile_Connect(&conn_info, NULL);

        if(handle == NULL) {
            return -1;
        }

目前各个区域对应的域名和端口如下, 其中`*`符号应使用设备的`ProductKey`替换, 如`ProductKey`为`a1IgnOND7vI`时对应的URL/PORT如下:

        #define HTTP2_ONLINE_SERVER_URL             "a1IgnOND7vI.iot-as-http2.cn-shanghai.aliyuncs.com"
        #define HTTP2_ONLINE_SERVER_PORT            443

        *.iot-as-http2.cn-shanghai.aliyuncs.com:443          // 上海正式
        *.iot-as-http2.us-west-1.aliyuncs.com:443            // 美西正式
        *.iot-as-http2.us-east-1.aliyuncs.com:443            // 美东正式
        *.iot-as-http2.eu-central-1.aliyuncs.com:443         // 德国正式
        *.iot-as-http2.ap-southeast-1.aliyuncs.com:443       // 新加坡正式
        *.iot-as-http2.ap-northeast-1.aliyuncs.com:443       // 日本正式

如果用户关心网络状态, 可以注册相应的回调函数, 目前支持网络断开连接, 和网络重连成功两个回调函数

## <a name="2. 文件上传">2. 文件上传</a>

使用`IOT_HTTP2_UploadFile_Request`请求文件上传, 例程以`UPLOAD_FILE_OPT_BIT_OVERWRITE`的方式上传, 每次上传都会覆盖云端的文件. 此接口为异步接口, 用户可以插入多个上传请求到内部队列中

        http2_upload_params_t fs_params;
        http2_upload_result_cb_t result_cb;

        memset(&result_cb, 0, sizeof(http2_upload_result_cb_t));
        result_cb.upload_completed_cb = upload_file_result;
        result_cb.upload_id_received_cb = upload_id_received_handle;

        memset(&fs_params, 0, sizeof(fs_params));
        fs_params.file_path = argv[1];      /* 文件名称以命令行参数传入 */
        fs_params.opt_bit_map = UPLOAD_FILE_OPT_BIT_OVERWRITE;

        ret = IOT_HTTP2_UploadFile_Request(handle, &fs_params, &result_cb, NULL);
        if(ret < 0) {
            return -1;
        }

例程中注册了2个回调函数, 分别用于接收上传的结果, 和接收云端返回的上传标示符(`upload_id`). 在SDK调用了`upload_file_result`后, 文件上传操作便结束了, 用户可进行下一步操作

        void upload_file_result(const char *file_path, int result, void *user_data)
        {
            upload_end++;
            EXAMPLE_TRACE("=========== file_path = %s, result = %d, finish num = %d ===========", file_path, result, upload_end);
        }

        void upload_id_received_handle(const char *file_path, const char *upload_id, void *user_data)
        {
            EXAMPLE_TRACE("=========== file_path = %s, upload_id = %s ===========", file_path, upload_id);

            if (upload_id != NULL) {
                memcpy(g_upload_id, upload_id, strlen(upload_id));
            }
        }

在上传过程中我们可以在log中看到HTTP2的报文交互:
---
1. 设备端请求云端打开文件上传的通道:

        [inf] on_frame_send_callback(143): [INFO] C ---------> S (HEADERS) stream_id [1]
        [inf] on_frame_send_callback(145): > :method: POST
        [inf] on_frame_send_callback(145): > :path: /stream/open/c/iot/sys/thing/file/upload
        [inf] on_frame_send_callback(145): > :scheme: https
        [inf] on_frame_send_callback(145): > x-auth-name: devicename
        [inf] on_frame_send_callback(145): > x-auth-param-client-id: a1IgnOND7vI.H2_FS01
        [inf] on_frame_send_callback(145): > x-auth-param-signmethod: hmacsha1
        [inf] on_frame_send_callback(145): > x-auth-param-product-key: a1IgnOND7vI
        [inf] on_frame_send_callback(145): > x-auth-param-device-name: H2_FS01
        [inf] on_frame_send_callback(145): > x-auth-param-sign: 8d6b80749ed63823dc16b2c1e7f049bbdd00bf2b
        [inf] on_frame_send_callback(145): > x-sdk-version: 301
        [inf] on_frame_send_callback(145): > x-sdk-version-name: 3.0.1
        [inf] on_frame_send_callback(145): > x-sdk-platform: c
        [inf] on_frame_send_callback(145): > content-length: 0
        [inf] on_frame_send_callback(145): > x-file-name: upload1M
        [inf] on_frame_send_callback(145): > x-file-overwrite: 1
        [inf] on_begin_headers_callback(393): [INFO] C <--------- S (HEADERS) stream_id [1]
        [inf] on_header_callback(363): < :status: 200
        [inf] on_header_callback(363): < x-request-id: 1103919500797702144
        [inf] on_header_callback(363): < x-next-append-position: 0
        [inf] on_header_callback(363): < x-data-stream-id: DS1103919500889976832
        [inf] on_header_callback(363): < x-file-upload-id: ULDS1103919500889976832
        [inf] on_header_callback(363): < x-response-status: 200

2. 通道打开成功, 接收到云端返回的文件上传标示符并调用用户回调函数:

        upload_id_received_handle|037 :: =========== file_path = upload1M, upload_id = ULDS1103919500889976832 ===========

3. 通道打开成功后, 设备端通过HTTP2请求上传文件:

        [inf] on_frame_send_callback(143): [INFO] C ---------> S (HEADERS) stream_id [3]
        [inf] on_frame_send_callback(145): > :method: POST
        [inf] on_frame_send_callback(145): > :path: /stream/send/c/iot/sys/thing/file/upload
        [inf] on_frame_send_callback(145): > :scheme: https
        [inf] on_frame_send_callback(145): > content-length: 1048576
        [inf] on_frame_send_callback(145): > x-data-stream-id: DS1103919500889976832
        [inf] on_frame_send_callback(145): > x-sdk-version: 301
        [inf] on_frame_send_callback(145): > x-sdk-version-name: 3.0.1
        [inf] on_frame_send_callback(145): > x-sdk-platform: c
        [inf] on_frame_send_callback(145): > x-file-upload-id: ULDS1103919500889976832
        [dbg] http2_stream_node_search(168): stream node not exist, stream_id = 3
        [inf] send_callback(63): send_callback data len 10249, session->remote_window_size=16777215!
        [inf] send_callback(72): send_callback data ends len = 10249!
        [dbg] http2_stream_node_search(168): stream node not exist, stream_id = 3
        [inf] iotx_http2_client_send(563): nghttp2_session_send 0
        [dbg] _http2_fs_part_send_sync(250): send len = 10240
        [inf] send_callback(63): send_callback data len 10249, session->remote_window_size=16766975!
        [inf] send_callback(72): send_callback data ends len = 10249!
        [inf] iotx_http2_client_send(563): nghttp2_session_send 0
        [dbg] _http2_fs_part_send_sync(250): send len = 20480
        [inf] send_callback(63): send_callback data len 10249, session->remote_window_size=16756735!
        [inf] send_callback(72): send_callback data ends len = 10249!
        [inf] iotx_http2_client_send(563): nghttp2_session_send 0
        [dbg] _http2_fs_part_send_sync(250): send len = 30720
        [inf] send_callback(63): send_callback data len 10249, session->remote_window_size=16746495!
        [inf] send_callback(72): send_callback data ends len = 10249!
        [inf] iotx_http2_client_send(563): nghttp2_session_send 0
        [dbg] _http2_fs_part_send_sync(250): send len = 40960

4. 文件上传结束, 等待云端上传结构应答, 应答中的`x-next-append-position`便是已上传文件的大小

        [inf] on_frame_recv_callback(196): on_frame_recv_callback, type = 8
        [inf] on_frame_recv_callback(197): on_frame_recv_callback, stream_id = 3
        [inf] on_frame_recv_callback(205): stream user data is not exist
        [inf] on_begin_headers_callback(393): [INFO] C <--------- S (HEADERS) stream_id [3]
        [inf] on_header_callback(363): < :status: 200
        [inf] on_header_callback(363): < x-request-id: 1103919501166800896
        [inf] on_header_callback(363): < x-next-append-position: 1048576
        [inf] on_header_callback(363): < x-data-stream-id: DS1103919500889976832
        [inf] on_header_callback(363): < x-response-status: 200
        [inf] on_frame_recv_callback(196): [dbg] _http2_fs_part_send_sync(250): on_frame_recv_callback, type = 1
        [inf] on_frame_recv_callback(197): on_frame_recv_callback, stream_id = 3
        [inf] on_frame_recv_callback(205): send len = 1048576
        [inf] _http2_fs_node_handle(350): file offset = 1048576 now

5. 最后SDK会关闭文件上传通道:

        [inf] on_frame_send_callback(143): [INFO] C ---------> S (HEADERS) stream_id [5]
        [inf] on_frame_send_callback(145): > :method: POST
        [inf] on_frame_send_callback(145): > :path: /stream/close/c/iot/sys/thing/file/upload
        [inf] on_frame_send_callback(145): > :scheme: https
        [inf] on_frame_send_callback(145): > x-data-stream-id: DS1103919500889976832
        [inf] on_frame_send_callback(145): > x-sdk-version: 301
        [inf] on_frame_send_callback(145): > x-sdk-version-name: 3.0.1
        [inf] on_frame_send_callback(145): > x-sdk-platform: c
        [dbg] http2_stream_node_search(168): stream node not exist, stream_id = 5
        [inf] iotx_http2_client_send(563): nghttp2_session_send 0
        [inf] on_begin_headers_callback(393): [INFO] C <--------- S (HEADERS) stream_id [5]
        [inf] on_header_callback(363): < :status: 200
        [inf] on_header_callback(363): < x-request-id: 1103919502177628160
        [inf] on_header_callback(363): < x-data-stream-id: DS1103919500889976832
        [inf] on_header_callback(363): < x-file-crc64ecma: 6947770692288575170
        [inf] on_header_callback(363): < x-response-status: 200
        [inf] on_header_callback(363): < x-file-store-id: 101184

## <a name="3. 断开连接">3. 断开连接</a>

所有文件上传结束后使用`IOT_HTTP2_UploadFile_Disconnect`断开云端连接

        ret = IOT_HTTP2_UploadFile_Disconnect(handle);

# <a name="功能API接口">功能API接口</a>

`src/http2/http2_upload_api.h`列出了HTTP2文件上传的所有API和相关数据类型定义

`src/http2/http2_wrapper.h`列出了HTTP2所需的底层接口


## <a name="HTTP2建立连接">HTTP2建立连接</a>

接口原型
---

    void *IOT_HTTP2_UploadFile_Connect(http2_upload_conn_info_t *conn_info, http2_status_cb_t *cb);

接口说明
---
创建HTTP2连接, 并注册相关状态回调函数. 此接口为同步接口, 当建连成功后会返回HTTP2连接句柄. 否则返回NULL


参数说明
---
| 参数        | 数据类型                    | 方向    | 说明
|-------------|-----------------------------|---------|-----------------------------
| conn_info   | http2_upload_conn_info_t *  | 输入    | 设备连接信息
| cb          | http2_status_cb_t *         | 输入    | 设备状态回调函数结构体指针


返回值说明
---
| 值      | 说明
|---------|-------------
| 非NULL  | 建连成功
| NULL    | 建连失败

参数附加说明
---
```
typedef struct {
    char  *product_key;
    char  *device_name;
    char  *device_secret;
    char  *url;
    int   port;
} http2_upload_conn_info_t;
```

* product_key: 三元组之一, 产品标示符
* device_name: 三元组之一, 设备名称
* device_secret: 三元组之一, 识别秘钥
* url: 云端服务器地址
* port: 云端服务器端口

```
typedef struct {
    http2_disconnect_cb_t   on_disconnect_cb;
    http2_reconnect_cb_t    on_reconnect_cb;
} http2_status_cb_t;
```

* on_disconnect_cb: HTTP2断连回调函数
* on_reconnect_cb: HTTP2重连回调函数

## <a name="文件上传请求">文件上传请求</a>

接口原型
---

    int IOT_HTTP2_UploadFile_Request(void *http2_handle, http2_upload_params_t *params, http2_upload_result_cb_t *cb, void *user_data);

接口说明
---
按照指定参数配置上传文件, 并注册相关结果回调函数. 此接口为异步接口, 上传结果由回调函数返回

参数说明
---
| 参数            | 数据类型                    | 方向    | 说明
|-----------------|-----------------------------|---------|---------------------------------------------------------
| http2_handle    | void *                      | 输入    | 调用`IOT_HTTP2_UploadFile_Connect`建连成功后返回的句柄
| params          | http2_upload_params_t *     | 输入    | 上传参数结构体指针
| cb              | http2_upload_result_cb_t    | 输入    | 上传结构回调函数结构体指针
| user_data       | void *                      | 输入    | 用户数据

返回值说明
---
| 值      | 说明
|---------|-----------------
| 0       | 函数调用成功
| < 0     | 函数调用失败


```
typedef struct {
    const char *file_path;      /* file path, filename must be ASCII string and strlen < 2014 */
    uint32_t part_len;          /* maximum content len of one http2 request, must be in range of 100KB ~ 100MB */
    const char *upload_id;      /* a specific id used to indicate one upload session, only required when UPLOAD_FILE_OPT_BIT_RESUME option set */
    uint32_t upload_len;        /* used to indicate the upload length, only required when UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN option set */
    uint32_t opt_bit_map;       /* option bit map, support UPLOAD_FILE_OPT_BIT_OVERWRITE, UPLOAD_FILE_OPT_BIT_RESUME and UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN */
} http2_upload_params_t;
```

* file_path: 文件路径, 注意文件名必须为ASCII编码, 且不能使用数字开头
* part_len: 文件上传分片大小, 也即HTTP2请求content_len的最大长度, 必须在100KB ~ 100MB范围内, 否则会使用http2_config.h里的默认长度
* upload_id: 上传标示符, 由首次上传时返回. 在需要使用断点续传方式上传时需添加`opt_bit_map`参数`UPLOAD_FILE_OPT_BIT_RESUME`, 并指定对应上传标示符
* upload_len: 指定本次请求的长度, 仅在`opt_bit_map`参数中有配置`UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN`时才能起作用
* opt_bit_map: 位定义的选项表, 可以使用按位或的方式配置此选项

    + 如`opt_bit_map = UPLOAD_FILE_OPT_BIT_OVERWRITE | UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN `表示使用覆盖方式上传指定的文件长度

```
#define UPLOAD_FILE_OPT_BIT_OVERWRITE       (0x00000001)
#define UPLOAD_FILE_OPT_BIT_RESUME          (0x00000002)
#define UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN    (0x00000004)
```

* UPLOAD_FILE_OPT_BIT_OVERWRITE: 使用覆盖的方式上传文件. 如果云端文件已存在, 而未使用覆盖方式, 则文件上传会失败. 未使用此选项, 将使用创建文件的方式上传文件
* UPLOAD_FILE_OPT_BIT_RESUME: 使用断点续传的方式上传文件. 使用此选项需填写上传标示符参数(`upload_id`)
* UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN: 使用指定长度的方式上传. 使用此选项需要填写上传长度参数(`upload_len`). 否则无需填写`upload_len`, 将上传整个文件

```
typedef struct {
    http2_upload_id_received_cb_t   upload_id_received_cb;
    http2_upload_completed_cb_t     upload_completed_cb;
} http2_upload_result_cb_t;
```

* upload_id_received_cb: 接收到云端服务器返回的上传标示符时, 将调用此回调函数
* upload_completed_cb: 文件上传结束时, 将调用此回调函数, result参数指示了上传结果

## <a name="HTTP2断开连接">HTTP2断开连接</a>

接口原型
---

    int IOT_HTTP2_UploadFile_Disconnect(void *handle);

接口说明
---
断开参数handle指定的HTTP2连接

参数说明
---
| 参数            | 数据类型    | 方向    | 说明
|-----------------|-------------|---------|---------------------------------------------------------
| http2_handle    | void *      | 输入    | 调用`IOT_HTTP2_UploadFile_Connect`建连成功后返回的句柄

返回值说明
---
| 值      | 说明
|---------|-----------------
| 0       | 函数调用成功
| < 0     | 函数调用失败

# <a name="需要对接的HAL接口">需要对接的HAL接口</a>

文件`src/http2/http2_wrapper.h`中包含了用户对接HTTP2文件上传需要适配的部分HAL接口

| 函数名                                                                                                                                                      | 说明
|-------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------
| [HAL_SSL_Destroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_SSL_Destroy#HAL_SSL_Destroy)  | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能
| [HAL_SSL_Establish](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_SSL_Establish)              | 建立一个TLS连接, 用于MQTT功能, HTTPS功能
| [HAL_SSL_Read](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_SSL_Read)                        | 从一个TLS连接中读数据, 用于MQTT功能, HTTPS功能
| [HAL_SSL_Write](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_SSL_Write)                      | 向一个TLS连接中写数据, 用于MQTT功能, HTTPS功能
| [HAL_MutexCreate](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_MutexCreate)            | 创建一个互斥量对象
| [HAL_MutexDestroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_MutexDestroy)          | 销毁一个互斥量对象
| [HAL_MutexLock](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_MutexLock)                | 锁住一个互斥量
| [HAL_MutexUnlock](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_MutexUnlock)            | 解锁一个互斥量
| [HAL_SemaphoreCreate](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_SemaphoreCreate)    | 创建信号量
| [HAL_SemaphoreDestroy](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_SemaphoreDestroy)  | 销毁信号量
| [HAL_SemaphorePost](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_SemaphorePost)        | post信号量
| [HAL_SemaphoreWait](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_SemaphoreWait)        | 等待信号量
| [HAL_ThreadCreate](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_ThreadCreate)          | 创建线程
| [HAL_ThreadDelete](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_ThreadDelete)          | 销毁线程
| [HAL_ThreadDetach](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/MultiThread_Requires#HAL_ThreadDetach)          | 分离线程
| [HAL_Fopen](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_Fopen)                              | 打开文件
| [HAL_Fread](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_Fread)                              | 读取文件数据
| [HAL_Fwrite](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_Fwrite)                            | 向文件写入数据
| [HAL_Fseek](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_Fseek)                              | 设置文件指针stream的位置
| [HAL_Fclose](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_Fclose)                            | 关闭文件
| [HAL_Ftell](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/HTTP2_Requires#HAL_Ftell)                              | 得到文件位置指针当前位置相对于文件首的偏移字节数
| [HAL_Printf](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Printf)                            | 打印函数
| [HAL_SleepMs](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_SleepMs)                          | 睡眠函数
| [HAL_Malloc](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Malloc)                            | 内存分配
| [HAL_Free](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HAL/Basic_Requires#HAL_Free)                                | 内存释放

