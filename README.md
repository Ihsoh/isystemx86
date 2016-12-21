ISystem x86
===========
ISystem x86 is a multitasking and x86 based operating system, in development since January of 2014.

This repository contains the kernel, system applications, user applications and some libraries. nasm and gcc are required to build ISystem x86 if building environment is ubuntu 32-bit. nasm, gcc, ia32-libs and libc-dev-i386 are required if building environment is ubuntu 64-bit.

## How to build? ##
You can run the *build.sh* to build ISystem x86. if no error occurred, the system images were generated in the *bin/*.

```
$ ./build.sh
```

## Kernel ##
The ISystem x86 kernel provides multitasking, memory management, a tree-structured directory filesystem, a console, a simple GUI, and some device drivers. PS/2 mouse, PS/2 keyboard, ATA harddisk, VESA, serial were supported.

## User Application ##
The user application was run in the Ring3. There are some examples in the directory *apps/*.

## System Application ##
The system application was run in the Ring0. The system application is kernel extension. There are some examples in the directory *sys/*.

## Library ##
The libraries are used to develop the user application and the system application. There are the libraries in the directory *libs/*.

|Name                |Detail                                   |
|--------------------|-----------------------------------------|
|baslanglib          |Basic Language Library.                  |
|dslib               |Data Structure.                          |
|encryptionlib       |Hash(md5, sha1, sha256, sha512) Library. |
|ilib                |Standard Library.                        |
|jsonlib             |JSON Library.                            |
|mempool             |Memory Pool Library.                     |
|pathlib             |Path Library.                            |
|regexlib            |Regular Expression Library.              |
|isys                |ISystem API.                             |

## Screenshot ##
![Alt GUI](http://ihsoh.github.io/images/screenshot/screenshot_2016-12-21.jpg "GUI")
