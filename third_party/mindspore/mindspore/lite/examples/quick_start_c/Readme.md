# 适用于OHOS的简易MSLite 教程
## 使用条件
必须提前下载OHOS的工具链，并确保可以使用cmake和ninja。如果没有下载工具链，可以前往[http://ci.openharmony.cn/dailys/dailybuilds](http://ci.openharmony.cn/dailys/dailybuilds)并选择形态组件为ohos-sdk进行下载。压缩包中的名称含有`native`的压缩包即为OHOS-NDK，ohos.toolchain.cmake的位置为`native/build/cmake/ohos.toolchain.cmake`。

## 编译方法
### 基于Linux编译

通过脚本build.sh即可下编译实例代码并下载ms模型文件。
build.sh文件接收两个参数：
 - 第二个参数是架构，目前支持arm32和arm64。
 - 第一个参数是ohos.toolchain.cmake的路径。

使用方法如下：
```sh
sh build.sh 'PATH TO ohos.toolchain.cmake' arm32
```
```sh
sh build.sh 'PATH TO ohos.toolchain.cmake' arm64
```

### 基于Windows编译

通过脚本build.bat即可编译代码，需要注意的是该脚本不会自动下载`.ms`模型，需要用户自己下载，[点击下载模型](https://download.mindspore.cn/model_zoo/official/lite/quick_start/mobilenetv2.ms)。
build.sh文件接收两个参数：
 - 第二个参数是架构，目前支持arm32和arm64。
 - 第一个参数是ohos.toolchain.cmake的路径。

使用方法如下：
```bat
sh build.bat "PATH TO ohos.toolchain.cmake" arm32
```
```bat
sh build.bat "PATH TO ohos.toolchain.cmake" arm64
```
## demo使用方法

脚本运行成功之后，编译得到的`demo`可执行程序会存放在`build`目录下。
`demo`接收一个参数，该参数为`.ms`模型文件的路径。

首先使用hdc_std(或hdc)工具将`demo`和模型文件推送到开发板的`/data`目录中，并修改`demo`的权限为可执行，然后在开发板中运行如下命令

```sh
cd /data
chmod 755 ./demo
./demo ./mobilenetv2.ms
```

若看到类似如下输出，则证明模型运行成功：
```sh
Tensor name: Softmax-65, tensor size is 4004 ,elements num: 1001.
output data is:
0.000018 0.000012 0.000026 0.000194 0.000156 0.001501 0.000240 0.000825 0.000016 0.000006 0.000007 0.000004 0.000004 0.000004 0.000015 0.000099 0.000011 0.000013 0.000005 0.000023 0.000004 0.000008 0.000003 0.000003 0.000008 0.000014 0.000012 0.000006 0.000019 0.000006 0.000018 0.000024 0.000010 0.000002 0.000028 0.000372 0.000010 0.000017 0.000008 0.000004 0.000007 0.000010 0.000007 0.000012 0.000005 0.000015 0.000007 0.000040 0.000004 0.000085 0.000023