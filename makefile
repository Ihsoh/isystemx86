#Filename:		makefile
#Author:		Ihsoh
#Date:			2014-1-24
#Descriptor:	Make ISystemX86

TargetDir=bin
Out=isystemx86.img
Target=$(TargetDir)/$(Out)

$(Target):	bin	\
			libs/ilib/bin/libilib.a 	\
			libs/dslib/bin/libdslib.a 	\
			libs/jsonlib/bin/libjsonlib.a	\
			libs/encryptionlib/bin/libencryptionlib.a	\
			libs/pathlib/bin/libpathlib.a 	\
			libs/baslanglib/bin/libbaslanglib.a 	\
			libs/mempoollib/bin/libmempoollib.a 	\
			libs/regexlib/bin/libregexlib.a 	\
			libs/imagelib/bin/libimagelib.a 	\
			libs/isys/bin/libisys.a 	\
			boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin \
			kernel/bin/kernel.bin
#tools/floppymaker/bin/floppymaker boot/bin/boot.bin kernelldr/bin/kernelldr.bin kernel/bin/kernel.bin bin/isystemx86.img
	cd apps/printf && make
	cd apps/fwrite && make
	cd apps/fappend && make
	cd apps/test && make

	cd apps/basic && make
	cd apps/edit && make
	cd apps/iasm && make
	cd apps/WDVM-ISystemx86 && make
	cd apps/ubasic && make
	cd apps/lua1 && make
	cd apps/lua2 && make
	cd apps/elf && make
	cd apps/lua3 && make

	cd wapps/image && make
	cd wapps/calc && make
	cd wapps/wedit && make
	cd wapps/tictactoe && make
	cd wapps/explorer && make

	cd sys/pci && make
	cd sys/screen && make
	cd sys/timer && make

	cd test/test_jsonlib && make
	cd test/test_elfso && make

	cd slib/test && make

	# 文本模式。
	# tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin kernel/bin/kernel.bin bin/isystemx86.img isystemx86.dm flat
	# tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin kernel/bin/kernel.bin bin/isystemx86.vhd isystemx86.dm vhd
	
	# 800*600分辨率的GUI模式。
	# tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrgm_800_600.bin kernel/bin/kernel.bin bin/isystemx86.img isystemx86.dm flat
	# tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrgm_800_600.bin kernel/bin/kernel.bin bin/isystemx86.vhd isystemx86.dm vhd
	
	# 1024*768分辨率的GUI模式。
	tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrgm_1024_768.bin kernel/bin/kernel.bin bin/isystemx86.img isystemx86.dm flat
	tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrgm_1024_768.bin kernel/bin/kernel.bin bin/isystemx86.vhd isystemx86.dm vhd

	if [ ! -d "doc" ]; then mkdir doc; fi

	if [ ! -d "doc/isystemx86-kernel-doc" ]; then mkdir doc/isystemx86-kernel-doc; fi
	if [ -f "doc/isystemx86-kernel-doc/*" ]; then rm -r doc/isystemx86-kernel-doc/*; fi
	cd tools/idoc/ && python idoc.py ../../kernel ../../doc/isystemx86-kernel-doc

	if [ ! -d "doc/libs" ]; then mkdir doc/libs; fi

	if [ ! -d "doc/libs/mempoollib-doc" ]; then mkdir doc/libs/mempoollib-doc; fi
	if [ -f "doc/mempoollib-doc/*" ]; then rm -r doc/libs/mempoollib-doc/*; fi
	cd tools/idoc/ && python idoc.py ../../libs/mempoollib ../../doc/libs/mempoollib-doc

bin:
	-mkdir bin

libs/ilib/bin/libilib.a:
	cd libs/ilib && make TargetDir=bin Out=libilib.a
	cd libs/ilib && make TargetDir=bin/pic Out=libilib.a CFlagsExt=-fPIC

libs/dslib/bin/libdslib.a:
	cd libs/dslib && make TargetDir=bin Out=libdslib.a
	cd libs/dslib && make TargetDir=bin/pic Out=libdslib.a CFlagsExt=-fPIC

libs/jsonlib/bin/libjsonlib.a:
	cd libs/jsonlib && make TargetDir=bin Out=libjsonlib.a
	cd libs/jsonlib && make TargetDir=bin/pic Out=libjsonlib.a CFlagsExt=-fPIC

libs/encryptionlib/bin/libencryptionlib.a:
	cd libs/encryptionlib && make TargetDir=bin Out=libencryptionlib.a
	cd libs/encryptionlib && make TargetDir=bin/pic Out=libencryptionlib.a CFlagsExt=-fPIC

libs/pathlib/bin/libpathlib.a:
	cd libs/pathlib && make TargetDir=bin Out=libpathlib.a
	cd libs/pathlib && make TargetDir=bin/pic Out=libpathlib.a CFlagsExt=-fPIC

libs/baslanglib/bin/libbaslanglib.a:
	cd libs/baslanglib && make TargetDir=bin Out=libbaslanglib.a
	cd libs/baslanglib && make TargetDir=bin/pic Out=libbaslanglib.a CFlagsExt=-fPIC

libs/mempoollib/bin/libmempoollib.a:
	cd libs/mempoollib && make TargetDir=bin Out=libmempoollib.a
	cd libs/mempoollib && make TargetDir=bin/pic Out=libmempoollib.a CFlagsExt=-fPIC

libs/regexlib/bin/libregexlib.a:
	cd libs/regexlib && make TargetDir=bin Out=libregexlib.a
	cd libs/regexlib && make TargetDir=bin/pic Out=libregexlib.a CFlagsExt=-fPIC

libs/imagelib/bin/libimagelib.a:
	cd libs/imagelib && make TargetDir=bin Out=libimagelib.a
	cd libs/imagelib && make TargetDir=bin/pic Out=libimagelib.a CFlagsExt=-fPIC

libs/isys/bin/libisys.a:
	cd libs/isys && make TargetDir=bin Out=libisys.a
	cd libs/isys && make TargetDir=bin/pic Out=libisys.a CFlagsExt=-fPIC

boot/bin/boot.bin:
	cd boot && make

kernelldr/bin/kernelldrtm.bin:
	cd kernelldr && make

kernel/bin/kernel.bin:
	cd kernel && make

.PHONY:clean

clean:
	-rm -f $(Target)
	
	cd libs/ilib && make clean TargetDir=bin Out=libilib.a
	cd libs/ilib && make clean TargetDir=bin/pic Out=libilib.a CFlagsExt=-fPIC
	
	cd libs/dslib && make clean TargetDir=bin Out=libdslib.a
	cd libs/dslib && make clean TargetDir=bin/pic Out=libdslib.a CFlagsExt=-fPIC

	cd libs/jsonlib && make clean TargetDir=bin Out=libjsonlib.a
	cd libs/jsonlib && make clean TargetDir=bin/pic Out=libjsonlib.a CFlagsExt=-fPIC
	
	cd libs/encryptionlib && make clean TargetDir=bin Out=libencryptionlib.a
	cd libs/encryptionlib && make clean TargetDir=bin/pic Out=libencryptionlib.a CFlagsExt=-fPIC

	cd libs/pathlib && make clean TargetDir=bin Out=libpathlib.a
	cd libs/pathlib && make clean TargetDir=bin/pic Out=libpathlib.a CFlagsExt=-fPIC
	
	cd libs/baslanglib && make clean TargetDir=bin Out=libbaslanglib.a
	cd libs/baslanglib && make clean TargetDir=bin/pic Out=libbaslanglib.a CFlagsExt=-fPIC

	cd libs/mempoollib && make clean TargetDir=bin Out=libmempoollib.a
	cd libs/mempoollib && make clean TargetDir=bin/pic Out=libmempoollib.a CFlagsExt=-fPIC

	cd libs/regexlib && make clean TargetDir=bin Out=libregexlib.a
	cd libs/regexlib && make clean TargetDir=bin/pic Out=libregexlib.a CFlagsExt=-fPIC
	
	cd libs/imagelib && make clean TargetDir=bin Out=libimagelib.a
	cd libs/imagelib && make clean TargetDir=bin/pic Out=libimagelib.a CFlagsExt=-fPIC

	cd libs/isys && make clean TargetDir=bin Out=libisys.a
	cd libs/isys && make clean TargetDir=bin/pic Out=libisys.a CFlagsExt=-fPIC

	cd boot && make clean
	cd kernelldr && make clean
	cd kernel && make clean

	cd apps/printf && make clean
	cd apps/fwrite && make clean
	cd apps/fappend && make clean
	cd apps/test && make clean

	cd apps/basic && make clean
	cd apps/edit && make clean
	cd apps/iasm && make clean
	cd apps/WDVM-ISystemx86 && make clean
	cd apps/ubasic && make clean
	cd apps/lua1 && make clean
	cd apps/lua2 && make clean
	cd apps/elf && make clean
	cd apps/lua3 && make clean

	cd wapps/image && make clean
	cd wapps/calc && make clean
	cd wapps/wedit && make clean
	cd wapps/tictactoe && make clean
	cd wapps/explorer && make clean

	cd sys/pci && make clean
	cd sys/screen && make clean
	cd sys/timer && make clean

	cd test/test_jsonlib && make clean
	cd test/test_elfso && make clean

	cd slib/test && make clean
