# Lobster

English | [简体中文](./README.zh_CN.md)

Download pre-built release [Lobster 1.3.0](https://github.com/lotusczp/Lobster/releases/tag/release_1_3_0)

This repository is for the main program of Lobster. Lobster is a general purpose tool for debugging and testing. The idea is to gather, display and manipulate runtime values in remote devices, especially embedded systems. The main features include:
- Plugin system to customize the communication with plenty of remote devices
- Watch window to display values.
- Oscilloscope to show the curves in realtime， whose UE is like CANoe/CANalyzer.
- Log system to store the values with timestamp for feature analysis.
- Replay system to show the curves of data saved by log file.
- A script system (JavaScript) to extend, which can be used as auto-testing system.
- Control page system to build custom UI using web.

The snapshot
![avatar](https://raw.githubusercontent.com/lotusczp/images/main/snapshot_lobster.png)


## Plugins
Lobster doesn't have any concrete implementation of communication with any remote devices. It use plugins to deal with them. Users can customize their own plugins using the [SDK](https://github.com/lotusczp/Lobster-SDK). I've already implemented several plugins
- [CCP Plugin](https://github.com/lotusczp/CCP-Plugin)
- [DBC Plugin](https://github.com/lotusczp/DBC-Plugin)
- [WocenPower Plugin](https://github.com/lotusczp/WocenPower-Plugin)
- [Tmctl Plugin](https://github.com/lotusczp/Tmctl-Plugin)
- [USBADC plugin](https://github.com/lotusczp/USBADC-plugin)

## Script
We use JavaScript (Qt support the interaction between JS and C++) as the script system of Lobster. The APIs are described in the help.

## Todo
- I will build a release version for download directly.
- Control page system is still under development
- Update help
- We need an auto update feature
- Currently the script system is using the deprecated QtScript module. Need to transport to new QJSEngine

## How to build
- Qt 5.15.2 (MSVC 2019, 64 bit) is tested. Others I still need to verify.

## Thanks to 
- Rusk Tao is the 2nd auth of this software.
- The oscilloscope is using [QCustomPlot](https://www.qcustomplot.com/).
- The JavaScript editor widget is using the codes from [this repo](https://github.com/kcrossen/Qt5.6-JavaScript-Editor-Widget) from [@kcrossen](https://github.com/kcrossen). Please contact me if there are any permission issues.