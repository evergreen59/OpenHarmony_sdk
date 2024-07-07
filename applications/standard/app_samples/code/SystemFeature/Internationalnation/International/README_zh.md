# 国际化

### 简介

本示例展示了i18n，intl，resourceManager在eTS中的使用，使用相关api实现系统语言和地区设置、时间和时区设置，展示了区域格式化示例。实现效果如下：

![home](screenshots/devices/home.png) ![language_region](screenshots/devices/language_region.png)

### 相关概念

国际化：Intl模块包含国际化能力基础接口，i18n模块包含国际化能力增强接口，资源管理模块，根据当前configuration（语言，区域，横竖屏，mccmnc）和device capability（设备类型，分辨率）提供获取应用资源信息读取接口。三个结合一起实现语言地区设置、时区设置和国际化资源管理相关功能。

### 相关权限

更新配置权限：ohos.permission.UPDATE_CONFIGURATION
设置时区权限：ohos.permission.SET_TIME_ZONE
设置系统时间权限：ohos.permission.SET_TIME

### 使用说明

1.启动应用，进入应用，首页分为三个按钮，点击**语言和地区**可以进入界面设置系统语言和地区。

2.点击**日期和时间**，可以进入界面设置系统时间和时区。

3.点击**区域格式示例**进入界面展示当前区域，当前系统语言下的时间、数字、货币、百分比、单复数、屏幕方向等信息的格式化显示。

### 约束与限制

1.本示例仅支持标准系统上运行。

2.本示例需要使用DevEco Studio 3.0 Beta4 (Build Version: 3.0.0.992, built on July 14, 2022)才可编译运行。

3.本示例需要使用 @ohos.i18n 系统权限的系统接口。使用Full SDK时需要手动从镜像站点获取，并在DevEco Studio中替换，具体操作可参考[替换指南](https://docs.openharmony.cn/pages/v3.2/zh-cn/application-dev/quick-start/full-sdk-switch-guide.md/)。

4.本示例所配置的权限ohos.permission.UPDATE_CONFIGURATION、ohos.permission.SET_TIME_ZONE、ohos.permission.SET_TIME均为system_basic级别(相关权限级别可通过[权限定义列表](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/security/permission-list.md)查看)，需要手动配置对应级别的权限签名(具体操作可查看[自动化签名方案](https://docs.openharmony.cn/pages/v3.2Beta/zh-cn/application-dev/security/hapsigntool-overview.md/))。
