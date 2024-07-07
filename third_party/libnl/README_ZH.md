# libnl

#### 简介 & 软件架构

- [参考官方文档](https://www.infradead.org/~tgr/libnl/)

#### 使用说明

- [参考官方API文档](https://www.infradead.org/~tgr/libnl/doc/api/group__cb.html)

#### 头文件生成

本仓中的include/netlink/version.h及lib/defs.h文件是通过以下步骤生成

1.生成头文件及所需工具，执行下列脚本
    ```
    bash install_tool.sh
    ```
注：上述脚本进行了如下操作，首先安装了autoconf、libtool、pkg-config工具，其次执行libnl根目录autogen.sh，生成configure、defs.h.in和defs.h.in~文件。然后执行configure，会在include/netlink目录下生成version.h，lib目录下生成defs.h文件。

#### 参与贡献

[如何贡献](https://gitee.com/openharmony/docs/blob/HEAD/zh-cn/contribute/参与贡献.md)

[Commit message规范](https://gitee.com/openharmony/device_qemu/wikis/Commit%20message%E8%A7%84%E8%8C%83)


#### 相关仓

[third_party_wpa_supplicant](https://gitee.com/openharmony/third_party_wpa_supplicant)

[drivers_peripheral](https://gitee.com/openharmony/drivers_peripheral)

