TargetDir=bin
Out=printf.elf
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/main.obj
Linker=ld -m elf_i386
Include=-I../../libs/
AppMaker=../../tools/appmaker/bin/appmaker
ELF32Obj=../../libs/ilib/bin/elf32.obj

$(Target): bin $(Objs)
	$(Linker) -Tsegs.lds -e elf32main --oformat elf32-i386 -o $(Target) $(ELF32Obj) $(Objs)	\
-L../../libs/ilib/bin -lilib	\
-L../../libs/dslib/bin -ldslib	\
-L../../libs/mempoollib/bin -lmempoollib

bin:
	-mkdir bin

bin/main.obj: main.c
	$(CC) $(CFlags) -c main.c -o bin/main.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Objs) $(Target)
