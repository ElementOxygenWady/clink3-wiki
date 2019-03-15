# <a name="目录">目录</a>
+ [WIFI provisoion API详解](#WIFI provisoion API详解)
    * [awss_start](#awss_start)
    * [awss_stop](#awss_stop)
    * [awss_config_press](#awss_config_press)
    * [awss_dev_ap_start](#awss_dev_ap_start)
    * [awss_dev_ap_stop](#awss_dev_ap_stop)
    * [awss_report_cloud](#awss_report_cloud)
    * [awss_report_reset](#awss_report_reset)

# <a name="WIFI provisoion API详解">WIFI provisoion API详解</a>

## <a name="awss_start">awss_start</a>

原型
---
```
int awss_start(void);
```

接口说明
---
启动配网服务

参数说明
---
void

返回值说明
---
| 值  | 说明
|-----|-------------
| 0   | 成功
| -1  | 启动失败

-----

## <a name="awss_stop">awss_stop</a>

原型
---
```
int awss_stop();
```

接口说明
---
停止配网服务

参数说明
---
void

返回值说明
---
| 值  | 说明
|-----|---------
| 0   | 成功
| -1  | 失败

-----

## <a name="awss_config_press">awss_config_press</a>

原型
---
```
int awss_config_press();
```

接口说明
---
使能配网，开始解awss报文

参数说明
---
void

返回值说明
---
| 值  | 说明
|-----|---------
| 0   | 成功
| -1  | 失败


-----

## <a name="awss_dev_ap_start">awss_dev_ap_start</a>

原型
---
```
int awss_dev_ap_start();
```

接口说明
---
启动设备热点配网，与awss_start互斥

参数说明
---
void

返回值说明
---
| 值  | 说明
|-----|---------
| 0   | 成功
| -1  | 失败

-----

## <a name="awss_dev_ap_stop">awss_dev_ap_stop</a>

原型
---
```
int awss_dev_ap_stop();
```

接口说明
---
停止设备热点配网

参数说明
---
void 

返回值说明
---
| 值  | 说明
|-----|---------
| 0   | 成功
| -1  | 失败

-----

## <a name="awss_report_cloud">awss_report_cloud</a>

原型
---
```
int awss_report_cloud();
```

接口说明
---
启动绑定服务，上报绑定信息

参数说明
---
void

返回值说明
---
| 值  | 说明
|-----|---------
| 0   | 成功
| -1  | 失败

-----

## <a name="awss_report_reset">awss_report_reset</a>

原型
---
```
int awss_report_reset();
```

接口说明
---
向云端上报解除绑定消息

参数说明
---
void

返回值说明
---
| 值  | 说明
|-----|---------
| 0   | 成功
| -1  | 失败

-----

