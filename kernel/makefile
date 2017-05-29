#Filename:	makefile
#Author:	Ihsoh
#Date:		2014-1-24
#Descriptor:Make kernel.bin

TargetDir=bin
Out=kernel.bin
Target=$(TargetDir)/$(Out)
CC=gcc
AS=as
CFlags= -O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 -w
ASFlags=--32 -march=i386 -mtune=i386

CPP=g++
CPPFLAGS= -std=c++11 -O0 -ffreestanding -fno-builtin -nostdinc	\
-fno-exceptions -fno-rtti -Wall -Wextra	\
-m32 -march=i386 -mtune=i386

StringObj=../libs/ilib/bin/string.obj

Objs=bin/kernel.obj	\
bin/386.obj bin/screen.obj bin/die.obj bin/keyboard.obj \
bin/console.obj bin/disk.obj bin/vdisk.obj bin/memory.obj \
bin/kfuncs.obj bin/cmos.obj bin/tasks.obj \
bin/vesa.obj bin/enfont.obj bin/vars.obj bin/image.obj bin/cpu.obj	\
bin/paging.obj bin/lock.obj bin/window.obj bin/klib.obj bin/kstring.obj bin/log.obj bin/rsdp.obj	\
bin/acpi.obj bin/rsdt.obj bin/fadt.obj bin/atapi.obj bin/serial.obj bin/mqueue.obj	\
bin/pci.obj bin/apic.obj bin/madt.obj bin/pic.obj bin/interrupts.obj bin/config.obj 	\
bin/ahci.obj bin/dma.obj bin/mouse.obj bin/kmpool.obj bin/knlldr.obj bin/irq.obj bin/timer.obj	\
bin/gui.obj bin/ata.obj bin/so.obj bin/time.obj

BMPObjs=bmp/bin/bmp.obj

UtilsObjs=utils/bin/img.obj utils/bin/sfstr.obj

DumpObjs=dump/bin/dump.obj dump/bin/msg.obj

WindowObjs=window/bin/control.obj window/bin/button.obj window/bin/label.obj window/bin/list.obj 	\
window/bin/progress.obj window/bin/scroll.obj window/bin/edit.obj

WindowsObjs=windows/bin/clock.obj windows/bin/console.obj windows/bin/sfunc.obj windows/bin/message.obj 	\
windows/bin/detail.obj windows/bin/explorer.obj windows/bin/taskbar.obj windows/bin/start.obj 	\
windows/bin/power.obj windows/bin/wmgr.obj windows/bin/run.obj windows/bin/dbgout.obj windows/bin/taskmgr.obj	\
windows/bin/task.obj

SyscallObjs=syscall/bin/fs.obj syscall/bin/keyboard.obj syscall/bin/mouse.obj syscall/bin/screen.obj 	\
syscall/bin/system.obj syscall/bin/gui.obj

FS_IFS1Objs=fs/ifs1/bin/blocks.obj fs/ifs1/bin/fs.obj

CppCRTObjs=cppcrt/bin/cppcrt.obj cppcrt/bin/ctor.obj cppcrt/bin/dtor.obj

ELFObjs=elf/bin/helper.obj elf/bin/parser.obj

PCIObjs=pci/bin/pci.obj

WavObjs=wav/bin/wav.obj

SBObjs=sb/bin/sb16.obj

NetObjs=net/bin/pcnet2.obj net/bin/endian.obj net/bin/net.obj net/bin/helper.obj net/bin/e1000.obj

Linker=ld -m elf_i386
Include=-I../libs -I../sys
Definition=-D_KERNEL_MODULE_ -D_KERNEL_DEBUG_

$(Target): bin bmp/bin dump/bin fs/ifs1/bin syscall/bin utils/bin window/bin windows/bin cppcrt/bin elf/bin pci/bin wav/bin sb/bin net/bin \
$(Objs) $(BMPObjs) $(UtilsObjs) $(DumpObjs) $(WindowObjs) $(WindowsObjs) $(SyscallObjs) $(FS_IFS1Objs) $(CppCRTObjs)	\
$(ELFObjs) $(PCIObjs) $(WavObjs) $(SBObjs) $(NetObjs)
	$(Linker) -Tkernel.lds -e main --oformat binary -o $(Target) $(Objs)	\
