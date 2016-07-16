TargetDir=bin
Out=iasm.bin
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 -w
Objs=bin/main.obj bin/error.obj bin/lexer.obj bin/encoder.obj bin/stable.obj bin/string.obj bin/parser.obj
Linker=ld -m elf_i386
Include=-I../../libs
AppMaker=../../tools/appmaker/bin/appmaker
MTA32Obj=../../libs/ilib/bin/mta32.obj

$(Target): bin $(Objs)
	$(Linker) -Tsegs.lds -e mta32main --oformat binary -o $(Target) $(MTA32Obj) $(Objs)	\
-L../../libs/ilib/bin -lilib	\
-L../../libs/mempoollib/bin -lmempoollib
	$(AppMaker) $(Target)

bin:
	-mkdir bin

bin/main.obj: main.c
	$(CC) $(CFlags) -c main.c -o bin/main.obj $(Include)

bin/error.obj: error.c error.h
	$(CC) $(CFlags) -c error.c -o bin/error.obj $(Include)

bin/lexer.obj: lexer.c lexer.h
	$(CC) $(CFlags) -c lexer.c -o bin/lexer.obj $(Include)

bin/encoder.obj: encoder.c encoder.h
	$(CC) $(CFlags) -c encoder.c -o bin/encoder.obj $(Include)

bin/stable.obj: stable.c stable.h
	$(CC) $(CFlags) -c stable.c -o bin/stable.obj $(Include)

bin/string.obj: string.c string.h
	$(CC) $(CFlags) -c string.c -o bin/string.obj $(Include)

bin/parser.obj: parser.c parser.h
	$(CC) $(CFlags) -c parser.c -o bin/parser.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Objs) $(Target)
