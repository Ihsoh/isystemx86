TargetDir=bin
Out=elf.bin
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/main.obj
Linker=ld -m elf_i386
Include=-I../../libs
AppMaker=../../tools/appmaker/bin/appmaker

$(Target): bin $(Objs)
	$(Linker) -Tsegs.lds -e main --oformat binary  -o $(Target) $(Objs)	\
-L../../libs/isys/bin -lisys	\
-L../../libs/ilib/bin -lilib	\
-L../../libs/mempoollib/bin -lmempoollib
	$(AppMaker) $(Target)

bin:
	-mkdir bin

bin/main.obj: main.c
	$(CC) $(CFlags) -c main.c -o bin/main.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Objs) $(Target)
