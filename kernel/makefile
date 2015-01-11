#Filename:	makefile
#Author:	Ihsoh
#Date:		2014-1-24
#Descriptor:Make kernel.bin

TargetDir=bin
Out=kernel.bin
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -I../ilib -m32 -march=i386 -mtune=i386
StringObj=../ilib/bin/string.obj
Objs=bin/kernel.obj bin/386.obj bin/screen.obj bin/die.obj bin/keyboard.obj \
bin/console.obj bin/ifs1disk.obj bin/ifs1vdisk.obj bin/ifs1blocks.obj bin/memory.obj \
bin/kfuncs.obj bin/ifs1hdisk.obj bin/cmos.obj bin/ifs1fs.obj bin/tasks.obj \
bin/vesa.obj bin/enfont.obj bin/scall_screen.obj bin/scall_keyboard.obj bin/scall_fs.obj \
bin/scall_system.obj bin/vars.obj bin/image.obj bin/scall_mouse.obj bin/cpu.obj	\
bin/paging.obj bin/lock.obj bin/window.obj bin/clock_window.obj bin/console_window.obj	\
bin/klib.obj bin/sfunc_window.obj $(StringObj)
Linker=ld -m elf_i386
Include=-I../ilib

$(Target): bin $(Objs)
	$(Linker) -Tkernel.lds -e main --oformat binary -o $(Target) $(Objs)

bin:
	-mkdir bin

bin/kernel.obj: kernel.c
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

bin/ifs1disk.obj: ifs1disk.c ifs1disk.h
	$(CC) $(CFlags) -c ifs1disk.c -o bin/ifs1disk.obj $(Include)

bin/ifs1vdisk.obj: ifs1vdisk.c ifs1vdisk.h
	$(CC) $(CFlags) -c ifs1vdisk.c -o bin/ifs1vdisk.obj $(Include)

bin/ifs1blocks.obj: ifs1blocks.c ifs1blocks.h
	$(CC) $(CFlags) -c ifs1blocks.c -o bin/ifs1blocks.obj $(Include)

bin/memory.obj: memory.c memory.h
	$(CC) $(CFlags) -c memory.c -o bin/memory.obj $(Include)

bin/kfuncs.obj: kfuncs.c kfuncs.h
	$(CC) $(CFlags) -c kfuncs.c -o bin/kfuncs.obj $(Include)

bin/ifs1hdisk.obj: ifs1hdisk.c ifs1hdisk.h
	$(CC) $(CFlags) -c ifs1hdisk.c -o bin/ifs1hdisk.obj $(Include)

bin/cmos.obj: cmos.c cmos.h
	$(CC) $(CFlags) -c cmos.c -o bin/cmos.obj $(Include)

bin/ifs1fs.obj: ifs1fs.c ifs1fs.h
	$(CC) $(CFlags) -c ifs1fs.c -o bin/ifs1fs.obj $(Include)

bin/tasks.obj: tasks.c tasks.h
	$(CC) $(CFlags) -c tasks.c -o bin/tasks.obj $(Include)

bin/vesa.obj: vesa.c vesa.h
	$(CC) $(CFlags) -c vesa.c -o bin/vesa.obj $(Include)

bin/enfont.obj: enfont.c enfont.h
	$(CC) $(CFlags) -c enfont.c -o bin/enfont.obj $(Include)

bin/scall_screen.obj: scall_screen.c scall_screen.h
	$(CC) $(CFlags) -c scall_screen.c -o bin/scall_screen.obj $(Include)

bin/scall_keyboard.obj: scall_keyboard.c scall_keyboard.h
	$(CC) $(CFlags) -c scall_keyboard.c -o bin/scall_keyboard.obj $(Include)

bin/scall_fs.obj: scall_fs.c scall_fs.h
	$(CC) $(CFlags) -c scall_fs.c -o bin/scall_fs.obj $(Include)

bin/scall_system.obj: scall_system.c scall_system.h
	$(CC) $(CFlags) -c scall_system.c -o bin/scall_system.obj $(Include)

bin/vars.obj: vars.c vars.h
	$(CC) $(CFlags) -c vars.c -o bin/vars.obj $(Include)

bin/image.obj: image.c image.h
	$(CC) $(CFlags) -c image.c -o bin/image.obj $(Include) -D_KERNEL_MODEL_

bin/scall_mouse.obj: scall_mouse.c scall_mouse.h
	$(CC) $(CFlags) -c scall_mouse.c -o bin/scall_mouse.obj $(Include)

bin/cpu.obj: cpu.c cpu.h
	$(CC) $(CFlags) -c cpu.c -o bin/cpu.obj $(Include)

bin/paging.obj: paging.c paging.h
	$(CC) $(CFlags) -c paging.c -o bin/paging.obj $(Include)

bin/lock.obj: lock.c lock.h
	$(CC) $(CFlags) -c lock.c -o bin/lock.obj $(Include)

bin/window.obj: window.c window.h
	$(CC) $(CFlags) -c window.c -o bin/window.obj $(Include)

bin/clock_window.obj: clock_window.c clock_window.h
	$(CC) $(CFlags) -c clock_window.c -o bin/clock_window.obj $(Include)

bin/console_window.obj: console_window.c console_window.h
	$(CC) $(CFlags) -c console_window.c -o bin/console_window.obj $(Include)

bin/klib.obj: klib.c klib.h
	$(CC) $(CFlags) -c klib.c -o bin/klib.obj $(Include)

bin/sfunc_window.obj: sfunc_window.c sfunc_window.h
	$(CC) $(CFlags) -c sfunc_window.c -o bin/sfunc_window.obj $(Include)

.PHONY:Clear

Clear:
	-rm -f $(Objs) $(Target)


