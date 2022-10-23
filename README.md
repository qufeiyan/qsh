# Qsh: Small but beautiful embedded system shell

**English** ｜ [中文](README_ZH.md)  

![license](https://img.shields.io/badge/license-MIT-orange)
![c99](https://img.shields.io/badge/standard-C99-blue)
![release](https://img.shields.io/badge/release-1.0.0-green)
![prs](https://img.shields.io/badge/PRs-welcome-brightgreen)
![poweredby](https://img.shields.io/badge/powered%20by-qufeiyan-red)

## Brief
`qsh` is a lightweight, simple command-line `shell` tool for embedded devices, designed for better debugging of embedded applications. Its 'flash' and 'ram' requirements for resource-constrained embedded 'MCU' devices are extremely low, the resource consumption is negligible, and its basic functions are not much different from traditional command-line parsers.

```bash
           /
 ___  ___ (___  Version    1.0.0
|   )|___ |   ) Copyright  (c) 2022
|__/| __/ |  /  Poweredby  qufeiyan
    |           Build      Oct 23 2022 16:30:25

-> $
```

---
## Current features
- Supports `auto-completion` of commands using the Tab key
- Support `history commands`, you can use the up and down arrow keys to switch history commands
- Support `cursor` left and right `movement` when entering command line
- Support `user-defined commands`
- Support `user-selectable configuration`, such as historical command support, history size, command line output buffer size and other configurations

---
## To do list
- View global variables in the current application on the command line
- Dynamically modify global variables in the current application to cooperate with debugging
- Support privilege authentication
- Support Embedded file system 
- Support Embedded multithreaded debugging
- ...... 

---
## Porting
The implementation of `qsh` is fully compliant with the `C99` standard and the porting process is extremely simple. It can be divided into the following two steps.

- Input and output function
    
    `qsh_port.c` file is placed in the source file directory, which implements a set of `linux` based methods by default to adapt to the input and output functions under `qsh`. If you want to port to an embedded `rtos` device, you only need to rewrite the serial port input and output methods according to the characteristics of a specific `rtos`.

- `qsh` thread 

    The `qsh` interpreter exists as a **independent thread** by default and requires user to create a thread when initializing the application according to the specific 'rtos' feature, with the thread body being `void qsh_entry(void *args)`. Please refer to the implementation of the 'test.c' file.

Of course, bare metal porting without `rtos` is not a problem, but it is not very recommended...

---
## Build
`qsh` provides a basic `makefile`, supports generating static library files (`.a` suffix), goes to the source file directory, and enters `make lib` in the context of the `gcc` compilation toolchain, 
You can generate the static library `libqsh.a` in the source code directory for easy porting.

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
The compilation information is as above.

---
## Testing 
There is a `test.c` file in the source code directory, which is written to briefly show a basic porting process.
At the same time, if you want to compile and run, enter 'make test' to generate the `test` executable file.

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
Run in a `linux` environment to enter `qsh`. as follows

```bash
ubuntu@primary:~/Home/code/qsh$ ./test
hello, qsh!
-> $list
```

Where `-> $` is the default command prompt for `qsh`, enter `list` above and press Enter to list all built-in commands currently supported by `qsh`.

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



