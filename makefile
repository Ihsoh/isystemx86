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

	cd sys/pci && make

#tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin kernel/bin/kernel.bin bin/isystemx86.img isystemx86.dm flat
	tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin kernel/bin/kernel.bin bin/isystemx86.vhd isystemx86.dm vhd
	cd tools/idoc/ && python idoc.py ../../kernel ../../doc/isystemx86-kernel-doc

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

boot/bin/boot.bin:
	cd boot && make

kernelldr/bin/kernelldrtm.bin:
	cd kernelldr && make

kernel/bin/kernel.bin:
	cd kernel && make

.PHONY:Clear

Clear:
	-rm -f $(Target)
	
	cd libs/ilib && make Clear
	cd libs/dslib && make Clear
	cd libs/jsonlib && make Clear
	cd libs/encryptionlib && make Clear
	cd libs/pathlib && make Clear

	cd boot && make Clear
	cd kernelldr && make Clear
	cd kernel && make Clear

	cd apps/printf && make Clear
	cd apps/fwrite && make Clear
	cd apps/fappend && make Clear
	cd apps/test && make Clear

	cd apps/basic && make Clear
	cd apps/edit && make Clear
	cd apps/iasm && make Clear
	cd apps/WDVM-ISystemx86 && make Clear

	cd sys/pci && make Clear
