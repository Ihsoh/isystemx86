TargetDir=bin
Out=basic.bin
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -I../ilib -m32 -march=i386 -mtune=i386
Objs=bin/main.obj bin/lexer.obj bin/die.obj
Linker=ld -m elf_i386
Include=-I../../ilib -I../../
AppMaker=../../tools/appmaker/bin/appmaker
MTA32Obj=../../ilib/bin/mta32.obj

$(Target): bin $(Objs)
	$(Linker) -Tsegs.lds -e mta32main --oformat binary -o $(Target) $(MTA32Obj) $(Objs) -L../../ilib/bin -lilib -L../../dslib/bin -ldslib
	$(AppMaker) $(Target)

bin:
	-mkdir bin

bin/main.obj: main.c
	$(CC) $(CFlags) -c main.c -o bin/main.obj $(Include)

bin/lexer.obj: lexer.c lexer.h
	$(CC) $(CFlags) -c lexer.c -o bin/lexer.obj $(Include)

bin/die.obj: die.c die.h
	$(CC) $(CFlags) -c die.c -o bin/die.obj $(Include)

.PHONY:Clear

Clear:
	-rm -f $(Objs) $(Target)
