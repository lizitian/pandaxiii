Design of testing software for prototype electronics in PandaX-III project
==========================================================================

Windows开发环境配置
-------------------

软件基于Qt 4框架进行开发，开发环境配置步骤如下：
1. 安装Visual Studio 2010，下载[vs_ultimateweb.exe](https://download.microsoft.com/download/4/0/6/4067968E-5530-4A08-B8EC-17D2B3F02C35/vs_ultimateweb.exe)。注：Qt 4.8仅支持VS 2008和VS 2010，如果使用其他版本VS，需要自行编译Qt库，或移植程序使用Qt 5。
2. 安装Visual Studio 2010 SP1，下载[VS10sp1-KB983509.exe](https://download.microsoft.com/download/2/3/0/230C4F4A-2D3C-4D3B-B991-2A9133904E35/VS10sp1-KB983509.exe)。
3. （建议）使用Windows更新，选择有关Visual Studio 2010的补丁，为VS 2010更新。注：为保证dll运行库的一致性，建议安装SP1补丁和更新；如不安装更新或者使用VS 2008，需要复制当前使用的crt版本到程序目录。
4. 安装Qt 4.8.7，下载[qt-opensource-windows-x86-vs2010-4.8.7.exe](https://download.qt.io/official_releases/qt/4.8/4.8.7/qt-opensource-windows-x86-vs2010-4.8.7.exe)。
5. 安装最新版Qt Creator开发环境，下载[qt-creator-opensource-windows-x86-x.x.x.exe](https://download.qt.io/official_releases/qtcreator/)。

Linux开发环境配置
-----------------

1. 从系统源安装Qt 4.8、gcc和make等工具。
2. 依次运行qmake、make命令。