$(StringObj) $(BMPObjs) $(UtilsObjs) $(DumpObjs) $(WindowObjs) $(WindowsObjs) $(SyscallObjs) $(FS_IFS1Objs) $(CppCRTObjs) $(ELFObjs) $(PCIObjs) $(WavObjs) $(SBObjs) $(NetObjs)	\
-L../libs/jsonlib/bin -ljsonlib 	\
-L../libs/dslib/bin -ldslib 	\
-L../libs/encryptionlib/bin -lencryptionlib	\
-L../libs/pathlib/bin -lpathlib	\
-L../libs/mempoollib/bin -lmempoollib

bin:
	-mkdir bin

bmp/bin:
	-mkdir bmp/bin

dump/bin:
	-mkdir dump/bin

fs/ifs1/bin:
	-mkdir fs/ifs1/bin

syscall/bin:
	-mkdir syscall/bin

utils/bin:
	-mkdir utils/bin

window/bin:
	-mkdir window/bin

windows/bin:
	-mkdir windows/bin

cppcrt/bin:
	-mkdir cppcrt/bin

elf/bin:
	-mkdir elf/bin

pci/bin:
	-mkdir pci/bin

wav/bin:
	-mkdir wav/bin

sb/bin:
	-mkdir sb/bin

net/bin:
	-mkdir net/bin	

bin/kernel.obj: kernel.c kernel.h
	$(CC) $(CFlags) -c kernel.c -o bin/kernel.obj $(Include) $(Definition)

bin/386.obj: 386.c 386.h types.h
	$(CC) $(CFlags) -c 386.c -o bin/386.obj $(Include) $(Definition)

bin/screen.obj: screen.c screen.h types.h 386.h
	$(CC) $(CFlags) -c screen.c -o bin/screen.obj $(Include) $(Definition)

bin/die.obj: die.c die.h screen.h types.h
	$(CC) $(CFlags) -c die.c -o bin/die.obj $(Include) $(Definition)

bin/keyboard.obj: keyboard.c keyboard.h types.h
	$(CC) $(CFlags) -c keyboard.c -o bin/keyboard.obj $(Include) $(Definition)

bin/console.obj: console.c console.h keyboard.h screen.h
	$(CC) $(CFlags) -c console.c -o bin/console.obj $(Include) $(Definition)

bin/disk.obj: disk.c disk.h
	$(CC) $(CFlags) -c disk.c -o bin/disk.obj $(Include) $(Definition)

bin/vdisk.obj: vdisk.c vdisk.h
	$(CC) $(CFlags) -c vdisk.c -o bin/vdisk.obj $(Include) $(Definition)

bin/memory.obj: memory.c memory.h
	$(CC) $(CFlags) -c memory.c -o bin/memory.obj $(Include) $(Definition)

bin/kfuncs.obj: kfuncs.c kfuncs.h
	$(CC) $(CFlags) -c kfuncs.c -o bin/kfuncs.obj $(Include) $(Definition)

bin/cmos.obj: cmos.c cmos.h
	$(CC) $(CFlags) -c cmos.c -o bin/cmos.obj $(Include) $(Definition)

bin/tasks.obj: tasks.c tasks.h
	$(CC) $(CFlags) -c tasks.c -o bin/tasks.obj $(Include) $(Definition)

bin/vesa.obj: vesa.c vesa.h
	$(CC) $(CFlags) -c vesa.c -o bin/vesa.obj $(Include) $(Definition)

bin/enfont.obj: enfont.c enfont.h
	$(CC) $(CFlags) -c enfont.c -o bin/enfont.obj $(Include) $(Definition)

bin/vars.obj: vars.c vars.h
	$(CC) $(CFlags) -c vars.c -o bin/vars.obj $(Include) $(Definition)

bin/image.obj: image.c image.h
	$(CC) $(CFlags) -c image.c -o bin/image.obj $(Include) $(Definition)

bin/cpu.obj: cpu.c cpu.h
	$(CC) $(CFlags) -c cpu.c -o bin/cpu.obj $(Include) $(Definition)

bin/paging.obj: paging.c paging.h
	$(CC) $(CFlags) -c paging.c -o bin/paging.obj $(Include) $(Definition)

bin/lock.obj: lock.c lock.h
	$(CC) $(CFlags) -c lock.c -o bin/lock.obj $(Include) $(Definition)

bin/window.obj: window.c window.h
	$(CC) $(CFlags) -c window.c -o bin/window.obj $(Include) $(Definition)

bin/klib.obj: klib.c klib.h
	$(CC) $(CFlags) -c klib.c -o bin/klib.obj $(Include) $(Definition)

