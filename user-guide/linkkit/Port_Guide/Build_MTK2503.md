# <a name="目录">目录</a>
+ [MTK2503/MTK6261 + Nucleus 移植示例](#MTK2503/MTK6261 + Nucleus 移植示例)
    * [配置SDK并提取所需文件](#配置SDK并提取所需文件)
    * [对接HAL接口](#对接HAL接口)
    * [将被抽取的文件和实现好的 `wrapper.c` 加入构建](#将被抽取的文件和实现好的 `wrapper.c` 加入构建)
    * [参考如下源码编写应用调用C-SDK](#参考如下源码编写应用调用C-SDK)
    * [例程讲解](#例程讲解)
    * [烧录固件](#烧录固件)
    * [观察运行效果](#观察运行效果)

# <a name="MTK2503/MTK6261 + Nucleus 移植示例">MTK2503/MTK6261 + Nucleus 移植示例</a>

> MTK2503/MTK6261芯片 + Nucleus操作系统是当前 2G 通信设备广为采用的一种嵌入式软硬件平台, 由于其采用异步的TCP/IP协议栈, 过去 `V3.0.1` 版本以前的C-SDK一直难以移植上去

在阅读本文之前, 请务必先看文档: [异步通知式底层通信模型](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/AsyncMQTT_Prog), 了解C-SDK从 `V3.0.1` 版本起为这种移植做出的改造
---

> 本文开发环境: `Windows XP`开发主机及MTK的USB转串口驱动, `ARM RVCT3.1`/`Perl`构建环境, `FlashTool`烧录工具, `Catcher`调试工具

## <a name="配置SDK并提取所需文件">配置SDK并提取所需文件</a>

运行顶级目录下的 `config.bat` 脚本, 双击打开, 在出现的界面首先关闭 `FEATURE_DEVICE_MODEL_ENABLED`, 然后进入 `MQTT Configuations` 子菜单

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_extract_step1.png" width="1000" height="500" />

前面已经介绍过, `FEATURE_ASYNC_PROTOCOL_STACK` 是专为这种系统开发的特性, 所以这里选中它, 打开

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_extract_step2.png" width="900" height="300" />

保存后退出, 运行顶级目录下的 `extract.bat` 脚本, 根据这种配置抽取移植所需要的源文件到 `output` 目录下

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_extract_step3.png" width="800" height="600" />

## <a name="对接HAL接口">对接HAL接口</a>

上图中可以看到 `extract.bat` 已经将需要对接的HAL接口列出并自动整理到 `output/eng/wrappers/wrapper.c`, 用户现在要根据系统的情况将这个文件中的空函数都填好实现

*可以直接使用SDK源码目录 `wrappers/os/nucleus` 中的参考代码, 根据自己的实际需要做一些调整*

## <a name="将被抽取的文件和实现好的 `wrapper.c` 加入构建">将被抽取的文件和实现好的 `wrapper.c` 加入构建</a>

拷贝 `output/eng` 下所有目录及文件到 `Nucleus` 的编译环境中, 确保编译通过, 可以将C-SDK作为一个库集成到原有系统中

## <a name="参考如下源码编写应用调用C-SDK">参考如下源码编写应用调用C-SDK</a>

以下是我们开发过程中经过实际验证的源文件, 您可参考编写自己的应用程序调用被集成的C-SDK

+ [aliMqttSap.h](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/aliMqttSap.h)
+ [aliMqttTask.h](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/aliMqttTask.h)
+ [aliMqttTask.c](https://code.aliyun.com/edward.yangx/public-docs/blob/master/docs/aliMqttTask.c)

## <a name="例程讲解">例程讲解</a>

> 以下对示例的 `aliMqttTask.c` 运行流程做一些讲解

监听系统事件, 构造应用程序入口
---
监听 `Nucleus` 中系统事件 `EVT_ID_SRV_NW_INFO_SERVICE_AVAILABILITY_CHANGED`, 当GPRS连接成功, 系统会触发该事件

    /* 系统事件回调函数 */
    mmi_ret srv_nw_name_main_evt_hdlr(mmi_event_struct *event)
    {
        ...
    switch (event->evt_id)
        ...
        case EVT_ID_SRV_NW_INFO_SERVICE_AVAILABILITY_CHANGED:
            ret = srv_nw_name_handle_service_availability_changed(event);
        ...

把事件传递给状态机处理函数: `aliyun_network_changed()`

    static mmi_ret srv_nw_name_handle_service_availability_changed(
    mmi_event_struct *event)
    {
        ...
        #if defined(__LL_ALIYUN_SUPPORT__)
        /* 进入业务回调函数，处理系统事件 */
        aliyun_network_changed((kal_int32)evt->new_status);
        #endif /*__LL_MYCOMMON_SUPPORT__*/
        ...
    }

如果 `aliyun_network_changed()` 发现事件是表达 `GPRS` 连接已就绪, 则启动应用程序 `ali_app_start_fun()`

    #define ALIYUN_NETWORK_GPRS_OK 3
    void aliyun_network_changed(kal_int8 status)
    {
        ...
        /* srv_nw_info_location_info_struct info; */
        DEBUG_TRACE("service_changedstatus.status=%d",status);

        g_gprs_status = status;
        /* 该事件表示GPRS连接成功 */
        if (g_gprs_status == ALIYUN_NETWORK_GPRS_OK) 
        {
            if(g_sim_init_complete == KAL_FALSE)
            {
                g_sim_init_complete = KAL_TRUE;
                StopTimer(ALIYUN_START_TIMER);
                /* 这里开始执行业务逻辑 */
                StartTimer(ALIYUN_START_TIMER,1000*20,ali_app_start_fun);

            }
        }
        ...
    }

同时, 我们也构造了一个专用于MQTT通信的任务: `mqtt_task_main()`

    kal_bool mqtt_create(comptask_handler_struct **handle)
    {
        static const comptask_handler_struct mqtt_handler_info =
        {   
            mqtt_task_main,  /* task entry function */
            mqtt_task_init,  /* task initialization function */
            NULL,           /* task configuration function */
            NULL, /* task reset handler */
            NULL,           /* task termination handler */
        };  

        *handle = (comptask_handler_struct*)&mqtt_handler_info;

        return KAL_TRUE;
    }

GPRS连接成功, 计算签名信息, 并解析签名中的服务器域名为IP
---
`ali_app_start_fun()` 首先会调用SDK提供的接口 `IOT_MQTT_Sign()` 获取签名信息, 其中含有服务器域名

    static void ali_app_start_fun(void)
    {
        uint32_t res = 0;
        iotx_dev_meta_info_t meta;
        ...
        res = IOT_Sign_MQTT(IOTX_CLOUD_REGION_SHANGHAI,&meta,&g_mqtt_signout);
        if (res == 0) {
            DEBUG_TRACE("signout.hostname: %s",g_mqtt_signout.hostname);
            DEBUG_TRACE("signout.port    : %d",g_mqtt_signout.port);
            DEBUG_TRACE("signout.clientid: %s",g_mqtt_signout.clientid);
            DEBUG_TRACE("signout.username: %s",g_mqtt_signout.username);
            DEBUG_TRACE("signout.password: %s",g_mqtt_signout.password);

用 `Nucleus` 的系统接口 `soc_gethostbyname()` 把域名解析成IP地址

            ...
            ...
            res = soc_gethostbyname(KAL_FALSE,MOD_MQTT,g_ali_request_id,g_mqtt_signout.hostname,
                     (kal_uint8 *)addr_buff, &addr_len,0,g_ali_nwk_account_id);

若域名成功解析, 则调用SDK提供的接口 `IOT_MQTT_Construct()` 发起TCP连接
---
            if (res >= SOC_SUCCESS)             // success
            {   
                DEBUG_TRACE("ali task socket_host_by_name SOC_SUCCESS");
                if(addr_len !=0 && addr_len<MAX_SOCK_ADDR_LEN)
                {   
                    //connect....
                    sprintf(ipaddr,"%d.%d.%d.%d",addr_buff[0],addr_buff[1],addr_buff[2],addr_buff[3]);
                    DEBUG_TRACE("ipaddr: %s[%d.%d.%d.%d]",ipaddr,addr_buff[0],addr_buff[1],addr_buff[2],addr_buff[3]);
                    HAL_Free(g_mqtt_signout.hostname);
                    g_mqtt_signout.hostname = HAL_Malloc(strlen(ipaddr) + 1);
                    if (g_mqtt_signout.hostname == NULL) {
                        DEBUG_TRACE("HAL_Malloc failed");
                        return;
                    }
                    memset(g_mqtt_signout.hostname,0,strlen(ipaddr) + 1);
                    memcpy(g_mqtt_signout.hostname,ipaddr,strlen(ipaddr));

                    DEBUG_TRACE("g_mqtt_signout.hostname: %s",g_mqtt_signout.hostname);

                    memset(&mqtt_params,0,sizeof(iotx_mqtt_param_t));

                    mqtt_params.port = g_mqtt_signout.port;
                    mqtt_params.host = g_mqtt_signout.hostname;
                    mqtt_params.client_id = g_mqtt_signout.clientid;
                    mqtt_params.username = g_mqtt_signout.username;
                    mqtt_params.password = g_mqtt_signout.password;

                    mqtt_params.request_timeout_ms = 2000;
                    mqtt_params.clean_session = 0;
                    mqtt_params.keepalive_interval_ms = 60000;
                    mqtt_params.read_buf_size = 1024;
                    mqtt_params.write_buf_size = 1024;

                    mqtt_params.handle_event.h_fp = example_event_handle;
                    mqtt_params.handle_event.pcontext = NULL;

                    g_mqtt_handle = IOT_MQTT_Construct(&mqtt_params);
                    if (g_mqtt_handle != NULL){
                        g_mqtt_connect_status = 1;
                        DEBUG_TRACE("IOT_MQTT_Construct Success");
                    }
                }
            }

比较特别的是

+ 在异步模式下, 如果 `IOT_MQTT_Construct()` 返回成功, 仅表示socket连接函数调用成功, 而不代表MQTT长连接已经建立
+ 调用结果是通过 `MSG_ID_APP_SOC_NOTIFY_IND` 事件返回的, 由上面提到的 `mqtt_task_main()` 任务处理

        void mqtt_task_main(task_entry_struct *task_entry_ptr)
        {
            ...
            while(1)
            {
                receive_msg_ext_q( task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm);
                stack_set_active_module_id(my_index, current_ilm.dest_mod_id);
                switch (current_ilm.msg_id)
                {
                    case MSG_ID_APP_SOC_NOTIFY_IND:
                    {
                        ERROR_TRACE("%s,%d", __FUNCTION__,__LINE__);
                        ali_socket_notify(current_ilm.local_para_ptr);
                        break;
                    }

TCP连接建立成功时, 调用SDK提供的接口 `IOT_MQTT_Nwk_Event_Handler()` 建立MQTT连接
---
TCP建连成功时, Nucleus系统会产生 `SOC_CONNECT` 事件, 我们编写了 `ali_socket_notify()` 函数包装 `IOT_MQTT_Nwk_Event_Handler()`

    void ali_socket_notify(void *msg_ptr)
    {
        ...
        switch (soc_notify->event_type)
        {
            ...
            case SOC_CONNECT:
            {
                DEBUG_TRACE("Ali SOC_CONNECT Event");
                ret = IOT_MQTT_Nwk_Event_Handler(g_mqtt_handle, IOTX_MQTT_SOC_CONNECTED  ,&nwk_param);
                if (ret == SUCCESS_RETURN) {
                    g_mqtt_connect_status = 2;
                    DEBUG_TRACE("Aliyun connect success,start yield");
                    example_subscribe(g_mqtt_handle);
                    StopTimer(ALIYUN_YIELD_TIMER);
                    StartTimer(ALIYUN_YIELD_TIMER,1000*2,ali_mqtt_yield);
                }
            }

以上的代码除了通过 `IOTX_MQTT_SOC_CONNECTED` 事件驱动 `IOT_MQTT_Nwk_Event_Handler()` 建立MQTT连接, 也订阅了Topic, 并启动了定时器 `ali_mqtt_yield()` 来维持心跳

启动心跳定时器, 调用SDK提供的接口 `IOT_MQTT_Yield()` 维持长连接
---

    void ali_mqtt_yield(void)
    {
        static int send_interval = 0;
        DEBUG_TRACE("ali_mqtt_yield...");
        IOT_MQTT_Yield(g_mqtt_handle,200);

        send_interval+=5;
        if (send_interval == 20) {
            send_interval = 0;
            example_publish(g_mqtt_handle);
        }
        StartTimer(ALIYUN_YIELD_TIMER,1000*5,ali_mqtt_yield);
    }

在异步模式下, `IOT_MQTT_Yield()` 已经变得 **只发不收** 了, 它只会起到维持心跳的作用

在心跳的定时器中, 调用SDK提供的接口 `IOT_MQTT_Publish()` 发出上行的报文消息给自己
---

    int example_publish(void *handle)
    {
        int res = 0;
        iotx_mqtt_topic_info_t topic_msg;
        char product_key[IOTX_PRODUCT_KEY_LEN] = {0};
        char device_name[IOTX_DEVICE_NAME_LEN] = {0};
        const char *fmt = "/%s/%s/get";
        char topic[128] = {0};
        char *payload = "hello,world";

        HAL_GetProductKey(product_key);
        HAL_GetDeviceName(device_name);

        HAL_Snprintf(topic, 128, fmt, product_key, device_name);

        memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
        topic_msg.qos = IOTX_MQTT_QOS0;
        topic_msg.retain = 0;
        topic_msg.dup = 0;
        topic_msg.payload = (void *)payload;
        topic_msg.payload_len = strlen(payload);

        res = IOT_MQTT_Publish(handle, topic, &topic_msg);
        if (res < 0) {
            DEBUG_TRACE("publish failed\n");
            return -1;
        }
        return 0;
    }

由于例程所使用的设备是特别在控制台配置过的, 它的 `/${productKey}/${deviceName}/get` 可订阅也可发布, 我们前面建连成功时已经订阅了它, 现在又向它发送报文, 因此可以看到

通过协议栈底层驱动, 接收被订阅Topic上的报文
---
这仍然是通过包装了 `IOT_MQTT_Nwk_Event_Handler()` 的 `ali_socket_notify()` 函数来处理的

    void ali_socket_notify(void *msg_ptr)
    {
        ...
        switch (soc_notify->event_type)
        {
            case SOC_READ:
            {
                DEBUG_TRACE("Ali SOC_READ Event");
                IOT_MQTT_Nwk_Event_Handler(g_mqtt_handle,IOTX_MQTT_SOC_READ,&nwk_param);
                DEBUG_TRACE("Ali SOC_READ End");
            }
            break;

当有数据可读的时候, Nucleus的协议栈会以 `SOC_READ` 事件通知, 在此如果不想做其它处理, 直接用 `IOT_MQTT_Nwk_Event_Handler()` 交给SDK, 即可分发给当初订阅相应Topic时注册的函数处理

## <a name="烧录固件">烧录固件</a>

在本示例程序的开发环境中, 编译固件的环境是

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_build_image_step1.png" width="600" height="400" />

打开 `cmd.exe` 命令行, 通过MTK提供的编译脚本启动固件的构建过程

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_build_image_step2.png" width="600" height="50" />

然后将编译出来的固件用 `FlashTool` 工具烧录到设备上

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_build_image_step3.png" width="800" height="600" />

+ 选择 `config file`, 在 `build` 目录下后缀 `.cfg`结尾文件
+ 选择 `option` 设置, 第一次烧录或有异常问题时, 勾选 `Format FAT`
+ 点击 `download`, 将设备的USB口连接电脑即开始下载

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_build_image_step3.png" width="800" height="700" />

## <a name="观察运行效果">观察运行效果</a>

打开 `Catcher` 调试工具, 由于 `MOD_NIL` 标记下的日志较少, 例程的日志打印到 `MOD_NIL` 这个filter上, 对其筛选可查看例程输出的日志

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_catcher_log1.png" width="1000" height="200" />

用串口工具操作GPRS设备连网成功, 例程则开始工作, 可在 `Catcher` 中观察到它建立MQTT连接, 订阅, 发布和接收到报文过程中的日志

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_catcher_log2.png" width="1200" height="300" />

同时若登录到IoT云端控制台(https://iot.console.aliyun.com), 也可以看到对应的设备上线, 并看到它发上来的报文, 表明接入成功

<img src="https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/mtk2503_catcher_log3.png" width="1000" height="200" />

