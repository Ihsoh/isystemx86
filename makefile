#Filename:		makefile
#Author:		Ihsoh
#Date:			2014-1-24
#Descriptor:	Make ISystemX86

TargetDir=bin
Out=isystemx86.img
Target=$(TargetDir)/$(Out)

$(Target): 	ilib/bin/libilib.a boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin \
			kernel/bin/kernel.bin
#tools/floppymaker/bin/floppymaker boot/bin/boot.bin kernelldr/bin/kernelldr.bin kernel/bin/kernel.bin bin/isystemx86.img
	cd ilib && make
	cd apps/basic && make
	cd apps/edit && make
	cd apps/paint && make
#tools/diskmaker/bin/diskmaker boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin kernel/bin/kernel.bin bin/isystemx86.img isystemx86.dm flat
	tools/diskmaker/bin/diskmaker boot/bin/boot.bin kernelldr/bin/kernelldrtm.bin kernel/bin/kernel.bin bin/isystemx86.vhd isystemx86.dm vhd

ilib/bin/libilib.a:
	cd ilib && make

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
	cd boot && make Clear
	cd kernelldr && make Clear
	cd kernel && make Clear

	cd apps/basic && make Clear
	cd apps/edit && make Clear
	cd apps/paint && make Clear
