# HiSpark\_aries介绍<a name="ZH-CN_TOPIC_0000001142448981"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [安装教程](#section161941989597)
-   [使用说明](#section161941989598)
-   [约束](#section119744591305)
-   [编译构建](#section137768191623)
-   [协议说明](#section1312121216216)
    -   [许可说明](#section129654513264)

-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

[开发板介绍](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-introduction-hi3518.md)

本目录主要用于放置hispark_aries开发板相关内容，详情参见目录章节。

## 目录<a name="section161941989596"></a>

```
├── liteos_a
│   ├── board         # liteos形态启动相关代码
│   └── drivers       # liteos内核宏配置信息
└── uboot
    ├── out                # 采用third_party\uboot\u-boot-2020.01编译成的U-Boot
    ├── prebuilts          # uboot编译依赖中间件
    ├── reg                # U-Boot配置文件、LICENSE文件
    ├── secureboot_ohos    # 安全启动相关的编译脚本
    └── secureboot_release # 安全启动相关的编译脚本
```

## 安装教程<a name="section161941989597"></a>

liteos-a请参考：[环境指导](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-steps-hi3518-setting.md)

## 使用说明<a name="section161941989598"></a>

liteos-a请参考：[使用说明](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-steps-hi3518.md)

## 约束<a name="section119744591305"></a>

当前支持Hi3518EV300芯片。

## 编译构建<a name="section137768191623"></a>

-   编译uboot

1.  进入hispark\_aries\\uboot目录，修改该目录下的Makefile中的OSDRV\_CROSS所定义的工具链的路径（工具链版本有更新，则需修改）。
2.  编译U-Boot，命令：make clean;make;

生成的U-Boot存放在hispark\_aries\\uboot\\out\\boot目录下。


## 协议说明<a name="section1312121216216"></a>

参见对应目录的LICENSE文件及代码声明

### 许可说明<a name="section129654513264"></a>

third\_party\\uboot\\u-boot-2020.01为U-Boot开源代码，遵循软件版本自带的开源许可声明，具体请参看third\_party\\uboot\\u-boot-2020.01\\Licenses目录下的README。

## 相关仓<a name="section1371113476307"></a>

[vendor_hisilicon](https://gitee.com/openharmony/vendor_hisilicon)

[device_soc_hisilicon](https://gitee.com/openharmony/device_soc_hisilicon)

