# libnl

#### Introduction & Software Architecture
- [Refer to the official documentation](https://www.infradead.org/~tgr/libnl/)

#### Usage Guidelines

- [Refer to the official API documentation](https://www.infradead.org/~tgr/libnl/doc/api/group__cb.html)

#### Generates  Header File

In this warehouse include/netlink/version.h and lib/defs.h file is generated through the following steps

1.Run the following script to generate the tools required for the header file
    ```
    bash install_tool.sh
    ```
Note:The preceding script performs following operations:Install the autoconf,libtool and pkg-config tools,execute the autogen.sh file in the libnl root directory,generate the configure,defs.h.in and defs.h.in~ files,and then execute the configure command,The version.h file is generated in the include/netlink directory,defs.h file is generated in the lib directory.
#### Contribution

[How to involve](https://gitee.com/openharmony/docs/blob/HEAD/zh-cn/contribute/参与贡献.md)

[Commit message spec](https://gitee.com/openharmony/device_qemu/wikis/Commit%20message%E8%A7%84%E8%8C%83)

#### Repositories Involved

[third_party_wpa_supplicant](https://gitee.com/openharmony/third_party_wpa_supplicant)

[drivers_peripheral](https://gitee.com/openharmony/drivers_peripheral)

