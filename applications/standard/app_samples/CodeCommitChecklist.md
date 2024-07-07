## 社区共建Sample合入要求

1. 新增Sample功能不能重复于当前已有Sample的功能；
2. 新增Sample的工程推荐使用eTS语言编写；
3. 新增Sample的工程推荐使用Stage模型编写；
4. 新增Sample的工程中需要包含UI自动化用例（ohosTest工程模块），覆盖基本的功能场景；
5. 新增Sample需要遵循[代码提交规范](#section1)、[工程结构规范](#section2)、[PR提交格式规范](#section3)、[ReadMe编写规范](#section4)和[UI自动化用例编写规范](#section5)；
6. 新增Sample的UX涉及需要符合[一多规范要求](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/key-features/multi-device-app-dev/Readme-CN.md)；
7. 新增Sample推荐使用当前最新版本IDE和SDK版本；
8. 工程合入时，需要提供测试用例报告，包括测试场景、操作步骤和测试结果信息，可以附在提交工程中；

## 代码提交规范<a name="section1"></a>

1. 所有文件，包括自动生成的编译文件package.json都要格式化；
2. 函数命名，C++大驼峰，TS、JS小驼峰，函数命名注意动宾结构；
3. 变量和参数命名，静态常量需使用全大写，函数的参数、全局变量和局部变量都使用小驼峰；
4. 函数参数命名，要规范化，见名知意；
5. if语句后必须跟“{”，哪怕只有一行代码；
6. 有break或return的条件，先break或return再走其他的逻辑；
7. 条件判断，当一个分支达成条件时及时返回，不需要再走其他分支；
8. 代码中避免出现魔鬼数字，需补上注释，或者用符合语义的名词常量代替，如下：
```ts
  // One minute has 60 seconds.
  let time = 60

  const ONE_MINUTE = 60
  let time = ONE_MINUTE
```
9. 字符串拼接使用模板字符串；
10. 组件使用，除了宽高属性可以在一行，其他的属性必须换行；
11. TS、JS中统一不带";"符号；
12. 字符串和图片资源等的使用，支持"$r"使用的，全部使用"$r"引用；
13. 新增文件注意开源协议、版权检查；
14. 截图要使用真机效果图；
15. readme中使用“.”，不要用“、”，检查错别字；
16. 异步方法中需要返回方法的返回值，不用声明变量，直接return，如下：
```ts
  return await this.mediaTest.getFileAssets(fetchOp)
```
17. 方法中的参数需要类型声明；
18. import 文件顺序，同类型放一起；
19. string.json中description要写简要描述，zh下要用中文；
20. TS、JS语言缩进为2格，C++语言缩进为4格；
21. 涉及应用截图时，图片不能包含人物、关键信息、网络等有侵权风险的资源；
22. 工程中不要配置签名信息，禁止上传local.properties和package-lock.json等系统自动生成的文件；
23. 规范日志格式，统一用[Sample_包名]开头，如时钟日志，使用[Sample_Clock]，日志打印需要使用Hilog接口，建议封装单独的[Logger类](https://gitee.com/openharmony/applications_app_samples/blob/master/MultiDeviceAppDev/Weather/product/src/main/ets/util/Logger.ts)，示例如下：
```ts
import hilog from '@ohos.hilog'
...
class Logger {
  private domain: number = 0xFF00
  private prefix: string = '[Sample_Clock]'
  private format: string = '%{public}, %{public}'
  ...
  debug(...args: any[]) {
    hilog.debug(this.domain, this.prefix, this.format, args)
  }
  ...
}
```
24. 注释“//”后要加一个空格；如果注释跟在代码后面，则“//”前要加一个空格；
```ts
  // 正确示例
  let a = 10

  let a = 10 // 正确示例
```
25. 代码中避免出现中文字符，要使用资源代替，符合国际化开发标准；


## 工程结构规范<a name="section2"></a>

Sample工程，应该区分开UI、业务逻辑、数据模块，工程示例结构如下：

```
main
|---Application // 应用级逻辑
|---MainAbility // 元能力组件
|---feature // 业务功能
|   |---IndexFeature.ts // Index的业务逻辑代码
|---pages // UI页面
|   |---Index.ets // Index的UI界面代码
|---model // 数据
|   |---IndexData.ts // Index数据模型
|---mock // Mock数据
|   |---MockIndexData.ts // IndexData的mock数据
|---net // 网络
|   |---IndexApi.ts // 网络接口
```

## PR提交格式规范<a name="section3"></a>

示例如下：

```
IssueNo: #I56EH7:关联问题
Description: 描述问题.
Sig: sig-systemapplications
Feature or Bugfix: Bugfix
Binary Source: No

Signed-off-by: jiangwensai <jiangwensai@huawei.com>
```

1. IssueNo管理issue信息；
2. Description描述修改变更内容；
3. Sig统一使用SIG_Sample；
4. Feature or Bugfix，如果是需求选择Feature，问题选择Bugfix；
5. Signed-off-by，注明开发者账号和邮箱；
	
## ReadMe编写规范<a name="section4"></a>

1. 标题：以特性名称命名；
2. 介绍：

	2.1 介绍sample的具体功能;

	2.2 需要介绍本sample的主要实现原理，如使用了什么API、有什么关键性的配置和实现等等；

	2.3 介绍应用的使用说明，具体的操作步骤和用法信息；

3. 效果预览：屏幕截屏或者视频，文件不超过4个；
4. 相关权限：介绍应用的使用权限，附上链接；
5. 依赖： 介绍对其他sample的依赖，附上sample链接；
6. 约束与限制：

	6.1 支持应用运行的操作系统版本和设备，示例如下：

		本示例仅支持标准系统上运行，支持设备：RK3568;

	6.2 API版本、SDK版本（如果依赖Full SDK，附上替换链接），示例如下：

		本示例仅支持APIXX版本SDK，版本号：3.X.X.X；（如果涉及Full SDK还需加上：本涉及涉及使用系统接口：XXX，需要手动替换Full SDK才能编译通过，具体操作可参考[替换指南]。）

	6.3 支持的IDE版本，示例如下：

		本示例需要使用DevEco Studio 3.0 Beta4 (Build Version: 3.0.0.992, built on July 14, 2022)才可编译运行；

	6.4 高等级APL特殊签名说明，示例如下：

		本示例涉及[相关权限]为system_basic(或者system_core)级别（相关权限级别可通过[权限定义列表]查看），需要配置高权限签名，可参考[特殊权限配置方法]；
		
[ReadMe模板链接](SampleReadTemplate.md)

## UI自动化用例编写规范<a name="section5"></a>

1. 用例命名规范为：“包名_测试功能_序号”，如“MyApp_StartAbility_001”，序号表示测试StartAbility功能的第1个用例;
2. 用例的开头和结尾都需要有日志打印；
3. 打印必须包括用例名称的关键字，用例中每条断言语句前必须添加日志，打印参数信息；
4. DOMAIN设置为0xF811；
5. 用例名称中的包名和日志中的包名单独用常量表示，方便包名变更整改；
6. 每条用例前需要添加注释；
```ts
  import hilog from '@ohos.hilog';
  ...
  const TAG = '[Sample_MyApp]'
  const DOMAIN = 0xF811
  const BUNDLE = 'MyApp_'
  ...
  
  /**
   * 介绍用例测试的功能和场景等信息
   */
  it(BUNDLE + 'StartAbility_001', function() {
    hilog.info(DOMAIN, TAG, BUNDLE + "StartAbility_001, begin")
    ...
	// 介绍单一步骤的功能
    hilog.info(DOMAIN, TAG, BUNDLE + "StartAbility_001, code:" + code)
    expect(0).asserEqual(code)
    ...
    hilog.info(DOMAIN, TAG, BUNDLE + "StartAbility_001, end")
  })
```