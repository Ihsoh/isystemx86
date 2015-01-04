ISystem x86
===========

This is a multiple task, single user, x86 operating system.
这是一个多任务，单用户，基于x86的操作系统。

构建系统：

以下步骤均假定当前目录为 ISystem x86 源代码目录的根目录。
	
* Ubuntu 10.04 LTS amd64
	* 汇编器和编译器
		apt-get install nasm	#NASM 已存在则跳过
		apt-get install gcc		#gcc  已存在则跳过	

	* 64位 Linux 编译32位程序所需的库
		apt-get install ia32-libs
		apt-get install libc6-dev-i386
		apt-get install linux32

	* 编译 Application Maker
		cd tools/appmaker/
		make Clear
		make
		cd ../../

	* 编译 Disk Maker
		cd tools/diskmaker/
		make Clear
		make
		cd ../../

	* 构建 ISystem x86
		make Clear
		make

* Ubuntu 10.04 LTS i386
	* 汇编器和编译器
		apt-get install nasm	#NASM 已存在则跳过
		apt-get install gcc		#gcc  已存在则跳过	

	* 编译 Application Maker
		cd tools/appmaker/
		make Clear
		make
		cd ../../

	* 编译 Disk Maker
		cd tools/diskmaker/
		make Clear
		make
		cd ../../

	* 构建 ISystem x86
		make Clear
		make
