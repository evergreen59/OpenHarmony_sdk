# NAPI框架生成工具集成测试

## 简介
  本文主要介绍如何将NAPI框架生成代码集成到OpenHarmony系统，进而进行集成测试。

## 准备

  1.硬件：rk3566开发套件。

  2.系统镜像：

  系统镜像的具体生成方法，可以左键单击以下链接了解：

  [生成代码集成到OpenHarmony](https://gitee.com/openharmony/napi_generator/blob/master/docs/ENSEMBLE_METHOD_ZH.md)

  3.应用hap包：hap包及源码路径如下：

```	
napi_generator/examples/app
```

  hap包的具体生成方法，可参考OpenHarmony/docs/zh-cn/application-dev文档中使用JS语言开发（FA模型）。其中修改index.js文件内容如下：


```	

	import router from '@ohos.router';
	import napitest from '@ohos.napitest';
	export default {
    	data: {
        	title: ""
    	},
    	onInit(){
        	this.title = this.$t('strings.world');
    	},
    	onclick: function () {
        	router.push({
            	url: "pages/second/second"
        	})
    	},
    	ontest: function () {
        	console.log("napitest  begin AAAAAAAAAAAAAAAAAA")
        	var  Entity = {
            	ENTITY_DEFAULT : "entity.system.default",
            	ENTITY_HOME : "entity.system.home",
            	ENTITY_VOICE : "entity.system.voice",
            	ENTITY_BROWSABLE : "entity.system.browsable",
            	ENTITY_VIDEO : "entity.system.video"
        	}
        	napitest.Space3.fun1("ggggg",Entity.ENTITY_DEFAULT);
        	console.log("napitest  end AAAAAAAAAAAAAAAAAA")
    		}
	}
```
  修改index.html文件内容如下：

```

	<!--index.hml-->
	<div class="container">
    	<text class="title">
        	Hello World
    	</text>
	<!-- 添加按钮，值为Next，并绑定onclick方法-->
    	<input class="btn" type="button" value="Next" onclick="onclick"></input>
    	<input class="btn" type="button" value="napitest" onclick="ontest"></input>
	</div>
```

## 使用说明

步骤一：安装镜像环境。

	 harmony@Ubuntu-64:~/OpenHarmony/out/rk3568-khdvk/packages/phone/images/pack$ ll
	 总用量 3329884
	 drwxrwxr-x 2 harmony harmony       4096 9月  27 14:14  ./
	 drwxrwxr-x 3 harmony harmony       4096 9月  27 15:30  ../
	 -rw-rw-r-- 1 harmony harmony 3409787466 9月  27 14:14 RK3568-KHDVK-RAMDISK-GPT-20220929-1817.img

步骤二：安装hap包。

  hdc中输入命令安装hap包：

	.\hdc.exe install -r E:\dingding\napihap\entry-release-standard-ark-signed.hap

  执行完成后，设备中会出现安装的APP。

步骤三：打印日志并验证结果。

  hap包安装成功后，进入hdc shell，输入命令实时打印日志并输出至windows中。

	.\hdc.exe hilog > log.txt

  然后单击设备中安装的APP，进入APP后单击测试按钮，执行完成后会在hdc安装目录下出现log.txt文件。

## 查看结果
log.txt中包含“======fun1(name: string, flags: Entity): number======”日志表示接口调用成功。如下所示：

    01-01 00:13:10.355  2020  2027 I 00000/NAPITESTNAPILayer: fun1_middle:93 *******fun1_middle begin**********
    01-01 00:13:10.357  2020  2038 D 01400/OHOS::ROSEN: RSRenderThread ProcessCommands size: 2
    01-01 00:13:10.358  2020  2038 D 01400/OHOS::ROSEN: RSRenderThread DrawFrame(790351535051) in GPU
    01-01 00:13:10.360  2020  2027 I 00000/NAPITESTNAPILayer: fun1_middle:107 *****fun1_middle xxxxx**********
    01-01 00:13:10.360  2020  2027 I 00000/NAPITESTNAPILayer: fun1:28 ======fun1(name: string, flags: Entity): number======
    01-01 00:13:10.360  2020  2027 I 00000/NAPITESTNAPILayer: fun1_middle:113 *******fun1_middle   end*********

## 相关仓

暂无
