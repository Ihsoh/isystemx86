#Filename:	makefile
#Author:	Ihsoh
#Date:		2014-1-24
#Descriptor:Make kernel.bin

TargetDir=bin
Out=kernel.bin
Target=$(TargetDir)/$(Out)
CC=gcc
AS=as
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
ASFlags=--32 -march=i386 -mtune=i386
StringObj=../libs/ilib/bin/string.obj

Objs=bin/kernel.obj	\
bin/386.obj bin/screen.obj bin/die.obj bin/keyboard.obj \
bin/console.obj bin/disk.obj bin/vdisk.obj bin/memory.obj \
bin/kfuncs.obj bin/hdisk.obj bin/cmos.obj bin/tasks.obj \
bin/vesa.obj bin/enfont.obj bin/vars.obj bin/image.obj bin/cpu.obj	\
bin/paging.obj bin/lock.obj bin/window.obj bin/klib.obj bin/kstring.obj bin/log.obj bin/rsdp.obj	\
bin/acpi.obj bin/rsdt.obj bin/fadt.obj bin/atapi.obj bin/serial.obj bin/mqueue.obj	\
bin/pci.obj bin/apic.obj bin/madt.obj bin/pic.obj bin/interrupts.obj bin/config.obj 	\
bin/ahci.obj bin/dma.obj bin/mouse.obj bin/kmpool.obj bin/knlldr.obj bin/irq.obj bin/timer.obj	\
bin/gui.obj

BMPObjs=bmp/bin/bmp.obj

UtilsObjs=utils/bin/img.obj

DumpObjs=dump/bin/dump.obj dump/bin/msg.obj

WindowObjs=window/bin/control.obj window/bin/button.obj window/bin/label.obj window/bin/list.obj 	\
window/bin/progress.obj window/bin/scroll.obj

WindowsObjs=windows/bin/clock.obj windows/bin/console.obj windows/bin/sfunc.obj windows/bin/message.obj 	\
windows/bin/detail.obj windows/bin/explorer.obj windows/bin/taskbar.obj windows/bin/start.obj 	\
windows/bin/power.obj windows/bin/wmgr.obj

SyscallObjs=syscall/bin/fs.obj syscall/bin/keyboard.obj syscall/bin/mouse.obj syscall/bin/screen.obj 	\
syscall/bin/system.obj syscall/bin/gui.obj

FS_IFS1=fs/ifs1/bin/blocks.obj fs/ifs1/bin/fs.obj

Linker=ld -m elf_i386
Include=-I../libs -I../sys

$(Target): bin bmp/bin dump/bin fs/ifs1/bin syscall/bin utils/bin window/bin windows/bin \
$(Objs) $(BMPObjs) $(UtilsObjs) $(DumpObjs) $(WindowObjs) $(WindowsObjs) $(SyscallObjs) $(FS_IFS1)
	$(Linker) -Tkernel.lds -e main --oformat binary -o $(Target) $(Objs)	\
$(StringObj) $(BMPObjs) $(UtilsObjs) $(DumpObjs) $(WindowObjs) $(WindowsObjs)	$(SyscallObjs) $(FS_IFS1)	\
-L../libs/jsonlib/bin -ljsonlib 	\
-L../libs/dslib/bin -ldslib 	\
-L../libs/encryptionlib/bin -lencryptionlib	\
-L../libs/pathlib/bin -lpathlib	\
-L../libs/mempoollib/bin -lmempoollib

bin:
	-mkdir bin

bmp/bin:
	-mkdir bmp/bin

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

bin/kernel.obj: kernel.c kernel.h
	$(CC) $(CFlags) -c kernel.c -o bin/kernel.obj $(Include)

bin/386.obj: 386.c 386.h types.h
	$(CC) $(CFlags) -c 386.c -o bin/386.obj $(Include)

bin/screen.obj: screen.c screen.h types.h 386.h
	$(CC) $(CFlags) -c screen.c -o bin/screen.obj $(Include)

bin/die.obj: die.c die.h screen.h types.h
	$(CC) $(CFlags) -c die.c -o bin/die.obj $(Include)

bin/keyboard.obj: keyboard.c keyboard.h types.h
	$(CC) $(CFlags) -c keyboard.c -o bin/keyboard.obj $(Include)

bin/console.obj: console.c console.h keyboard.h screen.h
	$(CC) $(CFlags) -c console.c -o bin/console.obj $(Include)

bin/disk.obj: disk.c disk.h
	$(CC) $(CFlags) -c disk.c -o bin/disk.obj $(Include)

bin/vdisk.obj: vdisk.c vdisk.h
	$(CC) $(CFlags) -c vdisk.c -o bin/vdisk.obj $(Include)

bin/memory.obj: memory.c memory.h
	$(CC) $(CFlags) -c memory.c -o bin/memory.obj $(Include)

bin/kfuncs.obj: kfuncs.c kfuncs.h
	$(CC) $(CFlags) -c kfuncs.c -o bin/kfuncs.obj $(Include)

bin/hdisk.obj: hdisk.c hdisk.h
	$(CC) $(CFlags) -c hdisk.c -o bin/hdisk.obj $(Include)

