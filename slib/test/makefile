TargetDir=bin
Out=test.elf.so
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 -fPIC
Objs=bin/main.obj
Linker=ld -m elf_i386
Include=-I../../libs/
ELFSO32Obj=../../libs/ilib/bin/pic/elfso32.obj

$(Target): bin $(Objs)
	$(Linker) -fPIC -shared --oformat elf32-i386 -o $(Target) $(ELFSO32Obj) $(Objs)	\
-L../../libs/ilib/bin/pic -lilib	\
-L../../libs/dslib/bin/pic -ldslib	\
-L../../libs/mempoollib/bin/pic -lmempoollib

bin:
	-mkdir bin

bin/main.obj: main.c
	$(CC) $(CFlags) -c main.c -o bin/main.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Objs) $(Target)