bin/kstring.obj: kstring.c kstring.h
	$(CC) $(CFlags) -c kstring.c -o bin/kstring.obj $(Include) $(Definition)

bin/log.obj: log.c log.h
	$(CC) $(CFlags) -c log.c -o bin/log.obj $(Include) $(Definition)

bin/rsdp.obj: rsdp.c rsdp.h
	$(CC) $(CFlags) -c rsdp.c -o bin/rsdp.obj $(Include) $(Definition)

bin/acpi.obj: acpi.c acpi.h
	$(CC) $(CFlags) -c acpi.c -o bin/acpi.obj $(Include) $(Definition)

bin/rsdt.obj: rsdt.c rsdt.h
	$(CC) $(CFlags) -c rsdt.c -o bin/rsdt.obj $(Include) $(Definition)

bin/fadt.obj: fadt.c fadt.h
	$(CC) $(CFlags) -c fadt.c -o bin/fadt.obj $(Include) $(Definition)

bin/atapi.obj: atapi.c atapi.h
	$(CC) $(CFlags) -c atapi.c -o bin/atapi.obj $(Include) $(Definition)

bin/serial.obj: serial.c serial.h
	$(CC) $(CFlags) -c serial.c -o bin/serial.obj $(Include) $(Definition)

bin/mqueue.obj: mqueue.c mqueue.h
	$(CC) $(CFlags) -c mqueue.c -o bin/mqueue.obj $(Include) $(Definition)

bin/toaruos_pci.obj: toaruos_pci.c toaruos_pci.h
	$(CC) $(CFlags) -c toaruos_pci.c -o bin/toaruos_pci.obj $(Include) $(Definition)

bin/pci.obj: pci.c pci.h
	$(CC) $(CFlags) -c pci.c -o bin/pci.obj $(Include) $(Definition)

bin/apic.obj: apic.c apic.h
	$(CC) $(CFlags) -c apic.c -o bin/apic.obj $(Include) $(Definition)

bin/madt.obj: madt.c madt.h
	$(CC) $(CFlags) -c madt.c -o bin/madt.obj $(Include) $(Definition)

bin/pic.obj: pic.c pic.h
	$(CC) $(CFlags) -c pic.c -o bin/pic.obj $(Include) $(Definition)

bin/interrupts.obj: interrupts.c interrupts.h
	$(CC) $(CFlags) -c interrupts.c -o bin/interrupts.obj $(Include) $(Definition)

bin/config.obj: config.c config.h
	$(CC) $(CFlags) -c config.c -o bin/config.obj $(Include) $(Definition)

bin/ahci.obj: ahci.c ahci.h
	$(CC) $(CFlags) -c ahci.c -o bin/ahci.obj $(Include) $(Definition)

bin/dma.obj: dma.c dma.h
	$(CC) $(CFlags) -c dma.c -o bin/dma.obj $(Include) $(Definition)

bin/mouse.obj: mouse.c mouse.h
	$(CC) $(CFlags) -c mouse.c -o bin/mouse.obj $(Include) $(Definition)

bin/kmpool.obj: kmpool.c kmpool.h
	$(CC) $(CFlags) -c kmpool.c -o bin/kmpool.obj $(Include) $(Definition)

bin/knlldr.obj: knlldr.c knlldr.h
	$(CC) $(CFlags) -c knlldr.c -o bin/knlldr.obj $(Include) $(Definition)

bin/irq.obj: irq.s
	$(AS) $(ASFlags) -c irq.s -o bin/irq.obj

bin/timer.obj: timer.c timer.h
	$(CC) $(CFlags) -c timer.c -o bin/timer.obj $(Include) $(Definition)

bin/gui.obj: gui.c gui.h
	$(CC) $(CFlags) -c gui.c -o bin/gui.obj $(Include) $(Definition)

bin/ata.obj: ata.c ata.h
	$(CC) $(CFlags) -c ata.c -o bin/ata.obj $(Include) $(Definition)

bin/so.obj: so.c so.h
	$(CC) $(CFlags) -c so.c -o bin/so.obj $(Include) $(Definition)

bin/time.obj: time.c time.h
	$(CC) $(CFlags) -c time.c -o bin/time.obj $(Include) $(Definition)


bmp/bin/bmp.obj: bmp/bmp.c bmp/bmp.h
	$(CC) $(CFlags) -c bmp/bmp.c -o bmp/bin/bmp.obj $(Include) $(Definition) -std=c99


utils/bin/img.obj: utils/img.c utils/img.h
	$(CC) $(CFlags) -c utils/img.c -o utils/bin/img.obj $(Include) $(Definition)

