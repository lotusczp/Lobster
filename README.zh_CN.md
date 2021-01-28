# Lobster

[English](./README.md) | 简体中文

Lobster是一个通用的上位机，主要用于嵌入式系统的远程调试与测试，也可以连接设备，进行自动脚本测试。 主要功能是可以通过插件与远程设备通信，读取采样值，写入控制量。主要功能如下：
- Lobster与远程的通信都是通过插件（DLL）进行的，用户可以根据需要定制自己的插件
- 显示数据的视窗，同时也是一个控制台
- 实时显示数据波形的示波器，其操作类似于CANoe/CANalyzer
- 数据存储功能，供后续分析
- 数据回放系统，可以载入之前记录的存储数据
- 一个基于JavaScript的脚本系统，可以读写数据及进行其他控制，用于建立自动调试、测试系统
- 控制页面，可用于用户自定义UI（基于Web）

界面截图
![avatar](https://raw.githubusercontent.com/lotusczp/images/main/snapshot_lobster.png)

## 插件系统

Lobster主程序不提供与嵌入式系统的数据交互。所有的数据传输都是由插件完成的。因此用户可根据自己的通信方式和协议开发自己的插件。开发插件需要的SDK在[Lobster SDK](https://github.com/lotusczp/Lobster-SDK)这个仓库。下面是我已经开发了的部分插件，供直接使用或者作为demo。
- [CCP协议插件，XCP的CAN实现](https://github.com/lotusczp/CCP-Plugin)
- [基于DBC的CAN通信插件](https://github.com/lotusczp/DBC-Plugin)
- [与沃森电源柜通信的插件](https://github.com/lotusczp/WocenPower-Plugin)
- [与恒河功率分析仪通信的Tmctl插件](https://github.com/lotusczp/Tmctl-Plugin)
- [与亚为科技的USBADC盒子通信的插件](https://github.com/lotusczp/USBADC-plugin)

## 脚本系统

Lobster使用Javascript作为脚本扩展，可以读写数据及进行其他控制，用于建立自动调试、测试系统等。所有的API在Help文件中给出。

## 下一步工作
- 提交一个编译好的release版本
- 控制页面仍然在开发中
- help文件需要更新
- 需要一个自动更新程序
- 当前的脚本系统使用了已经被Qt官方废弃的QtScript模块，准备替换为QJSEngine以支持新版本Qt

## 如何编译
当前测试过的Qt版本是Qt 5.15.2 (MSVC 2019, 64 bit)

## 感谢及声明
- 部分代码为Rusk Tao所写，并且共同参与设计
- 示波器使用了[QCustomPlot](https://www.qcustomplot.com/)库
- 其中的JavaScript编辑器部分使用了[@kcrossen](https://github.com/kcrossen)的[这个仓库](https://github.com/kcrossen/Qt5.6-JavaScript-Editor-Widget)。如有任何侵权问题请联系我。
