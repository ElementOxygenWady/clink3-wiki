# <a name="目录">目录</a>
+ [功能说明](#功能说明)
+ [例子程序讲解](#例子程序讲解)
+ [功能API接口](#功能API接口)
+ [需要对接的HAL接口](#需要对接的HAL接口)

# <a name="功能说明">功能说明</a>

设备发送消息重置云端状态, 目前包括清除topo关系(如果是子设备会踢出下线), 解除用户和设备绑定关系, 其它运行时数据不会被清除

# <a name="例子程序讲解">例子程序讲解</a>

下面的例子中使用了示例代码 `src/dev_reset/examples/dev_reset_example.c`

用户通过 `IOT_MQTT_Construct` 连接云端成功之后, 可使用 `IOT_DevReset_Report` 接口发送消息到云端, 对设备进行重置操作

    int main(int argc, char *argv[])
    {
        ...
        ...
        pclient = IOT_MQTT_Construct(&mqtt_params);
        if (NULL == pclient) {
            EXAMPLE_TRACE("MQTT construct failed");
            return -1;
        }

        res = IOT_DevReset_Report(&meta_info, example_devrst_evt_handle, NULL);
        if (res < 0) {
            return -1;
        }
        ...
        ...
    }


用户可填写 `IOT_DevReset_Report` 接口的第二个参数进行回调函数的注册, 当重置消息发送成功时, 会从该回调函数得到云端的应答消息


    void example_devrst_evt_handle(iotx_devrst_evt_type_t evt, void *msg)
    {
        switch (evt)
        {
            case IOTX_DEVRST_EVT_RECEIVED: {
                iotx_devrst_evt_recv_msg_t *recv_msg = (iotx_devrst_evt_recv_msg_t *)msg;
                if (recv_msg->msgid != reset_mqtt_packet_id) {
                    return;
                }
                EXAMPLE_TRACE("Receive Reset Responst");
                EXAMPLE_TRACE("Msg ID: %d", recv_msg->msgid);
                EXAMPLE_TRACE("Payload: %.*s", recv_msg->payload_len, recv_msg->payload);
                reset_reply_received = 1;
            }
            break;

            default:
                break;
        }
    }


# <a name="功能API接口">功能API接口</a>

原型
---

    int IOT_DevReset_Report(iotx_dev_meta_info_t *meta_info, iotx_devrst_evt_handle_t handle, void *extended);

接口说明
---
用于向云端上报设备重置消息, 执行结果通过用户注册的回调函数参数 `handle` 告知用户

参数说明
---

| 参数        | 数据类型                        | 方向    | 说明
|-------------|---------------------------------|---------|-------------------------
| meta_info   | iotx_linkkit_dev_meta_info_t    | 输入    | 设备的四元组信息
| handle      | iotx_devrst_evt_handle_t        | 输入    | 设备重置消息的回调函数
| extended    | void *                          | 输入    | 扩展参数, 保留

返回值说明
---
| 值      | 说明
|---------|-----------------
| >= 0    | 消息发送成功
| < 0     | 失败


# <a name="需要对接的HAL接口">需要对接的HAL接口</a>
| 函数名          | 说明
|-----------------|-----------------------------
| HAL_Snprintf    | 标准库函数snprintf的HAL实现


