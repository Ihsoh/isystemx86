TargetDir=bin
Out=pci.bin
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/main.obj bin/toaruos_pci.obj bin/386.obj bin/pci.obj
Linker=ld -m elf_i386
Include=-I../../libs/
SysMaker=../../tools/sysmaker/bin/sysmaker
MTA32Obj=../../libs/ilib/bin/mta32.obj

$(Target): bin $(Objs)
	$(Linker) -Tsegs.lds -e mta32main --oformat binary -o $(Target) $(MTA32Obj) $(Objs)	\
-L../../libs/ilib/bin -lilib	\
-L../../libs/mempoollib/bin -lmempoollib
	$(SysMaker) $(Target)

bin:
	-mkdir bin

bin/main.obj: main.c
	$(CC) $(CFlags) -c main.c -o bin/main.obj $(Include)

bin/toaruos_pci.obj: toaruos_pci.c toaruos_pci.h
	$(CC) $(CFlags) -c toaruos_pci.c -o bin/toaruos_pci.obj $(Include)

bin/386.obj: 386.c 386.h
	$(CC) $(CFlags) -c 386.c -o bin/386.obj $(Include)

bin/pci.obj: pci.c pci.h
	$(CC) $(CFlags) -c pci.c -o bin/pci.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Objs) $(Target)
