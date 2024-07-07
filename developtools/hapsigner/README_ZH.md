# Hap包签名工具

* 简介
* 目录
* 约束
* 编译构建
* 说明
  * 签名相关文件用法说明
  * 使用说明 
  * 接口说明 
* 相关仓

#### 简介

为了保证OpenHarmony应用的完整性和来源可靠，在应用构建时需要对应用进行签名。经过签名的应用才能在真机设备上安装、运行、和调试。本仓提供了签名工具的源码，包含密钥对生成、CSR文件生成、证书生成、Profile文件签名、Hap包签名等功能。


#### 目录

    developtools_hapsigner

    ├── autosign                # 一键签名脚本
	├── dist                    # SDK预置文件
    ├── hapsigntool             # 主代码
          ├──hap_sign_tool      # 主程序入口，完成输入参数的基础校验
          ├──hap_sign_tool_lib  # 签名工具库，解析命令字和参数列表，实现各模块逻辑功能
    ├── tools                   # 自动化测试脚本



#### 约束
Hap包签名工具基于Java语言开发，需要在Java8以上Java环境运行
（附：一键签名等脚本文件基于Python语言开发，使用需配置环境python3.x）
#### 编译构建

 1. 该工具基于Gradle 7.1编译构建，请确认环境已安装配置Gradle环境，并且版本正确
  
        gradle -v

 2. 下载代码，命令行打开文件目录至developtools_hapsigner/hapsigntool，执行命令进行编译打包
            
        gradle build 或者 gradle jar

 3. 编译后得到二进制文件，目录为: ./hap_sign_tool/build/libs/hap-sign-tool.jar

****
#### 说明
##### 签名相关文件用法说明

开发者通过IDE进行应用签名时，可在SDK中会获得如下签名相关文件：

```
签名密钥库文件：OpenHarmony.p12
Profile签名证书：OpenHarmonyProfileRelease.pem、OpenHarmonyProfileDebug.pem
Profile模板文件：UnsgnedReleasedProfileTemplate.json、UnsgnedDebugProfileTemplate.json
签名工具：hap-sign-tool.jar
```
上述文件的使用场景如下图所示。

**Profile签名场景：**

![signprofile.png](figures/signprofile_zh.png)

**应用签名场景：**

![signapp.png](figures/signapp_zh.png)
##### 使用说明

以下说明中使用jar包为编译构建中生成的二进制文件

1.命令行签名
   命令行签名分为profile文件签名和hap包签名。

   （1）签名profile文件的命令实例如下：

   
```shell
java -jar hap-sign-tool.jar  sign-profile -keyAlias "oh-profile1-key-v1" -signAlg "SHA256withECDSA" -mode "localSign" -profileCertFile "result\profile1.pem" -inFile "app1-profile-release.json" -keystoreFile "result\ohtest.jks" -outFile "result\app1-profile.p7b" -keyPwd "123456" -keystorePwd "123456"
```
该命令的参数说明如下：

    sign-profile : ProvisionProfile文件签名
         ├── -mode              #签名模式，必填项，包括localSign，remoteSign
         ├── -keyAlias          #密钥别名，必填项
         ├── -keyPwd            #密钥口令，可选项
         ├── -profileCertFile   #Profile签名证书（证书链，顺序为最终实体证书-中间CA证书-根证书），必填项
         ├── -inFile            #输入的原始Provision Profile文件，必填项
         ├── -signAlg           #签名算法，必填项，包括SHA256withECDSA / SHA384withECDSA
         ├── -keystoreFile      #密钥库文件，localSign模式时为必填项，JKS或P12格式
         ├── -keystorePwd       #密钥库口令，可选项
         ├── -outFile           #输出签名后的Provision Profile文件，p7b格式，必填项



（2）签名Hap包的命令实例如下：


