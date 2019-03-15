# <a name="目录">目录</a>
+ [目标平台为Windows](#目标平台为Windows)

# <a name="目标平台为Windows">目标平台为Windows</a>

> 在SDK的V2.3.0版本, SDK自带跨平台的编译系统可基于`cmake`或`i686-w64-mingw32-gcc`这样的工具产生Windows版本的二进制库
>
> 这部分详见历史文档中的: [V2.3.0版本的Windows版本编译](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Archived/V230_Windows_Build)

在SDK的V3.0.1版本以上, 新增了直接基于源代码文件移植对接的能力
---

基于这种能力, 不论您是想要把SDK移植到什么 Windows 目标系统上, 都可以直接取用SDK中需要的源文件, 用自己熟悉的方式去编译它们了

如何抽取自己需要的源文件取用, 详见文档: [不使用SDK自带编译系统时的移植示例](http://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Port_Guide/Extract_Example)