utils/bin/sfstr.obj: utils/sfstr.c utils/sfstr.h
	$(CC) $(CFlags) -c utils/sfstr.c -o utils/bin/sfstr.obj $(Include) $(Definition)


dump/bin/dump.obj: dump/dump.c dump/dump.h
	$(CC) $(CFlags) -c dump/dump.c -o dump/bin/dump.obj $(Include) $(Definition)

dump/bin/msg.obj: dump/msg.c dump/msg.h
	$(CC) $(CFlags) -c dump/msg.c -o dump/bin/msg.obj $(Include) $(Definition)


window/bin/control.obj: window/control.c window/control.h
	$(CC) $(CFlags) -c window/control.c -o window/bin/control.obj $(Include) $(Definition)

window/bin/button.obj: window/button.c window/button.h
	$(CC) $(CFlags) -c window/button.c -o window/bin/button.obj $(Include) $(Definition)

window/bin/label.obj: window/label.c window/label.h
	$(CC) $(CFlags) -c window/label.c -o window/bin/label.obj $(Include) $(Definition)

window/bin/list.obj: window/list.c window/list.h
	$(CC) $(CFlags) -c window/list.c -o window/bin/list.obj $(Include) $(Definition)

window/bin/progress.obj: window/progress.c window/progress.h
	$(CC) $(CFlags) -c window/progress.c -o window/bin/progress.obj $(Include) $(Definition)

window/bin/scroll.obj: window/scroll.c window/scroll.h
	$(CC) $(CFlags) -c window/scroll.c -o window/bin/scroll.obj $(Include) $(Definition)

window/bin/edit.obj: window/edit.c window/edit.h
	$(CC) $(CFlags) -c window/edit.c -o window/bin/edit.obj $(Include) $(Definition)


windows/bin/clock.obj: windows/clock.c windows/clock.h
	$(CC) $(CFlags) -c windows/clock.c -o windows/bin/clock.obj $(Include) $(Definition)

windows/bin/console.obj: windows/console.c windows/console.h
	$(CC) $(CFlags) -c windows/console.c -o windows/bin/console.obj $(Include) $(Definition)

windows/bin/sfunc.obj: windows/sfunc.c windows/sfunc.h
	$(CC) $(CFlags) -c windows/sfunc.c -o windows/bin/sfunc.obj $(Include) $(Definition)

windows/bin/message.obj: windows/message.c windows/message.h
	$(CC) $(CFlags) -c windows/message.c -o windows/bin/message.obj $(Include) $(Definition)

windows/bin/detail.obj: windows/detail.c windows/detail.h
	$(CC) $(CFlags) -c windows/detail.c -o windows/bin/detail.obj $(Include) $(Definition)

windows/bin/explorer.obj: windows/explorer.c windows/explorer.h
	$(CC) $(CFlags) -c windows/explorer.c -o windows/bin/explorer.obj $(Include) $(Definition)

windows/bin/taskbar.obj: windows/taskbar.c windows/taskbar.h
	$(CC) $(CFlags) -c windows/taskbar.c -o windows/bin/taskbar.obj $(Include) $(Definition)

windows/bin/start.obj: windows/start.c windows/start.h
	$(CC) $(CFlags) -c windows/start.c -o windows/bin/start.obj $(Include) $(Definition)

windows/bin/power.obj: windows/power.c windows/power.h
	$(CC) $(CFlags) -c windows/power.c -o windows/bin/power.obj $(Include) $(Definition)

windows/bin/wmgr.obj: windows/wmgr.c windows/wmgr.h
	$(CC) $(CFlags) -c windows/wmgr.c -o windows/bin/wmgr.obj $(Include) $(Definition)

windows/bin/run.obj: windows/run.c windows/run.h
	$(CC) $(CFlags) -c windows/run.c -o windows/bin/run.obj $(Include) $(Definition)

windows/bin/dbgout.obj: windows/dbgout.c windows/dbgout.h
	$(CC) $(CFlags) -c windows/dbgout.c -o windows/bin/dbgout.obj $(Include) $(Definition)

windows/bin/taskmgr.obj: windows/taskmgr.c windows/taskmgr.h
	$(CC) $(CFlags) -c windows/taskmgr.c -o windows/bin/taskmgr.obj $(Include) $(Definition)

windows/bin/task.obj: windows/task.c windows/task.h
	$(CC) $(CFlags) -c windows/task.c -o windows/bin/task.obj $(Include) $(Definition)