```shell
java -jar hap-sign-tool.jar sign-app -keyAlias "oh-app1-key-v1" -signAlg "SHA256withECDSA" -mode "localSign" -appCertFile "result\app1.pem" -profileFile "result\app1-profile.p7b" -inFile "app1-unsigned.zip" -keystoreFile "result\ohtest.jks" -outFile "result\app1-unsigned.hap" -keyPwd "123456" -keystorePwd "123456"
```
该命令的参数说明如下:

    sign-app : hap应用包签名
         ├── -mode              #签名模式，必填项，包括localSign，remoteSign
         ├── -keyAlias          #密钥别名，必填项
         ├── -keyPwd            #密钥口令，可选项
         ├── -appCertFile       #应用签名证书文件（证书链，顺序为最终实体证书-中间CA证书-根证书），必填项
         ├── -profileFile       #签名后的Provision Profile文件名，p7b格式，必填项
         ├── -profileSigned     #指示profile文件是否带有签名，1表示有签名，0表示没有签名，默认为1。可选项
         ├── -inForm            #输入的原始文件的格式，zip格式或bin格式，默认zip格式；可选项
         ├── -inFile            #输入的原始APP包文件，zip格式或bin格式，必填项
         ├── -signAlg           #签名算法，必填项，包括SHA256withECDSA / SHA384withECDSA
         ├── -keystoreFile      #密钥库文件，localSign模式时为必填项，JKS或P12格式
         ├── -keystorePwd       #密钥库口令，可选项
         ├── -outFile           #输出签名后的包文件，必填项
         

2.一键签名


为降低学习成本，提高开发效率，本项目还将基于应用签名工具提供一键签名脚本，免于输入繁杂的参数命令，脚本内容包括生成密钥对、最终实体证书、签名profile包、签名hap包的命令。
脚本以及配置文件位于目录autosign下：

 - create_root.sh/create_root.bat
 - create_appcert_sign_profile.sh/create_appcert_sign_profile.bat
 - sign_hap.sh/sign_hap.bat
 - createAppCertAndProfile.config
 - createRootAndSubCert.config
 - signHap.config

使用指导：
1. 准备依赖环境python3.5以上
2. 准备签名工具jar包：hap-sign-tool.jar（参照上文编译生成的产物）
3. 准备待签名的应用hap包和Provision profile模板文件
4. 使用文本编辑器编辑createAppCertAndProfile.config,signHap.config修改配置文件中的配置信息：common.keyPwd 和 common.issuerKeyPwd 参数值改成自己定义的口令信息
5. Linux运行create_appcert_sign_profile.sh、Windows运行create_appcert_sign_profile.bat生成签名所需文件
6. Linux运行sign_hap.sh、Windows运行sign_hap.bat对hap包进行签名

 > 说明：如需自定义生成密钥库文件，根CA，中间CA证书，profile签名证书，可执行以下步骤
 1.使用文本编辑器编辑createRootAndSubCert.config修改配置文件中的配置信息：common.keyPwd 和 common.issuerKeyPwd 参数值改成自己定义的口令信息
 2.Linux运行 create_root.sh、Windows运行create_root.bat生成所需密钥库文件，根CA，中间CA证书，profile签名证书


****
##### 接口说明
1.生成密钥对

     generate-keypair : 生成密钥对
         ├── -keyAlias          # 密钥别名，必填项
         ├── -keyPwd            # 密钥口令，可选项
         ├── -keyAlg            # 密钥算法，必填项，包括RSA/ECC
         ├── -keySize           # 密钥长度，必填项，RSA算法的长度为2048/3072/4096，ECC算法的长度NIST-P-256/NIST-P-384
         ├── -keystoreFile      # 密钥库文件，必填项，JKS或P12格式
         ├── -keystorePwd       # 密钥库口令，可选项

2.生成证书签名请求

    generate-csr : 生成证书签名请求
         ├── -keyAlias          # 密钥别名，必填项
         ├── -keyPwd            # 密钥口令，可选项
         ├── -subject           # 证书主题，必填项
         ├── -signAlg           # 签名算法，必填项，包括SHA256withRSA / SHA384withRSA / SHA256withECDSA / SHA384withECDSA
         ├── -keystoreFile      # 密钥库文件，必填项，JKS或P12格式
         ├── -keystorePwd       # 密钥库口令，可选项
         ├── -outFile           # 输出文件，可选项，如果不填，则直接输出到控制台