bin/cmos.obj: cmos.c cmos.h
	$(CC) $(CFlags) -c cmos.c -o bin/cmos.obj $(Include)

bin/tasks.obj: tasks.c tasks.h
	$(CC) $(CFlags) -c tasks.c -o bin/tasks.obj $(Include)

bin/vesa.obj: vesa.c vesa.h
	$(CC) $(CFlags) -c vesa.c -o bin/vesa.obj $(Include)

bin/enfont.obj: enfont.c enfont.h
	$(CC) $(CFlags) -c enfont.c -o bin/enfont.obj $(Include)

bin/vars.obj: vars.c vars.h
	$(CC) $(CFlags) -c vars.c -o bin/vars.obj $(Include)

bin/image.obj: image.c image.h
	$(CC) $(CFlags) -c image.c -o bin/image.obj $(Include) -D_KERNEL_MODEL_

bin/cpu.obj: cpu.c cpu.h
	$(CC) $(CFlags) -c cpu.c -o bin/cpu.obj $(Include)

bin/paging.obj: paging.c paging.h
	$(CC) $(CFlags) -c paging.c -o bin/paging.obj $(Include)

bin/lock.obj: lock.c lock.h
	$(CC) $(CFlags) -c lock.c -o bin/lock.obj $(Include)

bin/window.obj: window.c window.h
	$(CC) $(CFlags) -c window.c -o bin/window.obj $(Include)

bin/klib.obj: klib.c klib.h
	$(CC) $(CFlags) -c klib.c -o bin/klib.obj $(Include)

bin/kstring.obj: kstring.c kstring.h
	$(CC) $(CFlags) -c kstring.c -o bin/kstring.obj $(Include)

bin/log.obj: log.c log.h
	$(CC) $(CFlags) -c log.c -o bin/log.obj $(Include)

bin/rsdp.obj: rsdp.c rsdp.h
	$(CC) $(CFlags) -c rsdp.c -o bin/rsdp.obj $(Include)

bin/acpi.obj: acpi.c acpi.h
	$(CC) $(CFlags) -c acpi.c -o bin/acpi.obj $(Include)

bin/rsdt.obj: rsdt.c rsdt.h
	$(CC) $(CFlags) -c rsdt.c -o bin/rsdt.obj $(Include)

bin/fadt.obj: fadt.c fadt.h
	$(CC) $(CFlags) -c fadt.c -o bin/fadt.obj $(Include)

bin/atapi.obj: atapi.c atapi.h
	$(CC) $(CFlags) -c atapi.c -o bin/atapi.obj $(Include)

bin/serial.obj: serial.c serial.h
	$(CC) $(CFlags) -c serial.c -o bin/serial.obj $(Include)

bin/mqueue.obj: mqueue.c mqueue.h
	$(CC) $(CFlags) -c mqueue.c -o bin/mqueue.obj $(Include)

bin/toaruos_pci.obj: toaruos_pci.c toaruos_pci.h
	$(CC) $(CFlags) -c toaruos_pci.c -o bin/toaruos_pci.obj $(Include)

bin/pci.obj: pci.c pci.h
	$(CC) $(CFlags) -c pci.c -o bin/pci.obj $(Include)

bin/apic.obj: apic.c apic.h
	$(CC) $(CFlags) -c apic.c -o bin/apic.obj $(Include)

bin/madt.obj: madt.c madt.h
	$(CC) $(CFlags) -c madt.c -o bin/madt.obj $(Include)

bin/pic.obj: pic.c pic.h
	$(CC) $(CFlags) -c pic.c -o bin/pic.obj $(Include)

bin/interrupts.obj: interrupts.c interrupts.h
	$(CC) $(CFlags) -c interrupts.c -o bin/interrupts.obj $(Include)

bin/config.obj: config.c config.h
	$(CC) $(CFlags) -c config.c -o bin/config.obj $(Include)

bin/ahci.obj: ahci.c ahci.h
	$(CC) $(CFlags) -c ahci.c -o bin/ahci.obj $(Include)

bin/dma.obj: dma.c dma.h
	$(CC) $(CFlags) -c dma.c -o bin/dma.obj $(Include)

bin/mouse.obj: mouse.c mouse.h
	$(CC) $(CFlags) -c mouse.c -o bin/mouse.obj $(Include)

bin/kmpool.obj: kmpool.c kmpool.h
	$(CC) $(CFlags) -c kmpool.c -o bin/kmpool.obj $(Include)

bin/knlldr.obj: knlldr.c knlldr.h
	$(CC) $(CFlags) -c knlldr.c -o bin/knlldr.obj $(Include)

bin/irq.obj: irq.s
	$(AS) $(ASFlags) -c irq.s -o bin/irq.obj

bin/timer.obj: timer.c timer.h
	$(CC) $(CFlags) -c timer.c -o bin/timer.obj $(Include)

bin/gui.obj: gui.c gui.h
	$(CC) $(CFlags) -c gui.c -o bin/gui.obj $(Include)


