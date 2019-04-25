# <a name="目录">目录</a>
+ [Linux实现设备一键配网的参考实现](#Linux实现设备一键配网的参考实现)
        - [下载sdk源码](#下载sdk源码)
        - [config选中配网功能](#config选中配网功能)
        - [运行 extract.sh](#运行 extract.sh)
        - [修改output/Makefile](#修改output/Makefile)
        - [填写 output/eng/wrappers/wapper.c](#填写 output/eng/wrappers/wapper.c)
        - [修改example，加入配网功能](#修改example，加入配网功能)
        - [在 output 目录下编译](#在 output 目录下编译)
        - [如何将编译出来的demo程序上传到板子上](#如何将编译出来的demo程序上传到板子上)
        - [如何启动demo](#如何启动demo)
        - [如何在app进行操作](#如何在app进行操作)
        - [配网日志逐段说明  ](#配网日志逐段说明  )
        - [对每个阶段app上是什么样子附加说明](#对每个阶段app上是什么样子附加说明)

# <a name="Linux实现设备一键配网的参考实现">Linux实现设备一键配网的参考实现</a>

- 下载sdk源码
- config选中配网功能
- 运行 extract.sh
- 填写 output/eng/wrappers/wapper.c
- 在 output 目录下 make
- 使用编译出来的 .a 静态库或者直接使用 make prog 产生的应用程序得到demo
- 如何将demo上传到板子上
- 如何启动demo
- 如何在app进行操作
- 配网日志逐段说明
  - 哪些说明得到了ssid+pwd
  - 哪些说明连ap成功
- 对每个阶段app上是什么样子附加说明



本文就如何在捷高摄像头(Linux)的环境里面实现一键配网需要的HAL提供一个参考实现



### <a name="下载sdk源码">下载sdk源码</a>
https://code.aliyun.com/linkkit/c-sdk/repository/archive.zip?ref=v3.0.1

### <a name="config选中配网功能">config选中配网功能</a>
通过menuconfig选择配网模块并选定一键配网:
```
make distclean

make menuconfig
```

使能配网,选择一键配网，关闭日志开关的[操作步骤](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/1556076897987.gif)

### <a name="运行 extract.sh">运行 extract.sh</a>
运行./extract.sh，将代码抽取到output目录下，在output目录下看到这些文件夹：
```
eng  examples  Makefile
```

### <a name="修改output/Makefile">修改output/Makefile</a>
在Makefile中的CC和AR更新为：
```
CC := arm-buildroot-linux-uclibcgnueabihf-gcc
AR := arm-buildroot-linux-uclibcgnueabihf-ar
```
请export正确的交叉编译工具链(arm-buildroot-linux-uclibcgnueabihf)路径到PATH变量里面


### <a name="填写 output/eng/wrappers/wapper.c">填写 output/eng/wrappers/wapper.c</a>
将需要实现的HAL都填上，[参考的HAL](http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/wrappers_demo.tar)


可以见这个压缩包里面的.c和.h直接拷贝到output/eng/wrappers/，并覆盖原有文件

### <a name="修改example，加入配网功能">修改example，加入配网功能</a>
如下图，在linkkit-example-solo.c中加入awss_config_press()和awss_start()函数调用，使能配网
```
int main(int argc, char **argv)
{
    int res = 0;
    int cnt = 0;
    iotx_linkkit_dev_meta_info_t master_meta_info;
    int domain_type = 0, dynamic_register = 0, post_reply_need = 0;
#ifdef ATM_ENABLED
    if (IOT_ATM_Init() < 0) {
        EXAMPLE_TRACE("IOT ATM init failed!\n");
        return -1;
    }
#endif

    awss_config_press();    //打开配网开关
    awss_start();           //调用配网函数

    EXAMPLE_TRACE("cxc after awss_start!\n");
    memset(&g_user_example_ctx, 0, sizeof(user_example_ctx_t));
```

### <a name="在 output 目录下编译">在 output 目录下编译</a>
make clean;make prog


在output目录下，生成了build/linkkit-example-solo这个demo程序

### <a name="如何将编译出来的demo程序上传到板子上">如何将编译出来的demo程序上传到板子上</a>
捷高板子有一个以太网卡，也有wifi网卡。我们通过以太网卡登陆到板子里面


在电脑里面创建一个tftp-hpa服务器，参考[链接](https://www.jianshu.com/p/d90696a069df)

通过tftp将demo传到板子上(假设tftp服务器的ip地址为192.168.1.104)
```
cd /tmp; 
tftp -g -r linkkit-example-solo -l linkkit-example-solo 192.168.1.104; 
```
### <a name="如何启动demo">如何启动demo</a>
```
cd /tmp; 
chmod 777 linkkit-example-solo; 
./linkkit-example-solo > logs &
 ifconfig eth0 down && sleep 100 && ifconfig eth0 up       //断开以太网口100秒，确保设备联网用的是wifi网口
```
备注：如果需要确保设备开机后没有其他程序在调用Linkkit的接口，如果有的话需要先停止这些程序


### <a name="如何在app进行操作">如何在app进行操作</a>
具体见[操作步骤](https://code.aliyun.com/edward.yangx/public-docs/raw/master/images/app_awss_steps.gif)


配网过程中需要扫描的[二维码](https://cli.im/text?2c64a8a0b5f06f101ae9d174da20eb12)

### <a name="配网日志逐段说明  ">配网日志逐段说明  </a>
  - 哪些说明得到了ssid+pwd
  ```
[dbg] zconfig_got_ssid_passwd_callback(103): ssid:aos-linux-test1, bssid:28f366b2f160, 2
  ```
  - 哪些说明连ap成功
  ```
[dbg] __awss_start(62): awss connect ssid:aos-linux-test1 success
  ```
  

### <a name="对每个阶段app上是什么样子附加说明">对每个阶段app上是什么样子附加说明</a>
配网成功的app截图:

<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/done_awss.jpg" width="600"  />



点击“开始使用”，触发绑定动作。绑定成功的app截图：



<img src="http://linkkit-export.oss-cn-shanghai.aliyuncs.com/3.0.1_awss/done_binding.jpg" width="600"  />