3.生成根CA/中间CA证书

    generate-ca : 生成根CA/中间CA证书，如果密钥不存在，一起生成密钥
         ├── -keyAlias                        # 密钥别名，必填项
         ├── -keyPwd                          # 密钥口令，可选项
         ├── -keyAlg                          # 密钥算法，必填项，包括RSA/ECC
         ├── -keySize                         # 密钥长度，必填项，RSA算法的长度为2048/3072/4096，ECC算法的长度NIST-P-256/NIST-P-384
         ├── -issuer                          # 颁发者的主题，可选项，如果不填，表示根CA
         ├── -issuerKeyAlias                  # 颁发者的密钥别名，可选项，如果不填，表示根CA
         ├── -issuerKeyPwd                    # 颁发者的密钥口令，可选项
         ├── -subject                         # 证书主题，必填项
         ├── -validity                        # 证书有效期，可选项，默认为3650天
         ├── -signAlg                         # 签名算法，必填项，包括SHA256withRSA / SHA384withRSA / SHA256withECDSA / SHA384withECDSA
         ├── -basicConstraintsPathLen         # 路径长度，可选项，默认为0
         ├── -issuerKeystoreFile              # 签发者密钥库文件，可选项，JKS或P12格式
         ├── -issuerKeystorePwd               # 签发者密钥库口令，可选项
         ├── -keystoreFile                    # 密钥库文件，必填项，JKS或P12格式
         ├── -keystorePwd                     # 密钥库口令，可选项
         ├── -outFile                         # 输出文件，可选项，如果不填，则直接输出到控制台

4.生成应用调试/发布证书

    generate-app-cert : 生成应用调试/发布证书
         ├── -keyAlias                        # 密钥别名，必填项
         ├── -keyPwd                          # 密钥口令，可选项
         ├── -issuer                          # 颁发者的主题，必填项
         ├── -issuerKeyAlias                  # 颁发者的密钥别名，必填项
         ├── -issuerKeyPwd                    # 颁发者的密钥口令，可选项
         ├── -subject                         # 证书主题，必填项
         ├── -validity                        # 证书有效期，可选项，默认为3650天
         ├── -signAlg                         # 签名算法，必填项，包括SHA256withECDSA / SHA384withECDSA；
         ├── -keystoreFile                    # 密钥库文件，必填项，JKS或P12格式
         ├── -keystorePwd                     # 密钥库口令，可选项
         ├── -issuerKeystoreFile              # 签发者密钥库文件，可选项，JKS或P12格式
         ├── -issuerKeystorePwd               # 签发者密钥库口令，可选项
         ├── -outForm                         # 输出证书文件的格式，包括 cert / certChain，可选项，默认为certChain
         ├── -rootCaCertFile                  #  outForm为certChain时必填，根CA证书文件
         ├── -subCaCertFile                   #  outForm为certChain时必填，中间CA证书文件
         ├── -outFile                         #  输出证书文件(证书或证书链)，可选项，如果不填，则直接输出到控制台

5.生成profile调试/发布证书

    generate-profile-cert : 生成profile调试/发布证书
         ├── -keyAlias                        # 密钥别名，必填项
         ├── -keyPwd                          # 密钥口令，可选项
         ├── -issuer                          # 颁发者的主题，必填项
         ├── -issuerKeyAlias                  # 颁发者的密钥别名，必填项
         ├── -issuerKeyPwd                    # 颁发者的密钥口令，可选项
         ├── -subject                         # 证书主题，必填项
         ├── -validity                        # 证书有效期，可选项，默认为3650天
         ├── -signAlg                         # 签名算法，必填项，包括SHA256withECDSA / SHA384withECDSA；
         ├── -keystoreFile                    # 密钥库文件，必填项，JKS或P12格式
         ├── -keystorePwd                     # 密钥库口令，可选项
         ├── -issuerKeystoreFile              # 签发者密钥库文件，可选项，JKS或P12格式
         ├── -issuerKeystorePwd               # 签发者密钥库口令，可选项
         ├── -outForm                         # 输出证书文件的格式，包括 cert / certChain，可选项，默认为certChain
         ├── -rootCaCertFile                  #  outForm为certChain时必填，根CA证书文件
         ├── -subCaCertFile                   #  outForm为certChain时必填，中间CA证书文件
         ├── -outFile                         #  输出证书文件(证书或证书链)，可选项，如果不填，则直接输出到控制台