syscall/bin/fs.obj: syscall/fs.c syscall/fs.h
	$(CC) $(CFlags) -c syscall/fs.c -o syscall/bin/fs.obj $(Include) $(Definition)

syscall/bin/keyboard.obj: syscall/keyboard.c syscall/keyboard.h
	$(CC) $(CFlags) -c syscall/keyboard.c -o syscall/bin/keyboard.obj $(Include) $(Definition)

syscall/bin/mouse.obj: syscall/mouse.c syscall/mouse.h
	$(CC) $(CFlags) -c syscall/mouse.c -o syscall/bin/mouse.obj $(Include) $(Definition)

syscall/bin/screen.obj: syscall/screen.c syscall/screen.h
	$(CC) $(CFlags) -c syscall/screen.c -o syscall/bin/screen.obj $(Include) $(Definition)

syscall/bin/system.obj: syscall/system.c syscall/system.h
	$(CC) $(CFlags) -c syscall/system.c -o syscall/bin/system.obj $(Include) $(Definition)

syscall/bin/gui.obj: syscall/gui.c syscall/gui.h
	$(CC) $(CFlags) -c syscall/gui.c -o syscall/bin/gui.obj $(Include) $(Definition)


fs/ifs1/bin/blocks.obj: fs/ifs1/blocks.c fs/ifs1/blocks.h
	$(CC) $(CFlags) -c fs/ifs1/blocks.c -o fs/ifs1/bin/blocks.obj $(Include) $(Definition)

fs/ifs1/bin/fs.obj: fs/ifs1/fs.c fs/ifs1/fs.h
	$(CC) $(CFlags) -c fs/ifs1/fs.c -o fs/ifs1/bin/fs.obj $(Include) $(Definition)


cppcrt/bin/cppcrt.obj: cppcrt/cppcrt.cpp cppcrt/cppcrt.hpp
	$(CPP) $(CPPFLAGS) -c cppcrt/cppcrt.cpp -o cppcrt/bin/cppcrt.obj $(Include) $(Definition)

cppcrt/bin/ctor.obj: cppcrt/ctor.cpp
	$(CPP) $(CPPFLAGS) -c cppcrt/ctor.cpp -o cppcrt/bin/ctor.obj $(Include) $(Definition)

cppcrt/bin/dtor.obj: cppcrt/dtor.cpp
	$(CPP) $(CPPFLAGS) -c cppcrt/dtor.cpp -o cppcrt/bin/dtor.obj $(Include) $(Definition)


elf/bin/helper.obj: elf/helper.c
	$(CC) $(CFlags) -c elf/helper.c -o elf/bin/helper.obj $(Include) $(Definition)

elf/bin/parser.obj: elf/parser.c
	$(CC) $(CFlags) -c elf/parser.c -o elf/bin/parser.obj $(Include) $(Definition)


pci/bin/pci.obj: pci/pci.c
	$(CC) $(CFlags) -c pci/pci.c -o pci/bin/pci.obj $(Include) $(Definition)


wav/bin/wav.obj: wav/wav.c
	$(CC) $(CFlags) -c wav/wav.c -o wav/bin/wav.obj $(Include) $(Definition)


sb/bin/sb16.obj: sb/sb16.c
	$(CC) $(CFlags) -c sb/sb16.c -o sb/bin/sb16.obj $(Include) $(Definition)


net/bin/pcnet2.obj: net/pcnet2.c
	$(CC) $(CFlags) -c net/pcnet2.c -o net/bin/pcnet2.obj $(Include) $(Definition)

net/bin/endian.obj: net/endian.c
	$(CC) $(CFlags) -c net/endian.c -o net/bin/endian.obj $(Include) $(Definition)

net/bin/net.obj: net/net.c
	$(CC) $(CFlags) -c net/net.c -o net/bin/net.obj $(Include) $(Definition)

net/bin/helper.obj: net/helper.c
	$(CC) $(CFlags) -c net/helper.c -o net/bin/helper.obj $(Include) $(Definition)

net/bin/e1000.obj: net/e1000.c
	$(CC) $(CFlags) -c net/e1000.c -o net/bin/e1000.obj $(Include) $(Definition)


.PHONY:clean

clean:
	-rm -f $(Objs) $(BMPObjs) $(UtilsObjs) $(DumpObjs) $(WindowObjs)	\
$(WindowsObjs) $(SyscallObjs) $(FS_IFS1Objs) $(CppCRTObjs) $(ELFObjs) $(Target)
