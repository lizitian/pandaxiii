PandaX-III实验原型电子学测试软件的设计
======================================


软件基于Qt 4框架进行开发，参看[Qt 4.8官方文档](https://doc.qt.io/qt-4.8/)。应用界面ui对象采用[单继承、成员指针变量方式](https://doc.qt.io/qt-4.8/designer-using-a-ui-file.html)，涉及ui对象的操作封装于ui.cpp，因此只有ui.cpp文件需#include "ui_pandaxiii.h"文件，其他代码文件仅需#include "pandaxiii.h"。

目录结构
--------

```
dlls/         发布所需的动态库文件
.gitignore    Git版本控制文件
README.md     此文件
*.pro         项目文件
*.ui          用户界面文件
*.h           应用程序头文件
*.cpp         应用程序代码文件
```

Windows开发环境配置
-------------------

1. 安装Visual Studio 2010，下载[vs_ultimateweb.exe](https://download.microsoft.com/download/4/0/6/4067968E-5530-4A08-B8EC-17D2B3F02C35/vs_ultimateweb.exe)。注：无需激活，安装试用版即可；另外Qt 4.8仅支持VS 2008和VS 2010，如果使用其他版本VS，需要自行编译Qt库，或移植程序使用Qt 5。

![Step 1](https://lizitian.com/pandaxiii/s1.png)

2. 安装Visual Studio 2010 SP1，下载[VS10sp1-KB983509.exe](https://download.microsoft.com/download/2/3/0/230C4F4A-2D3C-4D3B-B991-2A9133904E35/VS10sp1-KB983509.exe)。

![Step 2](https://lizitian.com/pandaxiii/s2.png)

3. （建议）使用Windows更新，选择有关Visual Studio 2010的补丁，为VS 2010更新。注：为保证dll运行库的一致性，建议安装SP1补丁和更新；如不安装更新或者使用VS 2008，需要复制当前使用的crt版本到程序目录。

![Step 3](https://lizitian.com/pandaxiii/s3.png)

4. 安装Qt 4.8.7，下载[qt-opensource-windows-x86-vs2010-4.8.7.exe](https://download.qt.io/official_releases/qt/4.8/4.8.7/qt-opensource-windows-x86-vs2010-4.8.7.exe)。

![Step 4](https://lizitian.com/pandaxiii/s4.png)

5. 安装最新版Qt Creator开发环境，下载[qt-creator-opensource-windows-x86-x.x.x.exe](https://download.qt.io/official_releases/qtcreator/)。注：输入帐号的环节可以选择Skip跳过。

![Step 5](https://lizitian.com/pandaxiii/s5.png)

6. 配置Qt Creator。选择“工具”-“选项”-“构建和运行”；选择“Qt Versions”选项卡，选择“添加”，打开“C:\Qt\4.8.7\bin\qmake.exe”，选择“Apply”保存设置；选择“构建套件(Kit)”选项卡，选择“添加”，自己设置“名称”，设置“编译器”中“C”和“C++”均为“Microsoft Visual C++ Compiler 10.0 (x86)”，“Qt 版本”为“Qt 4.8.7 (4.8.7)”，选择“OK”保存设置。选择“Open Project”，找到项目文件夹，打开“pandaxiii.pro”文件，在接下来的“Configure Project”页面选择刚创建的构建套件，之后将左下角的Debug模式改为Release模式，选择“构建”-“构建项目(Ctrl+B)”编译，编译成功的文件在“build-pandaxiii-...-Release/release/pandaxiii.exe”。

![Step 6 Pic 1](https://lizitian.com/pandaxiii/s6p1.png)

![Step 6 Pic 2](https://lizitian.com/pandaxiii/s6p2.png)

![Step 6 Pic 3](https://lizitian.com/pandaxiii/s6p3.png)

![Step 6 Pic 4](https://lizitian.com/pandaxiii/s6p4.png)

![Step 6 Pic 5](https://lizitian.com/pandaxiii/s6p5.png)

Linux开发环境配置
-----------------

1. 从系统源安装Qt 4.8、gcc和make等工具

2. 依次运行：
```
qmake
make
```
