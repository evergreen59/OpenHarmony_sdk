# 打包拆包工具使用说明

## 简介

packing_tool子系统用于生成打包工具和拆包工具，其中打包工具用于hap，app，hqf，appqf,har包的生成，拆包工具用于对hap，app，hqf，appqf,har包的拆包及对hap，app，appqf的解析。具体的功能介绍如下：

打包工具子系统架构图如下：

![](https://gitee.com/openharmony/developtools_packing_tool/raw/master/img/%E6%89%93%E5%8C%85%E5%B7%A5%E5%85%B7%E6%9E%B6%E6%9E%84%E5%9B%BE.webp)

## 1. 打包指令说明

### 1.1 hap包模式打包指令

#### 1.1.1 示例

- Stage模型的打包命令示例：


```
java -jar path\app_packing_tool.jar --mode hap --json-path <option> --resources-path <option> --ets-path <option> --index-path <option> --pack-info-path <option> --out-path path\out\srcEntrance.hap --force true
```

- FA模型的打包命令示例：


```
java -jar app_packing_tool.jar --mode hap --json-path <option> --maple-so-path [option] --profile-path [option] --maple-so-dir [option] --dex-path [option] --lib-path [option] --resources-path [option] --index-path [option] --out-path <option> --force [option]
```

#### 1.1.2 参数含义及规范

| 指令             | 是否必选项 | 选项                                  | 描述                                                         | 备注            |
| ---------------- | ---------- | ------------------------------------- | ------------------------------------------------------------ | --------------- |
| --mode           | 是         | hap                                   | 打包类型。                                                   | NA              |
| --json-path      | 是         | NA                                    | .json文件路径，FA模型文件名必须为config.json；stage模型文件名必须为module.json。 | NA              |
| --profile-path   | 否         | NA                                    | CAPABILITY.profile文件路径                                   | NA              |
| --maple-so-path  | 否         | NA                                    | maple so文件输入路径，so文件路径，文件名必须以.so为后缀。如果是多个so需要用“，”分隔。 | NA              |
| --maple-so-dir   | 否         | NA                                    | maple so目录输入路径。                                       | NA              |
| --dex-path       | 否         | NA                                    | 1.dex文件路径，文件名必须以.dex为后缀。如果是多个dex需要用“，”分隔。2.dex文件路径也可以为目录。 | NA              |
| --lib-path       | 否         | NA                                    | lib库文件路径。                                              | NA              |
| --resources-path | 否         | NA                                    | resources资源包路径。                                        | NA              |
| --index-path     | 否         | NA                                    | .index文件路径，文件名必须为resources.index                  | NA              |
| --pack-info-path | 否         | NA                                    | pack.info文件路径，文件名必须为pack.info                     | NA              |
| --rpcid-path     | 否         | NA                                    | rpcid.sc文件路径，文件名必须为rpcid.sc                       | NA              |
| --js-path        | 否         | NA                                    | 存放js文件目录路径                                           | 仅stage模型生效 |
| --ets-path       | 否         | NA                                    | 存放ets文件目录路径                                          | 仅stage模型生效 |
| --out-path       | 是         | NA                                    | 目标文件路径，文件名必须以.hap为后缀。                       | NA              |
| --force          | 否         | true或者false                         | 默认值为false，如果为true，表示当目标文件存在时，强制删除。  | NA              |
| --dir-list       | 否         | 可指定目标文件夹列表，将其打入hap包内 | NA                                                           | NA              |

### 1.2 har包模式打包指令

#### 1.2.1 示例

```
java -jar app_packing_tool.jar --mode har --json-path [option] --jar-path [option>]--lib-path [option] --resources-path [option] --out-path [option] --force [option]
```

#### 1.2.2 参数含义及规范

| 指令        | 是否必选项 | 选项          | 描述                                                         |
| ----------- | ---------- | ------------- | ------------------------------------------------------------ |
| --mode      | 是         | har           | 打包类型。                                                   |
| --json-path | 是         | NA            | .json文件路径，文件名必须为config.json。                     |
| --jar-path  | 否         | NA            | 1.jar文件路径，文件名必须以.jar为后缀。如果是多个jar需要用“，”分隔。2.jar文件路径也可以为目录。 |
| --lib-path  | 否         | NA            | lib库文件路径。                                              |
| --res-path  | 否         | NA            | resources资源包路径。                                        |
| --out-path  | 是         | NA            | 目标文件路径，文件名必须以.hap为后缀。                       |
| --force     | 否         | true或者false | 默认值为false，如果为true，表示当目标文件存在时，强制删除。  |

### 1.3 app包模式打包指令

#### 1.3.1 示例

```
java -jar app_packing_tool.jar --mode app --hap-path <option> --out-path [option] --signature-path [option] --certificate-path [option] --pack-info [option]--force [option]
```

#### 1.3.2 参数含义及规范

| 指令               | 是否必选项 | 选项          | 描述                                                         |
| ------------------ | ---------- | ------------- | ------------------------------------------------------------ |
| --mode             | 是         | app           | 多个hap需满足hap的合法性校验。                               |
| --hap-path         | 是         | NA            | 1.hap包文件路径，文件名必须以.hap为后缀。如果时多个hap包需要用“，”分隔。2.hap包文件路径也可以是目录。 |
| --pack-info-path   | 是         | NA            | 文件名必须为pack.info                                        |
| --out-path         | 否         | NA            | 目标文件路径，文件名必须以.app为后缀。                       |
| --signature-path   | 否         | NA            | 签名路径。                                                   |
| --certificate-path | 否         | NA            | 证书路径。                                                   |
| --force            | 否         | true或者false | 默认值为false，如果为true，表示当目标文件存在时，强制删除。  |

#### 1.3.3 打包app时hap的合法性校验

在对工程内的hap包打包生成app包时，需要保证被打包的每个hap在json文件中配置的bundleName，versionCode和versionName，minCompatibleVersionCode相同，minAPIVersion，targetAPIVersion，apiReleaseType相同，moduleName唯一，对于fa模型，还需要保证json文件中配置的package唯一。

### 1.4 多工程打包

#### 1.4.1 示例

```
java -jar app_packing_tool.jar --mode multiApp --hap-list 1.hap,2.hap --app-list 1.app,2.app --out-path final.app
```

#### 1.4.2 参数含义及规范

| 指令       | 是否必选项 | 选项          | 描述                                                         |
| ---------- | ---------- | ------------- | ------------------------------------------------------------ |
| --mode     | 是         | multiApp      | 打包类型，在将多个hap打入同一个app时，需保证每个hap满足合法性校验规则。 |
| --hap-list | 否         | hap的路径     | 1.hap包文件路径，文件名必须以.hap为后缀。如果是多个hap包需要”，“分隔。2.hap文件路径也可以是目录。 |
| --app-list | 否         | app的路径     | 1.app文件路径，文件名必须以.app为后缀。如果是多个hap包需要用”，“分隔。2.app文件路径也可以是目录。3.--hap-list命令与--app-list不可以都不传。 |
| --force    | 否         | 默认值为false | 默认值为false，如果为true，表示当目标文件存在时，强制删除。  |

#### 1.4.3 多工程打包hap合法性校验

需要保证被打包的每个hap在json文件中配置的bundleName，versionCode和versionName，minCompatibleVersionCode相同，minAPIVersion，targetAPIVersion，apiReleaseType相同，moduleName唯一，同一设备entry唯一，对于fa模型，还需要保证json文件中配置的package唯一。

## 2. 拆包指令说明

### 2.1 hap包模式拆包指令

#### 2.1.1 示例

```
java -jar app_unpacking_tool.jar --mode <option> --hap-path <options> --out-path [option] --force [option]
```

#### 2.1.2 参数含义及规范

| 指令       | 是否必选项 | 选项          | 描述                                                         |
| ---------- | ---------- | ------------- | ------------------------------------------------------------ |
| --mode     | 是         | hap           | 拆包类型。                                                   |
| --hap-path | 是         | NA            | hap包路径。                                                  |
| --rpcid    | 否         | true或者false | 是否单独将rpcid文件从hap包中提取到指定目录，如果为true，将仅提取rpcid文件，不对hap包进行拆包。 |
| --out-path | 是         | NA            | 拆包目标文件路径。                                           |
| --force    | 否         | true或者false | 默认值为false，如果为true，表示当目标文件存在时，强制删除。  |

### 2.2 har包模式拆包指令

#### 2.2.1 示例

```
java -jar app_unpacking_tool.jar --mode <options> --har-path <option> --out-path [option] --force [option]
```

#### 2.2.2 参数含义及规范

| 指令       | 是否必选项 | 选项          | 描述                                                        |
| ---------- | ---------- |-------------| ----------------------------------------------------------- |
| --mode     | 是         | har         | 拆包类型。                                                  |
| --har-path | 是         | NA          | har包路径。                                                 |
| --out-path | 是         | NA          | 拆包目标文件路径。                                          |
| --force    | 否         | true或者false | 默认值为false，如果为true，表示当目标文件存在时，强制删除。 |

### 2.3 app包模式拆包指令

#### 2.3.1 示例

```
java -jar app_unpacking_tool.jar --mode <options> --app-path <option> --out-path [option] --force [option]
```

#### 2.3.2 参数含义及规范

| 指令       | 是否必选项 | 选项          | 描述                                                        |
| ---------- | ---------- |-------------| ----------------------------------------------------------- |
| --mode     | 是         | app         | 拆包类型。                                                  |
| --app-path | 是         | NA          | app包路径。                                                 |
| --out-path | 是         | NA          | 拆包目标文件路径。                                          |
| --force    | 否         | true或者false | 默认值为false，如果为true，表示当目标文件存在时，强制删除。 |

### 2.4 从hap包中获取rpcid文件

#### 2.4.1 示例

```
java -jar app_unpacking_tool.jar --mode hap --rpcid true --hap-path <option> --out-path <option> --force true
```

#### 2.4.2 参数含义及规范

| 指令       | 是否必选项 | 选项          | 描述                                                         |
| ---------- | ---------- | ------------- | ------------------------------------------------------------ |
| --mode     | 是         | hap           | 拆包类型                                                     |
| --rpcid    | 否         | true或者false | 是否单独将rpcid文件从hap包中提取到指定目录，如果为true，将仅提取rpcid文件，不对hap包进行拆包。 |
| --hap-path | 是         | NA            | hap包路径                                                    |
| --out-path | 是         | NA            | 拆包rpcid目标文件路径                                        |
| --force    | 否         | true或者false | 默认值为false，如果为true，表示当目标文件存在时，强制删除。  |

## 3. 包解析接口

### 3.1 接口目录

| 类名               | 接口原型                                                     | 类型     | 接口详细描述                                                 |
| ------------------ | ------------------------------------------------------------ | -------- | ------------------------------------------------------------ |
| UncompressEntrance | UncomperssResult parseApp(String appPath,String parseMode,String deviceType,String hapName) | Java接口 | 接口功能：根据参数解析app包的pack.info信息 输入参数：app包路径，解析参数 返回值：UncomperssResult |
| UncompressEntrance | UncomperssResult parseApp(InputStream input,String parseMode,String deviceType,String hapName,String outPath) | Java接口 | 接口功能：根据参数解析app包的pack.info信息 输入参数：app文件流，解析参数 返回值：UncomperssResult |
| UncompressEntrance | UncomperssResult parseHap(String hapPath)                    | Java接口 | 接口功能：根据参数解析app包的config.json信息 输入参数：hap包路径，解析参数 返回值：UncomperssResult |
| UncompressEntrance | UncomperssResult parseHap(InputStream input)                 | Java接口 | 接口功能：根据参数解析app包的config.json信息 输入参数：hap包文件流，解析参数 返回值：UncomperssResult |

## 4. 拆包工具信息字段

### 4.1 UncomperssResult（Bundle信息）结构体信息

| 字段         | 类型               | 描述                                                         | 备注 |
| ------------ | ------------------ | ------------------------------------------------------------ | ---- |
| result       | boolean            | 标识此次解析是否成功                                         | NA   |
| message      | String             | 标识此次解析失败的原因或Success                              | NA   |
| packInfos    | List\<PackInfo>    | bundle中pack.info文件的packages信息                          | NA   |
| profileInfos | List\<profileInfo> | 一个ProfileInfo对应一个module的信息                          | NA   |
| icon         | String             | 返回入口组件的icon路径，如果没有入口组件，则返回第一个组件的icon信息 | NA   |
| label        | String             | 返回入口组件的label，如果没有入口组件，则返回第一个组件的label信息 | NA   |

### 4.2 PackInfo结构体信息

| 字段                | 类型          | 描述                                    | 备注 |
| ------------------- | ------------- | --------------------------------------- | ---- |
| name                | String        | 包名                                    | NA   |
| moduleName          | String        | Hap名称                                 | NA   |
| moduleType          | String        | module的类型                            | NA   |
| deviceType          | List\<String> | 表示当前hap包所支持的设备类型           | NA   |
| deliveryWithInstall | boolean       | 标识当前hap是否在用户主动安装的时候安装 | NA   |

### 4.3 ProfileInfo结构体信息

| 字段         | 类型                           | 描述                                       | 备注                                                         |
| ------------ | ------------------------------ | ------------------------------------------ | ------------------------------------------------------------ |
| hapName      | String                         | 标识当前解析的hap包名称                    | NA                                                           |
| appInfo      | AppInfo结构体（见下述AppInfo） | 标识APP信息的结构体（见下述AppInfo信息）   | NA                                                           |
| deviceConfig | Map\<String,DeviceConfig>      | 标识设备信息                               | 存储类型为Map\<String,String>，存储设备类型名称及对应设备类型的信息，在stage模型中，这个字段存储在app结构体中 |
| hapInfo      | HapInfo结构体（见下述HapInfo） | 标识hap包中module信息（见下述HapInfo信息） | NA                                                           |

### 4.4 AppInfo结构体信息

| 字段                           | 类型    | 描述                                   | 备注          |
| ------------------------------ | ------- | -------------------------------------- | ------------- |
| bundleName                     | String  | 标识app的包名称                        | NA            |
| bundleType                     | String  | 标识bundle的类型，取值：<br/>- app：普通应用<br/>- atomicService：原子化服务 | NA        |
| vendor                         | String  | 标识app的供应商信息                    | NA            |
| releatedBundleName             | String  | 标识app相关bundle的包名                | NA            |
| versionName                    | String  | 标识app中的versionName信息             | NA            |
| versionCode                    | String  | 标识app中的versionCode信息             | NA            |
| targetApiVersion               | int     | 标识应用运行需要的API目标版本          | NA            |
| compatibleApiVersion           | int     | 标识应用兼容的API版本                  | NA            |
| appName                        | String  | 标识显示在桌面上的ability的label       | NA            |
| releaseType                    | String  | 标识应用运行需要的API目标版本的类型    | NA            |
| shellVersionCode               | String  | 标识HarmonyOS中的API版本               | NA            |
| shellVersionName               | String  | 标识HarmonyOS中的API版本名称           | NA            |
| multiFrameworkBundle           | String  | 标识双框架下app的bundleName            | NA            |
| debug                          | boolean | 标识应用是否可调试                     | NA            |
| icon                           | String  | 标识应用的图标路径                     | NA            |
| label                          | String  | 标识应用的label                        | NA            |
| description                    | String  | 标识App的描述信息                      | stage模型新增 |
| minCompatibleVersionCode       | int     | 标识该app pack能够兼容的最低历史版本号 | NA            |
| distributedNotificationEnabled | boolean | 标记该应用是否开启分布式通知           | stage模型新增 |
| jointUserid                    | String  | 标签标识一个用的共享userid             | FA模型特有    |
| process                        | String  | 标识应用的进程名                       | FA模型特有    |
| directLaunch                   | String  | 标识应用是否支持未解锁直接启动模式     | NA            |
| distributedNotificationEnabled | boolean | 标记该应用是否开启分布式通知           | stage模型特有 |

### 4.5 HapInfo结构体信息

| 字段                 | 类型                                | 描述                                          | 备注                                                    |
|--------------------|-----------------------------------|---------------------------------------------| ------------------------------------------------------- |
| appModel           | AppModel枚举值                       | 标识应用的框架模型<br/>- FA：FA模型<br/>- STAGE：Stage模型 | NA                               |
| packageStr         | String                            | 标识hap包的包信息                                  | FA模型特有                                              |
| name               | String                            | 标识当前module的名字                               | NA                                                      |
| description        | String                            | 标识hap包的描述信息                                 | FA模型特有                                              |
| supporteModes      | List\<String>                     | 标识hap包的支持的模式                                | NA                                                      |
| abilities          | AbilityInfo的数组                    | 标识hap包ability信息                             | NA                                                      |
| distro             | Distro结构体                         | 标识hap包的distro信息                             | NA                                                      |
| deviceType         | List\<String>                     | 标识hap可以运行在哪类设备上                             | 对应stage模型中的deviceTypes                            |
| metadata           | MetaData结构体（见下述metaData）          | 标识Hap的自定义元信息                                | NA                                                      |
| isJs               | boolean                           | 标识该应用是否是js应用                                | FA模型特有                                              |
| reqPermissions     | ReqPermission结构体数组（见下述ReqPermission） | 标识应用申请的权限的集合                                | 对应stage模型的requestPermissions                       |
| commonEvents       | CommonEvent结构体（见下述CommentEvent）   | 标识静态事件                                      | NA                                                      |
| shortcuts          | Shortcut结构体数组（见下述Shortcut）        | 标识应用的shortcuts信息                            | NA                                                      |
| distrofilter       | Distrofilter结构体                   | 标识应用市场按设备形态分发的信息                            | NA                                                      |
| srcEntrance        | String                            | 标识hap所对应的入口js代码路径                           | stage模型新增                                           |
| process            | String                            | 标识应用的进程名                                    | stage模型新增                                           |
| mainElement        | String                            | 标识hap的入口ability名称或者extension名称              | stage模型新增，FA模型将mainAbility的值赋值给mainElement |
| pages              | List\<String>                     | 列举JS Component中每个页面信息                       | stage模型新增                                           |
| extensionAbilities | List\<ExtensionAbilityInfo>       | 描述extensionAbility的配置信息                     | stage模型新增                                           |
| abilityFormInfos   | List\<AbilityFormInfo>            | 描述卡片的信息                                     | NA                                                      |
| dependencies       | List\<DependencyItem>             | 描述模块的依赖共享库列表                                | NA                                         |

### 4.6 AbilityInfo结构体信息

| 字段                | 类型                             | 描述                                              | 备注                            |
|-------------------| -------------------------------- | ------------------------------------------------- | ------------------------------- |
| name              | String                           | 标识当前ability的逻辑名                           | NA                              |
| description       | String                           | 标识ability的描述                                 | NA                              |
| icon              | String                           | 标识ability图标                                   | NA                              |
| label             | String                           | 标识ability对用户显示的名称                       | NA                              |
| type              | String                           | 标识ability类型                                   | Stage模型下该值直接赋予page类型 |
| uri               | String                           | 标识ability的uri信息                              | FA模型支持                      |
| launchType        | String                           | 标识ability中的launcherType信息                   | NA                              |
| metadata          | Metadata结构体（见下述MetaData） | 描述ability的配置信息                             | NA                              |
| orientation       | String                           | 描述ability的显示模式                             | NA                              |
| permissions       | List\<String>                    | 标识被其它应用的ability调用时需要申请的权限的集合 | NA                              |
| skills            | SkillInfo结构体数组              | 标识ability能够接收的意图的特征集                 | NA                              |
| backgroundModes   | List\<String>                    | 标识ability长时任务集合                           | NA                              |
| visible           | boolean                          | 标识ability是否可以被其它应用调用                 | NA                              |
| grantPermission   | boolean                          | 标识ability的grantPermission                      | FA模型支持，stage模型废弃       |
| readPermission    | String                           | 标识ability的读取权限                             | FA模型支持，stage模型废弃       |
| writepermission   | String                           | 标识ability的写权限                               | FA模型支持，stage模型废弃       |
| uriPermissionMode | String                           | 标识ability的uriPermissionMode                    | FA模型支持，stage模型废弃       |
| uriPermissionPath | String                           | 标识ability的uriPermissionPath                    | FA模型支持，stage模型废弃       |
| configChages      | List\<String>                    | 标识ability的configChanges                        | FA模型支持，stage模型废弃       |
| directLaunch      | boolean                          | 标识ability的directLaunch                         | FA模型支持，stage模型废弃       |
| mission           | String                           | 标识ability的mission                              | FA模型支持，stage模型废弃       |
| targetAbility     | String                           | 标识ability的targetAbility                        | FA模型支持，stage模型废弃       |
| mulitiUserShared  | boolean                          | 标识ability的mulitiUserShared                     | FA模型支持，stage模型废弃       |
| supportPipMode    | boolean                          | 标识ability的supportPipMode                       | FA模型支持，stage模型废弃       |
| srcEntrance       | String                           | 标识ability所对应的js代码路径                     | stage模型支持                   |
| continuable       | boolean                          | 标识ability是否可以迁移                           | NA                              |

### 4.7 Distro结构体信息

| 字段                | 类型    | 描述                                                         | 备注                                                         |
| ------------------- | ------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| moduleName          | String  | 标识当前module的名字                                         | 对应Stage模型中module结构体中的moduleName字段                |
| moduleType          | String  | 标识当前hap的类型                                            | 对应Stage模型中module结构体中的moduleType字段                |
| deliveryWithInstall | boolean | 标识当前hap是否在用户主动安装的时候安装                      | 对应Stage模型中module结构体中的deliveryWithInstall字段       |
| installationFree    | 整数值  | 标识当前hap是否支持免安装特性                                | 对应Stage模型中module结构体中的installationFree字段，json文件中如果配置了该字段为true，返回1，配置为false，返回0，未配置返回2 |
| virtualMachine      | String  | 标识当前hap运行的目标虚拟机类型，供云端分发使用，如应用市场和分发中心 | 对应Stage模型中module结构体中的virtualMachine字段            |

### 4.8 Metadata结构体信息

| 字段           | 类型                                                 | 描述                             | 备注                      |
| -------------- | ---------------------------------------------------- | -------------------------------- | ------------------------- |
| parameters     | MetadataInfo结构体的数组（见下述MetadataInfo信息）   | 标识Metadata的参数信息           | FA模型特有，stage模型废弃 |
| results        | MetadataInfo结构体的数组（见下述MetadataInfo信息）   | 标识Metadata的results信息        | FA模型特有，stage模型废弃 |
| customizeDatas | CustomizeData结构体的数组（见下述CustomizeData信息） | 标识Metadata的customizeDatas信息 | NA                        |

### 4.9 MetadataInfo结构体信息

| 字段        | 类型   | 描述                              | 备注                      |
| ----------- | ------ | --------------------------------- | ------------------------- |
| name        | String | 标识MetadataInfo的name信息        | FA模型特有，stage模型废弃 |
| description | String | 标识MetadataInfo的description信息 | FA模型特有，stage模型废弃 |
| type        | String | 标识MetadataInfo的type信息        | FA模型特有，stage模型废弃 |

### 4.10 CustomizeData结构体信息

| 字段  | 类型   | 描述                         | 备注                    |
| ----- | ------ | ---------------------------- | ----------------------- |
| name  | String | 标识CustomizeData的name信息  | 对应stage模型的metadata |
| value | String | 标识CustomizeData的value信息 | 对应stage模型的metadata |
| extra | String | 标识CustomizeData的extra信息 | 对应stage模型的metadata |

### 4.11 ReqPermission结构体信息

| 字段      | 类型                               | 描述                                                         | 备注 |
| --------- | ---------------------------------- | ------------------------------------------------------------ | ---- |
| name      | String                             | 标识ReqPermission的名称信息                                  | NA   |
| reason    | String                             | 当申请的权限为user_grant权限时此字段必填。描述申请权限的原因 | NA   |
| usedScene | UsedScene结构体（见下述UsedScene） | 描述权限使用的场景和时机。场景类型有：ability、调用时机（when）。可配置多个ability | NA   |

### 4.12 UsedScene结构体信息

| 字段    | 类型          | 描述                                                         | 备注 |
| ------- | ------------- | ------------------------------------------------------------ | ---- |
| ability | List\<String> | 标识需要使用到该权限的元能力（ability），该标签值为数组形式  | NA   |
| when    | String        | 标识使用该权限的时机，值为inuse/always，表示为仅允许前台使用和前后台都可使用 | NA   |

### 4.13 ShortCut结构体信息

| 字段       | 类型                   | 描述                                                         | 备注 |
| ---------- | ---------------------- | :----------------------------------------------------------- | ---- |
| shortcutId | String                 | 标识ShortCut的Id                                             | NA   |
| label      | String                 | 标识ShortCut的标签信息                                       | NA   |
| icon       | String                 | 标识ShortCut的图标信息                                       | NA   |
| intents    | IntentInfo结构体的列表 | 标识快捷方式内定义的目标intent信息集合，每个intent可配置两个子标签，targetClass,targetBundle | NA   |

### 4.14 IntentInfo结构体信息

| 字段         | 类型   | 描述             | 备注 |
| ------------ | ------ | ---------------- | ---- |
| targetClass  | String | 快捷方式目标类型 | NA   |
| targetBundle | String | 快捷方式目标包名 | NA   |

### 4.15 DistroFilter结构体信息

| 字段          | 类型                | 描述                                  | 备注 |
| ------------- | ------------------- | ------------------------------------- | ---- |
| apiVersion    | ApiVersion结构体    | 标识DistroFilter中的apiVersion信息    | NA   |
| screenShape   | ScreenShape结构体   | 标识DistroFilter中的screenShape信息   | NA   |
| screenDensity | ScreenDensity结构体 | 标识DistroFilter中的screenDensity信息 | NA   |
| screenWindow  | ScreenWindow结构体  | 标识DistroFilter中的screenWindow信息  | NA   |
| countryCode   | CountryCode结构体   | 标识DistroFilter中的countryCode信息   | NA   |

### 4.16 ApiVersion结构体信息

| 字段   | 类型          | 描述                     | 备注 |
| ------ | ------------- | ------------------------ | ---- |
| policy | String        | 标识结构体中的policy信息 | NA   |
| value  | List\<String> | 标识结构体中的value信息  | NA   |

### 4.17 ScreenShape结构体信息

| 字段   | 类型          | 描述                     | 备注 |
| ------ | ------------- | ------------------------ | ---- |
| policy | String        | 标识结构体中的policy信息 | NA   |
| value  | List\<String> | 标识结构体中的value信息  | NA   |

### 4.18 ScreenDensity结构体信息

| 字段   | 类型          | 描述                     | 备注 |
| ------ | ------------- | ------------------------ | ---- |
| policy | String        | 标识结构体中的policy信息 | NA   |
| value  | List\<String> | 标识结构体中的value信息  | NA   |

### 4.19 ScreenWindow结构体信息

| 字段   | 类型          | 描述                     | 备注 |
| ------ | ------------- | ------------------------ | ---- |
| policy | String        | 标识结构体中的policy信息 | NA   |
| value  | List\<String> | 标识结构体中的value信息  | NA   |

### 4.20 CountryCode结构体信息

| 字段   | 类型          | 描述                     | 备注 |
| ------ | ------------- | ------------------------ | ---- |
| policy | String        | 标识结构体中的policy信息 | NA   |
| value  | List\<String> | 标识结构体中的value信息  | NA   |

### 4.21 ExtensionAbilityInfo结构体信息

| 字段            | 类型              | 描述                                                         | 备注                                                         |
| --------------- | ----------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| name            | String            | 标识当前extensionAbility的逻辑名                             | stage模型支持                                                |
| srcEntrance     | String            | 标识extensionAbility所对应的js代码路径                       | stage模型支持                                                |
| icon            | String            | 标签标识extensionAbility图标                                 | stage模型支持                                                |
| label           | String            | 标识extensionAbility对用户显示的名称                         | stage模型支持                                                |
| description     | String            | 标识extensionAbility的描述                                   | stage模型支持                                                |
| type            | String            | 标识extensionAbility的类型：form、workScheduler、inputMethod、service、accessibility、dataShare、fileShare、wallpaper、backup | stage模型支持，目前仅解析了form、staticSubscriber的信息、其他类型（如：workScheduler、inputMethod、service、accessibility、dataShare、fileShare、wallpaper、backup）暂未解析 |
| permissions     | List\<String>     | 标识被其它应用的ability调用时需要申请的权限的集合            | stage模型支持                                                |
| readPermission  | String            | 标识读取ability的数据所需的权限                              | stage模型支持                                                |
| writePermission | String            | 标识向ability写数据所需的权限                                | stage模型支持                                                |
| visible         | boolean           | 标识extensionAbility是否可以被其它应用调用                   | stage模型支持                                                |
| skills          | SkillInfo数组     | 标识extensionAbility能够接收的意图的特征集                   | stage模型支持                                                |
| metadata        | CustomizeData信息 | 标识extensionAbility的元信息                                 | 将metadata中的信息赋值到CustomizeData中                      |
| uri             | String            | 标识extensionAbility提供的数据uri                            | stage模型支持                                                |

### 4.22 SkillInfo结构体信息

| 字段     | 类型                | 描述                                   | 备注 |
| -------- | ------------------- | -------------------------------------- | ---- |
| actions  | List\<String>       | 标识能够接收的意图的action值的集合     | NA   |
| entities | List\<String>       | 标识能够接收的Intent的元能力的类别集合 | NA   |
| uriInfos | UriInfo结构体的数组 | 标识向want过滤器添加数据规范集合       | NA   |

### 4.23 UriInfo结构体信息

| 字段          | 类型   | 描述                                 | 备注 |
| ------------- | ------ | ------------------------------------ | ---- |
| schema        | String | 标识ModuleUriInfo的schema信息        | NA   |
| host          | String | 标识ModuleUriInfo的host信息          | NA   |
| port          | String | 标识ModuleUriInfo的port信息          | NA   |
| pathStartWith | String | 标识ModuleUriInfo的pathStartWith信息 | NA   |
| pathRegex     | String | 标识ModuleUriInfo的pathStartWith信息 | NA   |
| path          | String | 标识ModuleUriInfo的path信息          | NA   |
| type          | String | 标识ModuleUriInfo的type信息          | NA   |

### 4.24 AbilityFormInfo结构体信息

| 字段                | 类型          | 描述                                                         | 备注           |
| ------------------- | ------------- | ------------------------------------------------------------ | -------------- |
| name                | String        | 标识forms的名称                                              | NA             |
| description         | String        | 标识forms的描述                                              | stage模型新增  |
| type                | String        | 标签标识卡片的类型                                           | NA             |
| src                 | String        | 标签JS卡片对应的UI代码                                       | NA             |
| isDefault           | boolean       | 标识该卡片是否为默认卡片，每个hap有且只能有一个默认卡片      | NA             |
| colorMode           | String        | 标识卡片的色调，取值为auto、dark、light其中之一              | NA             |
| updateEnabled       | boolean       | 标识该卡片是否支持定时刷新                                   | NA             |
| scheduledUpdateTime | String        | 标签标识卡片顶点刷新的时间，采用24小时计数，精确到分钟       | NA             |
| updateDuration      | int           | 标识卡片定时刷新的更新频率，单位为30分钟，取值为30的倍数值   | NA             |
| supportDimensions   | List\<String> | 标识卡片外观规格，取值为”1 * 2“，”2 * 2“，”2 * 4“，”4 * 4“   | NA             |
| defaultDimension    | String        | 标识卡片默认外观规格，取值必须在supportDimensions配置的列表中 | NA             |
| formConfigAbility   | String        | 标识卡片调整的Ability名称                                    | NA             |
| formVisibleNotify   | String        | 标识卡片是否被允许使用卡片可见性通知                         | NA             |
| MetaData            | MetaData      | 标识卡片的自定义信息                                         | 只有CustomData |
| providerAbility     | String        | 卡片的提供方所在的Ability或者extension名称，1.FA模型：如果卡片配置在service类型的ability中，providerAbility配置为mainAbility 2.FA模型：如果卡片配置在Page类型的Ability中，providerAbility配置为当前Ability 3.FA模型：如果没有配置mainAbility，providerAbility配置为当前hap包中的优先使用system.home，否则第一个page的Ability 4.stage模型中（follow上述规则），providerAbility配置为mainElement | NA             |

### 4.25 CommonEvent结构体信息

| 字段       | 类型          | 描述                                   | 备注                                             |
| ---------- | ------------- | -------------------------------------- | ------------------------------------------------ |
| name       | String        | 当前静态公共事件对应的类名             | Stage模型从staticSubscriber类型的Extension中获取 |
| permission | String        | 标识实现该静态公共事件需要申请的权限   | Stage模型从staticSubscriber类型的Extension中获取 |
| data       | List\<String> | 当前静态公共时间需要携带的额外数据数组 | Stage模型从staticSubscriber类型的Extension中获取 |
| type       | List\<String> | 配置当前静态公共时间的类别数组         | Stage模型从staticSubscriber类型的Extension中获取 |
| events     | List\<String> | 标识能够接收的意图的event值的集合      | Stage模型从staticSubscriber类型的Extension中获取 |

### 4.26 DependencyItem结构体信息

| 字段           | 类型   | 描述             | 备注 |
|--------------| ------ |----------------| ---- |
| bundleName   | String | 共享包的bundleName | NA   |
| moduleName   | String | 共享包的moduleName | NA   |