6.通用证书生成，可以生成自定义证书

    generate-cert : 通用证书生成，可以生成自定义证书
          ├── -keyAlias                          # 密钥别名，必填项
          ├── -keyPwd                            # 密钥口令，可选项
          ├── -issuer                            # 颁发者的主题，必填项
          ├── -issuerKeyAlias                    # 颁发者的密钥别名，必填项
          ├── -issuerKeyPwd                      # 颁发者的密钥口令，可选项
          ├── -subject                           # 证书主题，必填项
          ├── -validity                          # 证书有效期，可选项，默认为1095天
          ├── -keyUsage                          # 密钥用法，必选项，包括digitalSignature, nonRepudiation, keyEncipherment,
          ├                                        dataEncipherment, keyAgreement, certificateSignature, crlSignature，
          ├                                        encipherOnly和decipherOnly，如果证书包括多个密钥用法，用逗号分隔
          ├── -keyUsageCritical                  # keyUsage是否为关键项，可选项，默认为是
          ├── -extKeyUsage                       # 扩展密钥用法，可选项，包括clientAuthentication，serverAuthentication，
          ├                                        codeSignature，emailProtection，smartCardLogin，timestamp，ocspSignature
          ├── -extKeyUsageCritical               # extKeyUsage是否为关键项，可选项，默认为否
          ├── -signAlg                           # 签名算法，必填项，包括SHA256withRSA/SHA384withRSA/SHA256withECDSA/SHA384withECDSA 
          ├── -basicConstraints                  # 是否包含basicConstraints，可选项，默认为否
          ├── -basicConstraintsCritical          # basicConstraints是否包含为关键项，可选项，默认为否
          ├── -basicConstraintsCa                # 是否为CA，可选项，默认为否
          ├── -basicConstraintsPathLen           # 路径长度，可选项，默认为0
          ├── -issuerKeystoreFile                # 签发者密钥库文件，可选项，JKS或P12格式
          ├── -issuerKeystorePwd                 # 签发者密钥库口令，可选项
          ├── -keystoreFile                      # 密钥库文件，必填项，JKS或P12格式
          ├── -keystorePwd                       # 密钥库口令，可选项
          ├── -outFile                           # 输出证书文件，可选项，如果不填，则直接输出到控制台

7.ProvisionProfile文件签名

    sign-profile : ProvisionProfile文件签名
          ├── -mode            # 签名模式，必填项，包括localSign，remoteSign
          ├── -keyAlias        # 密钥别名，必填项
          ├── -keyPwd          # 密钥口令，可选项
          ├── -profileCertFile # Profile签名证书（证书链，顺序为最终实体证书-中间CA证书-根证书），必填项
          ├── -inFile          # 输入的原始Provision Profile文件，必填项
          ├── -signAlg         # 签名算法，必填项，包括SHA256withECDSA / SHA384withECDSA
          ├── -keystoreFile    # 密钥库文件，localSign模式时为必填项，JKS或P12格式
          ├── -keystorePwd     # 密钥库口令，可选项
          ├── -outFile         # 输出签名后的Provision Profile文件，p7b格式，必填项

8.ProvisionProfile文件验签

     verify-profile : ProvisionProfile文件验签
           ├── -inFile       # 已签名的Provision Profile文件，p7b格式，必填项
           ├── -outFile       # 验证结果文件（包含验证结果和profile内容），json格式，可选项；如果不填，则直接输出到控制台

9.hap应用包签名
  
     sign-app : hap应用包签名 
          ├── -mode          # 签名模式，必填项，包括localSign，remoteSign，remoteResign
          ├── -keyAlias      # 密钥别名，必填项
          ├── -keyPwd        # 密钥口令，可选项
          ├── -appCertFile   # 应用签名证书文件（证书链，顺序为最终实体证书-中间CA证书-根证书），必填项
          ├── -profileFile   # 签名后的Provision Profile文件名，profileSigned为1时为p7b格式，profileSigned为0时为json格式,必填项
          ├── -profileSigned # 指示profile文件是否带有签名，1表示有签名，0表示没有签名，默认为1。可选项
          ├── -inForm        # 输入的原始文件的格式，zip格式或bin格式，默认zip格式，可选项
          ├── -inFile        # 输入的原始APP包文件，zip格式或bin格式，必填项
          ├── -signAlg       # 签名算法，必填项，包括SHA256withECDSA / SHA384withECDSA
          ├── -keystoreFile  # 密钥库文件，localSign模式时为必填项，JKS或P12格式
          ├── -keystorePwd   # 密钥库口令，可选项
          ├── -outFile       # 输出签名后的包文件，必填项

10.hap应用包文件验签

      verify-app : hap应用包文件验签
         ├── -inFile          # 已签名的应用包文件，zip格式或bin格式，必填项
         ├── -outCertChain    # 签名的证书链文件，必填项
         ├── -outProfile      # 应用包中的profile文件，必填项



  
#### 相关仓
   不涉及