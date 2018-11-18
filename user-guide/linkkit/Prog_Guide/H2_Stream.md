# <a name="目录">目录</a>
+ [例程讲解](#例程讲解)
    * [流传输功能](#流传输功能)
    * [文件上传功能](#文件上传功能)
+ [H2 Stream功能API](#H2 Stream功能API)
+ [需要实现的HAL接口](#需要实现的HAL接口)

+ [例程讲解](#例程讲解)
+ [H2 Stream功能API](#H2 Stream功能API)
+ [需要实现的HAL接口](#需要实现的HAL接口)

# <a name="例程讲解">例程讲解</a>

注意: http2流传输功不是IOT平台固有能力, 需要业务端服务器接入CMP平台实现内容转存等相关业务, 因此不是所有的三元组都可以用来演示此功能

## <a name="流传输功能">流传输功能</a>

> 现对照 `examples/http2/http2_example_stream.c` 例程分步骤讲解如何使用相关API实现http2 流传输功能

1.http2建连.导入设备三元组,url及port(其中url和port可以传空，使用内部默认url)建立连接
---
```
    #define HTTP2_SERVER_URL                  ""//"10.101.12.205"
	#define HTTP2_SERVER_PORT             9999
	#define HTTP2_PRODUCT_KEY             "a1L5EUOh21s"
	#define HTTP2_DEVICE_NAME             "xxxxxxx"
	#define HTTP2_DEVICE_SECRET           "xxxxxxxxxxxxxxxxxxxxxxxx"

    device_conn_info_t conn_info;
    void *handle;
    memset(&conn_info, 0, sizeof( device_conn_info_t));
    conn_info.product_key = HTTP2_PRODUCT_KEY;
    conn_info.device_name = HTTP2_DEVICE_NAME;
    conn_info.device_secret = HTTP2_DEVICE_SECRET;
    conn_info.url = HTTP2_SERVER_URL;
    conn_info.port = HTTP2_SERVER_PORT;

    handle = IOT_HTTP2_Connect(&conn_info,&my_cb);
    if(handle == NULL) {
        return -1;
    }
```

2.配置流相关信息,打开2个业务流,一个用于数据上传,一个用于数据下载
---
```
    header_ext_info_t my_header_info = {
        {
            MAKE_HEADER("test_name", "test_http2_header"),
            MAKE_HEADER_CS("hello", "world"),
        },
        2
    };
    stream_data_info_t info_upload, info_download;
    memset(&info_upload,0,sizeof(stream_data_info_t));

    info_upload.identify = "iotx/vision/voice/intercom/live";

    memset(&info_download, 0, sizeof(stream_data_info_t));
    info_download.identify = "iotx/vision/voice/intercom/live";

    ret = IOT_HTTP2_Stream_Open(handle, &info_download, &my_header_info);
    if (ret < 0) {
        EXAMPLE_TRACE("=========iotx_http2_downstream_open failed %d!!!!!\n", ret);
        IOT_HTTP2_Disconnect(handle);
        return -1;
    }

    ret = IOT_HTTP2_Stream_Open(handle, &info_upload, &my_header_info);
    if(ret < 0) {
        EXAMPLE_TRACE("=========iotx_http2_upstream_open failed %d!!!!!\n", ret);
        IOT_HTTP2_Disconnect(handle);
        return -1;
    }
```

3.使能流下载, 在此流业务被关闭前, 服务器有数据时将自动下推数据到客户端
---
```
    ret = IOT_HTTP2_Stream_Query(handle, &info_download, &my_header_info);
    if (ret < 0) {
        EXAMPLE_TRACE("=========iotx_http2_downstream_query failed %d!!!!!\n", ret);
        IOT_HTTP2_Disconnect(handle);
        return -1;
    }
```

4.数据分批上传到服务端
---
```
    info_upload.stream = (char *)UPLOAD_STRING;
    info_upload.stream_len= sizeof(UPLOAD_STRING);
    info_upload.send_len = 0;
    info_upload.packet_len=2048;

    while(info_upload.send_len<info_upload.stream_len) {
        info_upload.stream = (char *)UPLOAD_STRING + info_upload.send_len;
        if(info_upload.stream_len-info_upload.send_len<info_upload.packet_len) {
            info_upload.packet_len = info_upload.stream_len-info_upload.send_len;
        }
        ret = IOT_HTTP2_Stream_Send(handle, &info_upload, NULL);
        if(ret <0 ) {
            EXAMPLE_TRACE("send err, ret = %d\n",ret);
            break;
        }
        EXAMPLE_TRACE("iotx_http2_stream_send info_upload.send_len =%d ret = %d\n", info_upload.send_len,ret);
    }
```

5.流操作完成后, 关闭相关流
---
```
    IOT_HTTP2_Stream_Close(handle, &info_upload);
    IOT_HTTP2_Stream_Close(handle, &info_download);
```

6.业务完成后断开h2连接
---
```
    ret = IOT_HTTP2_Disconnect(handle);
```

## <a name="文件上传功能">文件上传功能</a>

> 现对照 `examples/http2/http2_example_uploadfile.c` 例程分步骤讲解如何使用相关API实现http2 流传输功能

1.http2建连.导入设备三元组,url及port, 建立连接,与流传输功能步骤1相同
---

2.准备文件上传异步通知回调函数, 主要用来告知用户文件上传完成或失败
---
```
    static int upload_end = 0;
    void upload_file_result(const char * path,int result, void * user_data)
    {
        upload_end ++;
        EXAMPLE_TRACE("===========path = %s,result =%d,finish num =%d=========", path,result,upload_end);

    }
```

3.如果需要自定义header, 准备自定义头部
---
```
   http2_header header[] = {
        MAKE_HEADER("test_name", "test_http2_header"),
        MAKE_HEADER_CS("hello", "world"),
    };

    header_ext_info_t my_header_info = {
        header,
        2
    };

```

4.调用文件上传接口直接上传文件
---
```
    int goal_num = 0;
    ret = IOT_HTTP2_Stream_UploadFile(handle,"test1.zip","iotx/vision/voice/intercom/live",&my_header_info,
                                    upload_file_result, NULL);
    if(ret == 0) {
        goal_num++;
    }

    ret = IOT_HTTP2_Stream_UploadFile(handle,"test2.avi","iotx/vision/voice/intercom/live",&my_header_info,
                                    upload_file_result, NULL);
    if(ret == 0) {
        goal_num++;
    }
```

5.等待文件传输完成后, 断开连接
---
```
    while(upload_end != goal_num) {
        HAL_SleepMs(200);
    }
    ret = IOT_HTTP2_Disconnect(handle);
```


# <a name="H2 Stream功能API">H2 Stream功能API</a>

| 函数名                                                      | 说明
|-------------------------------------------------------------|---------------------------------------------------------
| [IOT_HTTP2_Connect](#IOT_HTTP2_Connect)       | 使用指定参数与IOT平台建立http2连接
| [IOT_HTTP2_Stream_Open](#IOT_HTTP2_Stream_Open)             | HTTP2会话阶段,使用指定参数打开一个业务流,并返回业务流唯一标识-channel_id
| [IOT_HTTP2_Stream_Send](#IOT_HTTP2_Stream_Send)             | HTTP2会话阶段,向指定业务流发送数据
| [IOT_HTTP2_Stream_Query](#IOT_HTTP2_Stream_Query)           | HTTP2会话阶段,向云端查询下行数据, 一个业务流, 仅需调用一次
| [IOT_HTTP2_Stream_Close](#IOT_HTTP2_Stream_Close)           | HTTP2会话阶段,关闭指定业务流
| [IOT_HTTP2_Disconnect](#IOT_HTTP2_Disconnect)               | HTTP2会话阶段,关闭http2连接
| [IOT_HTTP2_Stream_UploadFile](#IOT_HTTP2_Stream_UploadFile) | HTTP2会话阶段,异步上传文件, 文件上传成功或失败会通过回调函数通知结果





# <a name="需要实现的HAL接口">需要实现的HAL接口</a>
**以下函数为可选实现, 如果希望SDK提供http2 流功能, 则需要用户对接前8个, 如果需要实现文件传输功能需要额外实现后6个**

| 函数名                                      | 说明
|---------------------------------------------|-------------------------------------------------------------------------
| [HAL_SSL_Destroy](#HAL_SSL_Destroy)         | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能
| [HAL_SSL_Establish](#HAL_SSL_Establish)     | 建立一个TLS连接, 用于MQTT功能, HTTPS功能
| [HAL_SSL_Read](#HAL_SSL_Read)               | 从一个TLS连接中读数据, 用于MQTT功能, HTTPS功能
| [HAL_SSL_Write](#HAL_SSL_Write)             | 向一个TLS连接中写数据, 用于MQTT功能, HTTPS功能
| [HAL_TCP_Destroy](#HAL_TCP_Destroy)         | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能
| [HAL_TCP_Establish](#HAL_TCP_Establish)     | 建立一个TCP连接, 包含了域名解析的动作和TCP连接的建立
| [HAL_TCP_Read](#HAL_TCP_Read)               | 在指定时间内, 从TCP连接读取流数据, 并返回读到的字节数
| [HAL_TCP_Write](#HAL_TCP_Write)             | 在指定时间内, 向TCP连接发送流数据, 并返回发送的字节数
| [HAL_Fopen](#HAL_Fopen)                     | 打开文件
| [HAL_Fread](#HAL_Fread)                     | 读取文件数据
| [HAL_Fwrite](#HAL_Fwrite)                   | 向文件写入数据
| [HAL_Fseek](#HAL_Fseek)                     | 设置文件指针stream的位置
| [HAL_Fclose](#HAL_Fclose)                   | 关闭文件
| [HAL_Ftell](#HAL_Ftell)                     | 得到文件位置指针当前位置相对于文件首的偏移字节数