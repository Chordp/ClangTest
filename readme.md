---
title: 如何使用LLVM编译Windows驱动(混淆&Asm)
date: 2020-10-14 15:10:47
tags:
---
### 前言
---
> 总所周知 世界上最好的IDE **Viusal studio** 所用的msvc编译器无法使用x64的内联汇编
> 当然我们可以选择intel 的编译器 这里我们提供一个选择
> llvm对vs兼容性没有 intel 编译器好 但是他还有个功能是intel没有的 就是混淆

<!--more-->

#### 混淆效果&内联汇编
---
混淆前代码
```
#include "DriverEntry.h"

static VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	return;
}
EXTERN_C  NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObj, IN PUNICODE_STRING pRegistryString)
{
	UNREFERENCED_PARAMETER(pRegistryString);
	ULONG64 _Rax = 0;

	for (size_t i = 0; i < 10; i++)
	{
		DPRINT("Test\n");
	
	}
	__asm
	{
		mov _Rax, rax
	}
	DPRINT("Rax = %x\n", _Rax);

	pDriverObj->DriverUnload = DriverUnload;

	return STATUS_UNSUCCESSFUL;
}
```
混淆后IDA F5
![图 1](https://chordp.coding.net/p/hexo/d/blog/git/raw/master/4d5f75a4d5f282938b4e85fcf60921adcf86c162d960bf79ce0146c1c4f61543.png)  
![图 2](https://chordp.coding.net/p/hexo/d/blog/git/raw/master/5f1523b8f84d86ec6def2592c3f8ccff6d0112b83481f1c1f8c5891d54494642.png)  
这里我们使用的是 [goron](https://github.com/amimo/goron)



#### 如何实现
##### 1.下载安装官方版本LLVM
> 这一步其实自己设置环境变量 
```
LLVM_DIR
```

LLVM 官方下载地址:[LLVM](https://releases.llvm.org/)
##### 2.安装LLVM插件
**Vs2019** 插件下载地址:[llvm2019](https://marketplace.visualstudio.com/items?itemName=MarekAniola.mangh-llvm2019)
**Vs2017** 插件下载地址:[LLVM Compiler Toolchain](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.llvm-toolchain)
**2017以下不支持**

##### 3.修改WDK配置
###### 1.打开**PlatformToolsets**目录
我的:
``` 
D:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Microsoft\VC\v160\Platforms\x64\PlatformToolsets
```
###### 2.复制WindowsKernelModeDriver10.0并重命名
如下
![图 3](https://chordp.coding.net/p/hexo/d/blog/git/raw/master/86e3eb7cac42d41325dcf6b651d7785b1b6eea9cd8eb4c3882520ebe5b9fab6c.png)  
###### 3.修改WindowsKernelModeDriver10.0_LLVM中的配置
* Toolset.props
```
  <ClangPropsFile>$(VCTargetsPath)\Platforms\$(Platform)\PlatformToolsets\llvm\Toolset.props</ClangPropsFile>
  <Import Condition="Exists('$(ClangPropsFile)')" Project="$(ClangPropsFile)" />
```
![图 4](https://chordp.coding.net/p/hexo/d/blog/git/raw/master/1a164385b82fa23a6d879b42bd9fc8774118271f6183c5a4a9a435925408f2aa.png)  

* Toolset.targets
```
    <ClangTargetsFile>$(VCTargetsPath)\Platforms\$(Platform)\PlatformToolsets\llvm\Toolset.targets</ClangTargetsFile>
    <Import Condition="Exists('$(ClangTargetsFile)')" Project="$(ClangTargetsFile)" />
```

![图 5](https://chordp.coding.net/p/hexo/d/blog/git/raw/master/6529826c42712b6eb16208f6460314175025ce217721af553656d130cab026df.png)  

###### 4.配置项目
* 平台工具集 选择 WindowsKernelModeDriver10.0_LLVM

![图 6](https://chordp.coding.net/p/hexo/d/blog/git/raw/master/c310ef9a9e2b41b4ab6005ecf2478b514b28961ce7a7de8a76013d0bb3758d44.png)  
* 选择了我们上面修改的工具及 这里就会出现 **LLVM** 选项 我们这里只需要 **Clang-cl** 其他全选**否**(vs2017 有三个选项)
![图 7](https://chordp.coding.net/p/hexo/d/blog/git/raw/master/fa5337795e89a9969ffd32d2487c48e1ea8fcde27b2ec188c5897db381182e96.png)  
* 关闭Qspectre 缓解
![图 8](https://chordp.coding.net/p/hexo/d/blog/git/raw/master/0ab64f4e7d7fd76d5f1c623a9da1e90eab69546aa5b8995835f32eff03f6327e.png)  
* c/c++ 命令行  从父级或项目默认设置继承 取消勾选

###### 5. 可选
* 使用带混淆的llvm 比如 [ollvm](https://github.com/heroims/obfuscator/tree/llvm-9.0) 或者 [goron](https://github.com/amimo/goron) 等其他可以在
c/c++ 命令行 其他选项 添加混淆选项 以下是[goron](https://github.com/amimo/goron)的混淆选项
```
-mllvm -irobf-cse -mllvm -irobf-indbr -mllvm -irobf-icall -mllvm -irobf-indgv -mllvm -irobf-cff 
```
![图 10](https://chordp.coding.net/p/hexo/d/blog/git/raw/master/d30127101441e031cec5865082467ac2291fc9ce7180f4bc27696837d1f0153c.png)  
###### 6. 项目地址
* 里面包括修改好的项目 以及 修改好的编译器配置
[项目地址](https://github.com/Chordp/ClangTest)
