#Filename:		makefile
#Author:		Ihsoh
#Date:			2014-1-24
#Descriptor:	Make ISystemX86

TargetDir=bin
Out=isystemx86.img
Target=$(TargetDir)/$(Out)

$(Target):	bin ilib/bin/libilib.a dslib/bin/libdslib.a jsonlib/bin/libjsonlib.a	\
			boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin \
			kernel/bin/kernel.bin
#tools/floppymaker/bin/floppymaker boot/bin/boot.bin kernelldr/bin/kernelldr.bin kernel/bin/kernel.bin bin/isystemx86.img
	cd apps/printf && make
	cd apps/fwrite && make
	cd apps/fappend && make

	cd apps/basic && make
	cd apps/edit && make
	cd apps/iasm && make
	cd apps/WDVM-ISystemx86 && make
#tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin kernel/bin/kernel.bin bin/isystemx86.img isystemx86.dm flat
	tools/diskmaker/bin/diskmaker -m boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin kernel/bin/kernel.bin bin/isystemx86.vhd isystemx86.dm vhd
	cd tools/idoc/ && python idoc.py ../../kernel ../../doc/isystemx86-kernel-doc

bin:
	-mkdir bin

ilib/bin/libilib.a:
	cd ilib && make

dslib/bin/libdslib.a:
	cd dslib && make

jsonlib/bin/libjsonlib.a:
	cd jsonlib && make

boot/bin/boot.bin:
	cd boot && make

kernelldr/bin/kernelldrtm.bin:
	cd kernelldr && make

kernel/bin/kernel.bin:
	cd kernel && make

.PHONY:Clear

Clear:
	-rm -f $(Target)
	
	cd ilib && make Clear
	cd dslib && make Clear
	cd jsonlib && make Clear

	cd boot && make Clear
	cd kernelldr && make Clear
	cd kernel && make Clear

	cd apps/printf && make Clear
	cd apps/fwrite && make Clear
	cd apps/fappend && make Clear

	cd apps/basic && make Clear
	cd apps/edit && make Clear
	cd apps/iasm && make Clear
	cd apps/WDVM-ISystemx86 && make Clear
