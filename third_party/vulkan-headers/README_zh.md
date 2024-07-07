# Vulkan API Headers

仓库包含Vulkan API 和 API 注册表。Vulkan 是一个适用于高性能 3D 图形设备的低开销、跨平台 API。与 OpenGL ES (GLES) 一样，Vulkan 提供用于在应用中创建高品质实时图形的工具。与OpenGL ES相比使用 Vulkan 的优势明显，Vulkan可以大大降低 CPU 开销，另外Vulkan支持 SPIR-V 二进制Shader语言。

OpenHarmony引入后，在框架层新增Vulkan Wrapper，负责实现Vulkan WSI(Window System Integration, 窗口系统集成) 与本地窗口(NativeWindow)对接，另外提供Vulkan加载程序。目前仅支持native层调用Vulkan。


## 一、北向开发和OpenHarmony发行版
### 1.实现 Vulkan

设备成功运行Vulkan必需的条件：
    1. 加载程序，由OpenHarmony提供
    2. 实现了Vulkan API 的 VulKan 驱动程序，有 SoC 提供。

### 2. Vulkan Wrapper

`foundation/graphic/graphic_2d/frameworks/vulkan_wrapper`中实现的Vulkan加载程序是Vulkan应用与Vulkan驱动之前的Wrapper层。Vulkan Wrapper安装在`/system/lib[64]/libvulkan.so`中。Vulkan Wrapper 会提供标准 Vulkan API 函数符号、OpenHarmony WSI扩展的函数符号，以及许多其他可选扩展。


### 3. Vulkan 驱动程序

Vulkan驱动程序需要对外暴露的函数符号请参考`hdi_vulkan.h`中的定义。

## 二、南向应用开发者

### VK_OpenHarmony_OHOS_surface 扩展

    接口：vkCreateOHOSSurfaceOpenHarmony
    VkResult vkCreateOHOSSurfaceOpenHarmony(
        VkInstance                               instance,
        const VkOHOSSurfaceCreateInfoOpenHarmony pCreateInfo, 
        const VkAllocationCallbacks              pAllocator,
        VkSurfaceKHR                             pSurface)

    参数：
        instance是要关联Surface的实例。
        pCreateInfo是一个指向结构的指针，该VkOHOSSurfaceCreateInfoOpenHarmony 结构包含影响表面对象创建的参数。
        pAllocator是用于在没有更多特定内存分配器可用时做为Surface对象分配的分配器。
        pSurface是一个指向VkSurfaceKHR句柄的指针，在该句柄中返回创建的Surface对象。


## 三、Vulkan使用文档

API官方文档  https://registry.khronos.org/vulkan/


## 四、License

见 [LICENSE](LICENSE).
