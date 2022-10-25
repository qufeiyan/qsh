# qsh: 小而美的嵌入式系统 shell 工具 

**中文** | [English](README.md)  

![license](https://img.shields.io/badge/license-MIT-orange)
![c99](https://img.shields.io/badge/standard-C99-blue)
![release](https://img.shields.io/badge/release-1.0.0-green)
![prs](https://img.shields.io/badge/PRs-welcome-brightgreen)
![poweredby](https://img.shields.io/badge/powered%20by-qufeiyan-red)

## 简介
`qsh` 是一个面向嵌入式设备的轻量、简易版命令行 `shell` 工具, 为更好地调试嵌入式应用而设计。其对资源紧张的嵌入式 `mcu` 设备的 `flash`与 `ram` 要求极低，资源占用可以忽略不计，其基本功能与传统命令行解析器并无太大差别。

```bash
           /
 ___  ___ (___  Version    1.0.0
|   )|___ |   ) Copyright  (c) 2022
|__/| __/ |  /  Poweredby  qufeiyan
    |           Build      Oct 23 2022 16:30:25

-> $
```

---
## 当前功能
- 支持 `Tab` 键自动补全功能
- 支持历史记录功能，可使用上、下方向键切换历史命令
- 支持命令行输入时光标左右移动
- 支持用户自定义命令
- 支持用户可选项配置，诸如历史命令支持，历史大小，命令行输出缓冲大小等配置
- 支持全局变量追踪功能

---
## <span style = "color: red">待实现功能</span>
- 增加 `shell` 权限认证
- 嵌入式文件系统支持
- 嵌入式多线程调试支持
- ...... 

---
## 移植
`qsh` 的实现完全遵循 `C99` 标准，移植过程极为简单。可分为以下两个步骤进行。

- 输入、输出函数移植

    在源文件目录下放置了一个 `qsh_port.c` 文件，此文件默认实现了一组基于 `linux` 下的方法，以适配 `qsh` 下的输入、输出功能。用户如果想移植到某个嵌入式 `rtos` 设备中，只需要按照特定 `rtos` 的特性重写其中的串口输入、输出方法即可。

- `qah` 线程创建
 
    `qsh` 解释器默认以 **独立线程** 方式存在，需要用户按照特定 `rtos` 的特性在初始化应用时创建一个线程，线程主体为 `void qsh_entry(void *args)`。参考 `test.c` 文件的实现。

当然，裸机移植也不是问题，但不是很推荐...

---
## 编译
`qsh` 提供了一个基本的 `Makefile` 文件，支持生成静态库文件(`.a` 后缀)，进入源文件目录，在安装有 `gcc` 编译工具链的环境下输入 `make lib`, 
即可在源代码目录下生成静态库 `libqsh.a`，方便移植。

```js
ubuntu@primary:~/Home/code/qsh$ make lib
Makefile:5: "cc : gcc"
Makefile:6: "ar : ar"
gcc -c -g -O1 -DUSE_DEFAULT -I. qsh.c -o ./objs/qsh.o
gcc -c -g -O1 -DUSE_DEFAULT -I. qsh_internal.c -o ./objs/qsh_internal.o
gcc -c -g -O1 -DUSE_DEFAULT -I. qsh_port.c -o ./objs/qsh_port.o
ar rscv libqsh.a ./objs/qsh.o ./objs/qsh_internal.o ./objs/qsh_port.o
a - ./objs/qsh.o
a - ./objs/qsh_internal.o
a - ./objs/qsh_port.o
ubuntu@primary:~/Home/code/qsh$ ll libqsh.a 
-rw-rw-r-- 1 ubuntu ubuntu 62810 Oct 23 23:50 libqsh.a
```
编译信息如上。

---
## 测试 
在源代码目录下存在一个 `test.c` 文件，该文件基于
`linux` 系统编写，简单展示了一个基本的移植过程。
同时，如果想要编译运行, 输入 `make test`, 即可生成 `test` 可执行文件。
```js
ubuntu@primary:~/Home/code/qsh$ make test
Makefile:5: "cc : gcc"
Makefile:6: "ar : ar"
gcc -c -g -O1 -DUSE_DEFAULT -I. qsh.c -o ./objs/qsh.o
gcc -c -g -O1 -DUSE_DEFAULT -I. qsh_internal.c -o ./objs/qsh_internal.o
gcc -c -g -O1 -DUSE_DEFAULT -I. qsh_port.c -o ./objs/qsh_port.o
gcc -c -g -O1 -DUSE_DEFAULT -I. test.c -o ./objs/test.o
gcc -Tqsh.lds -g -O1 -DUSE_DEFAULT -I. ./objs/qsh.o ./objs/qsh_internal.o ./objs/qsh_port.o ./objs/test.o -o test
```
在 `linux` 环境下运行，即可进入 `qsh`。如下
```bash
ubuntu@primary:~/Home/code/qsh$ ./test
hello, qsh!
-> $list
```

其中，`-> $` 为 `qsh` 的默认命令提示符, 如上输入 `list` , 按下回车键，即可列出当前 `qsh` 支持的所有内置命令。

```bash
-> $list
--Qshell commands list:
-----------------------------------------------------
history         - Display the commands history list
quit            - quit qsh from current contex
list            - list all internal commands
info            - display current version of qsh
clr             - clear display area
-> $
```



