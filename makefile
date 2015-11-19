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
			libs/imagelib/bin/libisys.a 	\
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

	cd wapps/image && make
	cd wapps/calc && make
	cd wapps/wedit && make
	cd wapps/tictactoe && make

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

	-rm -r doc/isystemx86-kernel-doc/*
	cd tools/idoc/ && python idoc.py ../../kernel ../../doc/isystemx86-kernel-doc
	-rm -r doc/libs/mempoollib-doc/*
	cd tools/idoc/ && python idoc.py ../../libs/mempoollib ../../doc/libs/mempoollib-doc

bin:
	-mkdir bin

libs/ilib/bin/libilib.a:
	cd libs/ilib && make

libs/dslib/bin/libdslib.a:
	cd libs/dslib && make

libs/jsonlib/bin/libjsonlib.a:
	cd libs/jsonlib && make

libs/encryptionlib/bin/libencryptionlib.a:
	cd libs/encryptionlib && make

libs/pathlib/bin/libpathlib.a:
	cd libs/pathlib && make

libs/baslanglib/bin/libbaslanglib.a:
	cd libs/baslanglib && make

libs/mempoollib/bin/libmempoollib.a:
	cd libs/mempoollib && make

libs/regexlib/bin/libregexlib.a:
	cd libs/regexlib && make

libs/imagelib/bin/libimagelib.a:
	cd libs/imagelib && make

libs/imagelib/bin/libisys.a:
	cd libs/isys && make

boot/bin/boot.bin:
	cd boot && make

kernelldr/bin/kernelldrtm.bin:
	cd kernelldr && make

kernel/bin/kernel.bin:
	cd kernel && make

.PHONY:clean

clean:
	-rm -f $(Target)
	
	cd libs/ilib && make clean
	cd libs/dslib && make clean
	cd libs/jsonlib && make clean
	cd libs/encryptionlib && make clean
	cd libs/pathlib && make clean
	cd libs/baslanglib && make clean
	cd libs/mempoollib && make clean
	cd libs/regexlib && make clean
	cd libs/imagelib && make clean
	cd libs/isys && make clean

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

	cd wapps/image && make clean
	cd wapps/calc && make clean
	cd wapps/wedit && make clean
	cd wapps/tictactoe && make clean

	cd sys/pci && make clean
	cd sys/screen && make clean
	cd sys/timer && make clean

	cd test/test_jsonlib && make clean
	cd test/test_elfso && make clean

	cd slib/test && make clean