bmp/bin/bmp.obj: bmp/bmp.c bmp/bmp.h
	$(CC) $(CFlags) -c bmp/bmp.c -o bmp/bin/bmp.obj $(Include) -std=c99


utils/bin/img.obj: utils/img.c utils/img.h
	$(CC) $(CFlags) -c utils/img.c -o utils/bin/img.obj $(Include)


dump/bin/dump.obj: dump/dump.c dump/dump.h
	$(CC) $(CFlags) -c dump/dump.c -o dump/bin/dump.obj $(Include)

dump/bin/msg.obj: dump/msg.c dump/msg.h
	$(CC) $(CFlags) -c dump/msg.c -o dump/bin/msg.obj $(Include)


window/bin/control.obj: window/control.c window/control.h
	$(CC) $(CFlags) -c window/control.c -o window/bin/control.obj $(Include)

window/bin/button.obj: window/button.c window/button.h
	$(CC) $(CFlags) -c window/button.c -o window/bin/button.obj $(Include)

window/bin/label.obj: window/label.c window/label.h
	$(CC) $(CFlags) -c window/label.c -o window/bin/label.obj $(Include)

window/bin/list.obj: window/list.c window/list.h
	$(CC) $(CFlags) -c window/list.c -o window/bin/list.obj $(Include)

window/bin/progress.obj: window/progress.c window/progress.h
	$(CC) $(CFlags) -c window/progress.c -o window/bin/progress.obj $(Include)

window/bin/scroll.obj: window/scroll.c window/scroll.h
	$(CC) $(CFlags) -c window/scroll.c -o window/bin/scroll.obj $(Include)


windows/bin/clock.obj: windows/clock.c windows/clock.h
	$(CC) $(CFlags) -c windows/clock.c -o windows/bin/clock.obj $(Include)

windows/bin/console.obj: windows/console.c windows/console.h
	$(CC) $(CFlags) -c windows/console.c -o windows/bin/console.obj $(Include)

windows/bin/sfunc.obj: windows/sfunc.c windows/sfunc.h
	$(CC) $(CFlags) -c windows/sfunc.c -o windows/bin/sfunc.obj $(Include)

windows/bin/message.obj: windows/message.c windows/message.h
	$(CC) $(CFlags) -c windows/message.c -o windows/bin/message.obj $(Include)

windows/bin/detail.obj: windows/detail.c windows/detail.h
	$(CC) $(CFlags) -c windows/detail.c -o windows/bin/detail.obj $(Include)

windows/bin/explorer.obj: windows/explorer.c windows/explorer.h
	$(CC) $(CFlags) -c windows/explorer.c -o windows/bin/explorer.obj $(Include)

windows/bin/taskbar.obj: windows/taskbar.c windows/taskbar.h
	$(CC) $(CFlags) -c windows/taskbar.c -o windows/bin/taskbar.obj $(Include)

windows/bin/start.obj: windows/start.c windows/start.h
	$(CC) $(CFlags) -c windows/start.c -o windows/bin/start.obj $(Include)

windows/bin/power.obj: windows/power.c windows/power.h
	$(CC) $(CFlags) -c windows/power.c -o windows/bin/power.obj $(Include)

windows/bin/wmgr.obj: windows/wmgr.c windows/wmgr.h
	$(CC) $(CFlags) -c windows/wmgr.c -o windows/bin/wmgr.obj $(Include)


syscall/bin/fs.obj: syscall/fs.c syscall/fs.h
	$(CC) $(CFlags) -c syscall/fs.c -o syscall/bin/fs.obj $(Include)

syscall/bin/keyboard.obj: syscall/keyboard.c syscall/keyboard.h
	$(CC) $(CFlags) -c syscall/keyboard.c -o syscall/bin/keyboard.obj $(Include)

syscall/bin/mouse.obj: syscall/mouse.c syscall/mouse.h
	$(CC) $(CFlags) -c syscall/mouse.c -o syscall/bin/mouse.obj $(Include)

syscall/bin/screen.obj: syscall/screen.c syscall/screen.h
	$(CC) $(CFlags) -c syscall/screen.c -o syscall/bin/screen.obj $(Include)

syscall/bin/system.obj: syscall/system.c syscall/system.h
	$(CC) $(CFlags) -c syscall/system.c -o syscall/bin/system.obj $(Include)

syscall/bin/gui.obj: syscall/gui.c syscall/gui.h
	$(CC) $(CFlags) -c syscall/gui.c -o syscall/bin/gui.obj $(Include)


fs/ifs1/bin/blocks.obj: fs/ifs1/blocks.c fs/ifs1/blocks.h
	$(CC) $(CFlags) -c fs/ifs1/blocks.c -o fs/ifs1/bin/blocks.obj $(Include)

fs/ifs1/bin/fs.obj: fs/ifs1/fs.c fs/ifs1/fs.h
	$(CC) $(CFlags) -c fs/ifs1/fs.c -o fs/ifs1/bin/fs.obj $(Include)



.PHONY:clean

clean:
	-rm -f $(Objs) $(BMPObjs) $(UtilsObjs) $(DumpObjs) $(WindowObjs) $(WindowsObjs) $(SyscallObjs) $(Target)
