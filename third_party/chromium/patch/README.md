# chromium patch information

## 补丁介绍
主要介绍各个patch的相关信息

| 名称 | 描述 |
| --- | --- |
| 0001-cve.patch | 针对Chromium CVE漏洞的修复 |
| 0002-cef.patch | CEF代码 |
| 0003-3.2-Beta3-1115.patch | OpenHarmony特性代码 |
| 0004-3.2-Beta5.patch | OpenHarmony 3.2 Beta5 特性代码 |
| 0005-3.2-Release.patch | OpenHarmony 3.2 Release 特性代码 |


## 如何打补丁

```
cd src
git apply --whitespace=nowarn --ignore-whitespace -p2 ../patch/*.patch
```

上述命令会依次应用所有补丁。
