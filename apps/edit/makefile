TargetDir=bin
Out=edit.bin
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -I../ilib -m32 -march=i386 -mtune=i386
Objs=bin/main.obj
Linker=ld -m elf_i386
Include=-I../../ilib
AppMaker=../../tools/appmaker/bin/appmaker

$(Target): $(Objs)
	$(Linker) -Tsegs.lds -e main --oformat binary -o $(Target) $(Objs) -L../../ilib/bin -lilib
	$(AppMaker) $(Target)

bin/main.obj: main.c
	$(CC) $(CFlags) -c main.c -o bin/main.obj $(Include)

.PHONY:Clear

Clear:
	-rm -f $(Objs) $(Target)


