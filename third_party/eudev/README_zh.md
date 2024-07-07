# eudev
eudev 是一个通用的内核设备管理器。它运行在用户空间， 在Linux 系统上作为守护程序运行，并侦听（通过 netlink 套接字）在初始化新设备或从系统中删除设备时内核发出的内容。通过其提供的规则与事件的导出值和发现设备的属性匹配，命名并创建设备节点，并运行已配置的程序来设置和配置设备。

每当设备结构发生变化时，内核都会发出一个由设备管理器获取的 uevent ，然后设备管理器查找声明的规则，根据uevent中包含的信息，它会找到触发和执行所需操作所需的规则。这些动作可能涉及设备文件的创建或删除，还可能触发将特定固件文件加载到内核内存中。

## 目录结构

```
README.md               英文说明
README_zh.md            中文说明
COPYING                 版权文件
hwdb/                   硬件数据库文件
rules.d/                规则文件
export_include/         导出头文件
src/                    C++源码
test/                   测试文件
man/                    帮助文件
```

## OpenHarmony如何集成libudev
### 1.头文件引入
```cpp
#include "libudev.h"
```
### 2.BUILD.gn添加引用
```cpp
deps += [ "//third_party/eudev:libudev" ]
```
### 3.调用udev函数过程举例
```cpp
#include <stdio.h>
#include "libudev.h"

void test_device(struct udev *udev, const char *syspath)
{
    struct udev_device *device;
    printf("looking at device: %s\n", syspath);
    device = udev_device_new_from_syspath(udev, syspath);
    if (device == NULL) {
        printf("no device found\n");
        return;
    }
    printf("*** device: %p ***\n", device);
    const char *str = udev_device_get_devnode(device);
    if (str != NULL) {
        printf("devname:   '%s'\n", str);
    }
    dev_t devnum = udev_device_get_devnum(device);
    if (major(devnum) > 0) {
        printf("devnum:    %u:%u\n", major(devnum), minor(devnum));
    }
    udev_device_unref(device);
}

int main(int argc, char *argv[])
{
    struct udev *udev = udev_new();
    printf("udev context: %p\n", udev);
    if (udev == NULL) {
        printf("no context\n");
        return 1;
    }
    const char *path = "/sys/devices/virtual/mem/null";
    test_device(udev, path);
    udev_unref(udev);
    return 0;
}
```

## udev使用文档

官方文档  https://github.com/eudev-project/eudev

## COPYING

见 [COPYING](COPYING).
